#pragma once
#include "LedControl.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>

class GraphicsEngine
{
private:
    const int SCREEN_WIDTH = 128;
    const int SCREEN_HEIGHT = 64;
    const int OLED_ADDR = 0x3C;
    
    byte topGrid[8][8] = {0};
    byte botGrid[8][8] = {0};
    const int ROTATION_TOP = 2;
    const int ROTATION_BOT = 2;
public:
    LedControl lc;
    Adafruit_SSD1306 display;
    GraphicsEngine(int DIN_PIN, int CLK_PIN, int CS_PIN, int NUM_DEVICES);
    ~GraphicsEngine();
    void drawGrid(int index, byte grid[8][8]);
    void setLedRotated(int addr, int row, int col, byte state);
    void updateOLED(int orientation, byte topGrid[8][8], byte botGrid[8][8], int dropInterval);
    int countSand(byte grid[8][8]);
};