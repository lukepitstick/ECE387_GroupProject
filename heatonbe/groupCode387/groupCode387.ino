//card 1: 7E 00 1F F6 B4
//card 2: 7E 00 1F D9 32
//card 3: 7E 00 20 3B C6
//card 4: 7E 00 20 3C B0

//#include <USER.h>
#include <LiquidCrystal.h>

//card values
byte ref1[] = {0x7E,0x00,0x1F,0xF6,0xB4};
byte ref2[] = {0x7E,0x00,0x1F,0xD9,0x32};
byte ref3[] = {0x7E,0x00,0x20,0x3B,0xC6};
byte ref4[] = {0x7E,0x00,0x20,0x3C,0xB0};

//define LED pins
const int R = 13; //DONE 
const int G = 11;
const int B = 12;
const int Y = 10;

const int RGB_R = 7; //DONE
const int RGB_G = 8;
const int RGB_B = 9;

//LCD Pins
LiquidCrystal lcd(51, 49, 47, 45, 43, 41); //CHANGE!!!!
const int contrastPin = 6 ; //PWM

//Hook Pins (BOOLEANS)
boolean hR = false;
boolean hB = false;
boolean hG = false;
boolean hY = false;
boolean dO = false; //doorOpen


//Analog (HOOK SWITCHES)
const char sY = A0; //DONE //PA??
const char sG = A1; //were ints
const char sB = A2;
const char sR = A3;

  
//Keypad pins

//other pins
const int striker = 22; //DONE
//const int speaker = 23; //WIRE

//Constants
const int onTime = 2000; //PROBLEM AREA?
const int lockTime = 5000;
const int unlockTime = 7000;

//operating variables
boolean houseEmpty = false;
boolean locked = false; //change?

void setup() {
  //LCD
//  lcd.begin(16, 2);
//  pinMode(contrastPin, OUTPUT);
//  analogWrite(contrastPin, 100);

  pinMode(striker, OUTPUT);
 // pinMode(speaker, OUTPUT); //CHANGE

  //LED's
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(RGB_R, OUTPUT);  
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  Serial.begin(9600);  // connect to the serial port
}

void loop () {
  byte i = 0;
  byte val = 0;
  byte code[6];
  byte checksum = 0;
  byte bytesread = 0;
  byte tempbyte = 0;
  boolean verified = false;
  boolean scanned = false;
//  Serial.println(digitalRead(sR));
//  delay(200);
//  lcd.clear();
//  lcd.home();
//  lcd.print("ACCESS DENIED");
  
  // HOOK STATES DECLARED; LEDs ON/OFF//RED
  //RED
  if(digitalRead(sR)) { //OFF
    digitalWrite(R, LOW);
    hR = false;
  }
  else { //ON
    hR = true;
    digitalWrite(R, HIGH);
  }
  //BLUE
  if(digitalRead(sB)) { //OFF
    digitalWrite(B, LOW);
    hB = false;
  }
  else { //ON
    hB = true;
    digitalWrite(B, HIGH);
  }
  //GREEN
  if(digitalRead(sG)) { //OFF
    digitalWrite(G, LOW);
    hG = false;
  }
  else { //ON
    hG = true;
    digitalWrite(G, HIGH);
  }
  //YELLOW
  if(digitalRead(sY)) { //OFF
    digitalWrite(Y, LOW);
    hY = false;
  }
  else { //ON
    hY = true;
    digitalWrite(Y, HIGH);
  }
  

  if(hR || hB || hG || hY){//HOOK CONDITION
    houseEmpty = false;
//    unlock();
  }
  else{
//    lock();
    houseEmpty = true;
  }

  //RFID Read
  if(Serial.available() > 0) {
    if((val = Serial.read()) == 2) {                  // check for header 
      bytesread = 0; 
      while (bytesread < 12) {                        // read 10 digit code + 2 digit checksum
        if( Serial.available() > 0) { 
          val = Serial.read();
          if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) { // if header or stop bytes before the 10 digit reading 
            break;                                    // stop reading
          }

          // Do Ascii/Hex conversion:
          if ((val >= '0') && (val <= '9')) {
            val = val - '0';
          } else if ((val >= 'A') && (val <= 'F')) {
            val = 10 + val - 'A';
          }

          // Every two hex-digits, add byte to code:
          if (bytesread & 1 == 1) {
            // make some space for this hex-digit by
            // shifting the previous hex-digit with 4 bits to the left:
            code[bytesread >> 1] = (val | (tempbyte << 4));

            if (bytesread >> 1 != 5) {                // If we're at the checksum byte,
              checksum ^= code[bytesread >> 1];       // Calculate the checksum... (XOR)
            };
          } else {
            tempbyte = val;                           // Store the first hex digit first...
          };

          bytesread++;                                // ready to read next digit
        } 
      } 

      //RFID has been read, process
      if (bytesread == 12) {         // if 12 digit read is complete
        verified = verifyRFID(code,checksum);
        scanned = true;
//        Serial.print(verified);
        bytesread = 0;
      }
    }
  }//end of RFID read

  if(scanned) {
    if(verified & houseEmpty){
      unlock();
  //    lcd.clear();
  //    lcd.home();
  //    lcd.print("WELCOME");
      delay(unlockTime);
    }
    else if(verified & !houseEmpty){
      Serial.println("aready unlocked");
  //    lcd.clear();
  //    lcd.home();
  //    lcd.print("HOUSE IS ALREADY UNLOCKED");
    }
    else if(!verified){
  //    lcd.clear();
  //    lcd.home();
  //    lcd.print("ACCESS DENIED")
    }
  }//end of scanned
  else{
    if(houseEmpty){
      if(!locked){
        delay(lockTime);
        lock();
      }
    }
    else{
      unlock(); //may be redundant
    }
  }
}//end of loop

void unlock(){
  digitalWrite(striker,HIGH);
  locked = false;
}

void lock(){
  digitalWrite(striker,LOW);
  locked = true;
}  

boolean verifyRFID(byte* code, int checksum){
  boolean flag1 = true;
  boolean flag2 = true;
  boolean flag3 = true;
  boolean flag4 = true;

//  Serial.print("5-byte code: ");
  for (int i=0; i<5; i++) {
//    if (code[i] < 16) Serial.print("0");
    if (code[i] != ref1[i]) flag1 = false;
    if (code[i] != ref2[i]) flag2 = false;
    if (code[i] != ref3[i]) flag3 = false;
    if (code[i] != ref4[i]) flag4 = false;
//    Serial.print("code: ");
//    Serial.print(code[i], HEX);
//    Serial.print(" ");

  }
  if(flag1){
    return true;
  }
  if(flag2){
    return false;
  }
  if(flag3){
    return true;
  }
  if(flag4){
    return true;
  }
}