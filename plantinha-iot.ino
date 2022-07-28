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

/*
..######..##..........###.....######...######..########..######.
.##....##.##.........##.##...##....##.##....##.##.......##....##
.##.......##........##...##..##.......##.......##.......##......
.##.......##.......##.....##..######...######..######....######.
.##.......##.......#########.......##.......##.##.............##
.##....##.##.......##.....##.##....##.##....##.##.......##....##
..######..########.##.....##..######...######..########..######.
*/
class Display {
    uint8_t backlightPin;
    Adafruit_PCD8544* display;
    void drawNumber(int number, int x, int y, bool zeroPadding); 
    void displayBatteryLevel(int level, char* batteryLevelString);   
public:
    Display();
    Display(uint8_t clk, uint8_t din, uint8_t dc, uint8_t ce, uint8_t rst, uint8_t backlight);
    void drawUI(int batteryLevel, char* batteryLevelString);
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
..######..########..########..####.########.########..######.
.##....##.##.....##.##.....##..##.....##....##.......##....##
.##.......##.....##.##.....##..##.....##....##.......##......
..######..########..########...##.....##....######....######.
.......##.##........##...##....##.....##....##.............##
.##....##.##........##....##...##.....##....##.......##....##
..######..##........##.....##.####....##....########..######.
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
.########..####..######..########..##..........###....##....##
.##.....##..##..##....##.##.....##.##.........##.##....##..##.
.##.....##..##..##.......##.....##.##........##...##....####..
.##.....##..##...######..########..##.......##.....##....##...
.##.....##..##........##.##........##.......#########....##...
.##.....##..##..##....##.##........##.......##.....##....##...
.########..####..######..##........########.##.....##....##...
*/

Display::Display() {
}

Display::Display(uint8_t clk, uint8_t din, uint8_t dc, uint8_t ce, uint8_t rst, uint8_t backlight) {
    display = new Adafruit_PCD8544(clk, din, dc, ce, rst);
    backlightPin = backlight;
}

void Display::drawUI(int batteryLevel, char* batteryLevelString) {
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
..######..########....###.....######..##....##
.##....##....##......##.##...##....##.##...##.
.##..........##.....##...##..##.......##..##..
..######.....##....##.....##.##.......#####...
.......##....##....#########.##.......##..##..
.##....##....##....##.....##.##....##.##...##.
..######.....##....##.....##..######..##....##
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
.########.##....##.##.....##.##.....##..######.
.##.......###...##.##.....##.###...###.##....##
.##.......####..##.##.....##.####.####.##......
.######...##.##.##.##.....##.##.###.##..######.
.##.......##..####.##.....##.##.....##.......##
.##.......##...###.##.....##.##.....##.##....##
.########.##....##..#######..##.....##..######.
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
    Moisture = LOW,
    BatteryLevel = HIGH
};

/*
.########..########.########.####.##....##.########..######.
.##.....##.##.......##........##..###...##.##.......##....##
.##.....##.##.......##........##..####..##.##.......##......
.##.....##.######...######....##..##.##.##.######....######.
.##.....##.##.......##........##..##..####.##.............##
.##.....##.##.......##........##..##...###.##.......##....##
.########..########.##.......####.##....##.########..######.
*/

/**
 * Pins setup
 */
#define ANALOG_PIN A0
#define ANALOG_SELECTOR_PIN D6

#define DISPLAY_CLK_PIN D0
#define DISPLAY_DIN_PIN D1
#define DISPLAY_DC_PIN D2
#define DISPLAY_CE_PIN D3
#define DISPLAY_RST_PIN D4
#define DISPLAY_BACKLIGHT_PIN D5

// General defines
#define BATTERY_HISTORY_SIZE 50

/*
..######...##........#######..########.....###....##..........##.....##....###....########...######.
.##....##..##.......##.....##.##.....##...##.##...##..........##.....##...##.##...##.....##.##....##
.##........##.......##.....##.##.....##..##...##..##..........##.....##..##...##..##.....##.##......
.##...####.##.......##.....##.########..##.....##.##..........##.....##.##.....##.########...######.
.##....##..##.......##.....##.##.....##.#########.##...........##...##..#########.##...##.........##
.##....##..##.......##.....##.##.....##.##.....##.##............##.##...##.....##.##....##..##....##
..######...########..#######..########..##.....##.########.......###....##.....##.##.....##..######.
*/
uint8_t activeAnalogInput = AnalogInput::BatteryLevel;
uint8_t batteryLevel = 0;
uint8_t batteryLevelInputHistory[50];
uint8_t currentHistoryIndex = 0;
bool shouldUpdateDisplay = false;
char batteryLevelString[10];

Display display(DISPLAY_CLK_PIN, DISPLAY_DIN_PIN, DISPLAY_DC_PIN, DISPLAY_CE_PIN, DISPLAY_RST_PIN, DISPLAY_BACKLIGHT_PIN);

/*
.########.##.....##.##....##..######..########.####..#######..##....##..######.
.##.......##.....##.###...##.##....##....##.....##..##.....##.###...##.##....##
.##.......##.....##.####..##.##..........##.....##..##.....##.####..##.##......
.######...##.....##.##.##.##.##..........##.....##..##.....##.##.##.##..######.
.##.......##.....##.##..####.##..........##.....##..##.....##.##..####.......##
.##.......##.....##.##...###.##....##....##.....##..##.....##.##...###.##....##
.##........#######..##....##..######.....##....####..#######..##....##..######.
*/
int readAnalogPin()
{
    return analogRead(ANALOG_PIN);
}

int getBatteryLevel()
{
    activeAnalogInput = AnalogInput::BatteryLevel;
    delay(50); // delay for 50ms to allow the ADC to settle
    int voltage = map(analogRead(ANALOG_PIN), 0.0, 1023, 0, 4200);
    Serial.printf("Battery Level: %.2fV\n", voltage / 1000.0);
    sprintf(batteryLevelString, "%.2fV", voltage / 1000.0);
    Serial.flush();
    int level = map(voltage, 2700, 4200, 0, 100); // operationally, the battery voltage is between 2.7 and 4.2 V
    Serial.printf("Battery Level: %d%%\n", level);
    return level;
}

MoistureState getMoistureLevel()
{
    activeAnalogInput = AnalogInput::Moisture;
    delay(50); // delay for 50ms to allow the ADC to settle
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

/*
..######..########.########.##.....##.########.
.##....##.##..........##....##.....##.##.....##
.##.......##..........##....##.....##.##.....##
..######..######......##....##.....##.########.
.......##.##..........##....##.....##.##.......
.##....##.##..........##....##.....##.##.......
..######..########....##.....#######..##.......
*/
void setupGeneralPins()
{
    pinMode(ANALOG_PIN, INPUT);
    pinMode(ANALOG_SELECTOR_PIN, OUTPUT);
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

void loop()
{

    int batteryLevel = getBatteryLevel();
    if(batteryLevel < 0) {
        batteryLevel = 0;
    }
    display.drawUI(batteryLevel, batteryLevelString);
    delay(1000);
}
