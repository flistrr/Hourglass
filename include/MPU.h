#include <Wire.h>

class MPU {
private:
  int MPU_ADDR;

public:
  MPU(int mpu);
  ~MPU();
  void init();
  void getOrientation(int16_t &ax, int16_t &ay, int16_t &az, int &orientation);
  void readData(int16_t &ax, int16_t &ay, int16_t &az);
};

MPU::MPU(int mpu) : MPU_ADDR(mpu) {}

MPU::~MPU() {}

void MPU::init() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void MPU::readData(int16_t &ax, int16_t &ay, int16_t &az) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}

void MPU::getOrientation(int16_t &ax, int16_t &ay, int16_t &az, int &orientation){
  if (ay < -6000)
    orientation = 1;
  else if (ay > 6000)
    orientation = 0;
  else if (ax > 6000)
    orientation = 2;
  else if (ax < -6000)
    orientation = 3;
}