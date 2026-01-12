#include "Arduino.h"
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
} Menu;

const int8_t MPU_DATA[] = {6, 7, A5, A4, A3, A2, A1, A0};
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

Menu *current_menu = NULL;

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

void LCD_print(char *str)
{
    while (*str)
    {
        LCD_write_char(*str);
        str++;
    }
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

Menu menu_main = 
{
    "TESTBENCH",
    menu_main_items,
    NULL,
    0
};

MenuItem menu_opts_items[] = 
{
    {"about", option_about},
    {"autotest", testbench_autotest},
    {NULL, NULL}
};

Menu menu_opts = 
{
    "OPTIONS",
    menu_opts_items,
    menu_opts_back,
    0
};

typedef struct
{
  char *name;
  void (*test_board_proc)(void);
} TestBoard;

TestBoard test_boards[] = 
{
    {"SR8A", NULL},
    {"SGRDA", NULL},
    {"MGRFA", NULL},
    {"SR16A", NULL},
    {"SWRDA", NULL},
    {"M16RFA", NULL},
    {"ULA8-ADDERA", NULL},
    {"ULA8-BITWISEA", NULL},
    {"ULA8-SHIFTERA", NULL},
    {"ULA8-SXA", NULL},
    {"ULA8-XORA", NULL},
    {"ULA8DA", NULL},
    {"TBSUB8", NULL},
    {NULL, NULL}
};

void menu_opts_back(void)
{
    menu_show(&menu_main);
}

void open_menu_opts()
{
    menu_opts.selected_index = 0;
    menu_show(&menu_opts);
}

void menu_mark_selected(Menu *menu)
{
    int col = 0;
    for (int i = 0; i < menu->selected_index; i++)
        col += strlen(menu->items[i].name) + 1;
    LCD_setpos(1, col);
}

void menu_show(Menu* menu)
{
    LCD_noblink();
    LCD_clear();
    LCD_setpos(0, 0);
    LCD_print(menu->title);
    LCD_setpos(1, 0);

    int menu_index = 0;
    while (menu->items[menu_index].name != NULL)
    {
        LCD_print(menu->items[menu_index].name);
        LCD_print(" ");
        menu_index++;
    }

    current_menu = menu;

    menu_mark_selected(menu);

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
                //selected_board = index;
                //break;
            }

            delay(10);
        }
    }

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

// Wire.begin();
// if (!INA.begin())
// {
//     LCD_print("INA226 begin error");
//     while (console_read() == 0)
//         delay(10);
//     return;
// }

// float shunt = 0.1;
// float current_LSB_mA = 0.1;
// float current_zero_offset_mA = 0;
// uint16_t bus_V_scaling_e4 = 9433;

// INA.configure(shunt, current_LSB_mA, current_zero_offset_mA, bus_V_scaling_e4);

// while (console_read() == 0)
// {
//     float current = INA.getCurrent();
//     float busVoltage = INA.getBusVoltage();

//     char temp[16];

//     LCD_setpos(0, 0);

//     dtostrf(current * 1000.0, 1, 1, temp);
//     LCD_print(temp);
//     LCD_print("mA ");

//     dtostrf(busVoltage, 1, 1, temp);
//     LCD_print(temp);
//     LCD_print("V ");

//     delay(500);
// }

void testbench_autotest(void)
{
    LCD_noblink();
    LCD_clear();

    LCD_print("AUTOTEST");
    LCD_setpos(1, 0);

    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_ERROR);
    delay(1000);

    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_POWER_B);
    delay(1000);

    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_POWER_A);
    delay(1000);

    multiplex_write(MPU_ADDR_STATUS, MCU_STATE_RUN | MCU_STATE_POWER_C);
    delay(1000);

    multiplex_write(MPU_ADDR_STATUS, 0x00);

    LCD_setpos(1, 0);
    LCD_print("done           ");

    while (console_read() == 0)
        delay(10);

    menu_show(current_menu);
}

void setup() 
{
    for (int iAddr = 0; iAddr < 4; iAddr++)
        pinMode(MPU_ADDR[iAddr], OUTPUT);

    pinMode(MPU_WRITE, OUTPUT);
    pinMode(MPU_READ, OUTPUT);

    digitalWrite(MPU_WRITE, HIGH);
    digitalWrite(MPU_READ, HIGH);

    multiplex_write(MPU_ADDR_STATUS, 0x00);

    LCD_init();
    LCD_blink();
    LCD_clear();
    
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
