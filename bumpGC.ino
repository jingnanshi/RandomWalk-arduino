//red -> 2
//green -> 0
//white -> 1

/* maximum duration for bumping*/
const int BUMP_DURATION = 3000;

/* 
 *  team_color: our own robot color
 *  color: the color of the detected beacon
 *  
 *  find the line
 *  make sure facing the beacon
 *  check GC code
 *  if not us,
 *  rush forward
 *  
*/
boolean bumpGC(int team_color,int color, int timeElapsed, boolean for_f, boolean back_f)
{
  // if not passed bumping duration
  if (timeElapsed < BUMP_DURATION){

    // if not moving forward
    if (for_f != true){
      // make sure it is moving forward
      forward(255);
    }
    return false; // did not finish bumping procedure
  } else{
    backward(255);
    delay(20);
    return true; // finish bumping procedure
  }
   
};


