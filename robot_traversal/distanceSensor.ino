#define DISTSENSOR 14

int distanceReading(){
  int dist = analogRead(DISTSENSOR-14);
  return dist;
}
