/*    MOTOR CONTROL MODULLE     */
/*    ALL ANALOG CONTROL        */

#define LPLUS 9
#define LMINUS 8
#define RPLUS 7
#define RMINUS 12
#define LEN 6
#define REN 11


void motorEn(int pl){
  analogWrite(LEN, pl);
  analogWrite(REN, pl);
}

void initMotors(){
  pinMode(LPLUS, OUTPUT); 
  pinMode(RPLUS, OUTPUT); 
  pinMode(LMINUS, OUTPUT); 
  pinMode(RMINUS, OUTPUT); 
  pinMode(LEN, OUTPUT); 
  pinMode(LEN, OUTPUT); 
}

void halt(){
  motorEn(255);
  analogWrite(LPLUS, 0);
  analogWrite(LMINUS, 0); 

  analogWrite(RPLUS, 0);
  analogWrite(RMINUS, 0);
}

void forward(int powerLevel){
  motorEn(powerLevel);
  analogWrite(LPLUS, powerLevel);
  analogWrite(LMINUS, 0); 

  analogWrite(RPLUS, powerLevel);
  analogWrite(RMINUS, 0); 
}

void backward(int powerLevel){
  motorEn(powerLevel);
  analogWrite(LPLUS, 0);
  analogWrite(LMINUS, powerLevel); 

  analogWrite(RPLUS, 0);
  analogWrite(RMINUS, powerLevel); 
}


// turn left indefinitely 
void turnL(int powerLevel){
  motorEn(powerLevel);
  analogWrite(LPLUS, 0);
  analogWrite(LMINUS, powerLevel); 

  analogWrite(RPLUS, powerLevel);
  analogWrite(RMINUS, 0); 
}

// turn right indefinitely 
void turnR(int powerLevel){
  motorEn(powerLevel);
  analogWrite(LPLUS, powerLevel);
  analogWrite(LMINUS, 0); 

  analogWrite(RPLUS, 0);
  analogWrite(RMINUS, powerLevel); 
}

