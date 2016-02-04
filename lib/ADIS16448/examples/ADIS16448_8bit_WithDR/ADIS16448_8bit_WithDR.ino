////////////////////////////////////////////////////////////////////////////////////////////////////////
//  May 2015
//  Author: Juan Jose Chong <juan.chong@analog.com>
////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ADIS16448.ino
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  This Arduino project interfaces with an ADIS16448 using SPI and the accompanying C++ libraries, 
//  reads IMU data in LSBs, scales the data, and outputs measurements to a serial debug terminal (putty) via
//  the onboard USB serial port.
//
//  This project has been tested on an Arduino Duemilanove and Uno, but should be compatible with any other
//  8-Bit Arduino embedded platform. 
//
//  This example is free software. You can redistribute it and/or modify it
//  under the terms of the GNU Lesser Public License as published by the Free Software
//  Foundation, either version 3 of the License, or any later version.
//
//  This example is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
//  FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more details.
//
//  You should have received a copy of the GNU Lesser Public License along with 
//  this example.  If not, see <http://www.gnu.org/licenses/>.
//
//  Pinout for Arduino Uno/Diecimila/Duemilanove
//  Gray = RST = 4
//  Purple = SCLK = 13
//  Blue = CS = 7
//  Green = DOUT(MISO) = 11
//  Yellow = DIN(MOSI) = 12
//  Brown = GND
//  Red = VCC [3.3V ONLY]
//  White = DR = 2
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <ADIS16448.h>
#include <SPI.h>

// Initialize Variables
// Accelerometer
int AX = 0;
int AY = 0;
int AZ = 0;
float AXS = 0;
float AYS = 0;
float AZS = 0;
// Gyro
int GX = 0;
int GY = 0;
int GZ = 0;
float GXS = 0;
float GYS = 0;
float GZS = 0;
// Magnetometer
int MX = 0;
int MY = 0;
int MZ = 0;
float MXS = 0;
float MYS = 0;
float MZS = 0;
// Barometer
int BAR = 0;
float BARS = 0;
// Control Registers
int MSC = 0;
int SENS = 0;
int SMPL = 0;
// Temperature
float TEMPS = 0;
int TEMP = 0;

// Data Ready Flag
boolean validData = false;

// Call ADIS16448 Class
ADIS16448 IMU(7,2,4); //ChipSelect,DataReady,Reset Pin Assignments

void setup()
{
  Serial.begin(115200); // Initialize serial output via USB
  IMU.configSPI(); // Configure SPI communication
  
  #ifdef DEBUG
    Serial.println("**********DEBUG MODE**********");
  #endif
  
  delay(100); // Give the part time to start up
  IMU.regWrite(MSC_CTRL,0x6);  // Enable Data Ready on IMU
  delay(20); 
  IMU.regWrite(SENS_AVG,0x2); // Set Digital Filter on IMU
  delay(20);
  IMU.regWrite(SMPL_PRD,0x1), // Set Decimation on IMU
  delay(20);
  
  // Read the control registers once to print to screen
  MSC = IMU.regRead(MSC_CTRL);
  SENS = IMU.regRead(SENS_AVG);
  SMPL = IMU.regRead(SMPL_PRD);
  
  attachInterrupt(0, setDRFlag, RISING); // Attach interrupt to pin 2. Trigger on the rising edge
  
}

// Function used to read register values via SPI and load them into variables in LSBs
void grabData()
{
    // Put all the Data Registers you want to read here
    TEMP = 0;
    IMU.configSPI(); // Configure SPI before the read
    GX = IMU.regRead(XGYRO_OUT);
    GY = IMU.regRead(YGYRO_OUT);
    GZ = IMU.regRead(ZGYRO_OUT);
    AX = IMU.regRead(XACCL_OUT);
    AY = IMU.regRead(YACCL_OUT);
    AZ = IMU.regRead(ZACCL_OUT);
    MX = IMU.regRead(XMAGN_OUT);
    MY = IMU.regRead(YMAGN_OUT);
    MZ = IMU.regRead(ZMAGN_OUT);
    BAR = IMU.regRead(BARO_OUT);
    TEMP = IMU.regRead(TEMP_OUT);
}

// Function used to scale all acquired data (scaling functions are included in ADIS16448.cpp)
void scaleData()
{
    GXS = IMU.gyroScale(GX); //Scale X Gyro
    GYS = IMU.gyroScale(GY); //Scale Y Gyro
    GZS = IMU.gyroScale(GZ); //Scale Z Gyro
    AXS = IMU.accelScale(AX); //Scale X Accel
    AYS = IMU.accelScale(AY); //Scale Y Accel
    AZS = IMU.accelScale(AZ); //Scale Z Accel
    MXS = IMU.magnetometerScale(MX); //Scale X Magnetometer
    MYS = IMU.magnetometerScale(MY); //Scale Y Magnetometer
    MZS = IMU.magnetometerScale(MZ); //Scale Z Magnetometer
    BARS = IMU.pressureScale(BAR); //Scale Barometer
    TEMPS = IMU.tempScale(TEMP); //Scale Temp Sensor
}

// Data Ready Interrupt Routine
void setDRFlag()
{
  validData = !validData;
}

// Main loop. Scale and display registers read using the interrupt
void loop()
{
  if (validData) // If data present in the ADIS16448 registers is valid...
  {
    grabData(); // Grab data from the IMU
    
    scaleData(); // Scale data acquired from the IMU
    
    //Print control registers to the serial port
    Serial.println("Control Registers");
    Serial.print("MSC_CTRL: ");
    Serial.println((unsigned char)MSC,HEX);
    Serial.print("SENS_AVG: ");
    Serial.println((unsigned char)SENS,HEX);
    Serial.print("SMPL_PRD: ");
    Serial.println((unsigned char)SMPL,HEX);
    Serial.println(" ");
    Serial.println("Data Registers");
    
    //Print scaled gyro data
    Serial.print("XGYRO: ");
    Serial.println(GXS);
    Serial.print("YGYRO: ");
    Serial.println(GYS);
    Serial.print("ZGYRO: ");
    Serial.println(GZS);
  
    //Print scaled accel data
    Serial.print("XACCL: ");
    Serial.println(AXS);
    Serial.print("YACCL: ");
    Serial.println(AYS);
    Serial.print("ZACCL: ");
    Serial.println(AZS);
    
    //Print scaled magnetometer data
    Serial.print("XMAG: ");
    Serial.println(MXS);
    Serial.print("YMAG: ");
    Serial.println(MYS);
    Serial.print("ZMAG: ");
    Serial.println(MZS);
    
    //Print scaled barometer data
    Serial.print("BAR: ");
    Serial.println(BARS);
   
    //Print scaled temp data
    Serial.print("TEMP: ");
    Serial.println(TEMPS);
   
    delay(150); // Give the user time to read the data
    
    //Clear the serial terminal and reset cursor
    //Only works on supported serial terminal programs (Putty)
    Serial.print("\033[2J");
    Serial.print("\033[H");
  }
}
