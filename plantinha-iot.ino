/**
 * @file plantinha-iot.ino
 * @author Leonardo "Leu" Pereira <github.com/jlcvp>
 * @brief Gerenciador de saúde para plantinhas IoT
 * @version 0.1
 * @date 2022-07-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

extern uint8_t batteryLevel;
extern char batteryLevelString[10];

/*


   ____   ___
  6MMMMb/ `MM
 8P    YM  MM
6M      Y  MM    ___     ____     ____     ____     ____
MM         MM  6MMMMb   6MMMMb\  6MMMMb\  6MMMMb   6MMMMb\
MM         MM 8M'  `Mb MM'    ` MM'    ` 6M'  `Mb MM'    `
MM         MM     ,oMM YM.      YM.      MM    MM YM.
MM         MM ,6MM9'MM  YMMMMb   YMMMMb  MMMMMMMM  YMMMMb
YM      6  MM MM'   MM      `Mb      `Mb MM            `Mb
 8b    d9  MM MM.  ,MM L    ,MM L    ,MM YM    d9 L    ,MM
  YMMMM9  _MM_`YMMM9'YbMYMMMM9  MYMMMM9   YMMMM9  MYMMMM9



*/

/*
oooo                                    oooo
 888ooooo   ooooooooo8  ooooooo    ooooo888  ooooooooo8 oo oooooo    oooooooo8
 888   888 888oooooo8   ooooo888 888    888 888oooooo8   888    888 888ooooooo
 888   888 888        888    888 888    888 888          888                888
o888o o888o  88oooo888 88ooo88 8o  88ooo888o  88oooo888 o888o       88oooooo88

*/
class Display {
    uint8_t backlightPin;
    Adafruit_PCD8544* display;
    void drawNumber(int number, int x, int y, bool zeroPadding); 
    void displayBatteryLevel(int level, char* batteryLevelString);   
public:
    Display();
    Display(uint8_t clk, uint8_t din, uint8_t dc, uint8_t ce, uint8_t rst, uint8_t backlight);
    void drawUI();
    void setBacklight(bool on);
    void setup();
};

class Stack
{
    int top;
    int size;
    int *array;

public:
    Stack(int size);
    ~Stack();
    void push(int value);
    int pop();
    bool isEmpty();
    bool isFull();
};

/*
 oooooooo8                       o88    o8
888        ooooooooo  oo oooooo  oooo o888oo ooooooooo8  oooooooo8
 888oooooo  888    888 888    888 888  888  888oooooo8  888ooooooo
        888 888    888 888        888  888  888                 888
o88oooo888  888ooo88  o888o      o888o  888o  88oooo888 88oooooo88
           o888
*/

const uint8_t batterySprite_Width = 16;
const uint8_t batterySprite_Height = 9;
PROGMEM const unsigned char batterySprite[] = {
    0x7f, 0xfc, 0x80, 0x02, 0x80, 0x02, 0x80, 0x01, 
    0x80, 0x01, 0x80, 0x01, 0x80, 0x02, 0x80, 0x02, 
    0x7f, 0xfc
};

const uint8_t digit_Width = 3;
const uint8_t digit_Height = 5;
PROGMEM const unsigned char digitSprite[10][5] = {
    { 0xe0, 0xa0, 0xa0, 0xa0, 0xe0 }, // 0
    { 0x40, 0xc0, 0x40, 0x40, 0xe0 }, // 1
    { 0xe0, 0x20, 0xe0, 0x80, 0xe0 }, // 2
    { 0xe0, 0x20, 0x60, 0x20, 0xe0 }, // 3
    { 0xa0, 0xa0, 0xe0, 0x20, 0x20 }, // 4
    { 0xe0, 0x80, 0xe0, 0x20, 0xe0 }, // 5
    { 0xe0, 0x80, 0xe0, 0xa0, 0xe0 }, // 6
    { 0xe0, 0x20, 0x40, 0x40, 0x40 }, // 7
    { 0xe0, 0xa0, 0xe0, 0xa0, 0xe0 }, // 8
    { 0xe0, 0xa0, 0xe0, 0x20, 0xe0 }  // 9
};

const uint8_t lightning_Width = 8;
const uint8_t lightning_Height = 9;
PROGMEM const unsigned char lightningSprite[] = {
    0x3c, 0x38, 0x70, 0x7f, 0xfe, 0x0c, 0x18, 0x10, 0x20
};



/*
ooooooooo   o88                         o888
 888    88o oooo   oooooooo8 ooooooooo   888   ooooooo oooo   oooo
 888    888  888  888ooooooo  888    888 888   ooooo888 888   888
 888    888  888          888 888    888 888 888    888  888 888
o888ooo88   o888o 88oooooo88  888ooo88  o888o 88ooo88 8o   8888
                             o888                       o8o888
*/

Display::Display() {
}

Display::Display(uint8_t clk, uint8_t din, uint8_t dc, uint8_t ce, uint8_t rst, uint8_t backlight) {
    display = new Adafruit_PCD8544(clk, din, dc, ce, rst);
    backlightPin = backlight;
}

void Display::drawUI() {
    display->clearDisplay();
    displayBatteryLevel(batteryLevel, batteryLevelString);
    
    // get runtime
    unsigned long runtime = millis() / 1000;
    int hours = runtime / 3600;
    int minutes = (runtime % 3600) / 60;
    int seconds = runtime % 60;

    // draw time
    display->setCursor(5, 40);
    display->printf("%02d", hours);
    display->print(":");
    display->printf("%02d",minutes);
    display->print(":");
    display->printf("%02d",seconds);
    Serial.printf("Runtime: %d:%d:%d\n", hours, minutes, seconds);
    display->display();
}

void Display::setBacklight(bool on) {
    digitalWrite(backlightPin, on ? HIGH : LOW);
}

void Display::setup() {
    display->begin();
}

void Display::drawNumber(int number, int x, int y, bool zeroPadding)
{
    if((number < 0) || (number > 99999999)) {
        display->print("*ERR*");
        return;
    }

    Stack stack = Stack(8);

    do {
        stack.push(number % 10);
        number /= 10;
    } while(number > 0);

    while(!stack.isEmpty()) {
        display->drawBitmap(x, y, digitSprite[stack.pop()], digit_Width, digit_Height, 1);
        x += digit_Width + 1;
    }
}

void Display::displayBatteryLevel(int level, char* batteryLevelString)
{
    display->drawBitmap(10, 10, batterySprite, batterySprite_Width, batterySprite_Height, 1);

    drawNumber(level, 12, 12, false);
    
    display->setCursor(10, 20);
    display->print(batteryLevelString);
}
/*
 oooooooo8   o8                        oooo
888        o888oo  ooooooo    ooooooo   888  ooooo
 888oooooo  888    ooooo888 888     888 888o888
        888 888  888    888 888         8888 88o
o88oooo888   888o 88ooo88 8o  88ooo888 o888o o888o

*/
Stack::Stack(int size) {
    top = -1;
    this->size = size;
    this->array = (int*)malloc(size * sizeof(int));
}

Stack::~Stack() {
    free(array);
}

void Stack::push(int value) {
    if (top == size - 1) {
        Serial.println("Stack is full");
        return;
    }
    top++;
    array[top] = value;
}

int Stack::pop() {
    if (top == -1) {
        Serial.println("Stack is empty");
        return -1;
    }
    int value = array[top];
    top--;
    return value;
}

bool Stack::isEmpty() {
    return top == -1;
}

bool Stack::isFull() {
    return top == size - 1;
}

/*
  ____  ___  __  ___   ___ ___  __    __     ____
 6MMMMb `MM 6MMb `MM    MM `MM 6MMb  6MMb   6MMMMb\
6M'  `Mb MMM9 `Mb MM    MM  MM69 `MM69 `Mb MM'    `
MM    MM MM'   MM MM    MM  MM'   MM'   MM YM.
MMMMMMMM MM    MM MM    MM  MM    MM    MM  YMMMMb
MM       MM    MM MM    MM  MM    MM    MM      `Mb
YM    d9 MM    MM YM.   MM  MM    MM    MM L    ,MM
 YMMMM9 _MM_  _MM_ YMMM9MM__MM_  _MM_  _MM_MYMMMM9
*/

enum MoistureState
{
    Dry,
    Humid,
    Wet,
    Soaked
};

enum AnalogInput
{
    Moisture = 0x4,
    BatteryLevel = 0x7
};

enum AppState
{
    Startup,
    Sleep,
    SensorReadings,
    DisplayOutput
};

/*
________              __
`MMMMMMMb.           69M68b
 MM    `Mb          6M' Y89
 MM     MM   ____  _MM_____ ___  __     ____     ____
 MM     MM  6MMMMb MMMMM`MM `MM 6MMb   6MMMMb   6MMMMb\
 MM     MM 6M'  `Mb MM   MM  MMM9 `Mb 6M'  `Mb MM'    `
 MM     MM MM    MM MM   MM  MM'   MM MM    MM YM.
 MM     MM MMMMMMMM MM   MM  MM    MM MMMMMMMM  YMMMMb
 MM     MM MM       MM   MM  MM    MM MM            `Mb
 MM    .M9 YM    d9 MM   MM  MM    MM YM    d9 L    ,MM
_MMMMMMM9'  YMMMM9 _MM_ _MM__MM_  _MM_ YMMMM9  MYMMMM9
*/

/**
 * /*
 * oooooooooo  o88
 *  888    888 oooo  oo oooooo    oooooooo8
 *  888oooo88   888   888   888  888ooooooo
 *  888         888   888   888          888
 * o888o       o888o o888o o888o 88oooooo88
 * */
#define ANALOG_PIN A0
#define ANALOG_MUX_A D7
#define ANALOG_MUX_B D8

#define DISPLAY_DIN_PIN D1
#define DISPLAY_DC_PIN D2
#define DISPLAY_CE_PIN D3
#define DISPLAY_RST_PIN D4
#define DISPLAY_BACKLIGHT_PIN D5
#define DISPLAY_CLK_PIN D6


// /*
//   ooooooo8                                                           o888
// o888    88   ooooooooo8 oo oooooo   ooooooooo8 oo oooooo   ooooooo    888
// 888    oooo 888oooooo8   888   888 888oooooo8   888    888 ooooo888   888
// 888o    88  888          888   888 888          888      888    888   888
//  888ooo888    88oooo888 o888o o888o  88oooo888 o888o      88ooo88 8o o888o

// */
#define BATTERY_HISTORY_SIZE 50
#define MICROSSECONDS 1000000
#define LOOPSLEEP 200
#define MAX_WAKE_TIME_SECONDS 300
/*
____     ___
`Mb(     )d'
 YM.     ,P
 `Mb     d'   ___   ___  __   ____
  YM.   ,P  6MMMMb  `MM 6MM  6MMMMb\
  `Mb   d' 8M'  `Mb  MM69 " MM'    `
   YM. ,P      ,oMM  MM'    YM.
   `Mb d'  ,6MM9'MM  MM      YMMMMb
    YM,P   MM'   MM  MM          `Mb
    `MM'   MM.  ,MM  MM     L    ,MM
     YP    `YMMM9'Yb_MM_    MYMMMM9
*/

/*
  ooooooo8 o888            oooo                    o888
o888    88  888   ooooooo   888ooooo     ooooooo    888       oooo   oooo ooooooo   oo oooooo    oooooooo8
888    oooo 888 888     888 888    888   ooooo888   888        888   888  ooooo888   888    888 888ooooooo
888o    88  888 888     888 888    888 888    888   888         888 888 888    888   888                888
 888ooo888 o888o  88ooo88  o888ooo88    88ooo88 8o o888o          888    88ooo88 8o o888o       88oooooo88

*/

uint8_t batteryLevel = 0;
uint8_t batteryLevelInputHistory[BATTERY_HISTORY_SIZE];

MoistureState moistureLevel = MoistureState::Dry;

uint8_t currentHistoryIndex = 0;
uint64_t lastDisplayUpdate = 0;
uint64_t lastWakeUp = 0;
bool shouldUpdateDisplay = false;
char batteryLevelString[10];

Display display(DISPLAY_CLK_PIN, DISPLAY_DIN_PIN, DISPLAY_DC_PIN, DISPLAY_CE_PIN, DISPLAY_RST_PIN, DISPLAY_BACKLIGHT_PIN);
AppState appState = AppState::Startup;
/*
________
`MMMMMMM                                   68b
 MM    \                             /     Y89
 MM    ___   ___ ___  __     ____   /M     ___   _____  ___  __     ____
 MM   ,`MM    MM `MM 6MMb   6MMMMb./MMMMM  `MM  6MMMMMb `MM 6MMb   6MMMMb\
 MMMMMM MM    MM  MMM9 `Mb 6M'   Mb MM      MM 6M'   `Mb MMM9 `Mb MM'    `
 MM   ` MM    MM  MM'   MM MM    `' MM      MM MM     MM MM'   MM YM.
 MM     MM    MM  MM    MM MM       MM      MM MM     MM MM    MM  YMMMMb
 MM     MM    MM  MM    MM MM       MM      MM MM     MM MM    MM      `Mb
 MM     YM.   MM  MM    MM YM.   d9 YM.  ,  MM YM.   ,M9 MM    MM L    ,MM
_MM_     YMMM9MM__MM_  _MM_ YMMMM9   YMMM9 _MM_ YMMMMM9 _MM_  _MM_MYMMMM9

*/

void setAnalogInput(AnalogInput input) {
    uint8_t aOut = input & 0b001;
    uint8_t bOut = input & 0b010;
    // uint8_t cOut = input & 0b100; // Not used atm
    digitalWrite(ANALOG_MUX_A, aOut);
    digitalWrite(ANALOG_MUX_B, bOut);

    delay(50); // delay for 50ms to allow the ADC to settle
}

int readAnalogPin()
{
    return analogRead(ANALOG_PIN);
}

int getBatteryLevel()
{
    setAnalogInput(AnalogInput::BatteryLevel);
    int voltage = map(readAnalogPin(), 0.0, 1023, 0, 4200);
    Serial.printf("Battery Level: %.2fV\n", voltage / 1000.0);
    sprintf(batteryLevelString, "%.2fV", voltage / 1000.0);
    Serial.flush();
    int level = map(voltage, 2700, 4200, 0, 100); // operationally, the battery voltage is between 2.7 and 4.2 V
    Serial.printf("Battery Level: %d%%\n", level);
    return level;
}

MoistureState getMoistureLevel()
{
    setAnalogInput(AnalogInput::Moisture);
    int value = map(analogRead(ANALOG_PIN), 0.0, 1023, 0, 100);

    if (value < 20)
    {
        return MoistureState::Dry;
    }
    else if (value < 40)
    {
        return MoistureState::Humid;
    }
    else if (value < 60)
    {
        return MoistureState::Wet;
    }
    else
    {
        return MoistureState::Soaked;
    }
}

void updateBatteryLevel()
{
    batteryLevelInputHistory[currentHistoryIndex] = getBatteryLevel();
    currentHistoryIndex = (currentHistoryIndex + 1) % BATTERY_HISTORY_SIZE;

    //get the average of the last 50 readings
    int sum = 0;
    for (int i = 0; i < BATTERY_HISTORY_SIZE; i++)
    {
        sum += batteryLevelInputHistory[i];
    }
    int average = sum / BATTERY_HISTORY_SIZE;
    if (batteryLevel != average)
    {
        batteryLevel = average;
        shouldUpdateDisplay = true;
    }
}

void updateMoistureLevel()
{
    MoistureState moistureState = getMoistureLevel();
    if (moistureState != moistureLevel)
    {
        moistureLevel = moistureState;
        shouldUpdateDisplay = true;
    }
}

void updateSensors()
{
    updateBatteryLevel();
    updateMoistureLevel();
}

/*
  ____
 6MMMMb\
6M'    `           /
MM         ____   /M    ___   ___ __ ____
YM.       6MMMMb /MMMMM `MM    MM `M6MMMMb
 YMMMMb  6M'  `Mb MM     MM    MM  MM'  `Mb
     `Mb MM    MM MM     MM    MM  MM    MM
      MM MMMMMMMM MM     MM    MM  MM    MM
      MM MM       MM     MM    MM  MM    MM
L    ,M9 YM    d9 YM.  , YM.   MM  MM.  ,M9
MYMMMM9   YMMMM9   YMMM9  YMMM9MM_ MMYMMM9
                                   MM
                                   MM
                                  _MM_
*/

void setupGeneralPins()
{
    pinMode(ANALOG_PIN, INPUT);
    pinMode(ANALOG_MUX_A, OUTPUT);
    pinMode(ANALOG_MUX_B, OUTPUT);
}

void setupDisplay() {
    pinMode(DISPLAY_CLK_PIN, OUTPUT);
    pinMode(DISPLAY_DIN_PIN, OUTPUT);
    pinMode(DISPLAY_DC_PIN, OUTPUT);
    pinMode(DISPLAY_CE_PIN, OUTPUT);
    pinMode(DISPLAY_RST_PIN, OUTPUT);
    pinMode(DISPLAY_BACKLIGHT_PIN, OUTPUT);
    display.setup();
}

void setup()
{
    Serial.begin(9600);
    setupGeneralPins();
    setupDisplay();
}

/*
___       ___                             ___
`MMb     dMM'         68b                 `MM
 MMM.   ,PMM          Y89                  MM
 M`Mb   d'MM    ___   ___ ___  __          MM   _____     _____  __ ____
 M YM. ,P MM  6MMMMb  `MM `MM 6MMb         MM  6MMMMMb   6MMMMMb `M6MMMMb
 M `Mb d' MM 8M'  `Mb  MM  MMM9 `Mb        MM 6M'   `Mb 6M'   `Mb MM'  `Mb
 M  YM.P  MM     ,oMM  MM  MM'   MM        MM MM     MM MM     MM MM    MM
 M  `Mb'  MM ,6MM9'MM  MM  MM    MM        MM MM     MM MM     MM MM    MM
 M   YP   MM MM'   MM  MM  MM    MM        MM MM     MM MM     MM MM    MM
 M   `'   MM MM.  ,MM  MM  MM    MM        MM YM.   ,M9 YM.   ,M9 MM.  ,M9
_M_      _MM_`YMMM9'Yb_MM__MM_  _MM_      _MM_ YMMMMM9   YMMMMM9  MMYMMM9
                                                                  MM
                                                                  MM
                                                                 _MM_
*/

void loop()
{
    switch(appState) {
        case AppState::Startup:
            // initial readings and UI
            updateSensors();
            appState = AppState::DisplayOutput;
        break;
        case AppState::Sleep:
            appState = AppState::SensorReadings;
            ESP.deepSleep(10 * MICROSSECONDS);
        break;
        case AppState::SensorReadings:
            updateSensors();
            appState = AppState::DisplayOutput;
        break;
        case AppState::DisplayOutput:
            if(shouldUpdateDisplay) {   
                display.drawUI();
            }

            if((millis() - lastWakeUp)/1000 > MAX_WAKE_TIME_SECONDS) {
                appState = AppState::Sleep;
            } else {
                appState = AppState::SensorReadings;
            }
        break;
    }

    // int batteryLevel = getBatteryLevel();
    // if(batteryLevel < 0) {
    //     batteryLevel = 0;
    // }
    // display.drawUI(batteryLevel, batteryLevelString);
    delay(LOOPSLEEP);
}
