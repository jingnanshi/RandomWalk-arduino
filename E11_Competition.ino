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

/* Direction Statesd */
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
  pinMode(10, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(14,INPUT);
  initServo();
  for (int i = 0; i < 5; i ++){
    distanceMedian.addValue(analogRead(0));
  }
//
//  while (true){
//    
//  }
  
//  while (true){
//    distanceMedian.addValue(analogRead(0));
//    
//  }
  
  initMotors();
  team_color = digitalRead(teamSwitch);
//  Serial.println(team_color);
  initServo();
  for (int i = 0; i < 5; i ++){
    distanceMedian.addValue(analogRead(0));
  } 
  // go out and hit the bonus beacon
  // ended in the black section
//  forward(255);
//  delay(1100);
//  halt();
//  
//  turnR(255);
//  delay(120); //150
//  halt();
//  
//  forward(255);
//  delay(300);
//  halt();
//  
//  backward(255);
//  delay(180);
//  halt();
//  
////  
////  turn left for a short period of time
//  turnL(255);
//  delay(350); // turn 180 to the back
//  halt();
////
//  for (int i = 0; i < 5; i ++){
//    distanceMedian.addValue(analogRead(0));
//  } 
////  
//  // turn left until the mid or left reads black
//  while (true){
//      turnL(255);
//      if (midReading() == -1 || leftReading() == -1){
//        break;
//      }
//  }
//  halt();
    
  
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

//    servoRight();
//    delay(1000);
//    for (int i = 0; i < 5; i ++){
//      distanceMedian.addValue(analogRead(0));
//    } 
  
}

void loop() {
  for (int i = 0; i < 5; i ++){
    distanceMedian.addValue(analogRead(0));
  } 
  
  team_color = digitalRead(teamSwitch);

  // state one: on the black rim, trying to go to the blue line

  /* Black: -1 Blue: 0  White: 1 */
//  
// while (true){
//   
//   servoRight();
//
//   for (int i = 0; i < 5; i ++){
//    distanceMedian.addValue(analogRead(0));
//   } 
//   
//   if (rightReading() == -1 && midReading() == -1){
//     turnR(255);
//     delay(5);
//     Serial.println("right");
//   }
//
//   // if left is white, turn left
//   else if (leftReading() == 1 && midReading() == 1){
//
//     turnL(255);
//     delay(5);
//     Serial.println("left");
//   }
//   else{
//     forward(250);
//     delay(10);
//     Serial.println("forward");
//   }
//   
//    distanceMedian.addValue(analogRead(0));
//    
//   if (distanceMedian.getMedian() < 188){
//     halt();
//
//     tone(4,500);
//     delay(100);
//     noTone(4);
//
//     turnR(255);
//     delay(230);
//     halt();
//     
//     break;
//   }
//   
// }


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
          delay(10);
          Serial.println("forward");
        }
        
        
      }

      
      
      // if distance has become small enough
      // read gold code
      while (true){

        delay(100);
        
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
        
        if (team_color == color){
            break;
        }
          
        // if gold code is detected
        if (goldCode != (-99)){
          
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
                forward(255);
                delay(200);
                halt();
                backward(255);
                delay(200);
                halt();
                
              } else {
                forwardUntilHit();
                
                halt();
                backward(255);
                delay(200);
                halt();
              }
          
            } else {
              break;
            }

          }
        }
     
      }
      break; 
      
  }
  
  tone(4,500);
  delay(500);
  noTone(4);

  while (true){
    
  }
  
  // state three: finish taking the beacon, backward back along the blue line 
  //              turn the distance sensor backwards
  //              stop until the distance sensor reads a big value
  //              turn left until left see black

  while (true){

  	// turn left a little bit	
  	turnL(255);
  	delay(100);
  	halt();
   
  	// turn left until see blue line again
  	while (true){
  		turnL(255);
  		if (midReading() == 0 || leftReading() == 0 || rightReading() == 0){
  			halt();
  			break;
  		}
  	}
  	// forward see black
  	while (midReading() != -1){
  		forward(255);
  	}
  	halt();

   while (true){
    
   }

  	// repeat the taking beacon procedure (possibly write a function to abstract & save line spaces)
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
                unsigned long start_time = millis();     
                forwardUntilHit();  
                unsigned long end_time = millis();
                
                backward(255);
                delay(end_time - start_time);
                halt();
                
              } else {
                forwardUntilHit();
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
    
  }

  while (true){
    
  }

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

void forwardUntilHit(){
  while (distanceMedian.getMedian() < 550){
    forward(255);
    distanceMedian.addValue(analogRead(0));
  }
  halt();
}


