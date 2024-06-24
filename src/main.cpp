#include <Wire.h>
#include <Arduino.h>
#include <SoftwareWire.h> // Include the SoftwareWire library

#define UNSEAL_KEY_1 0xCCDF   // Custom Unseal Key Part 1
#define UNSEAL_KEY_2 0x7EE0   // Custom Unseal Key Part 2
#define CLEAR_PF_COMMAND 0x42 // Command to clear the PF flag

// Standard and common non-standard Smart Battery commands
#define BATTERY_MODE 0x03
#define TEMPERATURE 0x08
#define VOLTAGE 0x09
#define CURRENT 0x0A
#define RELATIVE_SOC 0x0D
#define ABSOLUTE_SOC 0x0E
#define REMAINING_CAPACITY 0x0F
#define FULL_CHARGE_CAPACITY 0x10
#define TIME_TO_FULL 0x13
#define CHARGING_CURRENT 0x14
#define CHARGING_VOLTAGE 0x15
#define BATTERY_STATUS 0x16
#define CYCLE_COUNT 0x17
#define DESIGN_CAPACITY 0x18
#define DESIGN_VOLTAGE 0x19
#define SPEC_INFO 0x1A
#define MFG_DATE 0x1B
#define SERIAL_NUM 0x1C
#define STATE_OF_HEALTH 0x4F
#define DJI_SERIAL 0xD8 // String
#define CELL2_VOLTAGE 0x3E
#define CELL1_VOLTAGE 0x3F

#define bufferLen 32
uint8_t i2cBuffer[bufferLen];
byte deviceAddress = 0x0B;

SoftwareWire myWire(6, 5); // Create a SoftwareWire instance with SDA on pin 6 and SCL on pin 5

void printManufactureDate(int mdate)
{
  int mday = B00011111 & mdate;
  int mmonth = mdate >> 5 & B00001111;
  int myear = 1980 + (mdate >> 9 & B01111111);
  Serial.print(mday);
  Serial.print(".");
  Serial.print(mmonth);
  Serial.print(".");
  Serial.println(myear);
}

void readStringData(byte address, byte command)
{
  myWire.beginTransmission(address);
  myWire.write(command);
  byte error = myWire.endTransmission();

  if (error == 0)
  {
    myWire.requestFrom(address, bufferLen);
    uint8_t length_read = myWire.read();
    length_read = constrain(length_read, 0, bufferLen - 1);
    for (uint8_t i = 0; i < length_read; i++)
    {
      if (myWire.available())
      {
        i2cBuffer[i] = myWire.read();
      }
      else
      {
        i2cBuffer[i] = '\0';
      }
    }
    i2cBuffer[length_read] = '\0';

    Serial.println((char *)i2cBuffer); // Print as string
  }
  else
  {
    Serial.print("Error requesting data from address 0x");
    if (address < 16)
      Serial.print("0");
    Serial.println(address, HEX);
  }
}

uint8_t read_byte()
{
  while (1)
  {
    if (myWire.available())
    {
      return myWire.read();
    }
  }
}

int fetchWord(byte address, byte command)
{
  myWire.beginTransmission(address);
  myWire.write(command);
  myWire.endTransmission(false);
  delay(1); // FIX wire bug
  myWire.requestFrom(address, 2, true);

  uint8_t b1 = read_byte();
  uint8_t b2 = read_byte();
  myWire.endTransmission();
  return (int)b1 | (((int)b2) << 8);
}

void readBatteryInfo(byte address)
{
  Serial.println("Reading Battery Information...");

  // Read and print serial number
  Serial.print("Serial Number: ");
  readStringData(address, DJI_SERIAL);

  // Read and print cycle count
  Serial.print("Cycle Count: ");
  Serial.println(fetchWord(address, CYCLE_COUNT));

  // Read and print voltage
  Serial.print("Voltage: ");
  Serial.println(fetchWord(address, VOLTAGE) / 1000.0, 3);

  // Read and print other information
  Serial.print("Design Capacity: ");
  Serial.println(fetchWord(address, DESIGN_CAPACITY));

  Serial.print("Design Voltage: ");
  Serial.println(fetchWord(address, DESIGN_VOLTAGE) / 1000.0, 3);

  Serial.print("Manufacture Date (D.M.Y): ");
  printManufactureDate(fetchWord(address, MFG_DATE));

  Serial.print("State of Health: ");
  Serial.println(fetchWord(address, STATE_OF_HEALTH));

  Serial.print("Full Charge Capacity: ");
  Serial.println(fetchWord(address, FULL_CHARGE_CAPACITY));

  Serial.print("Remaining Capacity: ");
  Serial.println(fetchWord(address, REMAINING_CAPACITY));

  Serial.print("Relative State of Charge (%): ");
  Serial.println(fetchWord(address, RELATIVE_SOC));

  Serial.print("Absolute State of Charge (%): ");
  Serial.println(fetchWord(address, ABSOLUTE_SOC));

  Serial.print("Minutes remaining for full charge: ");
  Serial.println(fetchWord(address, TIME_TO_FULL));

  Serial.print("Cell 1 Voltage: ");
  Serial.println(fetchWord(address, CELL1_VOLTAGE) / 1000.0, 3);

  Serial.print("Cell 2 Voltage: ");
  Serial.println(fetchWord(address, CELL2_VOLTAGE) / 1000.0, 3);

  Serial.print("State of Health: ");
  Serial.println(fetchWord(address, STATE_OF_HEALTH));

  Serial.print("Battery Mode (BIN): 0b");
  Serial.println(fetchWord(address, BATTERY_MODE), BIN);

  Serial.print("Battery Status (BIN): 0b");
  Serial.println(fetchWord(address, BATTERY_STATUS), BIN);

  Serial.print("Charging Current: ");
  Serial.println(fetchWord(address, CHARGING_CURRENT));

  Serial.print("Charging Voltage: ");
  Serial.println(fetchWord(address, CHARGING_VOLTAGE));

  Serial.print("Temperature: ");
  Serial.println((fetchWord(address, TEMPERATURE) / 10.0) - 273.15, 1);

  Serial.print("Current (mA): ");
  Serial.println(fetchWord(address, CURRENT));

  Serial.println(".");
}

void sendUnsealCommand(byte address, uint16_t unsealKey)
{
  for (int i = 0; i < 3; i++)
  { // Retry mechanism
    myWire.beginTransmission(address);
    myWire.write(unsealKey >> 8);   // Send high byte
    myWire.write(unsealKey & 0xFF); // Send low byte
    byte error = myWire.endTransmission();

    if (error == 0)
    {
      Serial.print("Sent unseal command (0x");
      Serial.print(unsealKey, HEX);
      Serial.print(") to address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      return; // Exit the function if successful
    }
    else
    {
      Serial.print("Error sending unseal command (0x");
      Serial.print(unsealKey, HEX);
      Serial.print(") to address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }

    delay(2000); // Delay before retrying
  }
}

void sendPFCommand(byte address, byte command)
{
  for (int i = 0; i < 3; i++)
  { // Retry mechanism
    myWire.beginTransmission(address);
    myWire.write(command);
    byte error = myWire.endTransmission();

    if (error == 0)
    {
      Serial.print("Sent PF clear command to address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      return; // Exit the function if successful
    }
    else
    {
      Serial.print("Error sending PF clear command to address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }

    delay(2000); // Delay before retrying
  }
}

void setup()
{
  myWire.begin(); // Initialize SoftwareWire with SDA on pin 6 and SCL on pin 5
  Serial.begin(9600);
  while (!Serial)
    ; // Wait for Serial to be ready
  Serial.println("I2C Command Sender");

  // Attempt to read the serial number and other information
  readBatteryInfo(deviceAddress);

  // Continuously send unseal and PF clear commands
  for (int i = 0; i < 5; i++)
  { // Adjust the number of iterations as needed
    sendUnsealCommand(deviceAddress, UNSEAL_KEY_1);
    delay(3000); // Increased delay between unseal commands
    sendUnsealCommand(deviceAddress, UNSEAL_KEY_2);
    delay(3000); // Increased delay before sending PF clear command
    sendPFCommand(deviceAddress, CLEAR_PF_COMMAND);
    delay(3000); // Delay before next iteration
  }
}

void loop()
{
}
