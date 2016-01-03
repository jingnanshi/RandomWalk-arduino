/*

Random Robot Traversal Algorithm:

Based on Paper "Dispersing robots in an unknown environments" 
(http://www-users.cs.umn.edu/~gini/papers/Morlok04dars.pdf)

*/

#include <math.h>  // for the absolute value function

const int BOUNDARY = 520;
const int TURN_POWER = TURN_POWER;
const int FORWARD_DELAY = 20;

const int random_forward_degree = 10;
const int delay_per_degree = 20;

const int random_turn_base = 120;
const int random_turn_ceiling = 240;

void setup() {

  Serial.begin(9600);

}

void loop() {
  
  // if the robot is about to hit a wall
  if (distanceReading() > BOUNDARY){ 
    halt();
    // randomly turn
    randomTurn();
  } else {
    // randomly move forward
    randomForward();
  }

}

/* randomTurn(): randomly choose an angle to turn
*
*/

void randomTurn(){
  // turn left or right random number
  long randNumber1 = random(1,10);
  // turning degree random number
  long randNumber2 = random(random_turn_base,random_turn_ceiling);
  int turn_time  = (int) randNumber2 * delay_per_degree;

  // turn right
  if (randNumber1 >= 5){
    
    turnR(TURN_POWER);
    delay(turn_time);
    halt();
  }
  // turn left
  else{
    turnL(TURN_POWER);
    delay(turn_time);
    halt();
  }
}

/* randomForward(): randomly choose an angle and move forward
*
*/

void randomForward(){
  long rand_turn = random(random_forward_degree);
  long rand_dir = random(1,10);

  if (rand_dir > 5){

    turnR(TURN_POWER);
    delay(rand_turn * delay_per_degree);

  } else {
    turnL(TURN_POWER);
    delay(rand_turn * delay_per_degree);

  }
  forward(TURN_POWER);
  delay(FORWARD_DELAY);
  
}


