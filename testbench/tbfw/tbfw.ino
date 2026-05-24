#include "Arduino.h"
#include "Wire.h"
#include "INA226.h"

const int CONSOLE_LEFT = 1;
const int CONSOLE_RIGHT = 2;
const int CONSOLE_OK = 4;
const int CONSOLE_CANCEL = 8;

typedef struct  
{
    const char* name;
    void (*action)();
} MenuItem;

typedef struct 
{
    const char* title;
    MenuItem* items;
    void (*back_action)(void); 
    int selected_index;
    int left_item_index;
} Menu;

const int8_t MPU_DATA[] = {4, 5, 6, 7, A3, A2, A1, A0};
const int8_t MPU_ADDR[] = {8, 9, 10, 11};
const int8_t MPU_WRITE = 12;
const int8_t MPU_READ = 13;

const int8_t MPU_ADDR_STATUS = 0x06;
const int8_t MPU_ADDR_CONSOLE = 0x07;

const int8_t MCU_STATE_ERROR = 0x40;
const int8_t MCU_STATE_RUN = 0x80;
const int8_t MCU_STATE_POWER_A = 0x01;
const int8_t MCU_STATE_POWER_B = 0x02;
const int8_t MCU_STATE_POWER_C = 0x04;

const int8_t MPU_ADDR_DIR[] = {0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D};
const int8_t MPU_ADDR_DATA[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

Menu *current_menu = NULL;
bool ina_ok = false;

INA226 INA(0x40);

uint8_t multiplex_read(uint8_t address)
{
    for (int iAddr = 0; iAddr < 4; iAddr++)
        digitalWrite(MPU_ADDR[iAddr], (address >> iAddr) & 0x01);

    for (int iData = 0; iData < 8; iData++)
        pinMode(MPU_DATA[iData], INPUT);

    digitalWrite(MPU_READ, LOW);
    delayMicroseconds(1);

    uint8_t data = 0;
    for (int iData = 0; iData < 8; iData++)
    {
        if (digitalRead(MPU_DATA[iData]))
            data |= (1 << iData);
    }

    digitalWrite(MPU_READ, HIGH);

    return data;
}

void multiplex_write(uint8_t address, uint8_t data)
{
    for (int iAddr = 0; iAddr < 4; iAddr++)
        digitalWrite(MPU_ADDR[iAddr], (address >> iAddr) & 0x01);

    for (int iData = 0; iData < 8; iData++)
    {
        pinMode(MPU_DATA[iData], OUTPUT);
        digitalWrite(MPU_DATA[iData], (data >> iData) & 0x01);
    }

    digitalWrite(MPU_WRITE, LOW);
    delayMicroseconds(1);
    digitalWrite(MPU_WRITE, HIGH);
}

void LCD_write_nibble(uint8_t rs, uint8_t byte)
{
    uint8_t lcd = byte & 0x0F | (rs ? 0x40 : 0x00);

    multiplex_write(MPU_ADDR_CONSOLE, lcd);
    delayMicroseconds(1);    
    multiplex_write(MPU_ADDR_CONSOLE, lcd | 0x80);
    delayMicroseconds(1);    
    multiplex_write(MPU_ADDR_CONSOLE, lcd);
    delayMicroseconds(100);
}

void LCD_write(uint8_t rs, uint8_t byte)
{
    LCD_write_nibble(rs, byte >> 4);
    LCD_write_nibble(rs, byte & 0x0F);
}

void LCD_write_register(uint8_t byte)
{
    LCD_write(0, byte);
    delay(1);
}

void LCD_write_char(uint8_t byte)
{
    LCD_write(1, byte);
    delayMicroseconds(100);
}

void LCD_clear()
{
    LCD_write_register(0x01);  
    delay(2);
}

void LCD_clear_line(int line)
{
    LCD_setpos(line, 0);
    char clear_line[17];
    memset(clear_line, ' ', 16);
    clear_line[16] = '\0';
    LCD_print(clear_line);
    LCD_setpos(line, 0);
}

void LCD_blink()
{
    LCD_write_register(0x0D);   
}

void LCD_noblink()
{
    LCD_write_register(0x0C);   
}

void LCD_setpos(uint8_t row, uint8_t col)
{
    LCD_write_register(0x80 | (col + row * 0x40));   
}

void LCD_print(const char *str)
{
    while (*str)
    {
        LCD_write_char(*str);
        str++;
    }
}

void LCD_print(int num)
{
    char buffer[12];
    itoa(num, buffer, 10);
    LCD_print(buffer);
}

void LCD_print(float num)
{
    char buffer[12];
    dtostrf(num, 0, 2, buffer);
    LCD_print(buffer);
}

void LCD_print(float num, int width, int precision)
{
    char buffer[12];
    dtostrf(num, width, precision, buffer);
    LCD_print(buffer);
}

void LCD_init()
{
    int increment_decrement = 1;    // 1 = increment, 0 = decrement
    int display_on_off = 1;        // 1 = display on, 0 = display off
    int shift_display = 0;          // 1 = shift, 0 = no shift
    int cursor = 0;
    int blink = 0;
    int data_length = 0; // 0 = 4 bits, 1 = 8 bits
    int number_of_lines = 1; // 0 = 1 line, 1 = 2 lines
    int font = 0; // 0 = 5x8 dots, 1 = 5x10 dots

    // Wait for more than 15 ms after VCC rises to 4.5 V
    delay(50);

    // Function set (Interface is 8 bits long.)
    // Wait for more than 4.1 ms
    LCD_write_nibble(0, 0x03);
    delayMicroseconds(4500);

    // Function set (Interface is 8 bits long.)
    LCD_write_nibble(0, 0x03);
    delayMicroseconds(4500);

    // Function set (Interface is 4 bits long.)
    LCD_write_nibble(0, 0x03);
    delayMicroseconds(150);

    LCD_write_nibble(0, 0x02);

    // Function set
    // Sets interface data length (DL), number of display lines (N), and character font (F).
    LCD_write_register(0x20 | (data_length << 4) | (number_of_lines << 3) | (font << 2));

    // Display on/off control
    // Sets entire display (D) on/off, cursor on/off (C), and blinking of cursor position character (B).
    LCD_write_register(0x08 | (display_on_off << 2) | (cursor << 1) | blink);

    // Entry mode set
    // Sets cursor move direction (I/D) and specifies to shift the display (S).
    LCD_write_register(0x04 | (increment_decrement << 1) | shift_display);

    // Clear display
    // Clears entire display and sets DDRAM address 0 in address counter
    LCD_write_register(0x01);
    delay(2);
}

uint8_t current_testbench_dir[6];
uint8_t current_testbench_write_data[6];
uint8_t current_testbench_read_data[6];
uint8_t new_testbench_dir[6];
uint8_t new_testbench_write_data[6];

void testbench_init(void)
{
    memset(new_testbench_dir, 0xFF, sizeof(new_testbench_dir));
    memset(new_testbench_write_data, 0x00, sizeof(new_testbench_write_data));
    memset(current_testbench_dir, 0xFF, sizeof(current_testbench_dir));
    memset(current_testbench_write_data, 0x00, sizeof(current_testbench_write_data));

    for (int iByte = 0; iByte < 6; iByte++)
    {
        multiplex_write(MPU_ADDR_DIR[iByte], new_testbench_dir[iByte]);
        multiplex_write(MPU_ADDR_DATA[iByte], new_testbench_write_data[iByte]);
    }

    testbench_read();
}

void testbench_bit_dir(uint8_t bit, uint8_t dir)
{
    if (bit >= 48)
        return;

    uint8_t bit_mask = 1 << (bit % 8);
    uint8_t byte_index = bit >> 3;
    if (dir)
        new_testbench_dir[byte_index] &= ~bit_mask;
    else
        new_testbench_dir[byte_index] |= bit_mask;
}

void testbench_bit_set(uint8_t bit, uint8_t data)
{
    if (bit >= 48)
        return;

    uint8_t bit_mask = 1 << (bit % 8);
    uint8_t byte_index = bit >> 3;
    if (data)
        new_testbench_write_data[byte_index] |= bit_mask;
    else
        new_testbench_write_data[byte_index] &= ~bit_mask;
}

uint8_t testbench_bit_get(uint8_t bit)
{
    if (bit >= 48)
        return 0;

    uint8_t bit_mask = 1 << (bit % 8);
    uint8_t byte_index = bit >> 3;
    return (current_testbench_read_data[byte_index] & bit_mask) ? 1 : 0;
}

void testbench_write(void)
{
    for (int iByte = 0; iByte < 6; iByte++)
    {
        if (current_testbench_dir[iByte] != new_testbench_dir[iByte])
        {
            multiplex_write(MPU_ADDR_DIR[iByte], new_testbench_dir[iByte]);
            current_testbench_dir[iByte] = new_testbench_dir[iByte];
        }

        if (current_testbench_write_data[iByte] != new_testbench_write_data[iByte])
        {
            multiplex_write(MPU_ADDR_DATA[iByte], new_testbench_write_data[iByte]);
            current_testbench_write_data[iByte] = new_testbench_write_data[iByte];
        }
    }
}

void testbench_read(void)
{
    for (int iByte = 0; iByte < 6; iByte++)
        current_testbench_read_data[iByte] = multiplex_read(MPU_ADDR_DATA[iByte]);
}

void open_menu_opts(void);
void menu_opts_back(void);
void option_about(void);
void testbench_select_board(void);
void testbench_select_sim(void);
void testbench_autotest(void);
  
MenuItem menu_main_items[] = 
{
    {"test", testbench_select_board},
    {"sim", testbench_select_sim},
    {"opts", open_menu_opts},
    {NULL, NULL}
};

 const char* title;
    MenuItem* items;

Menu menu_main = 
{
    title: "TESTBENCH",
    items: menu_main_items
};

MenuItem menu_ammeter_items[] = 
{
    {"wide", testbench_ammeter_wide},
    {"narrow", testbench_ammeter_narrow},
    {"header", testbench_ammeter_header},
    {NULL, NULL}
};

Menu menu_ammeter = 
{
    title: "Select slot",
    items: menu_ammeter_items,
    back_action: menu_ammeter_back
};

MenuItem menu_opts_items[] = 
{
    {"about", option_about},
    {"autotest", testbench_autotest},
    {"amps", open_menu_ammeter},
    {NULL, NULL}
};

Menu menu_opts = 
{
    title: "OPTIONS",
    items: menu_opts_items,
    back_action: menu_opts_back
};

typedef struct
{
  char *name;
  void (*test_board_proc)(void);
} TestBoard;

TestBoard test_boards[] = 
{
//    {"SR8A", NULL},
//    {"SGRDA", NULL},
//    {"MGRFA", NULL},
//    {"SR16A", NULL},
//    {"SWRDA", NULL},
//    {"M16RFA", NULL},
//    {"ULA8-ADDERA", NULL},
//    {"ULA8-BITWISEA", NULL},
//    {"ULA8-SHIFTERA", NULL},
//    {"ULA8-SXA", NULL},
//    {"ULA8-XORA", NULL},
//    {"ULA8DA", NULL},
    {"CLKCP", run_clock_cpa},
    {"TBSUB8", run_testbench_sub8},
    {NULL, NULL}
};

void menu_opts_back(void)
{
    menu_show(&menu_main);
}

void menu_ammeter_back(void)
{
    menu_show(&menu_opts);
}

void open_menu_opts()
{
    menu_opts.selected_index = 0;
    menu_show(&menu_opts);
}

void open_menu_ammeter()
{
    menu_ammeter.selected_index = 0;
    menu_show(&menu_ammeter);
}

void menu_mark_selected(Menu *menu)
{
    menu_mark_selected(menu, false);
}

void menu_mark_selected(Menu *menu, bool force_redraw)
{
    // Adjust left_item_index to ensure selected_index is visible
    bool redraw_items = force_redraw;

    if (menu->selected_index < menu->left_item_index)
    {
        menu->left_item_index = menu->selected_index;
        redraw_items = true;
    }
    else
    {
        int lcd_col = 0;
        int right_item_index = menu->left_item_index;
        while (menu->items[right_item_index].name != NULL)
        {
            int item_length = strlen(menu->items[right_item_index].name);
            if (lcd_col + item_length >= 16)
                break;
            lcd_col += item_length + 1;
            right_item_index++;
        }

        if (menu->selected_index >= right_item_index)
        {
            menu->left_item_index = menu->selected_index;
            redraw_items = true;
        }
    }

    // Redraw items if needed
    if (redraw_items)
    {
        LCD_noblink();
        LCD_clear_line(1);

        int lcd_col = 0;
        int menu_index = menu->left_item_index;
        while (menu->items[menu_index].name != NULL)
        {
            if (lcd_col + strlen(menu->items[menu_index].name) >= 16)
                break;

            LCD_print(menu->items[menu_index].name);
            LCD_print(" ");
            lcd_col += strlen(menu->items[menu_index].name) + 1;
            menu_index++;
        }

        LCD_blink();
    }

    // display the item index relative to item count
    char str[6];
    itoa(menu_items_count(menu), str, 10);
    int col = 15 - strlen(str) - 1;
    LCD_setpos(0, col);
    LCD_print("/");
    LCD_print(str);

    itoa(menu->selected_index + 1, str, 10);
    LCD_setpos(0, col - strlen(str) - 1);
    LCD_print(" ");
    LCD_print(str);

    // mark the selected item
    LCD_setpos(1, 0);

    col = 0;
    for (int i = menu->left_item_index; i < menu->selected_index; i++)
        col += strlen(menu->items[i].name) + 1;
    LCD_setpos(1, col);
}

int menu_items_count(Menu *menu)
{
    int count = 0;
    while (menu->items[count].name != NULL)
        count++;
    return count;
}

void menu_show(Menu* menu)
{
    LCD_noblink();
    LCD_clear();
    LCD_setpos(0, 0);
    LCD_print(menu->title);

    int menu_item_count = menu_items_count(menu);

    current_menu = menu;

    menu_mark_selected(menu, true);

    LCD_blink();
}

void menu_move_left()
{
    if (current_menu == NULL)
        return;

    if (current_menu->selected_index > 0)
    {
        current_menu->selected_index--;
        menu_mark_selected(current_menu);
    }
}

void menu_move_right()
{
    if (current_menu == NULL)
        return;

    if (current_menu->items[current_menu->selected_index + 1].name != NULL)
    {
        current_menu->selected_index++;
        menu_mark_selected(current_menu);
    }
}

void menu_select()
{
    if (current_menu == NULL)
        return;

    if (current_menu->items[current_menu->selected_index].action != NULL)
        current_menu->items[current_menu->selected_index].action();        
}

void menu_back()
{
    if (current_menu == NULL)
        return;

    if (current_menu->back_action != NULL)
        current_menu->back_action();        
}

int console_read()
{
    static int last_button = 0;
    int button = (~multiplex_read(MPU_ADDR_CONSOLE)) & 0x0F;
    int pressed = button & (~last_button);
    last_button = button;
    return pressed;
}

void option_about(void)
{
    LCD_noblink();
    LCD_clear();
    LCD_print("Testbench v1.0.0");
    LCD_setpos(1, 0);
    LCD_print("Capaverde 2025");

    while (console_read() == 0)
        delay(10);

    menu_show(current_menu);
}

void testbench_select_board(void)
{
    static char *clear_line = "                ";

    LCD_noblink();
    LCD_clear();
    LCD_print("Select:");

    int selected_board = -1;

    int count = 0;
    while (test_boards[count].name != NULL)
        count++;

    int index = 0;

    while (selected_board == -1)
    {
        LCD_setpos(1, 0);
        LCD_print(clear_line);
        LCD_setpos(1, 0);
        LCD_print(test_boards[index].name);

        char str_count[6];
        itoa(count, str_count, 10);
        char str_index[6];
        itoa(index + 1, str_index, 10);

        LCD_setpos(0, 15 - strlen(str_count) - strlen(str_index));
        LCD_print(str_index);
        LCD_print("/");
        LCD_print(str_count);

        while (true)
        {
            int button = console_read();

            if (button & CONSOLE_LEFT)
            {
                if (index > 0)
                    index--;
                else
                    index = count - 1;
                break;
            }

            else if (button & CONSOLE_RIGHT)
            {
                if (index < count - 1)
                    index++;
                else
                    index = 0;
                break;
            }

            else if (button & CONSOLE_CANCEL)
            {
                selected_board = -2;
                break;
            }


            else if (button & CONSOLE_OK)
            {
                selected_board = index;
                break;
            }

            delay(10);
        }
    }

    if (selected_board >= 0 && test_boards[selected_board].test_board_proc)
        test_boards[selected_board].test_board_proc();

    menu_show(current_menu);
}

void testbench_select_sim(void)
{
    LCD_noblink();
    LCD_clear();
    LCD_print("not implemented");

    while (console_read() == 0)
        delay(10);

    menu_show(current_menu);
}

bool internal_autotest()
{
    bool result = ina_ok;

    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_ERROR);
    delay(200);

    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_POWER_B);
    delay(200);

    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_POWER_A);
    delay(200);

    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_POWER_C);
    delay(200);

    multiplex_write(MPU_ADDR_STATUS, 0x00);

    if (!ina_ok)
    {
        if (!INA.isConnected())
            result = false;
    }

    return result;
}

void testbench_autotest(void)
{
    LCD_noblink();
    LCD_clear();

    LCD_print("AUTOTEST");
    LCD_setpos(1, 0);

    LCD_setpos(1, 0);

    if (internal_autotest())
    {
        LCD_print("done           ");
    }
    else
    {
        if (!ina_ok)
            LCD_print("INA226 begin error");
        else
            LCD_print("unknown error");
    }

    while (console_read() == 0)
        delay(10);

    menu_show(current_menu);
}

bool internal_ammeter_init()
{
    if (!INA.begin())
        return false;

    float shunt = 0.1;
    float current_LSB_mA = 0.1;
    float current_zero_offset_mA = 0;
    uint16_t bus_V_scaling_e4 = 9433;

    INA.configure(shunt, current_LSB_mA, current_zero_offset_mA, bus_V_scaling_e4);

    return INA.isConnected();
}

void testbench_ammeter_narrow(void)
{
    testbench_ammeter(1);
}

void testbench_ammeter_wide(void)
{
    testbench_ammeter(2);
}

void testbench_ammeter_header(void)
{
    testbench_ammeter(4);
}

void testbench_ammeter(int slot)
{
    LCD_noblink();
    LCD_clear();
    LCD_print("AMMETER");

    multiplex_write(MPU_ADDR_STATUS, slot & (0x07));

    int show_data = 0;

    while (console_read() == 0)
    {
        if (show_data == 0)
        {
            float current = INA.getCurrent();
            float busVoltage = INA.getBusVoltage();

            LCD_setpos(1, 0);

            LCD_print(current * 1000.0, 1, 1);
            LCD_print("mA ");

            LCD_print(busVoltage, 1, 1);
            LCD_print("V ");
        }

        delay(1);
        show_data++;
        if (show_data > 500)
            show_data = 0;
    }

    multiplex_write(MPU_ADDR_STATUS, 0x00);
    menu_show(current_menu);
}

void run_testbench_sub8(void)
{
    LCD_noblink();
    LCD_clear();

    const uint8_t PIN_TEST[] = {25, 26, 27, 28, 1, 2, 3, 4};
    const uint8_t PIN_DATA[] = {5, 6, 7, 8, 9, 10, 11, 12};
    const uint8_t PIN_WRITE_DATA = 34;
    const uint8_t PIN_WRITE_DIR = 35;
    const uint8_t PIN_INV_READ_ENABLE = 36;

    LCD_clear_line(1);
    LCD_print("warm up");

    // liga a placa em teste
    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_POWER_A);

    testbench_init();

    float amps_max = 0;

    // dá tempo para a placa estabilizar
    for (int i = 0; i < 10; i++)
    {
        if (ina_ok)
        {
            float current = INA.getCurrent() * 1000.0;
            if (current > amps_max)
                amps_max = current;
        }

        delay(50);
    }

    bool result = !ina_ok || amps_max > 0.1;

    if (result)
    {
        testbench_bit_dir(PIN_WRITE_DATA, OUTPUT);      // write data
        testbench_bit_dir(PIN_WRITE_DIR, OUTPUT);       // write dir
        testbench_bit_dir(PIN_INV_READ_ENABLE, OUTPUT); // inv read enable

        testbench_bit_set(PIN_INV_READ_ENABLE, HIGH);
        testbench_write();

        // configura o registro de direção da placa em teste para escrita
        LCD_clear_line(1);
        LCD_print("writing...");

        for (int i = 0; i < 8; i++)
            testbench_bit_dir(PIN_TEST[i], INPUT);  // test 0-7

        for (int i = 0; i < 8; i++)
            testbench_bit_dir(PIN_DATA[i], OUTPUT); // data 0-7

        for (int i = 0; i < 8; i++)
            testbench_bit_set(PIN_DATA[i], LOW);
        testbench_write();

        testbench_bit_set(PIN_WRITE_DIR, HIGH);
        testbench_write();
        testbench_bit_set(PIN_WRITE_DIR, LOW);
        testbench_write();

        for (int value = 0; value < 256; value++)
        {
            if (ina_ok)
            {
                float current = INA.getCurrent() * 1000.0;
                if (current > amps_max)
                    amps_max = current;
            }

            // escreve zeros e uns
            for (int i = 0; i < 8; i++)
                testbench_bit_set(PIN_DATA[i], (value >> i) & 0x01);
            testbench_write();

            testbench_bit_set(PIN_WRITE_DATA, HIGH);
            testbench_write();
            testbench_bit_set(PIN_WRITE_DATA, LOW);
            testbench_write();

            testbench_read();

            int read_back = 0;
            for (int i = 0; i < 8; i++)
                read_back |= testbench_bit_get(PIN_TEST[i]) << i;

            if (read_back != value)
            {
                result = false;
                break;
            }       
        }            
    }
    
    // configura o registro de direção da placa em teste para leitura
    if (result)
    {
        LCD_clear_line(1);
        LCD_print("reading...");

        for (int i = 0; i < 8; i++)
            testbench_bit_dir(PIN_DATA[i], OUTPUT); // data 0-7
        testbench_write();

        for (int i = 0; i < 8; i++)
            testbench_bit_set(PIN_DATA[i], HIGH);
        testbench_write();

        testbench_bit_set(PIN_WRITE_DIR, HIGH);
        testbench_write();
        testbench_bit_set(PIN_WRITE_DIR, LOW);
        testbench_write();

        for (int i = 0; i < 8; i++)
            testbench_bit_dir(PIN_TEST[i], OUTPUT);  // test 0-7

        for (int i = 0; i < 8; i++)
            testbench_bit_dir(PIN_DATA[i], INPUT); // data 0-7

        testbench_write();

        for (int value = 0; value < 256; value++)
        {
            if (ina_ok)
            {
                float current = INA.getCurrent() * 1000.0;
                if (current > amps_max)
                    amps_max = current;
            }

            // escreve zeros e uns
            for (int i = 0; i < 8; i++)
                testbench_bit_set(PIN_TEST[i], (value >> i) & 0x01);
            testbench_write();

            testbench_bit_set(PIN_INV_READ_ENABLE, LOW);
            testbench_write();

            testbench_read();

            testbench_bit_set(PIN_INV_READ_ENABLE, HIGH);
            testbench_write();

            int read_back = 0;
            for (int i = 0; i < 8; i++)
                read_back |= testbench_bit_get(PIN_DATA[i]) << i;

            if (read_back != value)
            {
                result = false;
                break;
            }
        }            
    }

    run_turn_off_power(true, false);

    if (result)
    {
        LCD_clear();
        LCD_print("TEST PASSED");
        LCD_setpos(1, 0);
        LCD_print(amps_max, 1, 1);
        LCD_print(" mA");
    }
    else    
    {
        LCD_clear_line(0);
        LCD_print("TEST FAILED");
        multiplex_write(MPU_ADDR_STATUS, MCU_STATE_ERROR);
    }

    while (console_read() == 0)
        delay(10);

    multiplex_write(MPU_ADDR_STATUS, 0x00);
}

void run_clock_cpa(void)
{
    LCD_noblink();
    LCD_clear();

    const uint8_t PIN_LED[] = {32, 33, 34, 36};
    const uint8_t PIN_CLOCK_IN[] = {25, 1, 26, 27};
    const uint8_t PIN_CLOCK_OUT[] = {6, 30, 5, 4};
    const uint8_t PIN_BREAK_IN = 3;
    const uint8_t PIN_BREAK_OUT = 28;
    const uint8_t PIN_CLOCK_STOPPED_LED = 37;
    const uint8_t PIN_STEP_OUT = 9;
    const uint8_t PIN_HALFSTEP_OUT = 8;
    const uint8_t PIN_PULSE_LED = 12;
    const uint8_t PIN_CLOCK_RESET = 10;

    LCD_clear_line(1);
    LCD_print("warm up");

    // liga a placa em teste
    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_POWER_C);

    testbench_init();

    for (int i = 0; i < 4; i++)
        testbench_bit_dir(PIN_LED[i], OUTPUT);
    for (int i = 0; i < 4; i++)
        testbench_bit_dir(PIN_CLOCK_IN[i], OUTPUT);
    for (int i = 0; i < 4; i++)
        testbench_bit_dir(PIN_CLOCK_OUT[i], INPUT);

    testbench_bit_dir(PIN_BREAK_IN, OUTPUT);
    testbench_bit_dir(PIN_BREAK_OUT, INPUT);
    testbench_bit_dir(PIN_CLOCK_STOPPED_LED, OUTPUT);
    testbench_bit_dir(PIN_STEP_OUT, INPUT);
    testbench_bit_dir(PIN_HALFSTEP_OUT, INPUT);
    testbench_bit_dir(PIN_PULSE_LED, OUTPUT);
    testbench_bit_dir(PIN_CLOCK_RESET, OUTPUT);

    testbench_write();

    // configura o registro de direção da placa em teste para escrita
    LCD_clear_line(1);
    LCD_print("testing...");

    // apaga todos os leds de seleção de clock
    for (int i = 0; i < 4; i++)
        testbench_bit_set(PIN_LED[i], LOW);

    testbench_bit_set(PIN_CLOCK_STOPPED_LED, LOW);

    // habilita os botões de seleção de clock
    for (int i = 0; i < 4; i++)
        testbench_bit_set(PIN_CLOCK_IN[i], HIGH);

    testbench_bit_set(PIN_BREAK_IN, LOW);
    testbench_bit_set(PIN_CLOCK_RESET, HIGH);

    testbench_write();

    // reseta os capacitores de debounce
    testbench_read();

    uint8_t button_step = 1;
    uint8_t button_half_step = 1;
    uint8_t led_half_step = 0;
    int selected_clock = -1;
    while (console_read() == 0)
    {
        int new_selected_clock = selected_clock;

        testbench_read();

        if (testbench_bit_get(PIN_BREAK_OUT) == 0)
        {
            new_selected_clock = -1;
        }
        else
        {
            for (int i = 0; i < 4; i++)
                if (testbench_bit_get(PIN_CLOCK_OUT[i]) != 0)
                    new_selected_clock = i;
        }

        if (new_selected_clock != selected_clock)
        {
            selected_clock = new_selected_clock;
            for (int i = 0; i < 4; i++)
                testbench_bit_set(PIN_LED[i], i == selected_clock);
            testbench_bit_set(PIN_CLOCK_STOPPED_LED, selected_clock != -1);
            testbench_bit_set(PIN_PULSE_LED, LOW);
            testbench_bit_set(PIN_CLOCK_RESET, LOW);
            testbench_write();
        }

        if (selected_clock == 0)
        {
            testbench_bit_set(PIN_PULSE_LED, (((long)(millis() / 500)) & 1));
            testbench_write();
        }

        if (selected_clock == -1)
        {
            if (testbench_bit_get(PIN_STEP_OUT) == 0)
            {
                button_step = 1 - button_step;
                led_half_step = button_step;
                testbench_bit_set(PIN_PULSE_LED, led_half_step);
                testbench_write();
            }

            if ((testbench_bit_get(PIN_HALFSTEP_OUT) ^ button_half_step) == 1)
            {
                button_half_step = 1 - button_half_step;
                if (button_half_step == 0)
                {
                    led_half_step = 1 - led_half_step;
                    testbench_bit_set(PIN_PULSE_LED, led_half_step);
                    testbench_write();
                }
            }
        }
    }

    run_turn_off_power(true, false);

    multiplex_write(MPU_ADDR_STATUS, 0x00);
}

void run_turn_off_power(bool run_led, bool error_led)
{
    multiplex_write(MPU_ADDR_STATUS, (run_led ? MCU_STATE_RUN : 0x00) | (error_led ? MCU_STATE_ERROR : 0x00));

    // coloca todos os canais da testbench em modo de leitura
    for (int iMultiplex = 0; iMultiplex < 6; iMultiplex++)
        multiplex_write(MPU_ADDR_DIR[iMultiplex], 0xFF);
}

void setup() 
{
    Serial.begin(115200);

    for (int iAddr = 0; iAddr < 4; iAddr++)
        pinMode(MPU_ADDR[iAddr], OUTPUT);

    pinMode(MPU_WRITE, OUTPUT);
    pinMode(MPU_READ, OUTPUT);

    digitalWrite(MPU_WRITE, HIGH);
    digitalWrite(MPU_READ, HIGH);

    // coloca todos os canais da testbench em modo de leitura
    run_turn_off_power(false, false);

    LCD_init();
    LCD_blink();
    LCD_clear();
    LCD_print("wait...");

    Wire.begin();
    ina_ok = internal_ammeter_init();

    internal_autotest();

    menu_show(&menu_main);
}

void loop() 
{
    int button = console_read();

    if (button & CONSOLE_LEFT)
      menu_move_left();

    else if (button & CONSOLE_RIGHT)
      menu_move_right();

    else if (button & CONSOLE_OK)
      menu_select();

    else if (button & CONSOLE_CANCEL)
      menu_back();
}
