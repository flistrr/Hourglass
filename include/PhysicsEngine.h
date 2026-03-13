#pragma once
#include <Arduino.h>

class PhysicsEngine
{
private:
public:
    PhysicsEngine();
    ~PhysicsEngine();
    void updatePhysicsDownLeft(byte grid[8][8]);
    void updatePhysicsDownRight(byte grid[8][8]);
    void updatePhysicsUpLeft(byte grid[8][8]);
    void updatePhysicsUpRight(byte grid[8][8]);
    void updatePhysicsSidewaysPlusY(byte grid[8][8]);
    void updatePhysicsSidewaysMinusY(byte grid[8][8]);
    int countSand(byte grid[8][8]);
};
