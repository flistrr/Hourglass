#pragma once
#include "SandPhysics.h"
#include <Adafruit_SSD1306.h>

class OLED {
private:
  Adafruit_SSD1306 display;
  int OLED_ADDR;

public:
  OLED(const int &addr, const int &SCREEN_WIDTH, const int &SCREEN_HEIGHT);
  ~OLED();
  void init();
  void showTime(int &minutes, int &seconds, int &orientation);
  void showEmpty();
};

OLED::OLED(const int &addr, const int &SCREEN_WIDTH, const int &SCREEN_HEIGHT) {
  display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
  OLED_ADDR = addr;
}

OLED::~OLED() {}

void OLED::init() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    return;
  }
  display.clearDisplay();
}

void OLED::showTime(int &minutes, int &seconds, int &orientation) {
  display.clearDisplay();

  if (orientation == 0)
    display.setRotation(2);
  else
    display.setRotation(0);

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(32, 25);

  if (orientation == 2 || orientation == 3) {
    display.print("--:--");
  } else {
    if (minutes < 10)
      display.print("0");
    display.print(minutes);
    display.print(":");
    if (seconds < 10)
      display.print("0");
    display.print(seconds);
  }
  display.display();
}

void OLED::showEmpty() { display.print("--:--"); }

