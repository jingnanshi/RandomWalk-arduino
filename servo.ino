// servotest.ino
// David_Harris@hmc.edu 1 October 2011
#include <Servo.h>

// pins
#define SERVOPIN 10

// Global variable for the servo information
Servo servo;

void servoFront(){
  initServo();
  servo.write(82);
  delay(10); 
}

void servoRight(){
  initServo();
  servo.write(0); 
  delay(7);
}

void testServo()
{
  initServo();
  servo.write(0); 
  delay(2000);
  servo.write(45); 
  delay(2000);
  servo.write(82);
  delay(2000); 
  servo.write(135);
  delay(2000); 
  servo.write(180); 
  delay(2000);
  
}

void initServo()
{
  pinMode(SERVOPIN, OUTPUT);
  servo.attach(SERVOPIN);
}
