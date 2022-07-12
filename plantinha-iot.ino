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
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

/**
 * PINS DEFINITION
 */
#define ANALOG_PIN A0
#define DISPLAY_CLK_PIN D0
#define DISPLAY_DIN_PIN D1
#define DISPLAY_DC_PIN D2
#define DISPLAY_CE_PIN D3
#define DISPLAY_RST_PIN D4
#define DISPLAY_BACKLIGHT_PIN D5

/**
 * ENUMS
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

/**
 * GFX Sprites
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
    { 0xe0, 0xa0, 0xe0, 0x80, 0x60 }  // 9
};

const uint8_t lightning_Width = 8;
const uint8_t lightning_Height = 9;
PROGMEM const unsigned char lightningSprite[] = {
    0x3c, 0x38, 0x70, 0x7f, 0xfe, 0x0c, 0x18, 0x10, 0x20
};

/** 
 * VARIABLES
 */
unsigned char activeAnalogInput = AnalogInput::BatteryLevel;

Adafruit_PCD8544 display = Adafruit_PCD8544(DISPLAY_CLK_PIN, DISPLAY_DIN_PIN, DISPLAY_DC_PIN, DISPLAY_CE_PIN, DISPLAY_RST_PIN);


int readAnalogPin()
{
    return analogRead(ANALOG_PIN);
}

int getBatteryLevel()
{
    activeAnalogInput = AnalogInput::BatteryLevel;
    delay(25); // delay for 25ms to allow the ADC to settle
    int voltage = map(analogRead(ANALOG_PIN), 0.0, 1023, 0, 4200);
    Serial.printf("Battery Level: %.2fV\n", voltage / 1000.0);
    Serial.flush();
    int level = map(voltage, 2700, 4200, 0, 100) / 100; // operationally, the battery voltage is between 2.7 and 4.2 V
    return level;
}

MoistureState getMoistureLevel()
{
    activeAnalogInput = AnalogInput::Moisture;
    delay(25); // delay for 25ms to allow the ADC to settle
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

void setupPins()
{
    pinMode(ANALOG_PIN, INPUT);
    pinMode(DISPLAY_CLK_PIN, OUTPUT);
    pinMode(DISPLAY_DIN_PIN, OUTPUT);
    pinMode(DISPLAY_DC_PIN, OUTPUT);
    pinMode(DISPLAY_CE_PIN, OUTPUT);
    pinMode(DISPLAY_RST_PIN, OUTPUT);
    pinMode(DISPLAY_BACKLIGHT_PIN, OUTPUT);
}

void setupDisplay() {
    display.begin();
    
}

void setup()
{
    Serial.begin(115200);
    setupPins();
    // setupDisplay(); // display.ino
}

int soilMoisture = 0;

void loop()
{
    if (soilMoisture > 500)
    {
        digitalWrite(LED_BUILTIN_AUX, HIGH);
    }
    else
    {
        digitalWrite(LED_BUILTIN_AUX, LOW);
    }

    delay(200);
}
