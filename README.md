# ⏳ Digital LED Hourglass

An interactive, gravity-responsive digital hourglass built with an Arduino Nano. This project simulates the physics of falling sand using two 8x8 LED matrices and an accelerometer to detect the orientation of the device in real-time.

## ✨ Features
* **Gravity-Driven Simulation:** Uses accelerometer data to determine which way is "down" and flows the LED "sand" accordingly.
* **Dual Matrix Display:** Utilizes two 8x8 LED matrices to represent the top and bottom glass bulbs of a traditional hourglass.
* **Compact Design:** Powered by the small footprint of an Arduino Nano.

## 🛠️ Hardware Components
To build this project, you will need the following hardware:
* **Arduino Nano** (The brain of the project)
* **2x 8x8 LED Matrices** (Typically driven by MAX7219 ICs)
* **MPU6050 Accelerometer/Gyroscope** (To detect tilt and orientation)
* **Jumper Wires**
* **Breadboard** or **Custom PCB**
* **Power Supply** (e.g., USB cable or battery pack)

## 🔌 Wiring & Pinout
*Ensure your Arduino Nano is powered off before making connections.*

**MPU6050 (I2C Communication):**
* `VCC` -> `5V`
* `GND` -> `GND`
* `SDA` -> `A4` (Arduino Nano SDA)
* `SCL` -> `A5` (Arduino Nano SCL)

**OLED (I2C Communication):**
* `VCC` -> `5V`
* `GND` -> `GND`
* `SDA` -> `A4` (Arduino Nano SDA)
* `SCL` -> `A5` (Arduino Nano SCL)

**8x8 LED Matrices (SPI Communication via MAX7219):**
* `VCC` -> `5V`
* `GND` -> `GND`
* `DIN` -> `D11` (MOSI)
* `CS`  -> `D10` (SS - *You can change this in the code*)
* `CLK` -> `D13` (SCK)
*(Note: If your matrices are daisy-chained, connect the `DOUT` of the first matrix to the `DIN` of the second).*

## 💻 Installation & Setup
1. **Clone the repository:**
   ```bash
   git clone [https://github.com/flistrr/Hourglass.git](https://github.com/flistrr/Hourglass.git)
