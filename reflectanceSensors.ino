/* color constants */
const int white_hi = 500;
const int blue_hi = 860;
const int blue_lo = 500;
const int black_lo = 900;

const int left_white_hi = 500;
const int left_blue_hi = 790;
const int left_blue_lo = 500;
const int left_black_lo = 900;

const int right_white_hi = 500;
const int right_blue_hi = 790;
const int right_blue_lo = 500;
const int right_black_lo = 900;

/* Pins for the left, middle and right reflectance sensors*/
const int left_RS = 16;
const int right_RS = 17;
const int mid_RS = 18;

// initialize the reflectance sensors
void initializeRSensors(){
  pinMode(left_RS, INPUT);
  pinMode(right_RS, INPUT);
  pinMode(mid_RS, INPUT);
}

/* Black: -1 Blue: 0  White: 1 */

int midReading(){
  int mid = analogRead(mid_RS);
 
  if (mid >= black_lo){
    return -1;
  } else if (mid >= blue_lo){
    return 0;
  } else{
    return 1;
  }
//  return -2;
}

int leftReading(){
  int left = analogRead(left_RS);
  if (left>= left_black_lo){
    return -1;
  } else if (left >= left_blue_lo){
    return 0;
  } else{
    return 1;
  }
//  return -2;
}

int rightReading(){
  int right = analogRead(right_RS);
  if (right>= right_black_lo){
    return -1;
  } else if (right >= right_blue_lo){
    return 0;
  } else{
    return 1;
  }
//  return -2;
}

boolean inBlue(int value, int dir){
  if (dir == 0){
    if (value > blue_lo){
    if (value < blue_hi){
        return true;
     }
    }
  }
  
  return false;
}

boolean inWhite(int value, int dir){
  if (dir == 0){
    if (value < white_hi){
      return true;
    }
  }
  return false;
}

boolean inBlack(int value, int dir){
  if (dir == 0){
    if (value > black_lo){
      return true;
    }
  }
  
  return false;
}


