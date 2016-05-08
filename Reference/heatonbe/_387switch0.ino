int R = 13;
int B = 12;
int G = 11;
int Y = 10;

boolean hR = false;
boolean hB = false;
boolean hG = false;
boolean hY = false;
boolean dO = false; //doorOpen

void setup() {
  Serial.begin(9600);
  pinMode(R, INPUT); //R
  pinMode(G, INPUT); //B
  pinMode(G, INPUT); //G
  pinMode(Y, INPUT); //Y
}

void loop() {
  //RED
  if(digitalRead(A3)) { //OFF
    digitalWrite(R, LOW);
    hR = false;
  }
  else { //ON
    hR = true;
    digitalWrite(R, HIGH);
  }
  //BLUE
  if(analogRead(A2)) { //OFF
    digitalWrite(B, LOW);
    hB = false;
  }
  else { //ON
    hB = true;
    digitalWrite(B, HIGH);
  }
  //GREEN
  if(analogRead(A1)) { //OFF
    digitalWrite(G, LOW);
    hG = false;
  }
  else { //ON
    hG = true;
    digitalWrite(G, HIGH);
  }
  
  //YELLOW
  if(analogRead(A0)) { //OFF
    digitalWrite(Y, LOW);
    hY = false;
  }
  else { //ON
    hY = true;
    digitalWrite(Y, HIGH);
  }
  //----------------------------------------------------------

  //REED SWITCH
  if(analogRead(A5)) { dO = false; }
  else { dO = true; }



  //----------------------------------------------------------
  Serial.println();
  if(hR || hB || hG || hY) {
    Serial.print("Currently Home: ");
    if(hR) { Serial.print("R. "); }
    if(hB) { Serial.print("B. "); }
    if(hG) { Serial.print("G. "); }
    if(hY) { Serial.print("Y. "); }
  }
  else {
    if (dO) {
      Serial.print("Home Empty. Door Closed");
    }
    else {
      Serial.print("WARNING: Home Empty. Door Open");
    }
  }   
  //---------------------------------------------------------- 
  delay(500);
}
