#include "LedControl.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "PhysicsEngine.h"
#include "MPUReader.h"
#include "GraphicsEngine.h"

const int DIN_PIN = 12;
const int CLK_PIN = 11;
const int CS_PIN = 10;
const int NUM_DEVICES = 2;

const int MPU_ADDR = 0x68;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_ADDR = 0x3C;

byte topGrid[8][8] = {0};
byte botGrid[8][8] = {0};
const int ROTATION_TOP = 2;
const int ROTATION_BOT = 2;

PhysicsEngine physics = PhysicsEngine();
GraphicsEngine gfx = GraphicsEngine(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES);

unsigned long lastPhysics = 0;
unsigned long lastDrop = 0;
unsigned long lastEqualize = 0;
unsigned long lastOledUpdate = 0;

const unsigned long TARGET_TIME_MS = 60000;
unsigned long dropInterval = 1000;

int orientation = 0;
int totalSandParticles = 0;

void attemptEqualize();
void readMPU(int16_t &ax, int16_t &ay, int16_t &az);

void setup()
{
  Serial.begin(115200);

  for (int i = 0; i < NUM_DEVICES; i++)
  {
    gfx.lc.shutdown(i, false);
    gfx.lc.setIntensity(i, 2);
    gfx.lc.clearDisplay(i);
  }

  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  if (!gfx.display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
  {
    Serial.println(F("SSD1306 failed"));
  }
  gfx.display.clearDisplay();

  for (int x = 0; x < 8; x++)
    for (int y = 0; y < 8; y++)
      topGrid[x][y] = 1;
  topGrid[7][0] = 0;

  totalSandParticles = gfx.countSand(topGrid) + gfx.countSand(botGrid);
  if (totalSandParticles > 0)
    dropInterval = TARGET_TIME_MS / totalSandParticles;
}

void loop()
{
  unsigned long currentMillis = millis();

  int16_t ax, ay, az;
  readMPU(ax, ay, az);

  if (ay < -6000)
    orientation = 1;
  else if (ay > 6000)
    orientation = 0;
  else if (ax > 6000)
    orientation = 2;
  else if (ax < -6000)
    orientation = 3;

  if (currentMillis - lastPhysics > 40)
  {
    if (orientation == 0)
    {
      physics.updatePhysicsDownLeft(topGrid);
      physics.updatePhysicsDownLeft(botGrid);
    }
    else if (orientation == 1)
    {
      physics.updatePhysicsUpRight(topGrid);
      physics.updatePhysicsUpRight(botGrid);
    }
    else if (orientation == 2)
    {
      physics.updatePhysicsSidewaysPlusY(topGrid);
      physics.updatePhysicsSidewaysPlusY(botGrid);
    }
    else if (orientation == 3)
    {
      physics.updatePhysicsSidewaysMinusY(topGrid);
      physics.updatePhysicsSidewaysMinusY(botGrid);
    }

    gfx.drawGrid(0, topGrid);
    gfx.drawGrid(1, botGrid);
    lastPhysics = currentMillis;
  }

  if (currentMillis - lastDrop > dropInterval)
  {
    bool sandMoved = false;

    if (orientation == 0 && topGrid[7][0] == 1 && botGrid[0][7] == 0)
    {
      topGrid[7][0] = 0;
      botGrid[0][7] = 1;
      sandMoved = true;
    }
    else if (orientation == 1 && botGrid[0][7] == 1 && topGrid[7][0] == 0)
    {
      botGrid[0][7] = 0;
      topGrid[7][0] = 1;
      sandMoved = true;
    }
    if (sandMoved)
    {
      lastDrop = currentMillis;
    }
  }

  if ((orientation == 2 || orientation == 3) && (currentMillis - lastEqualize > 400))
  {
    attemptEqualize();
    lastEqualize = currentMillis;
  }

  if (currentMillis - lastOledUpdate > 500)
  {
    gfx.updateOLED(orientation, topGrid, botGrid, dropInterval);
    lastOledUpdate = currentMillis;
  }
}

void attemptEqualize()
{
  int cTop = gfx.countSand(topGrid);
  int cBot = gfx.countSand(botGrid);

  if (abs(cTop - cBot) <= 2)
    return;

  if (cTop > cBot)
  {
    if (botGrid[0][7] == 1)
      return;

    bool found = false;

    if (orientation == 2)
    {
      for (int y = 0; y < 8 && !found; y++)
      {
        for (int x = 7; x >= 0 && !found; x--)
        {
          if (topGrid[x][y] == 1)
          {
            topGrid[x][y] = 0;
            found = true;
          }
        }
      }
    }
    else if (orientation == 3)
    {
      for (int y = 7; y >= 0 && !found; y--)
      {
        for (int x = 7; x >= 0 && !found; x--)
        {
          if (topGrid[x][y] == 1)
          {
            topGrid[x][y] = 0;
            found = true;
          }
        }
      }
    }

    if (found)
      botGrid[0][7] = 1;
  }
  else
  {
    if (topGrid[7][0] == 1)
      return;

    bool found = false;

    if (orientation == 2)
    {
      for (int y = 0; y < 8 && !found; y++)
      {
        for (int x = 0; x < 8 && !found; x++)
        {
          if (botGrid[x][y] == 1)
          {
            botGrid[x][y] = 0;
            found = true;
          }
        }
      }
    }
    else if (orientation == 3)
    {
      for (int y = 7; y >= 0 && !found; y--)
      {
        for (int x = 0; x < 8 && !found; x++)
        {
          if (botGrid[x][y] == 1)
          {
            botGrid[x][y] = 0;
            found = true;
          }
        }
      }
    }

    if (found)
      topGrid[7][0] = 1;
  }
}

void readMPU(int16_t &ax, int16_t &ay, int16_t &az)
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}