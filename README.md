# Attendance System with RFID and Time Tracking

## Project Description
This project is a fully automated student attendance system using RFID cards, an RTC module, a servo motor and a liquid crystal display (LCD). It reads RFID cards to record student attendance, log sign-in and sign-out times, and push the data into an Excel sheet using PLX-DAQ software for easy access and analysis. The system uses a microcontroller (Arduino) to interface with the RFID reader, RTC, LCD, and servo motor, providing a streamlined solution for managing student attendance.

## Hardware Requirements
- **Arduino UNO**
- **MFRC522 RFID Reader**
- **DS3231 RTC Module**
- **LiquidCrystal I2C Display**
- **Servo Motor**

## Software Requirements
- **PLX-DAQ**: Software used to transfer data from Arduino to Excel.
- **Arduino IDE**: Used for programming the Arduino.
- **Libraries**: 
  - `SPI.h`
  - `Wire.h`
  - `MFRC522.h` (for RFID)
  - `RTClib.h` (for RTC module)
  - `LiquidCrystal_I2C.h` (for LCD)
  - `Servo.h` (for Servo Motor control)

## Features
- **RFID-based Attendance**: Each student is identified by a unique RFID card, and their attendance is logged in the system.
- **Real-Time Clock Integration**: The system logs the current date and time of sign-in and sign-out using the DS3231 RTC module.
- **Servo Motor**: A servo motor is used to control access, allowing students to enter once their card is successfully scanned.
- **Excel Data Logging**: PLX-DAQ is used to push the attendance data into an Excel sheet for easy tracking and management.
  - Data recorded includes:
    - **Student Name**
    - **Student ID**
    - **Date**
    - **Sign-in Time**
    - **Sign-out Time**
    - **RFID Card ID**

## System Workflow
1. **Initialization**: The system initializes the RFID reader, RTC module, LCD, and servo motor.
2. **RFID Card Detection**: When a valid RFID card is detected, the system logs the student’s details, date, and time.
3. **Sign-in and Sign-out**: If a student signs in, their data is recorded with the current time. If they attempt to sign in again, it logs a "sign-out" time, and the system marks the student as leaving.
4. **Excel Logging with PLX-DAQ**: PLX-DAQ listens for serial data from the Arduino and logs it in an Excel sheet. Each entry includes student details, date, and time.

## Code Overview

### Arduino Code
The Arduino code is responsible for:
- Reading the RFID card data.
- Retrieving the current date and time from the RTC module.
- Managing the servo motor for access control.
- Pushing data to the serial output, formatted for PLX-DAQ to log into Excel.

### RFID Card Code
The RFID code is used to recognize student cards and check if the student is already signed in or out.

### PLX-DAQ Integration
- PLX-DAQ is configured to listen to the serial output from the Arduino.
- Data from the Arduino (student name, ID, date, time) is sent via the serial port and automatically logged into an Excel sheet.
- The system will display the student's name and ID, as well as the sign-in time, sign-out time, and current session count in Excel.

## Setup Instructions

### Arduino Setup
1. **Connect the Components**:
   - Connect the **MFRC522 RFID Reader** to the Arduino according to the specified pins in the code.
   - Connect the **RTC DS3231 Module** to the Arduino.
   - Connect the **LCD** via I2C.
   - Connect the **Servo Motor** to control access.

2. **Install Necessary Libraries**:
   - Install the `SPI.h`, `Wire.h`, `MFRC522.h`, `RTClib.h`, `LiquidCrystal_I2C.h`, and `Servo.h` libraries in the Arduino IDE.
  
3. **Upload the Code**:
   - Upload the Code: Upload the provided `attendance_system_code.ino` and `rfid_card_code.ino` to your Arduino.
   - Note: The system is pre-configured with two cards. Modify the `cards` array to add more cards.

### PLX-DAQ Setup
1. **Download and Install PLX-DAQ**:
   - Download PLX-DAQ from the official site and install it on your computer.
   
2. **Connect Arduino to Computer**:
   - Connect the Arduino via USB to the computer where PLX-DAQ is running.

3. **Run PLX-DAQ**:
   - Open PLX-DAQ and configure it to listen to the correct serial port where your Arduino is connected.
   - Start the communication to begin logging data to Excel.

4. **Data Logging**:
   - When a student scans their RFID card, the data (name, student ID, date, sign-in and sign-out times) will be automatically logged into the Excel sheet.

## Example Data in Excel
| LABEL  | SCount | N/A | SNo    | Name            | Date       | TimeIn  | TimeOut |
|--------|--------|-----|--------|-----------------|------------|---------|---------|
| DATA   | 1      | N/A | 975550 | Salma El Gohary | 2025-01-01 | 10:30:00| 10:35:00|
| DATA   | 2      | N/A | 978035 | Nour Badr       | 2025-01-01 | 11:00:00| 11:05:00|

## How to Use
1. **Sign-In**: A student presents their RFID card. The system logs the sign-in time.
2. **Sign-Out**: A student presents their card again, and the system logs the sign-out time.
3. **PLX-DAQ**: PLX-DAQ ensures all data is pushed into Excel for tracking and analysis.

## Notes
- Make sure the RFID cards' UIDs are correctly set in the code.
- Ensure that PLX-DAQ is configured to listen to the correct COM port.

## Challenges
1. **Servo Malfunction**
   - Initially, the servo motor would not move during sign-in or sign-out. After troubleshooting, I discovered an issue with the wiring. We reconnected the wires properly and reviewed the code to ensure it was controlling the servo correctly.
2. **RFID Data Not Saving to Excel**
   - At first, the RFID data wasn't being transferred to the Excel sheet. I carefully reviewed the code and removed unnecessary sections to simplify it, ensuring that the correct data was being sent to the PLX-DAQ software.
3. **Incorrect Time Display**
   - Initially, the system only displayed the time since the code started rather than the current time. I experimented with multiple libraries but couldn’t get it to work. After further debugging, I fixed the code and successfully displayed the correct time on the Excel sheet.

## Skills
1. **Embedded Systems Programming**
   - I learned how to connect and program various devices like RFID readers, LCDs, and servo motors using Arduino.
     
3. **Data Logging**
   - I successfully used PLX-DAQ to send data from Arduino to Excel and ensure accurate attendance tracking.
4. **Circuit Design & Troubleshooting**
   - I designed and tested the circuits connecting all the hardware components, making sure everything worked correctly.
     
4. **Problem Solving & Debugging**
   - I fixed issues in the system, such as card detection problems and motor malfunctions, by troubleshooting and finding solutions.

## Future Improvements
1. **Better RFID Reader**
   - A reader with a larger detection range would make the system more user-friendly and improve the overall process.
     
2. **Portable Power Option**
   - Making the system battery-powered or compatible with a portable charger would make it more convenient and flexible to use anywhere.
     
3. **Database for Larger Scale Implementation**
   - For larger setups, like in schools or businesses, using a database to store RFID card info would eliminate the need to change the code each time a new card is added, making the system more efficient and practical.
