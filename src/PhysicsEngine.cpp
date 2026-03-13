#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine()
{
}
PhysicsEngine::~PhysicsEngine()
{
}


void PhysicsEngine::updatePhysicsDownLeft(byte grid[8][8])
{
    for (int x = 7; x >= 0; x--)
        for (int y = 0; y < 8; y++)
        {
            if (grid[x][y] == 1)
            {
                if (x < 7 && y > 0 && grid[x + 1][y - 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x + 1][y - 1] = 1;
                }
                else if (x < 7 && grid[x + 1][y] == 0)
                {
                    grid[x][y] = 0;
                    grid[x + 1][y] = 1;
                }
                else if (y > 0 && grid[x][y - 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x][y - 1] = 1;
                }
            }
        }
}

void PhysicsEngine::updatePhysicsUpRight(byte grid[8][8])
{
    for (int x = 0; x < 8; x++)
        for (int y = 7; y >= 0; y--)
        {
            if (grid[x][y] == 1)
            {
                if (x > 0 && y < 7 && grid[x - 1][y + 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x - 1][y + 1] = 1;
                }
                else if (x > 0 && grid[x - 1][y] == 0)
                {
                    grid[x][y] = 0;
                    grid[x - 1][y] = 1;
                }
                else if (y < 7 && grid[x][y + 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x][y + 1] = 1;
                }
            }
        }
}

void PhysicsEngine::updatePhysicsSidewaysPlusY(byte grid[8][8])
{
    for (int y = 7; y >= 0; y--)
        for (int x = 0; x < 8; x++)
        {
            if (grid[x][y] == 1)
            {
                if (y < 7 && grid[x][y + 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x][y + 1] = 1;
                }
                else if (y < 7 && x < 7 && grid[x + 1][y + 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x + 1][y + 1] = 1;
                }
                else if (y < 7 && x > 0 && grid[x - 1][y + 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x - 1][y + 1] = 1;
                }
            }
        }
}

void PhysicsEngine::updatePhysicsSidewaysMinusY(byte grid[8][8])
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
        {
            if (grid[x][y] == 1)
            {
                if (y > 0 && grid[x][y - 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x][y - 1] = 1;
                }
                else if (y > 0 && x < 7 && grid[x + 1][y - 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x + 1][y - 1] = 1;
                }
                else if (y > 0 && x > 0 && grid[x - 1][y - 1] == 0)
                {
                    grid[x][y] = 0;
                    grid[x - 1][y - 1] = 1;
                }
            }
        }
}

