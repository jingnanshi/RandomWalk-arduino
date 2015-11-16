#define DISTSENSOR 14

int distanceRead(){
  int dist = analogRead(DISTSENSOR-14);
  return dist;
}
