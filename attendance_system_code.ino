#include <SPI.h>
#include <MFRC522.h> //library for rfid sensor
#include <RTClib.h> //library for rtc module
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //library for i2c module
#include <Servo.h> //library for servo motor

// Define pins for rfid sensor
#define SS_PIN 10
#define RST_PIN 9

Servo myservo;  // Create servo object to control a servo.
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
RTC_DS3231 rtc; // Instance of the class for RTC
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Instance for lcd display

const int numOfCards = 2; // The number of cards used. This can change as you want
byte cards[numOfCards][4] = {{0x8E, 0x91, 0xB5, 0x7D}, {0x01, 0x1E, 0x06, 0x26}}; // Array of UIDs of RFID cards
int n = 0; // The total number of students
int numCard[numOfCards]; // This array contains the details of cards that have already been detected or not.
String names[numOfCards] = {"Salma El Gohary", "Nour Badr"}; // Student names
long sNumbers[numOfCards] = {975550, 978035}; // Student IDs

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();  // Initialize SPI bus
  mfrc522.PCD_Init(); // Initialize MFRC522 card

  // Setup for LCD screen
  lcd.begin(20, 4); // Initialize the LCD

  myservo.attach(6);  // Attach the servo on pin 6

  // Setup for RTC module
  #ifndef ESP8266
  while (!Serial); // For Leonardo/Micro/Zero
  #endif
  if (!rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  } else {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Time will set automatically when it compiles
    // rtc.adjust(DateTime(2020, 1, 9, 16, 59, 45)); // Manually set time
  }
  if (rtc.lostPower()) {
    Serial.println(F("RTC lost power, let's set the time!"));
  }

  // Display initialization
  Serial.println("CLEARSHEET");
  Serial.println(F("LABEL,SCount,N/A,SNo,Name,Date,TimeIn,TimeOut"));
}

void readRFID() {
  int j = -1;
  byte card_ID[4]; // Card UID size 4 bytes

  if (!mfrc522.PICC_IsNewCardPresent()) { // Look for new card
    return; // If no card is present, return
  }

  if (!mfrc522.PICC_ReadCardSerial()) { // Select one of the cards
    return; // If reading the card serial fails, return
  }

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    card_ID[i] = mfrc522.uid.uidByte[i];
  }

  for (int i = 0; i < numOfCards; i++) {
    if (card_ID[0] == cards[i][0] && card_ID[1] == cards[i][1] && card_ID[2] == cards[i][2] && card_ID[3] == cards[i][3]) {
      j = i;
    }
  }

  if (j == -1) { // Check the card validity
    invalid();
  } else if (numCard[j] == 1) { // Check if the card has already been detected
    alreadyRead(names[j], sNumbers[j], j);
  } else {
    // Send data to display and log it
    logCardData(names[j], sNumbers[j], j);
  }
  delay(1000);
}

void servo() { // Method for servo motor
  int pos = 0; // Variable to store the servo position
  for (pos = 90; pos >= 0; pos -= 1) { // Goes from 0 degrees to 90 degrees
    myservo.write(pos);
    delay(5);
  }
  delay(5000);
  for (pos = 0; pos <= 90; pos += 1) { // Goes from 90 degrees to 0 degrees
    myservo.write(pos);
    delay(5);
  }
}

void logCardData(String name, long sNumber, int j) {
  displayAllow(name, sNumber);
  DateTime now = rtc.now();
  numCard[j] = 1; // Mark the card as detected
  n++; // Increment the student count

  // Display details to the console (serial monitor)
  Serial.print("DATA,");
  Serial.print(n); // Print number
  Serial.print(",");
  Serial.print(sNumber); // Print student ID
  Serial.print(",");
  Serial.print(name); // Print student name
  Serial.print(",");
  Serial.print(now.year(), DEC); // Print year
  Serial.print(F("-"));
  if (now.month() < 10) { Serial.print("0"); Serial.print(now.month(), DEC); }
  else Serial.print(now.month(), DEC); // Print month
  Serial.print(F("-"));
  if (now.day() < 10) { Serial.print("0"); Serial.print(now.day(), DEC); }
  else Serial.print(now.day(), DEC); // Print day
  Serial.print(",");
  if (now.hour() < 10) { Serial.print("0"); Serial.print(now.hour(), DEC); }
  else Serial.print(now.hour(), DEC); // Print hour
  Serial.print(F(":"));
  if (now.minute() < 10) { Serial.print("0"); Serial.print(now.minute(), DEC); }
  else Serial.print(now.minute(), DEC); // Print minute
  Serial.print(F(":"));
  if (now.second() < 10) { Serial.print("0"); Serial.print(now.second(), DEC); }
  else Serial.print(now.second(), DEC); // Print second
  Serial.println(" ");
}

void invalid() {
  Serial.println(F("Invalid Card."));
  lcd.home();
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print(F("Warning!"));
  lcd.setCursor(4, 2);
  lcd.print(F("Invalid Card!"));
  lcd.setCursor(3, 3);
  lcd.print(F("Access Denied!"));
  delay(3000);
  lcd.clear();
}

void alreadyRead(String name, long sNumber, int j) {
  DateTime now = rtc.now();
  numCard[j] = 0; // Reset card detection status
  n--; // Decrease the student count

  // Display details to the console (serial monitor)
  Serial.print("DATA,");
  Serial.print(n); // Print number
  Serial.print(",");
  Serial.print(sNumber); // Print student ID
  Serial.print(",");
  Serial.print(name); // Print student name
  Serial.print(",");
  Serial.print(now.year(), DEC); // Print year
  Serial.print(F("-"));
  if (now.month() < 10) { Serial.print("0"); Serial.print(now.month(), DEC); }
  else Serial.print(now.month(), DEC); // Print month
  Serial.print(F("-"));
  if (now.day() < 10) { Serial.print("0"); Serial.print(now.day(), DEC); }
  else Serial.print(now.day(), DEC); // Print day
  Serial.print(",");
  Serial.print(F("--:--:--"));
  Serial.print(",");
  if (now.hour() < 10) { Serial.print("0"); Serial.print(now.hour(), DEC); }
  else Serial.print(now.hour(), DEC); // Print hour
  Serial.print(F(":"));
  if (now.minute() < 10) { Serial.print("0"); Serial.print(now.minute(), DEC); }
  else Serial.print(now.minute(), DEC); // Print minute
  Serial.print(F(":"));
  if (now.second() < 10) { Serial.print("0"); Serial.println(now.second(), DEC); }
  else Serial.print(now.second(), DEC);
  Serial.println(" ");

  lcd.home();
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print(F("Thank You"));
  lcd.setCursor(4, 2);
  lcd.print(F("Come Again!"));
  delay(3000);
  servo();
  lcd.clear();
}

void printLoopLCD() { // Method for displaying date, time, and number of students to LCD
  DateTime now = rtc.now();
  lcd.home();
  lcd.setCursor(1, 0);
  lcd.print(F("Welcome Students!"));
  lcd.setCursor(0, 1);
  lcd.print(now.year(), DEC); // Print year
  lcd.print(F("-"));
  if (now.month() < 10) { lcd.print("0"); lcd.print(now.month(), DEC); }
  else lcd.print(now.month(), DEC); // Print month
  lcd.print(F("-"));
  if (now.day() < 10) { lcd.print("0"); lcd.print(now.day(), DEC); }
  else lcd.print(now.day(), DEC); // Print day
  lcd.setCursor(12, 1);
  if (now.hour() < 10) { lcd.print("0"); lcd.print(now.hour(), DEC); }
  else lcd.print(now.hour(), DEC); // Print hour
  lcd.print(F(":"));
  if (now.minute() < 10) { lcd.print("0"); lcd.print(now.minute(), DEC); }
  else lcd.print(now.minute(), DEC); // Print minute
  lcd.print(F(":"));
  if (now.second() < 10) { lcd.print("0"); lcd.print(now.second(), DEC); }
  else lcd.print(now.second(), DEC); // Print second
  lcd.setCursor(0, 3);
  lcd.print(F("No of Students:"));
  lcd.setCursor(16, 3);
  lcd.print(n);
  delay(1000);
}

void displayAllow(String name, long sNumber) {
  lcd.home();
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print(F("Welcome!"));
  lcd.setCursor(1, 1);
  lcd.print(F("Authorized Access"));
  lcd.setCursor(0, 2);
  lcd.print(F("Name:"));
  lcd.setCursor(5, 2);
  lcd.print(name);
  lcd.setCursor(0, 3);
  lcd.print(F("SNo :"));
  lcd.setCursor(6, 3);
  lcd.print(sNumber);
  delay(3000);
  servo();
  lcd.clear();
}

void checkTime() {
  // Check if the time is 6pm, the system will stop working (time out)
  lcd.home();
  lcd.clear();

  if (n != 0) { // Check if the number of students is 0
    Serial.println(F("Warning!"));
    Serial.print(F("Count not zero!"));
    lcd.setCursor(6, 1);
    lcd.print(F("Warning!"));
    lcd.setCursor(3, 2);
    lcd.print(F("Count not zero!"));
    delay(5000);
  }
  //else {
    Serial.println(F("Session has expired!"));
    Serial.print(F("No of students :"));
    Serial.println(n);
    lcd.home();
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print(F("Library"));
    lcd.setCursor(0, 1);
    lcd.print(F("Session has expired!"));
    lcd.setCursor(0, 3);
    lcd.print(F("No of students :"));
    lcd.print(n);
  // }

  delay(46800000); // Delay for 12 hours
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();
  if (now.hour() < 7 || now.hour() >= 18) {
    checkTime();
  }
  printLoopLCD(); // Print details on LCD screen (loop)
  readRFID(); // Read RFID card
}
