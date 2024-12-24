#include <SPI.h>
#include <MFRC522.h> //library for rfid sensor
#include <RTClib.h> //library for rtc module
#include <SD.h> //library for sd card module
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //library for i2c module
#include <Servo.h> //library for servo motor



//define pins for rfid sensor
#define SS_PIN 10
#define RST_PIN 9
//define select pin for SD card module
#define CS_SD 4




Servo myservo;  // create servo object to control a servo.
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
RTC_DS3231 rtc;// Instance of the class for RTC
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);//instance for lcd display




const int numOfCards = 2;//the nuber of cards used. this can change as you want
byte cards[numOfCards][4] = {{0x8E, 0x91, 0xB5, 0x7D},{0x01, 0x1E, 0x06, 0x26}}; // array of UIDs of rfid cards
int n = 0;//n is for the total number of students//j is for to detect the card is valid or not
int numCard[numOfCards]; //this array content the details of cards that already detect or not .
String names[numOfCards] = {"Salma El Gohary","Nour Badr"};//student names
long sNumbers[numOfCards] = {975550,978035};//student sNumbers




void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
 


  //setup for lcd screen
  lcd.begin(20,4);// initialize the lcd




  myservo.attach(6);  // attaches the servo on pin 9 to the servo object




  //setup for RTC module
  #ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
  #endif
  if (! rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  }
  else{
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//time will set auto when it compiles
    //rtc.adjust(DateTime(2020, 1, 9, 16, 59, 45));// manually set time
  }
  if (rtc.lostPower()) {
    Serial.println(F("RTC lost power, lets set the time!"));
  }




  //setup for display option (serial monitor)
  Serial.println("CLEARSHEET");
  Serial.println(F("LABEL,SCount,N/A,SNo,Name,Date,TimeIn,TimeOut"));// make four columns




  //setup sd card
//  Serial.print(F("Initializing SD card..."));
  digitalWrite(CS_SD,LOW);
}




void readRFID(){
  int j = -1;
  byte card_ID[4];//card UID size 4byte
 
  if ( ! mfrc522.PICC_IsNewCardPresent()) {//look for new card
    return;//got to start of loop if there is no card present
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {// Select one of the cards
    return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
  }




  for (byte i = 0; i < mfrc522.uid.size; i++) {
    card_ID[i] = mfrc522.uid.uidByte[i];
  }
 
  for (int i = 0; i < numOfCards; i++) {
    if (card_ID[0] == cards[i][0] && card_ID[1] == cards[i][1] && card_ID[2] == cards[i][2] && card_ID[3] == cards[i][3]) {
      j = i;
    }
  }
 
  if(j == -1) {//check the card validity
    invalid();
  }
  else if (numCard[j] == 1) { //to check if the card already detect
    alreadyRead(names[j], sNumbers[j], j);
  }
  else {
    //send data to display and save
    logCardData(names[j], sNumbers[j], j);
  }
  delay(1000);
}




void servo(){//method for servo motor
  int pos = 0;    // variable to store the servo position
   for (pos = 90; pos >= 0; pos -= 1) { // goes from 0 degrees to 90 degrees
    // in steps of 1 degree
    myservo.write(pos);              
    delay(5);                      
  }
  delay(5000);
   for (pos = 0; pos <= 90; pos += 1) {// goes from 90 degrees to 0 degrees
    myservo.write(pos);            
    delay(5);                      
  }
}




void logCardData(String name, long sNumber, int j){
  displayAllow(name,sNumber);
  DateTime now = rtc.now();
  numCard[j] = 1;//put 1 in the numCard array : numCard[j]={1,1} to let the arduino know if the card was detecting
  n++;//to get the count




  //display details to the console (serial monitor)
  Serial.print("DATA,");
  Serial.print(n);
  Serial.print(",");//print number
  Serial.print(",");
  Serial.print(sNumber); //print name of student
  Serial.print(",");
  Serial.print(name); //print name of student
  Serial.print(",");
  Serial.print(now.year(),DEC); //print year
  Serial.print(F("-"));
  if(now.month()<10){Serial.print("0");Serial.print(now.month(),DEC);}
  else Serial.print(now.month(),DEC); //print month
  Serial.print(F("-"));
  if(now.day()<10){Serial.print("0");Serial.print(now.day(),DEC);} //if the day is one digit this will display it with zero in front as two digits.
  else Serial.print(now.day(),DEC);
  Serial.print(",");
  if(now.hour()<10){Serial.print("0");Serial.print(now.hour(),DEC);}
  else Serial.print(now.hour(),DEC);
  Serial.print(F(":"));
  if(now.minute()<10){Serial.print("0");Serial.print(now.minute(),DEC);}
  else Serial.print(now.minute(),DEC);
  Serial.print(F(":"));
  if(now.second()<10){Serial.print("0");Serial.print(now.second(),DEC);}
  else Serial.print(now.second(),DEC);
  Serial.print(",");
  Serial.println(F("--:--:-- "));




}




void invalid(){
  Serial.println(F("Invalid Card."));
  digitalWrite(CS_SD,LOW);
  File  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(F("Invalid Card."));
    myFile.close();
    Serial.println(F("done."));
  } else {
    // Serial.println(F("error opening test.txt"));
  }
  digitalWrite(CS_SD,LOW);
 
  lcd.home ();
  lcd.clear();
  lcd.setCursor (6, 0 );// go home
  lcd.print(F("Warning!"));
  lcd.setCursor (4, 2 );
  lcd.print(F("Invalid Card!"));
  lcd.setCursor (3, 3 );
  lcd.print(F("Access Denied!"));
  delay(3000);
  lcd.clear();
}




void alreadyRead(String name, long sNumber, int j){
  DateTime now = rtc.now();
  numCard[j] = 0;//put 1 in the numCard array : numCard[j]={1,1} to let the arduino know if the card was detecting
  n--;//to get the count
 
  //display details to the console (serial monitor)
  Serial.print("DATA,");
  Serial.print(n);
  Serial.print(",");//print number
  Serial.print(",");
  Serial.print(sNumber); //print name of student
  Serial.print(",");
  Serial.print(name); //print name of student
  Serial.print(",");
  Serial.print(now.year(),DEC); //print year
  Serial.print(F("-"));
  if(now.month()<10){Serial.print("0");Serial.print(now.month(),DEC);}
  else Serial.print(now.month(),DEC); //print month
  Serial.print(F("-"));
  if(now.day()<10){Serial.print("0");Serial.print(now.day(),DEC);}
  else Serial.print(now.day(),DEC);
  Serial.print(",");
  Serial.print(F("--:--:--"));
  Serial.print(",");
  if(now.hour()<10){Serial.print("0");Serial.print(now.hour(),DEC);}
  else Serial.print(now.hour(),DEC);
  Serial.print(F(":"));
  if(now.minute()<10){Serial.print("0");Serial.print(now.minute(),DEC);}
  else Serial.print(now.minute(),DEC);
  Serial.print(F(":"));
  if(now.second()<10){Serial.print("0");Serial.println(now.second(),DEC);}
  else Serial.print(now.second(),DEC);
  Serial.println(" ");






  lcd.home ();
  lcd.clear();
  lcd.setCursor (5, 1 );// go home
  lcd.print(F("Thank You"));
  lcd.setCursor (4, 2 );
  lcd.print(F("Come Again!"));
  delay(3000);
  servo();
  lcd.clear();
}




void printLoopLCD(){//method for display date and time and number of students attendance to the lcd dislpay
  DateTime now = rtc.now();
  lcd.home ();// go home
  lcd.setCursor (1, 0);
  lcd.print(F("Welcome Students!"));  
  lcd.setCursor (0, 1 );
  lcd.print(now.year(),DEC); //print year+
  lcd.print(F("-"));
  if(now.month()<10){lcd.print("0");lcd.print(now.month(),DEC);}
  else lcd.print(now.month(),DEC); //print month
  lcd.print(F("-"));
  if(now.day()<10){lcd.print("0");lcd.print(now.day(),DEC);}
  else lcd.print(now.day(),DEC);
  //lcd.print(now.day(),DEC); //print date
  lcd.setCursor (12, 1 );
  if(now.hour()<10){lcd.print("0");lcd.print(now.hour(),DEC);}
  else lcd.print(now.hour(),DEC);
  //lcd.print(now.hour(),DEC);
  lcd.print(F(":"));
  if(now.minute()<10){lcd.print("0");lcd.print(now.minute(),DEC);}
  else lcd.print(now.minute(),DEC);
  //lcd.print(now.minute(),DEC);
  lcd.print(F(":"));
  if(now.second()<10){lcd.print("0");lcd.print(now.second(),DEC);}
  else lcd.print(now.second(),DEC);
  //lcd.print(now.second(),DEC);
  lcd.setCursor (0, 3 );
  lcd.print(F("No of Students:"));
  lcd.setCursor (16, 3 );
  lcd.print(n);
  delay(1000);
}




void displayAllow(String name,long sNumber){
  lcd.home ();
  lcd.clear();
  lcd.setCursor (6, 0 );
  lcd.print(F("Welcome!"));  
  lcd.setCursor (1, 1 );
  lcd.print(F("Authorized Access"));
  lcd.setCursor (0, 2 );
  lcd.print(F("Name:"));
  lcd.setCursor (5, 2 );
  lcd.print(name);
  lcd.setCursor (0, 3 );
  lcd.print(F("SNo :"));
  lcd.setCursor (6, 3 );
  lcd.print(sNumber);
  delay(3000);
  servo();
  lcd.clear();
}




void checkTime(){
  //check the time if the time is 6pm the system will stop working (time out)
    lcd.home ();
    lcd.clear();
   
    if(n != 0){ //check the nuber of student is 0 or not
      Serial.println(F("Warning!"));
      Serial.print(F("Count not zero!"));
      lcd.setCursor (6, 1 );
      lcd.print(F("Warning!"));
      lcd.setCursor (3, 2 );
      lcd.print(F("Count not zero!"));
      delay(5000);
     }
     //else{
      Serial.println(F("Session has expired!"));
      Serial.print(F("No of students :"));
      Serial.println(n);
      lcd.home ();
      lcd.clear();
      lcd.setCursor (6, 0 );
      lcd.print(F("Library"));
      lcd.setCursor (0, 1 );
      lcd.print(F("Session has expired!"));  
      lcd.setCursor (0, 3 );
      lcd.print(F("No of students :"));
      lcd.print(n);
 //    }    
         
    delay(46800000);//delay for 12 hours
    lcd.clear();
}




void loop() {
  DateTime now = rtc.now();
  if (now.hour() < 7 || now.hour() >= 18) {
    checkTime();
  }
  printLoopLCD();//print details on lcd screen(loop)
  readRFID();
}
