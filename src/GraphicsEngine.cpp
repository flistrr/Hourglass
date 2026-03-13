#include "GraphicsEngine.h"

// ЗВЕРНІТЬ УВАГУ НА ДВОКРАПКУ ":" ПІСЛЯ ПАРАМЕТРІВ
GraphicsEngine::GraphicsEngine(int DIN_PIN, int CLK_PIN, int CS_PIN, int NUM_DEVICES)
    : lc(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES),                    
      display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_ADDR){
    for(int i = 0; i < NUM_DEVICES; i++) {
        lc.shutdown(i, false);
        lc.setIntensity(i, 8);
        lc.clearDisplay(i);
    }
}
GraphicsEngine::~GraphicsEngine()
{
}
void GraphicsEngine::drawGrid(int addr, byte grid[8][8]) {
  for (int x = 0; x < 8; x++) for (int y = 0; y < 8; y++) setLedRotated(addr, x, y, grid[x][y]);
}
void GraphicsEngine::setLedRotated(int addr, int x, int y, byte state) {
  int rotation = (addr == 0) ? ROTATION_TOP : ROTATION_BOT;
  int nx = x, ny = y;
  switch (rotation) {
    case 1: nx = y; ny = 7 - x; break;
    case 2: nx = 7 - x; ny = 7 - y; break;
    case 3: nx = 7 - y; ny = x; break;
  }
  lc.setLed(addr, nx, ny, state);
}
void GraphicsEngine::updateOLED(int orientation, byte topGrid[8][8], byte botGrid[8][8], int dropInterval) {
  display.clearDisplay();
  
  if (orientation == 1) {
    display.setRotation(0); 
  } else {
    display.setRotation(2);
  }
  
  int sandSource = (orientation == 0) ? countSand(topGrid) : 
                   (orientation == 1) ? countSand(botGrid) : 0;
  
  unsigned long remainingTimeMS = sandSource * dropInterval;
  int seconds = (remainingTimeMS / 1000) % 60;
  int minutes = (remainingTimeMS / 60000);

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(32, 25); 
  
  if (orientation == 2 || orientation == 3) {
    display.print("--:--");
  } else {
    if (minutes < 10) display.print("0");
    display.print(minutes);
    display.print(":");
    if (seconds < 10) display.print("0");
    display.print(seconds);
  }

  display.display();
}

int GraphicsEngine::countSand(byte grid[8][8]) {
  int c = 0;
  for(int i=0; i<64; i++) if(grid[i/8][i%8]) c++;
  return c;
}
