# Unbrick DJI Mavic Battery

This readme file provides detailed instructions on how to use the provided Arduino Uno R3 script to unbrick a DJI Mavic battery by overriding the PF flag. This script is designed for the Arduino Uno R3 but may also work with other Arduino boards (although it has not been tested on them).

## How It Works

The script utilizes the SoftwareWire library to communicate with the battery via I2C. It performs the following steps:

1. **Initialization**:

   - Initializes the SoftwareWire library and starts the serial communication.
   - Defines various Smart Battery commands and sets up the I2C communication with the battery.

2. **Reading Battery Information**:

   - Reads and prints battery information, including the serial number, cycle count, voltage, design capacity, manufacture date, state of health, and more.

3. **Unsealing the Battery**:

   - Sends the custom unseal command to the battery using the defined unseal keys. This allows access to protected battery features.

4. **Clearing the PF Flag**:

   - Sends the PF clear command to clear the PF flag, which effectively unbricks the battery.

5. **Iteration**:
   - Repeats the unseal and PF clear commands for a specified number of iterations (adjustable in the script).

## Usage

To use this script, follow these steps:

1. **Hardware Setup**:

   - Connect your Arduino Uno R3 or a compatible board to the DJI Mavic battery using the appropriate I2C pins (SDA on pin 6 and SCL on pin 5).

2. **Upload Script**:

   - Upload the provided script to your Arduino board.

3. **Serial Monitor**:

   - Open the serial monitor at a baud rate of 9600.

4. **Power On**:

   - Power on the DJI Mavic battery.

5. **Execution**:
   - The script will:
     1. Read and display battery information.
     2. Send the unseal and PF clear commands.
     3. Repeat the process for the specified number of iterations.

## Additional Notes

- If the battery is completely discharged, you might need a bench power supply to charge it slightly before running the script. You may also need to bypass the BMS by opening the battery and connecting the bench power supply directly to the battery terminals. A power supply set to around 16.2 volts and 1 ampere worked in some cases.
- Modifying the script or using it with other Arduino boards may require adjustments to the pin assignments and I2C communication settings.

**Important**: Using this script involves risk and should be done with caution. Ensure you understand the implications of sending commands to your battery and handle all components safely.

---

Please make sure to read through and understand these instructions thoroughly before attempting to use the script. If you have any questions or run into issues, seek assistance from a knowledgeable source.
