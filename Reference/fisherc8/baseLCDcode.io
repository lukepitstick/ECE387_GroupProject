// Cameron Fisher
// Yamuna Rajasekhar
// ECE 387 Sec B
// Utilizes a rotary encoder and 16x2 LCD screen connected
// to an Arduino Uno, to give the user the ability to set
// a 4 digit password, and then requires verification before
// displaying a welcome screen
/**********************************************************/
#include <LiquidCrystal.h>// include the library code

// initialize lcd display pins
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
const int contrastPin = 5;
const int pushButton1 = 7;

// define "n", which controls the digit observed by the password/cursor
int n = 0;
int i = 0;
int buttonState = 0;
int lastButtonState = 0;

// set up angular transcoder (swPin = button)
const int clkPin = 8;
const int dtPin = 2;
const int swPin = 3;
int encoderVal = 0;

// create an array to store the password in
const int pwLength = 4;
int password[pwLength];
int testPassword[pwLength];
boolean pwSet = false;
boolean pwVerified = false;

/*********************************************************/
void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.home();

  pinMode(contrastPin, OUTPUT);
  pinMode(pushButton1, INPUT);

  // adjust contrast of LCD
  analogWrite(contrastPin, 100);

  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT);
  digitalWrite(swPin, HIGH);

  lcd.print("Create Password:");
  lcd.setCursor(0, 1);
}
/*********************************************************/
void loop() {
  
  if (!pwSet) {
    createPassword();
  } else {
    checkPassword();
  }
}
/**************************************************************/
// method that checks if the rotary encoder has been turned, and
// returns an int value cooresponding to the change in angle
int getEncoderTurn(void)
{
  static int oldA = HIGH;
  static int oldB = HIGH;
  int result = 0;
  int newA = digitalRead(clkPin);//read the value of clkPin to newA
  int newB = digitalRead(dtPin);//read the value of dtPin to newB
  if (newA != oldA || newB != oldB)
  {
    // something has changed
    if (oldA == HIGH && newA == LOW)
    {
      result = (oldB * 2 - 1);
    }
  }
  oldA = newA;
  oldB = newB;
  return result;
}
/**********************************************************************/
// Allows user to create a 4 digit password that will be stored into an 
// integer array. When it's finished it automatically sets up the display
// for password verificiation, which is the next module.
void createPassword(void)
{
  buttonState = digitalRead(swPin);

  // retrieve value of the rotary encoder's angle, then
  // use its position to cycle through 9 digits
  if (n < pwLength) {
    int change = getEncoderTurn();
    encoderVal = encoderVal + change;
    if (encoderVal == 10)
    {
      encoderVal = 0;
    }
    if (encoderVal == -1) {
      encoderVal = 9;
    }
    // display the current value on LCD and reset cursor
    lcd.print(encoderVal);
    lcd.setCursor(n, 1);
    
    // four digits have been entered...
  } else {
    pwSet = true;
    delay(500);
    lcd.clear();
    lcd.home();
    lcd.print("Password Saved");
    delay(2000);
    lcd.clear();
    lcd.home();
    lcd.print("Verify Password:");
    encoderVal = 0;
  }

  // if rotary button is pressed, store value into password[]
  if (buttonState != lastButtonState) {
    if (digitalRead(swPin) == LOW) {
      lcd.noCursor();
      password[n] = encoderVal;
      n++;
    }
    delay(100);
  }
  lastButtonState = buttonState;
}
/***********************************************************************/
// allows user to enter another 4 digit pin, this time it calls 
// verifyPassoword() to check if the entered password is the same as the
// one previously saved. If not, you get unlimited chances to enter the
// correct password. If it is the same, it welcomes you to the system.
void checkPassword(void)
{
  buttonState = digitalRead(swPin);

  // retrieve value of the rotary encoder's angle, then
  // use its position to cycle through 9 digits
  if (i < pwLength) {
    int change = getEncoderTurn();
    encoderVal = encoderVal + change;
    if (encoderVal == 10)
    {
      encoderVal = 0;
    }
    if (encoderVal == -1) {
      encoderVal = 9;
    }
    // display the current value on LCD and reset cursor
    lcd.print(encoderVal);
    lcd.setCursor(i, 1);
    
    // once 4 digits have been entered...
  } else {
    delay(500);
    lcd.clear();
    lcd.home();
    
    // if password was correct
    if (verifyPassword(password, testPassword)) {
      lcd.print("Welcome back");
      
    // if password was incorrect
    } else {
     lcd.print("Password invalid");
     delay(2000);
     lcd.clear();
    lcd.home();
    lcd.print("Verify Password:");
    encoderVal = 0;
    i = 0;
    }
  }

  // if rotary button is pressed, store value into password[]
  if (buttonState != lastButtonState) {
    if (digitalRead(swPin) == LOW) {
      lcd.noCursor();
      testPassword[i] = encoderVal;
      i++;
    }
    delay(100);
  }
  lastButtonState = buttonState;
}
/********************************************************************/
// takes in 2 integer arrays, compares the values, and returns TRUE if
// they are the same array. (In this scenario, returns TRUE if the 
// entered password is the same as the actual password)
boolean verifyPassword(int actual[], int entered[])
{
  boolean verified = true;
  
  for(int i = 0; i < pwLength; i++) {
    if (actual[i] != entered[i]) {
     verified = false; 
    }
  }
  return verified;
}
