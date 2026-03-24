#include <LedControl.h>

class MATRIXES
{
private:
    LedControl lc;
public:
    MATRIXES(const int &DIN_PIN, const int &CLK_PIN, const int &CS_PIN, const int &NUM_DEVICES);
    ~MATRIXES();
    void init();
    void setLedRotated(int &addr, int &row, int &col, byte &state, int &rotation_TOP, int &rotation_BOT);
    void drawGrid(int addr, byte grid[8][8], int &rotation_TOP, int &rotation_BOT);
};

MATRIXES::MATRIXES(const int &DIN_PIN, const int &CLK_PIN, const int &CS_PIN, const int &NUM_DEVICES) : lc(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES)
{

}

MATRIXES::~MATRIXES()
{
}

void MATRIXES::init(){
    int count = lc.getDeviceCount();
    for (int i = 0; i < count; i++) {
        lc.shutdown(i, false);
        lc.setIntensity(i, 2);
        lc.clearDisplay(i);
    }
}

void MATRIXES::setLedRotated(int &addr, int &x, int &y, byte &state, int &rotation_TOP, int &rotation_BOT) {
  int rotation = (addr == 0) ? rotation_TOP : rotation_BOT;
  int nx = x, ny = y;
  switch (rotation) {
  case 1:
    nx = y;
    ny = 7 - x;
    break;
  case 2:
    nx = 7 - x;
    ny = 7 - y;
    break;
  case 3:
    nx = 7 - y;
    ny = x;
    break;
  }
  lc.setLed(addr, nx, ny, state);
}

void MATRIXES::drawGrid(int addr, byte grid[8][8], int &rotation_TOP, int &rotation_BOT) {
  for (int x = 0; x < 8; x++)
    for (int y = 0; y < 8; y++)
      setLedRotated(addr, x, y, grid[x][y], rotation_TOP, rotation_BOT);
}