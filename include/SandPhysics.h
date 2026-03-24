#pragma once
#include <Arduino.h>

class SandPhysics
{
private:
public:
    SandPhysics();
    ~SandPhysics();
    void updatePhysicsDownLeft(byte grid[8][8]);
    void updatePhysicsUpRight(byte grid[8][8]);
    void updatePhysicsSideways(byte grid[8][8]);
    void updatePhysicsSidewaysNegative(byte grid[8][8]);
    void attemptEqualize(byte topGrid[8][8], byte botGrid[8][8]);
    int countSand(byte grid[8][8]);
};

SandPhysics::SandPhysics()
{
}

SandPhysics::~SandPhysics()
{
}

void SandPhysics::updatePhysicsSidewaysNegative(byte grid[8][8]) {
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++) {
      if (grid[x][y] == 1) {
        if (y > 0 && grid[x][y - 1] == 0) {
          grid[x][y] = 0;
          grid[x][y - 1] = 1;
        } else if (y > 0 && x > 0 && grid[x - 1][y - 1] == 0) {
          grid[x][y] = 0;
          grid[x - 1][y - 1] = 1;
        } else if (y > 0 && x < 7 && grid[x + 1][y - 1] == 0) {
          grid[x][y] = 0;
          grid[x + 1][y - 1] = 1;
        } else if (x > 0 && grid[x - 1][y] == 0) {
          grid[x][y] = 0;
          grid[x - 1][y] = 1;
        } else if (x < 7 && grid[x + 1][y] == 0) {
          grid[x][y] = 0;
          grid[x + 1][y] = 1;
        }
      }
    }
}


void SandPhysics::updatePhysicsSideways(byte grid[8][8]) {
  for (int y = 7; y >= 0; y--)
    for (int x = 7; x >= 0; x--) {
      if (grid[x][y] == 1) {
        if (y < 7 && grid[x][y + 1] == 0) {
          grid[x][y] = 0;
          grid[x][y + 1] = 1;
        }
        else if (y < 7 && x < 7 && grid[x + 1][y + 1] == 0) {
          grid[x][y] = 0;
          grid[x + 1][y + 1] = 1;
        }
        else if (y < 7 && x > 0 && grid[x - 1][y + 1] == 0) {
          grid[x][y] = 0;
          grid[x - 1][y + 1] = 1;
        }
        else if (x < 7 && grid[x + 1][y] == 0) {
          grid[x][y] = 0;
          grid[x + 1][y] = 1;
        }
        else if (x > 0 && grid[x - 1][y] == 0) {
          grid[x][y] = 0;
          grid[x - 1][y] = 1;
        }
      }
    }
}


void SandPhysics::updatePhysicsDownLeft(byte grid[8][8]) {
  for (int x = 7; x >= 0; x--)
    for (int y = 0; y < 8; y++) {
      if (grid[x][y] == 1) {
        if (x < 7 && y > 0 && grid[x + 1][y - 1] == 0) {
          grid[x][y] = 0;
          grid[x + 1][y - 1] = 1;
        } else if (x < 7 && grid[x + 1][y] == 0) {
          grid[x][y] = 0;
          grid[x + 1][y] = 1;
        } else if (y > 0 && grid[x][y - 1] == 0) {
          grid[x][y] = 0;
          grid[x][y - 1] = 1;
        }
      }
    }
}

void SandPhysics::updatePhysicsUpRight(byte grid[8][8]) {
  for (int x = 0; x < 8; x++)
    for (int y = 7; y >= 0; y--) {
      if (grid[x][y] == 1) {
        if (x > 0 && y < 7 && grid[x - 1][y + 1] == 0) {
          grid[x][y] = 0;
          grid[x - 1][y + 1] = 1;
        } else if (x > 0 && grid[x - 1][y] == 0) {
          grid[x][y] = 0;
          grid[x - 1][y] = 1;
        } else if (y < 7 && grid[x][y + 1] == 0) {
          grid[x][y] = 0;
          grid[x][y + 1] = 1;
        }
      }
    }
}


void SandPhysics::attemptEqualize(byte topGrid[8][8], byte botGrid[8][8]) {
  int cTop = countSand(topGrid);
  int cBot = countSand(botGrid);

  if (abs(cTop - cBot) <= 2)
    return;

  if (cTop > cBot) {
    if (botGrid[0][7] == 1)
      return;
    bool found = false;
    for (int x = 0; x < 8 && !found; x++) {
      for (int y = 0; y < 8 && !found; y++) {
        if (topGrid[x][y] == 1) {
          topGrid[x][y] = 0;
          botGrid[0][7] = 1;
          found = true;
        }
      }
    }
  } else {
    if (topGrid[7][0] == 1)
      return;

    bool found = false;
    for (int x = 0; x < 8 && !found; x++) {
      for (int y = 0; y < 8 && !found; y++) {
        if (botGrid[x][y] == 1) {
          botGrid[x][y] = 0;
          topGrid[7][0] = 1;
          found = true;
        }
      }
    }
  }
}

int SandPhysics::countSand(byte grid[8][8]) {
  int c = 0;
  for (int i = 0; i < 64; i++)
    if (grid[i / 8][i % 8])
      c++;
  return c;
}