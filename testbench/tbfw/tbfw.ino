#include <LiquidCrystal.h>

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

const int lcd_rs = 7;
const int lcd_en = 8;
const int lcd_d4 = 9;
const int lcd_d5 = 10;
const int lcd_d6 = 11;
const int lcd_d7 = 12;

const int bt_left = A4;
const int bt_cancel = A3;
const int bt_ok = A2;
const int bt_right = A1;

Menu *current_menu = NULL;

LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

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
    lcd.setCursor(col, 1);
}

void menu_show(Menu* menu)
{
    lcd.noBlink();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(menu->title);
    lcd.setCursor(0, 1);

    int menu_index = 0;
    while (menu->items[menu_index].name != NULL)
    {
        lcd.print(menu->items[menu_index].name);
        lcd.print(" ");
        menu_index++;
    }

    current_menu = menu;

    menu_mark_selected(menu);

    lcd.blink();
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
    int button = 0;

    if (!digitalRead(bt_left))
        button |= CONSOLE_LEFT;
    if (!digitalRead(bt_right))
        button |= CONSOLE_RIGHT;
    if (!digitalRead(bt_ok))
        button |= CONSOLE_OK;
    if (!digitalRead(bt_cancel))
        button |= CONSOLE_CANCEL;

    int pressed = button & (~last_button);

    last_button = button;

    return pressed;
}

void option_about(void)
{
    lcd.noBlink();
    lcd.clear();
    lcd.print("Testbench v1.0.0");
    lcd.setCursor(0, 1);
    lcd.print("Capaverde 2025");

    while (console_read() == 0)
        delay(10);

    menu_show(current_menu);
}

void testbench_select_board(void)
{
    static char *clear_line = "                ";

    lcd.noBlink();
    lcd.clear();
    lcd.print("Select:");

    int selected_board = -1;

    int count = 0;
    while (test_boards[count].name != NULL)
        count++;

    int index = 0;

    while (selected_board == -1)
    {
        lcd.setCursor(0, 1);
        lcd.print(clear_line);
        lcd.setCursor(0, 1);
        lcd.print(test_boards[index].name);

        char str_count[6];
        itoa(count, str_count, 10);
        char str_index[6];
        itoa(index + 1, str_index, 10);

        lcd.setCursor(15 - strlen(str_count) - strlen(str_index), 0);
        lcd.print(str_index);
        lcd.print("/");
        lcd.print(str_count);

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
    lcd.noBlink();
    lcd.clear();
    lcd.print("not implemented");

    while (console_read() == 0)
        delay(10);

    menu_show(current_menu);
}

void testbench_autotest(void)
{
    lcd.noBlink();
    lcd.clear();
    lcd.print("not implemented");

    while (console_read() == 0)
        delay(10);

    menu_show(current_menu);
}

void setup() 
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.noBlink();
    lcd.clear();

    pinMode(bt_left, INPUT);
    pinMode(bt_cancel, INPUT);
    pinMode(bt_ok, INPUT);
    pinMode(bt_right, INPUT);

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
