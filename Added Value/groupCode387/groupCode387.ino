//card 1: 7E 00 1F F6 B4
//card 2: 7E 00 1F D9 32
//card 3: 7E 00 20 3B C6
//card 4: 7E 00 20 3C B0

//#include <USER.h>
#include <LiquidCrystal.h>

//card values
byte ref1[] = {0x7E, 0x00, 0x1F, 0xF6, 0xB4};
byte ref2[] = {0x7E, 0x00, 0x1F, 0xD9, 0x32};
byte ref3[] = {0x7E, 0x00, 0x20, 0x3B, 0xC6};
byte ref4[] = {0x7E, 0x00, 0x20, 0x3C, 0xB0};

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
const int speaker = 23;

//Constants
const int onTime = 2000; //PROBLEM AREA?
const int lockTime = 5000;
const int unlockTime = 7000;
const int doorTime = 10000; //change to 10000 after debug
int lockmillis;
unsigned long doormillis;

//operating variables
boolean houseEmpty = false;
boolean locked = true; //change?
boolean lockFlag = true;
boolean doorFlag = true;
boolean ajarDone = true;
int speakertimetmp = 0;
int user;

void setup() {
  //LCD
  lcd.begin(16, 2);
  lcd.print("Apt. 387");
  pinMode(contrastPin, OUTPUT);
  analogWrite(contrastPin, 40);

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
  Serial1.begin(9600);
  
  color(255,0,0);
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
  //  lcd.clear();
  //  lcd.home();
  //  lcd.print("ACCESS DENIED");

  //lcd.print("Apt. 387");
  //
  //  if(lockFlag) {
  //    lcd.clear();
  //    lcd.home();
  //    lcd.print("Apt. 387");
  //  }


  int door = digitalRead(A5);
  Serial.println(door);
  if (!door && ajarDone) {
    doorFlag = true;
  } //reset door ajar timer when door is closed
  else {
    if (doorFlag) {
      doormillis = millis();
      doorFlag = false;
      ajarDone = false;
    }
    else {
      int timetmpD = (millis() - doormillis);
      //Serial.println("doormillis: "+doormillis);
      Serial.print("timetmpDoor: ");
      Serial.println(timetmpD);
      if (timetmpD > doorTime) {
        if (timetmpD < doorTime + 100) {
          lcd.clear();
          lcd.home();
          lcd.print("DOOR AJAR");
          Serial.println("door ajar ONCE");
          color(255,255,255);
        }
        if (!door) {
          doorFlag = true;
          ajarDone = true;
          noTone(speaker);
          lcd.clear();
          lcd.home();
          lcd.print("Apt. 387"); //done with loop
          if(locked) {
            color(255,0,0);
          }else{
            color(0,0,255);
          }
        }
        else {
          //sound
          tone(speaker, 3000);
        }
      }
    }
  }
  //RGB LEDs
  if (!houseEmpty && millis() > 100 && !door) {
    color(0, 255, 0);
  }
//  else if (locked) {
//    color(255, 0, 0);
//  }

  // HOOK STATES DECLARED; LEDs ON/OFF//RED
  //RED
  if (digitalRead(sR)) { //OFF
    digitalWrite(R, LOW);
    hR = false;
  }
  else { //ON
    hR = true;
    digitalWrite(R, HIGH);
  }
  //BLUE
  if (digitalRead(sB)) { //OFF
    digitalWrite(B, LOW);
    hB = false;
  }
  else { //ON
    hB = true;
    digitalWrite(B, HIGH);
  }
  //GREEN
  if (digitalRead(sG)) { //OFF
    digitalWrite(G, LOW);
    hG = false;
  }
  else { //ON
    hG = true;
    digitalWrite(G, HIGH);
  }
  //YELLOW
  if (digitalRead(sY)) { //OFF
    digitalWrite(Y, LOW);
    hY = false;
  }
  else { //ON
    hY = true;
    digitalWrite(Y, HIGH);
  }


  if (hR || hB || hG || hY) { //HOOK CONDITION
    houseEmpty = false;
    lockFlag = true;
    //    unlock();
  }
  else {
    //    lock();
    //lockFlag = false;
    houseEmpty = true;
  }

  //RFID Read
  if (Serial1.available() > 0) {
    if ((val = Serial1.read()) == 2) {                 // check for header
      bytesread = 0;
      while (bytesread < 12) {                        // read 10 digit code + 2 digit checksum
        if ( Serial1.available() > 0) {
          val = Serial1.read();
          if ((val == 0x0D) || (val == 0x0A) || (val == 0x03) || (val == 0x02)) { // if header or stop bytes before the 10 digit reading
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
        user = verifyRFID(code,checksum);
      if(user != 0)
        verified = true;
        else
        verified = false;
        scanned = true;
        //        Serial.print(verified);
        bytesread = 0;
      }
    }
  }//end of RFID read

  if (scanned) {
    if (verified & houseEmpty) {
      unlock();
      lcd.clear();
      lcd.home();
      lcd.print("WELCOME");
      switch (user){
        case 1:
          lcd.print(" Luke");
          break;
        case 2: 
          lcd.print(" Cameron");
          break;
        case 3:
          lcd.print(" Zach");
          break;
      }
      tone(speaker, 1000, 500);
      delay(3000);
      lcd.clear();
      lcd.home();
      lcd.print("Apt. 387");
    }
    else if (verified & !houseEmpty) {
      Serial.println("aready unlocked");
      lcd.clear();
      lcd.home();
      lcd.print("ALREADY UNLOCKED");
      tone(speaker, 1000, 500);
      delay(3000);
      lcd.clear();
      lcd.home();
      lcd.print("Apt. 387");
    }
    else if (!verified) {
      lcd.clear();
      lcd.home();
      lcd.print("ACCESS DENIED");
      tone(speaker, 500, 2000);
      delay(3000);
      lcd.clear();
      lcd.home();
      lcd.print("Apt. 387");
    }
  }//end of scanned
  else {
    if (houseEmpty) {
      if (!locked) {
        //        if(hR) digitalWrite(R,HIGH);
        //        if(hY) digitalWrite(Y,HIGH);
        //        if(hG) digitalWrite(G,HIGH);
        //        if(hB) digitalWrite(Y,HIGH);
        //        delay(lockTime);
        if (lockFlag) {
          lockmillis = millis();
          lockFlag = false;
        }
        else {
          int timetmp = (millis() - lockmillis);
          Serial.print("timetmp: ");
          Serial.println(timetmp);
          if (timetmp > lockTime) {
            lock();
            lockFlag = true;
            lcd.clear();
            lcd.home();
            lcd.print("Apt. 387");
          }
          //          if(timetmp < 300 && timetmp > 1){
          //            tone(speaker,3000);
          //          }else if(timetmp < 301) {
          //           noTone(speaker);
          //          }
        }
      }
    }
    else {
      unlock(); //may be redundant
    }
  }
}//end of loop

void unlock() {
  digitalWrite(striker, HIGH);
  locked = false;
  if(houseEmpty)
  color(0,0,255);
}

void lock() {
  digitalWrite(striker, LOW);
  locked = true;
  color(255,0,0);
}

int verifyRFID(byte* code, int checksum) {
  boolean flag1 = true;
  boolean flag2 = true;
  boolean flag3 = true;
  boolean flag4 = true;

  //  Serial.print("5-byte code: ");
  for (int i = 0; i < 5; i++) {
    //    if (code[i] < 16) Serial.print("0");
    if (code[i] != ref1[i]) flag1 = false;
    if (code[i] != ref2[i]) flag2 = false;
    if (code[i] != ref3[i]) flag3 = false;
    if (code[i] != ref4[i]) flag4 = false;
    //    Serial.print("code: ");
    //    Serial.print(code[i], HEX);
    //    Serial.print(" ");

  }
  if (flag1) {
    return 1;
  }
  if (flag2) {
    return 0;
  }
  if (flag3) {
    return 2;
  }
  if (flag4) {
    return 3;
  }
}
void color (unsigned char red, unsigned char green, unsigned char blue) // the color generating function
{
  analogWrite(RGB_R, red);
  analogWrite(RGB_G, green);
  analogWrite(RGB_B, blue);
}