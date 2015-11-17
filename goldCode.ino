
/* Constants */

const unsigned long delayTime = 250; // time beteen samples in [us]
const int numSamples = 31;
const byte maxCorrelation = 31;
const byte numGoldCodes = 9;
const int broadcastLED_1 = 13;
const int broadcastLED_2 = 10;
const int broadcastLED_3 = 5;
const int broadcastLED_4 = 2;

/* Global Variables */

int rawValues[numSamples];
boolean binaryValues[numSamples];

boolean GoldCodes[numGoldCodes][numSamples] = 
{ 
  {0,0,0,0,0,0,0,1,0,0,0,1,1,0,1,1,0,0,0,0,1,1,0,0,1,1,1,0,0,1,1},
  {1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,1,0,0},
  {0,1,0,0,0,0,1,1,0,1,0,0,0,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0},
  {1,0,1,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,0,1},
  {0,0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,1,1,0,1,0,0,1,1,1},
  {1,1,1,0,0,0,1,0,0,1,1,0,1,1,1,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0},
  {0,1,1,0,0,1,1,0,1,1,0,1,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,0},
  {1,0,0,1,0,1,1,1,0,1,1,0,0,1,1,1,0,1,1,0,0,0,1,0,0,1,0,0,0,1,1},
  {0,0,0,1,0,0,1,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,1,0,0,1,1,0,0,1}
};

boolean unclaimedGoldCodes[numGoldCodes][numSamples] = {
  {0,1,0,1,0,0,0,1,1,0,0,0,1,1,0,1,0,1,1,1,0,0,0,0,1,0,0,0,1,0,0},
  {1,0,1,1,0,0,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,1,1,1},
  {0,0,1,1,0,1,1,0,0,1,0,0,1,0,1,1,1,0,0,1,1,0,1,0,1,0,0,1,1,0,1},
  {1,1,1,1,0,0,0,0,1,0,1,0,0,0,0,1,1,0,0,0,1,0,0,0,0,1,0,1,0,1,0},
  {0,1,1,1,0,1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,1,0,1,1,0,0,1,0,0,0,0},
  {1,0,0,0,1,1,0,0,1,1,0,0,1,1,1,1,0,0,1,0,1,0,0,1,0,1,1,1,1,0,0},
  {0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,0,1,1,0,0,1,0,1,0,0,0,0,0,1,1,0},
  {1,1,0,0,1,1,1,0,1,0,0,1,0,1,1,0,1,1,0,1,1,0,0,0,1,1,0,0,0,0,1},
  {0,1,0,0,1,0,1,0,0,0,1,0,0,1,0,1,0,0,1,1,1,0,1,1,1,0,1,1,0,1,1}
};


/*------*/


void takeMeasurement()
{
 unsigned long currentTime = micros();  // total elapsed microseconds.
 unsigned long nextSampleTime = micros() + delayTime; // time at which we take the next measurement
 
  for (byte i=0;i<numSamples;i++)
  {

    while ( currentTime < nextSampleTime) 
    {
      // update time:
      currentTime = micros();
    };
    
    nextSampleTime = nextSampleTime + delayTime;  // increment the time
    rawValues[i] = analogRead(PhotoTransistor); 
  } 
}

void binarizeMeasurement(){
   // compute the average of the rawValues 
  int rawAverage = 0;
  
  for(byte i=0;i<numSamples;i++)
    rawAverage += rawValues[i];
  
  rawAverage /= numSamples;
  
  // compare every rawValue with the average, and assign 1 or 0
  for (byte j=0;j<numSamples;j++)
  {
   if (rawValues[j] > rawAverage)
     binaryValues[j] = 1;
   else
     binaryValues[j] = 0;
  }
}

int fullCorrelate(){
  int score;
  int unclaimed_score;
  
  for (byte i = 0; i < numGoldCodes; i++){
    score = correlate(GoldCodes[i], binaryValues, numSamples, numSamples);
    unclaimed_score = correlate(unclaimedGoldCodes[i], binaryValues, numSamples, numSamples);
//    Serial.println(unclaimed_score);
//    fabs(score) == 31

    /*
       1 - 9 ----> white
       -1 - -9 -----> green
       11 - 19 -----> red
       -99 ------> did not read anything
    */
    if (fabs(score) == 31){
      if (score > 0){
        // if green
//        Serial.println("Color: green");   
          return -(i+1);
      } else {
        // if white
//        Serial.println("Color: white");
          return (i+1);
      }
      
    } 

    // unclaimed gold code detection
    if (fabs(unclaimed_score) == 31){
      // if red
//      Serial.println("Color: red");
      return ((i+1) + 10);
    }
  }
  return -99;
}

/* 
 * The Correlation function: correlate(boolean *gc1, boolean *gc2)  
 * Inputs: gc1 and gc2: pointers of two arrays

*/

double correlate(boolean *gc1, boolean *gc2, int gc1_size, int gc2_size) {

  // initialize max score
  int max_score = 0;

  // for loop through all the 31 possibilities
  for (int i = 0; i < gc1_size; i ++){

    // create a shifted array from gc2
    boolean shiftedGC[gc2_size];
    memset(shiftedGC, 0, gc2_size*sizeof(boolean));

    for(int j = 0 ; j < gc2_size ; j ++){

      // the index of the array element in gc2
      // shift to the left:
      // e.g.: 100001101
      // shift by 1 bit to the left --->
      //       110000110
      int shift_from = j+i;

      // if the shift_from index exceeds the largest possible index,
      // wrap to the begining of the array
      if (shift_from >= gc2_size){
        shift_from = shift_from - gc2_size;
      }

      shiftedGC[j] = gc2[shift_from];
    }

    // calculate each shift's correlation score by taking dot product
    int correlation_score = dotProduct(gc1,shiftedGC, gc2_size);
//    Serial.print(correlation_score);
//    Serial.print(" ");
    
    // update max score
    if (fabs(correlation_score) > fabs(max_score)){
      max_score = correlation_score;
    }

  }

  // print the correlation score for each offset
  return max_score;

}

/*  Dot product function:
 *   takes in pointer values of two arrays, return the dot product 
 *   of the two arrays; two arrays have to be the same size,
 *   return the dot product value
*/

int dotProduct(boolean *n1, 
                  boolean *n2, int array_size)
{
  int dp=0;

  for (int i=0; i < array_size; i++)
  {
     if (n1[i] == n2[i]){
      dp += 1;
     } else {
      dp -= 1;
     }
  }
  return dp;
} 

/* broadcastGoldCode: takes in the station number

   teamNum == 1: White team
   teamNum == 0: Green Team

*/

unsigned long Timer = micros();

void broadcastGoldCode(int stationNum, int teamNum){
//  
//  // traverse through the corresponding gold code array
//  for (byte i = 0; i < numSamples; i ++ ){
//    
//    unsigned long currentTime = micros();
//    unsigned long nextSampleTime = micros() + delayTime; 
//    
//    int current_signal = GoldCodes[stationNum-1][i];
//    
//    // if I'm green, I have to invert the signal
//    if (teamNum == 0){
//      // invert the signal
//      current_signal = invert(current_signal);
//    }
//
////    Serial.print(current_signal);
//
//    while ( currentTime < nextSampleTime) 
//    {
//      // update time:
//      currentTime = micros();
//    };
//    
//    nextSampleTime = nextSampleTime + delayTime;  // increment the time
//    
//    // if i'm white, I don't have to invert the signal
//    digitalWrite(broadcastLED_1, current_signal); 
////    digitalWrite(broadcastLED_2, current_signal); 
////    digitalWrite(broadcastLED_3, current_signal); 
////    digitalWrite(broadcastLED_4, current_signal); 
//  }

  
//    
////  // traverse through the corresponding gold code array
//  for (int i = 0; i < numSamples; i ++ ){
//    
//    unsigned long currentTime = micros();
//    unsigned long nextSampleTime = micros() + delayTime; 
//    
//    int current_signal = GoldCodes[stationNum-1][i];
//    
//    // if I'm green, I have to invert the signal
//    if (teamNum == 0){
//      // invert the signal
//      current_signal = invert(current_signal);
//    }
//
////    Serial.print(current_signal);
//
//    while ( currentTime < nextSampleTime) 
//    {
//      // update time:
//      currentTime = micros();
//    };
//    
//    nextSampleTime = nextSampleTime + delayTime;  // increment the time
//    
//    // if i'm white, I don't have to invert the signal
////    digitalWrite(broadcastLED_1, current_signal); 
//    digitalWrite(broadcastLED_2, current_signal); 
////    digitalWrite(broadcastLED_3, current_signal); 
////    digitalWrite(broadcastLED_4, current_signal); 
//  }
////  
////  // traverse through the corresponding gold code array
//  for (int i = 0; i < numSamples; i ++ ){
//    
//    unsigned long currentTime = micros();
//    unsigned long nextSampleTime = micros() + delayTime; 
//    
//    int current_signal = GoldCodes[stationNum-1][i];
//    
//    // if I'm green, I have to invert the signal
//    if (teamNum == 0){
//      // invert the signal
//      current_signal = invert(current_signal);
//    }
//
////    Serial.print(current_signal);
//
//    while ( currentTime < nextSampleTime) 
//    {
//      // update time:
//      currentTime = micros();
//    };
//    
//    nextSampleTime = nextSampleTime + delayTime;  // increment the time
//    
//    // if i'm white, I don't have to invert the signal
////    digitalWrite(broadcastLED_1, current_signal); 
////    digitalWrite(broadcastLED_2, current_signal); 
//    digitalWrite(broadcastLED_3, current_signal); 
////    digitalWrite(broadcastLED_4, current_signal); 
//  }
////
////  // traverse through the corresponding gold code array
//  for (int i = 0; i < numSamples; i ++ ){
//    
//    unsigned long currentTime = micros();
//    unsigned long nextSampleTime = micros() + delayTime; 
//    
//    int current_signal = GoldCodes[stationNum-1][i];
//    
//    // if I'm green, I have to invert the signal
//    if (teamNum == 0){
//      // invert the signal
//      current_signal = invert(current_signal);
//    }
//
////    Serial.print(current_signal);
//
//    while ( currentTime < nextSampleTime) 
//    {
//      // update time:
//      currentTime = micros();
//    };
//    
//    nextSampleTime = nextSampleTime + delayTime;  // increment the time
//    
//    // if i'm white, I don't have to invert the signal
////    digitalWrite(broadcastLED_1, current_signal); 
////    digitalWrite(broadcastLED_2, current_signal); 
////    digitalWrite(broadcastLED_3, current_signal); 
//    digitalWrite(broadcastLED_4, current_signal); 
//  }
  
}


/* invert the boolean variable passed to it*/
boolean invert(boolean num){
  if (num == 0){
    return 1;
  }
  return 0;
}

/* read the station number*/
int readStationNum(int score){
    /*
       1 - 9 ----> white
       -1 - -9 -----> green
       11 - 19 -----> red
       -99 ------> did not read anything
    */
    
    // if red
    if (score > 10){
      return (score - 10);
    }

    // green
    if (score <= -1 && score >= -9){
      return fabs(score);
    }

    // white
    if (score >= 1 && score <= 9){
      return score;
    }

    return -1;
}

/* read the color of the station
   red = 2
   green = 0
   white = 1
*/
int convertToColor(int score){
    /*
       1 - 9 ----> white
       -1 - -9 -----> green
       11 - 19 -----> red
       -99 ------> did not read anything
    */
    
    // if red
    if (score > 10){
      return 2;
    }

    // green
    if (score <= -1 && score >= -9){
      return 0;
    }

    // white
    if (score >= 1 && score <= 9){
      return 1;
    }

    return -1;
}

/* test function using timer interrups*/
//void broadcastGoldCode2(){
//
//    int current_signal = GoldCodes[8][bCount];
//    
//    // if I'm green, I have to invert the signal
////    if (teamNum == 0){
//      // invert the signal
//      current_signal = invert(current_signal);
////    }
//
//    if (bCount == 31){
//       bCount = 0;
//    }
//    bCount = bCount +1;
//    // if i'm white, I don't have to invert the signal
//    digitalWrite(13, current_signal); 
//}

