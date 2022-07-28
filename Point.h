#ifndef point
#define point

#include "Arduino.h"

class Point {
  public:
    Point(int x, int y, char dir);
    int xPos;
    int yPos;
    char direction;
};

#endif
