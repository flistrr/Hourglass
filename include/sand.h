#include <LedControl.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class Sand
{
private:
    LedControl lc = LedControl(12, 11, 10, 2);

public:
    Sand();
    ~Sand();
    void resetMatrixes();
};

Sand::Sand()
{
    resetMatrixes();
    lc.setIntensity(0, 8);
    lc.clearDisplay(0);
}

void Sand::resetMatrixes()
{
  for (int index = 0; index < lc.getDeviceCount(); index++)
  {
    lc.shutdown(index, false);
  }
}

