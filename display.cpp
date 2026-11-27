#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "display.h"

static Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void oledBegin() {
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED FAIL"));
    return;
  }

  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.display();
}

void oledRender(const Status &s) {
  oled.clearDisplay();

  oled.setTextSize(2);
  oled.setCursor(0, 0);
  oled.print(s.stateName);

  oled.setTextSize(3);
  oled.setCursor(0, 18);
  if (s.ppm > PPM_MAX_VALID) oled.print(F(">1000"));
  else oled.print((int)s.ppm);

  oled.setTextSize(2);
  oled.setCursor(0, 48);
  oled.print(s.note);

  oled.display();
}