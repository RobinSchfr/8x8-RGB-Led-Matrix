#include "Arduino.h"
#include "Point.h"

Point::Point(int x, int y, char dir) {
  xPos = x;
  yPos = y;
  direction = dir;
}
