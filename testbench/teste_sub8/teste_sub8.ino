const int PIN_DATA[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
const int PIN_TEST[] = { A0, A1, A2, A3, A4, A5, 13, 12};

const int PIN_WRITE_DATA = 10;
const int PIN_WRITE_DIR = 11;
const int PIN_READ_ENABLE = A6;

void set_data_read() 
{
    for (int iPin = 0; iPin < 8; iPin++)
        pinMode(PIN_DATA[iPin], INPUT);
}

void set_data_write() 
{
    for (int iPin = 0; iPin < 8; iPin++)
        pinMode(PIN_DATA[iPin], OUTPUT);
}

void set_test_read() 
{
    for (int iPin = 0; iPin < 8; iPin++)
        pinMode(PIN_TEST[iPin], INPUT);
}

void set_test_write() 
{
    for (int iPin = 0; iPin < 8; iPin++)
      pinMode(PIN_TEST[iPin], OUTPUT);
}

void set_test_dir(int dir)
{
    set_data_write();

    for (int iBit = 0; iBit < 8; iBit++)
        digitalWrite(PIN_DATA[iBit], (dir >> iBit) & 1);

    digitalWrite(PIN_WRITE_DIR, HIGH);
    delay(1);
    digitalWrite(PIN_WRITE_DIR, LOW);
    delay(1);
}

void write_data(int data)
{
    set_data_write();

    for (int iBit = 0; iBit < 8; iBit++)
        digitalWrite(PIN_DATA[iBit], (data >> iBit) & 1);

    digitalWrite(PIN_WRITE_DATA, HIGH);
    delay(1);
    digitalWrite(PIN_WRITE_DATA, LOW);
    delay(1);
}

int read_data()
{
    set_data_read();

    int data = 0;
    for (int iBit = 0; iBit < 8; iBit++)
        data = data | (digitalRead(PIN_DATA[iBit]) ? (1 << iBit) : 0);

    return data;
}

void write_test(int test)
{
    set_test_write();

    for (int iBit = 0; iBit < 8; iBit++)
        digitalWrite(PIN_TEST[iBit], (test >> iBit) & 1);
}

int read_test()
{
    set_test_read();

    int data = 0;
    for (int iBit = 0; iBit < 8; iBit++)
        data = data | (digitalRead(PIN_TEST[iBit]) ? (1 << iBit) : 0);

    return data;
}

void setup() 
{
    set_data_read();
    set_test_read();

    pinMode(PIN_WRITE_DATA, OUTPUT);
    pinMode(PIN_WRITE_DIR, OUTPUT);
    pinMode(PIN_READ_ENABLE, OUTPUT);

    digitalWrite(PIN_WRITE_DATA, LOW);
    digitalWrite(PIN_WRITE_DIR, LOW);
    digitalWrite(PIN_READ_ENABLE, HIGH);

    Serial.begin(115200, SERIAL_8N1);

    Serial.println("testbench.sub8");

    int test;

    // coloca todos os bits como output
    set_test_dir(0x00);

    for (int test_data = 0; test_data < 256; )
    {
        write_data(test_data);
        test = read_test();
        Serial.print(test_data, HEX);
        Serial.print(" ");
        Serial.print(test, HEX);
        Serial.print(" ");
        Serial.println(test == test_data ? "OK" : "ERR");

        //test_data = test_data * 2
        //if (test_data == 0)
        //    test_data = 1;
        test_data++;
    }    
}

void loop() 
{
}
