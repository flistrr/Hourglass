#include "LedControl.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- НАЛАШТУВАННЯ ДИСПЛЕЯ ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
#define OLED_ADDR 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- НАЛАШТУВАННЯ ПІНІВ ---
const int DIN_PIN = 12;
const int CLK_PIN = 11;
const int CS_PIN  = 10;
const int BATTERY_PIN = A0;

// --- НАЛАШТУВАННЯ СИСТЕМИ ---
const int NUM_DEVICES = 2; 
const int MPU_ADDR = 0x68;
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES);

// --- ЗМІННІ ПІСКУ ---
byte topGrid[8][8] = {0};
byte botGrid[8][8] = {0};
const int ROTATION_TOP = 2; 
const int ROTATION_BOT = 2; 

// --- ТАЙМЕРИ ---
unsigned long lastPhysics = 0;
unsigned long lastDrop = 0;
unsigned long lastEqualize = 0;
unsigned long lastOledUpdate = 0;

// --- НАЛАШТУВАННЯ ЧАСУ ---
const unsigned long TARGET_TIME_MS = 60000; // 1 хвилина
unsigned long dropInterval = 1000; 

// --- СТАН ---
int orientation = 0; 
int totalSandParticles = 0;

// --- ПРОТОТИПИ ФУНКЦІЙ ---
void updatePhysicsDownLeft(byte grid[8][8]); 
void updatePhysicsUpRight(byte grid[8][8]); 
void updatePhysicsSideways(byte grid[8][8]);
void drawGrid(int index, byte grid[8][8]);
void setLedRotated(int addr, int row, int col, byte state); 
void attemptEqualize();
void readMPU(int16_t &ax, int16_t &ay, int16_t &az);
int countSand(byte grid[8][8]);
void updateOLED();
int getBatteryPercentage();

void setup() {
  Serial.begin(115200);
  
  // Ініціалізація матриць
  for(int i=0; i<NUM_DEVICES; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 2);
    lc.clearDisplay(i);
  }
  
  // Ініціалізація MPU6050
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); Wire.write(0x6B); Wire.write(0); Wire.endTransmission(true);

  // Ініціалізація OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
    Serial.println(F("SSD1306 failed"));
  }
  display.clearDisplay();

  // Початкове заповнення піском (верхня частина)
  for(int x=0; x<8; x++) for(int y=0; y<8; y++) topGrid[x][y] = 1;
  // Трохи порожнечі для старту (щоб пісок почав сипатися красиво)
  topGrid[7][0] = 0;

  totalSandParticles = countSand(topGrid) + countSand(botGrid);
  if (totalSandParticles > 0) dropInterval = TARGET_TIME_MS / totalSandParticles;
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. MPU ТА ОРІЄНТАЦІЯ
  int16_t ax, ay, az;
  readMPU(ax, ay, az);

  if (ay < -6000) orientation = 1;       
  else if (ay > 6000) orientation = 0;   
  else if (abs(ax) > 6000) orientation = 2; 

  // 2. ФІЗИКА (рух піску всередині матриць)
  if (currentMillis - lastPhysics > 40) {
    if (orientation == 0) { updatePhysicsDownLeft(topGrid); updatePhysicsDownLeft(botGrid); }
    else if (orientation == 1) { updatePhysicsUpRight(topGrid); updatePhysicsUpRight(botGrid); }
    else if (orientation == 2) { updatePhysicsSideways(topGrid); updatePhysicsSideways(botGrid); }
    
    drawGrid(0, topGrid);
    drawGrid(1, botGrid);
    lastPhysics = currentMillis;
  }

  // 3. ТАЙМЕР ПАДІННЯ (переміщення піску між матрицями)
  if (currentMillis - lastDrop > dropInterval) {
    bool sandMoved = false;
    // Падіння вниз
    if (orientation == 0 && topGrid[7][0] == 1 && botGrid[0][7] == 0) {
        topGrid[7][0] = 0; botGrid[0][7] = 1; sandMoved = true;
    }
    // Падіння вгору (перевертання годинника)
    else if (orientation == 1 && botGrid[0][7] == 1 && topGrid[7][0] == 0) {
        botGrid[0][7] = 0; topGrid[7][0] = 1; sandMoved = true;
    }
    
    if (sandMoved) {
       lastDrop = currentMillis;
    }
  }

  // 4. ЕКВАЛАЙЗЕР (вирівнювання рівнів піску при горизонтальному положенні)
  if (orientation == 2 && (currentMillis - lastEqualize > 400)) {
    attemptEqualize();
    lastEqualize = currentMillis;
  }

  // 5. ОНОВЛЕННЯ ЕКРАНУ OLED
  if (currentMillis - lastOledUpdate > 500) {
    updateOLED();
    lastOledUpdate = currentMillis;
  }
}

// --- ДОПОМІЖНІ ФУНКЦІЇ ---

void updateOLED() {
  display.clearDisplay();
  
  int sandSource = (orientation == 0) ? countSand(topGrid) : 
                   (orientation == 1) ? countSand(botGrid) : 0;
  
  unsigned long remainingTimeMS = sandSource * dropInterval;
  int seconds = (remainingTimeMS / 1000) % 60;
  int minutes = (remainingTimeMS / 60000);

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(32, 25); 
  
  if (orientation == 2) {
    display.print("--:--");
  } else {
    if (minutes < 10) display.print("0");
    display.print(minutes);
    display.print(":");
    if (seconds < 10) display.print("0");
    display.print(seconds);
  }

  // Батарея
  int batPercent = getBatteryPercentage();
  display.setTextSize(1);
  display.setCursor(95, 0);
  display.print(batPercent); display.print("%");
  
  display.drawRect(95, 10, 30, 6, SSD1306_WHITE);
  int fill = map(batPercent, 0, 100, 0, 28);
  display.fillRect(96, 11, fill, 4, SSD1306_WHITE);

  display.display();
}

int getBatteryPercentage() {
  int raw = analogRead(BATTERY_PIN);
  // Калібрування (налаштуй під свій дільник напруги):
  // 3.0V = ~614, 4.2V = ~860 (при живленні 5V)
  int percent = map(raw, 614, 860, 0, 100);
  Serial.print("Battery: "); Serial.print(percent); Serial.println("%");
  return constrain(percent, 0, 100);
}

void drawGrid(int addr, byte grid[8][8]) {
  for (int x = 0; x < 8; x++) for (int y = 0; y < 8; y++) setLedRotated(addr, x, y, grid[x][y]);
}

void setLedRotated(int addr, int x, int y, byte state) {
  int rotation = (addr == 0) ? ROTATION_TOP : ROTATION_BOT;
  int nx = x, ny = y;
  switch (rotation) {
    case 1: nx = y; ny = 7 - x; break;
    case 2: nx = 7 - x; ny = 7 - y; break;
    case 3: nx = 7 - y; ny = x; break;
  }
  lc.setLed(addr, nx, ny, state);
}

// Фізика піску
void updatePhysicsDownLeft(byte grid[8][8]) {
  for (int x = 7; x >= 0; x--) for (int y = 0; y < 8; y++) {
    if (grid[x][y] == 1) {
      if (x < 7 && y > 0 && grid[x+1][y-1] == 0) { grid[x][y]=0; grid[x+1][y-1]=1; }
      else if (x < 7 && grid[x+1][y] == 0) { grid[x][y]=0; grid[x+1][y]=1; }
      else if (y > 0 && grid[x][y-1] == 0) { grid[x][y]=0; grid[x][y-1]=1; }
    }
  }
}

void updatePhysicsUpRight(byte grid[8][8]) {
  for (int x = 0; x < 8; x++) for (int y = 7; y >= 0; y--) {
    if (grid[x][y] == 1) {
      if (x > 0 && y < 7 && grid[x-1][y+1] == 0) { grid[x][y]=0; grid[x-1][y+1]=1; }
      else if (x > 0 && grid[x-1][y] == 0) { grid[x][y]=0; grid[x-1][y]=1; }
      else if (y < 7 && grid[x][y+1] == 0) { grid[x][y]=0; grid[x][y+1]=1; }
    }
  }
}

void updatePhysicsSideways(byte grid[8][8]) {
  for (int y = 7; y >= 0; y--) for (int x = 0; x < 8; x++) {
    if (grid[x][y] == 1) {
      if (y < 7 && grid[x][y+1] == 0) { grid[x][y]=0; grid[x][y+1]=1; }
      else if (y < 7 && x < 7 && grid[x+1][y+1] == 0) { grid[x][y]=0; grid[x+1][y+1]=1; }
      else if (y < 7 && x > 0 && grid[x-1][y+1] == 0) { grid[x][y]=0; grid[x-1][y+1]=1; }
    }
  }
}

void attemptEqualize() {
  int cTop = countSand(topGrid);
  int cBot = countSand(botGrid);
  if (abs(cTop - cBot) <= 2) return;
  if (cTop > cBot) {
    if (botGrid[0][7] == 1) return; 
    bool found = false;
    for(int x=0; x<8; x++) for(int y=0; y<8; y++) if(topGrid[x][y] == 1) { topGrid[x][y]=0; found=true; break; }
    if(found) botGrid[0][7] = 1;
  } else {
    if (topGrid[7][0] == 1) return;
    bool found = false;
    for(int x=0; x<8; x++) for(int y=0; y<8; y++) if(botGrid[x][y] == 1) { botGrid[x][y]=0; found=true; break; }
    if(found) topGrid[7][0] = 1;
  }
}

int countSand(byte grid[8][8]) {
  int c = 0;
  for(int i=0; i<64; i++) if(grid[i/8][i%8]) c++;
  return c;
}

void readMPU(int16_t &ax, int16_t &ay, int16_t &az) {
  Wire.beginTransmission(MPU_ADDR); Wire.write(0x3B); Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true); 
  ax = Wire.read()<<8|Wire.read(); ay = Wire.read()<<8|Wire.read(); az = Wire.read()<<8|Wire.read();  
}