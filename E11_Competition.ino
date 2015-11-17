#include <math.h>  // for the absolute value function
#include <TimerOne.h>
#include <FastRunningMedian.h>

/* CONSTANTS */
const int PhotoTransistor = 5; // phototransistorPin
const int teamSwitch = 3;      // team selection switch
int team_color = 0;
int powerLevel = 200;

/* Timers*/
int bump_timer = 0;

/* Direction States */
boolean for_f = false;
boolean back_f = false;
boolean left_f = false;
boolean right_f = false;

/* Flushable Array 
 *  indicate whether a station needs to be flashed to change or not (notice that array index starts at 0)
*/
const boolean flashable[9] = {true, true, false, false, false, false, false, true, true};

FastRunningMedian<unsigned int,5, 0> distanceMedian;

/* boolean constants*/
boolean game_start = true;
boolean game_end = false;

void setup() {

  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14,INPUT);
  
  initMotors();
  team_color = digitalRead(teamSwitch);
//  Serial.println(team_color);
  initServo();
  for (int i = 0; i < 5; i ++){
    distanceMedian.addValue(analogRead(0));
  } 
  // go out and hit the bonus beacon
  // ended in the black section
  forward(255);
  delay(1100);
  halt();
  
  turnR(255);
  delay(120); //150
  halt();
  
  forward(255);
  delay(300);
  halt();
  
  backward(255);
  delay(180);
  halt();
  
//  
//  // turn left for a short period of time
  turnL(255);
  delay(350); // turn 180 to the back
  halt();
//
  for (int i = 0; i < 5; i ++){
    distanceMedian.addValue(analogRead(0));
  } 
//  
  // turn left until the distance reading > 550; double check to make sure facing the center
  while (true){
      turnL(255);
      if (midReading() == -1 || leftReading() == -1){
        break;
      }
  }
  halt();
//
//  while (analogRead(0) < 750){
//      forward(140);
//  }
//  halt();

//  while (true){
//    
//  }
  // turn left for a short period of time
  // turn the distance sensor to the front
  // turn left until distance sensor reading > 550
  // forward
  // turn right for a short period of time
  
  


//  while (true){
//    digitalWrite(13,1);
//  }
  
//  while (true){
//    int left = analogRead(16);
//    int right = analogRead(17);
//    int mid = analogRead(18);
//    int left = leftReading();
//    int right = rightReading();
//    int mid = midReading();
//
//    Serial.print(left);
//    Serial.print(" ");
//    Serial.print(mid);
//    Serial.print(" ");
//    Serial.print(right);
//    Serial.println(" ");
//      distanceMedian.addValue(analogRead(0));
//      Serial.println(distanceMedian.getMedian());
//    testServo();
//    servoRight();
//  }

    servoRight();
    delay(1000);
    for (int i = 0; i < 5; i ++){
      distanceMedian.addValue(analogRead(0));
    } 
  
}

void loop() {
  for (int i = 0; i < 5; i ++){
    distanceMedian.addValue(analogRead(0));
  } 
  
  team_color = digitalRead(teamSwitch);

  // state one: on the black rim, trying to go to the blue line

  /* Black: -1 Blue: 0  White: 1 */
  
 while (true){
   
   servoRight();
//   distanceMedian.addValue(analogRead(0));
   for (int i = 0; i < 5; i ++){
    distanceMedian.addValue(analogRead(0));
   } 
   
   if (rightReading() == -1 && midReading() == -1){

     turnR(255);
     delay(5);
     
     Serial.println("right");
   }

   // if left is white, turn left
   else if (leftReading() == 1 && midReading() == 1){

     turnL(255);
     delay(5);
     Serial.println("left");
   }
   else{
     forward(250);
     delay(10);
     Serial.println("forward");
   }
   
    distanceMedian.addValue(analogRead(0));
    
   if (distanceMedian.getMedian() < 188){
     halt();
     tone(4,500);
     delay(100);
     noTone(4);
     halt();
     turnR(255);
     delay(230);
     halt();
//     forward(150);
//     delay(120);
//     turnR(255);
//     delay(300);
     
     halt();
     break;
   }
   
 }



  // state two: on the blue line, forward until distance sensor reads a large value
  //            then determine the station num, and decide whether to broadcast or 
  //            bump

  while (true){

      servoFront();
      // forward a little bit to exit the black circle
      halt();
      forward(210);
      delay(100);
      halt();
      
      // turn until see the blue line
      while ((midReading() != 0 && midReading() != -1 )|| rightReading() == -1 || leftReading() == -1){
        turnR(255);
      }
      halt();

      // update the distance reading array
      for (int i = 0; i < 5; i ++){
        distanceMedian.addValue(analogRead(0));
      }
      
      // follow the blue line
      // turn the distance sensor to the front
      while (true){
        distanceMedian.addValue(analogRead(0));
        Serial.println(distanceMedian.getMedian());

        if (distanceMedian.getMedian() > 520){
          halt();
          tone(4,500);
          delay(100);
          noTone(4);
          break;
        }  
        
        if (midReading() == 0){
          halt();
          forward(200);
          delay(10);
          Serial.println("forward");
        }
        else if (rightReading() == 0){
          halt();
          turnR(255);
          delay(10);
          Serial.println("right");
        }
        // if left is blue, turn left 
        else if (leftReading() == 0){
          halt();
          turnL(255);
          delay(10);
          Serial.println("left");
        } else{
          halt();
          forward(200);
          delay(30);
          Serial.println("forward");
        }
        
        
      }
      
      // if distance has become small enough
      // read gold code
      while (true){
        takeMeasurement();
        binarizeMeasurement();
        /*
       1 - 9 ----> white
       -1 - -9 -----> green
       11 - 19 -----> red
       -99 ------> did not read anything
        */
  
        int goldCode = fullCorrelate();
        int stationNum = readStationNum(goldCode);

         /*red = 2
         green = 0
         white = 1*/
        int color = convertToColor(goldCode);

        // if gold code is detected
        if (goldCode != (-99)){
          
          if (team_color == color){
            break;
          }

          // if the station number exsits
          if (stationNum != (-1)){
            // if the current station display color is different from our color
            if (team_color != color){

              // determine whether bump or broadcast
              // do the appropriate action
              boolean flash = flashable[stationNum-1];

              if (flash){
                broadcastGoldCode(stationNum, team_color);
                // turn left and right
                backward(255);
                delay(100);
                forward(255);
                delay(100);
                halt();
                
              } else {
                forward(255);
                delay(200);
                halt();
                backward(255);
                delay(200);
                halt();
              }
          
            }
            if (team_color != color){
              digitalWrite(13,LOW);
            }
          }
        }
     
      }
       
      tone(4,500);
      delay(500);
      noTone(4);
      while (true){
        
      }
      
  }

  while (true){
    
  }

  // state three: finish taking the beacon, backward back along the blue line 
  //              turn the distance sensor backwards
  //              stop until the distance sensor reads a big value
  //              turn left until left see black

  // that's bascically it??? how to check the bonus one?
  
  takeMeasurement();
  binarizeMeasurement();
  /*
       1 - 9 ----> white
       -1 - -9 -----> green
       11 - 19 -----> red
       -99 ------> did not read anything
  */
  
  int goldCode = fullCorrelate();
  int stationNum = readStationNum(goldCode);

   /*red = 2
   green = 0
   white = 1*/
  int color = convertToColor(goldCode);

  // make sure the robot is on the line

  if (goldCode != (-99))  // if a gold code was detected:
  {
//    Serial.println("Detected Gold Code: ");
//    Serial.println(goldCode);
//    Serial.println("Detected Station: ");
//    Serial.println(stationNum);
//    Serial.println("Color: ");
//    Serial.println(color);
  }

//  // if there is a station
  if (stationNum != (-1)){
    // if the current station display color is different from our color
    if (team_color != color){
      Serial.println();
      Serial.println(stationNum);
      Serial.println(team_color);
      broadcastGoldCode(stationNum, team_color);
    }
    if (team_color != color){
      digitalWrite(13,LOW);
    }
  }
  
}

void setPowerLevel(int pl){
  powerLevel = pl;
}

/* 
 *  Search line function:
 *  dir ---> 0: forward
 *      ---> 1: right
 *      ---> -1: left
 *  receive the value returned by readLine() function, control the robot moving left or right
*/

void search(int dir) {
  if(dir == 1) {
    halt(); 
    turnR(240);
  }
  if(dir == -1) {
    halt(); 
    turnL(240); 
  }
}

/*
 * Move forward function
 * only move forward when the current reading is 0 (moving forward)
 * 
*/
void mov(int dir) {
  if (dir == 0) {
    forward(255);
  } 
}







/* 
 *  Read the floor and return a value indicating which way to turn
    
   red = 2
   green = 0
   white = 1
*/
//int readLine(int teamColor){
//  
//  int left = analogRead(left_RS);
//  int right = analogRead(right_RS);
//  int mid = analogRead(mid_RS);
//
//  /* if green, counterclockwise */
//  if (teamColor == 0){
//      /* CASE ONE: */
//      if (inBlue(mid)){
//        
//      }
//  
//      /* CASE TWO: */
//  }
//
//}


