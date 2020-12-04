
#include <Wire.h>
#include <Adafruit_DotStar.h>
#include <TM1637Display.h>
#include <TimeLib.h>
#include "RTClib.h"

#define TM1637_CLK 3//pins definitions for TM1637 and can be changed to other ports
#define TM1637_DIO 4

#define DOTSTAR_DATA_PIN 7
#define DOTSTAR_CLOCK_PIN 8
#define DOTSTAR_BRIGHTNESS 32

TM1637Display tm1637(TM1637_CLK, TM1637_DIO);
Adafruit_DotStar dotstar = Adafruit_DotStar(1, DOTSTAR_DATA_PIN, DOTSTAR_CLOCK_PIN, DOTSTAR_BGR);
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
}

void init7SegmentDisplay()
{
  tm1637.clear();
  tm1637.setBrightness(0x04);
}

void initDotStar()
{
  dotstar.begin();
  dotstar.show();
  dotstar.setBrightness(DOTSTAR_BRIGHTNESS);
}

void initRTC()
{
  if (rtc.begin() == false) error(80);

  Serial.println("Something, something...");
  Serial.print("rtc.initialized() = ");
  Serial.println(rtc.initialized());
  Serial.print("rtc.lostPower() = ");
  Serial.println(rtc.lostPower());

  if (! rtc.initialized() || rtc.lostPower())
  {
    //   Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
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
}

void setup()
{
  initSerial();
  init7SegmentDisplay();
  initDotStar();
  initRTC();
}

int counter = 0;

void loop()
{
  counter = (counter + 1) % 2000000;

  if (counter == 0) displayTime();
  if (counter == 1000000) displayDate();
  if (counter == 500000) displaySeparator();
}


int time = 0;

void displayTime()
{
  DateTime now = rtc.now();
  time = now.hour() * 100 + now.minute();

  Serial.print("Time: ");
  Serial.println(time);

  tm1637.showNumberDecEx(time, 0x40, false);
}

void displaySeparator()
{
  tm1637.showNumberDec(time, false);
}


void displayDate()
{
  DateTime now = rtc.now();
  int date = now.month() * 100 + now.day();

  Serial.print("Date: ");
  Serial.println(date);

  tm1637.showNumberDec(date, false);
}
