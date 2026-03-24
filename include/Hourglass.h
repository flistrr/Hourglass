#include <Arduino.h>
#include "Oled.h"
#include "Matrixes.h"
#include "SandPhysics.h"
#include "MPU.h"

class Hourglass
{
private:
    unsigned long lastPhysics = 0;
    unsigned long lastDrop = 0;
    unsigned long lastEqualize = 0;
    unsigned long lastOledUpdate = 0;

    // ---------- Initializing Sand Physics ----------
    SandPhysics sandPhysics;
    // ------------------------------------------------

    // ---------- Initializing OLED display ----------
    const int SCREEN_WIDTH = 128;
    const int SCREEN_HEIGHT = 64;
    const int OLED_ADDR = 0x3C;

    OLED oled;
    // ------------------------------------------------

    // ---------- Initializing LED matrixes ----------
    const int DIN_PIN = 12;
    const int CLK_PIN = 11;
    const int CS_PIN = 10;
    const int NUM_DEVICES = 2;

    MATRIXES matrixes;
    // ------------------------------------------------

    // ---------- Initializing MPU6050 ----------
    const int MPU_ADDR = 0x68;

    MPU mpu;
    // ------------------------------------------------

    // ---------- Global variables ----------
    byte topGrid[8][8] = {0};
    byte botGrid[8][8] = {0};
    int ROTATION_TOP = 2;
    int ROTATION_BOT = 2;

    const unsigned long TARGET_TIME_MS = 60000;
    unsigned long dropInterval = 1000;

    int orientation = 0;
    int totalSandParticles = 0;
    // ------------------------------------------------

public:
    Hourglass();
    ~Hourglass();
    void init();
    void update(unsigned long currentMillis);
    void updateOled();
};

Hourglass::Hourglass() : matrixes(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES), mpu(MPU_ADDR), oled(OLED_ADDR, SCREEN_WIDTH, SCREEN_HEIGHT)
{
}

Hourglass::~Hourglass()
{
}

void Hourglass::init()
{

    matrixes.init();

    mpu.init();

    oled.init();

    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            topGrid[x][y] = 1;
    topGrid[7][0] = 0;

    totalSandParticles = sandPhysics.countSand(topGrid) + sandPhysics.countSand(botGrid);
    if (totalSandParticles > 0)
        dropInterval = TARGET_TIME_MS / totalSandParticles;
}

void Hourglass::update(unsigned long currentMillis)
{
    int16_t ax, ay, az;
    mpu.readData(ax, ay, az);

    mpu.getOrientation(ax, ay, az, orientation);
    if (currentMillis - lastPhysics > 40)
    {
        if (orientation == 0)
        {
            sandPhysics.updatePhysicsDownLeft(topGrid);
            sandPhysics.updatePhysicsDownLeft(botGrid);
        }
        else if (orientation == 1)
        {
            sandPhysics.updatePhysicsUpRight(topGrid);
            sandPhysics.updatePhysicsUpRight(botGrid);
        }
        else if (orientation == 2)
        {
            sandPhysics.updatePhysicsSideways(topGrid);
            sandPhysics.updatePhysicsSideways(botGrid);
        }
        else if (orientation == 3)
        {
            sandPhysics.updatePhysicsSidewaysNegative(topGrid);
            sandPhysics.updatePhysicsSidewaysNegative(botGrid);
        }

        matrixes.drawGrid(0, topGrid, ROTATION_TOP, ROTATION_BOT);
        matrixes.drawGrid(1, botGrid, ROTATION_TOP, ROTATION_BOT);
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

    if ((orientation == 2 || orientation == 3) &&
        (currentMillis - lastEqualize > 400))
    {
        sandPhysics.attemptEqualize(topGrid, botGrid);
        lastEqualize = currentMillis;
    }

    if (currentMillis - lastOledUpdate > 500)
    {
        updateOled();
        lastOledUpdate = currentMillis;
    }
}

void Hourglass::updateOled()
{
    int sandSource = (orientation == 0)   ? sandPhysics.countSand(topGrid)
                     : (orientation == 1) ? sandPhysics.countSand(botGrid)
                                          : 0;

    unsigned long remainingTimeMS = sandSource * dropInterval;
    int seconds = (remainingTimeMS / 1000) % 60;
    int minutes = (remainingTimeMS / 60000);

    oled.showTime(minutes, seconds, orientation);
}