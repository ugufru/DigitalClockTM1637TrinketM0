
#include <Wire.h>
#include <Adafruit_DotStar.h>
#include <TM1637Display.h>
#include <TimeLib.h>
#include "RTClib.h"

#define DOTSTAR_DATA_PIN 7
#define DOTSTAR_CLOCK_PIN 8
#define DOTSTAR_BRIGHTNESS 32
#define TM1637_CLK 3
#define TM1637_DIO 4

Adafruit_DotStar dotstar = Adafruit_DotStar(1, DOTSTAR_DATA_PIN, DOTSTAR_CLOCK_PIN, DOTSTAR_BGR);
TM1637Display tm1637(TM1637_CLK, TM1637_DIO);
RTC_PCF8523 rtc;

void flashDotStar(uint32_t color, int count)
{
  for (int x = 0; x < count; x++)
  {
    dotstar.setPixelColor(0, color);
    dotstar.show();
    delay(50);

    dotstar.setPixelColor(0, 0);
    dotstar.show();
    delay(50);
  }
}

uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85)
  {
    return dotstar.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return dotstar.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return dotstar.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void error(int pos)
{
  flashDotStar(Wheel(pos), 5);
  abort();
}

void initSerial()
{
  Serial.begin(9600);
  delay(2000); // Wait 2 seconds to establish thes connection before printing.
}

void init7SegmentDisplay()
{
  tm1637.clear();
  tm1637.setBrightness(0x01);
  Serial.println("TM1637 quad 7 segment LED initialized.");
}

void initDotStar()
{
  dotstar.begin();
  dotstar.show();
  dotstar.setBrightness(DOTSTAR_BRIGHTNESS);
  Serial.println("Trinket DotStar initialized.");
}

void initRTC()
{
  if (rtc.begin() == false) error(80);
  if (rtc.initialized()) Serial.print("PCF8523 RTC initialized.");
  if (rtc.lostPower()) Serial.print("PCF8523 RTC lost power.");

  if (! rtc.initialized() || rtc.lostPower())
  {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // When the RTC was stopped and stays connected to the battery, it has
  // to be restarted by clearing the STOP bit. Let's do this to ensure
  // the RTC is running.
  rtc.start();

  Serial.println("RTC is running.");
}

void setup()
{
  initSerial();
  init7SegmentDisplay();
  initDotStar();
  initRTC();
}


uint8_t previousState = -1;

void loop()
{
  DateTime now = rtc.now();

  uint8_t state = now.second() % 4;

  if (state != previousState)
  {
    Serial.println(now.timestamp());

    switch (state)
    {
      case 0:
        displayTime(now);
        break;
      case 1:
        displaySeparator(now);
        break;
      case 2:
      case 3:
        displayDate(now);
        break;
    }

    uint32_t color = Wheel(now.second() * 4.2);
    color = dotstar.gamma32(color);
    dotstar.setPixelColor(0, color);
    dotstar.show();

    previousState = state;
  }

  delay(100);
}


void displayTime(DateTime now)
{
  tm1637.showNumberDecEx(now.twelveHour(), 0x40, false, 2, 0);
  tm1637.showNumberDec(now.minute(), true, 2, 2);
}

void displaySeparator(DateTime now)
{
  tm1637.showNumberDec(now.twelveHour(), false, 2, 0);
  tm1637.showNumberDec(now.minute(), true, 2, 2);
}

void displayDate(DateTime now)
{
  tm1637.showNumberDec(now.month(), false, 2, 0);
  tm1637.showNumberDec(now.day(), false, 2, 2);
}
