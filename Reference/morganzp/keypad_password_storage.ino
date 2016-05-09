#include <Keypad.h>

#include <Servo.h>//was already installed on my version of this IDE 1.6.0

Servo lockMech;//declaring my servo  
int deg=90;
const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad
const int  verifLight= 10; //password lights, verification is green, false is red
const int  falseLight = 12;
const int  buttonLight =11;
//boolean servoUnlock=false;//may not need
char combo[4]="";//this is the users password attempt stored in an array
char unlock[4]={'6','A','4','#'};//this is the hardcoded password since its meant as a backup to get into a house
boolean light[4]={false,false,false,false};//just a boolean array that stores comparison of user input and hardcoded password
//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]=
{                     
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};               

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {9,8,7,6}; //Rows 0 to 3 again utilizing the pins 6-9
byte colPins[numCols]= {5,4,3,2}; //Columns 0 to 3 again utilizing the pins 2-5
//Initializes the internal keymap to be equal to userKeymap and uses pins 6-9 as rows and 2-5 as columns
Keypad keypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);//initializes an instance of the Keypad class


void setup(){
  lockMech.attach(13);//attaches servo to Digital IO pin 13
  lockMech.write(deg);
  Serial.begin(9600);
  pinMode(verifLight,OUTPUT);//initialize the led pin as output
  pinMode(falseLight,OUTPUT);
  pinMode(buttonLight, OUTPUT);
  Serial.println("Press any key to start password attempt");
}

void loop(){
 
  
  
 char key = keypad.getKey();
  if (key != NO_KEY)//NO_KEY is self explanatory 
  {
  //----------------key 1 of the password-----------------------------
  Serial.print("\nEnter KEY 1: ");
  char key = keypad.waitForKey();
  digitalWrite(buttonLight,HIGH);
  delay(200);
  digitalWrite(buttonLight,LOW);  
  Serial.println(key);
  combo[0]=key;
  //----------------key 2 of the password-----------------------------
  Serial.print("\nEnter KEY 2: ");
  key = keypad.waitForKey();
  digitalWrite(buttonLight,HIGH);
  delay(200);  
  digitalWrite(buttonLight,LOW);
  Serial.println(key);
  combo[1]=key;
  //----------------key 3 of the password-----------------------------
  Serial.print("\nEnter KEY 3: ");
  key = keypad.waitForKey();
  digitalWrite(buttonLight,HIGH);
  delay(200);
  digitalWrite(buttonLight,LOW);  
  Serial.println(key);
  combo[2]=key;
  //----------------key 4 of the password-----------------------------
  Serial.print("\nEnter KEY 4: ");
  key = keypad.waitForKey();
  digitalWrite(buttonLight,HIGH);
  delay(200);
  digitalWrite(buttonLight,LOW);  
  Serial.println(key);
  combo[3]=key;
  //-----------------------------------------------------
  for(int i=0;i<4;i++){
     Serial.print(combo[i] );
   if(combo[i]==unlock[i])//compares users input to hardcoded password and sets each char comparison true or false 
    light[i]=true;
   else 
    light[i]=false; 
  }

  
if(light[1]&&light[1]&&light[2]&&light[3]){//if password is correct then turn on light and call servoResponse
  Serial.println("\nPassword Correct");
    digitalWrite(verifLight, HIGH);
    delay(1000); 
    digitalWrite(verifLight, LOW);
  
servoResponse(true);    
}

else{
  Serial.println("\nAccess Denied");
  digitalWrite(falseLight,HIGH);
  delay(1000);
  digitalWrite(falseLight,LOW);
}
}
}

void servoResponse(boolean unlock){
  if(unlock){
    digitalWrite(verifLight, HIGH);
  Serial.println("Door Unlocking");
  for (deg = 90; deg <= 180; deg += 1) { // 0 degrees to 90 degrees
    lockMech.write(deg);              //accumulates degress every pass through the loop until it unlocks fully at 90 degrees
    delay(15);    
  }
  delay(5000);//keeps door unlocked for 5 seconds before relocking
  digitalWrite(verifLight, LOW);
  
  Serial.println("Door Locking");
  digitalWrite(falseLight, HIGH);
  for (deg = 180; deg >=90 ; deg--) { //locks door after 5 seconds
    
    lockMech.write(deg);              
    delay(15);    
  }
    delay(2000);
       digitalWrite(falseLight, LOW) ;
  }
  
return ;
}
