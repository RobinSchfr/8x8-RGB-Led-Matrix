#include "colors.h"
#include "numbers.h"
#include "Point.h"
#include "animations.h"

#include <EEPROM.h>
#include <LinkedList.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN 4
#define NUMPIXELS 64

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

boolean running;
boolean idleMode = true;
long timer;
long speed;

long selectedSpeed = 3;
int speeds[] = {800, 500, 300, 200};

char direction;
LinkedList<Point*> bodyParts = LinkedList<Point*>();
int xHead;
int yHead;
int xApple;
int yApple;
int snakeLength;
boolean showHighscore;
boolean uBlocked, dBlocked, lBlocked, rBlocked;
long selectedTheme = 1;
unsigned long snakeColor = GREEN;
unsigned long appleColor = RED;

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  Serial.begin(9600);
  EEPROM.begin(512);
  pixels.begin();
  pixels.setBrightness(20);
  resetData();
  spawnApple();
  spawnHead();
  running = false;
}

void loop() {
  if (idleMode) {
    int effect = random(6);
    for (int i = 0; i < 64; i++) {
      pixels.setPixelColor(i, patterns[effect][i]);
    }
    pixels.show();
    delay(3000);
    fadeOut();
    delay(1000);
  }
  checkSerial();
  if (running) {
    draw();
    if (millis() > speed + timer) {
      timer = millis();
      move();
    }
  } else {
    drawTitleScreen();
  }
}

void move() {
  int newX;
  int newY;
  Point *head = bodyParts.get(0);
  char lastDirection = head->direction;
  if (direction == 'u') {
    newX = head->xPos;
    newY = head->yPos - 1;
  }
  if (direction == 'd') {
    newX = head->xPos;
    newY = head->yPos + 1;
  }
  if (direction == 'l') {
    newX = head->xPos - 1;
    newY = head->yPos;
  }
  if (direction == 'r') {
    newX = head->xPos + 1;
    newY = head->yPos;
  }
  if (!collide(newX, newY)) {
    Point *newHead = new Point(newX, newY, direction);
    bodyParts.add(0, newHead);
    Point *deletedHead = bodyParts.pop();
    delete deletedHead;
    if (gotApple(newX, newY)) {
      snakeLength++;
      appendNewBody();
      spawnApple();
    }
  }
  else {
    setHighscore();
    showScore(snakeLength);
    setup();
  }
}

void appendNewBody() {
  Point *lastBodyPart = bodyParts.get(bodyParts.size() - 1);
  int x = lastBodyPart->xPos;
  int y = lastBodyPart->yPos;
  int newX;
  int newY;
  char dir = lastBodyPart->direction;
  if (dir == 'u') {
    newX = x;
    newY = y - 1;
  }
  if (dir == 'd') {
    newX = x;
    newY = y + 1;
  }
  if (dir == 'l') {
    newX = x - 1;
    newY = y;
  }
  if (dir == 'r') {
    newX = x + 1;
    newY = y;
  }
  Point *newBodyPart = new Point(newX, newY, dir);
  bodyParts.add(newBodyPart);
}

boolean gotApple(int x, int y) {
  return (x == xApple && y == yApple);
}

boolean collide(int x, int y) {
  boolean collide = false;
  if ((x > 7 || x < 0) || (y > 7 || y < 0)) {
    collide = true;
  }
  if (!collide) {
    Point *currentBodyPart;
    int otherX;
    int otherY;
    for (int i = 0; i < bodyParts.size(); i++) {
      currentBodyPart = bodyParts.get(i);
      otherX = currentBodyPart->xPos;
      otherY = currentBodyPart->yPos;
      if ((x == otherX && y == otherY) && bodyParts.size() > 2) {
        collide = true;
      }
    }
  }
  return collide;
}

void draw() {
  pixels.clear();
  setPx(xApple, yApple, appleColor);

  Point *element;
  for (int i = 0; i < bodyParts.size(); i++) {
    element = bodyParts.get(i);
    setPx(element->xPos, element->yPos, snakeColor);
  }
  pixels.show();
}

void drawTitleScreen() {
  pixels.clear();
  for (int i = 0; i < 8; i++) {
    setPx(i, 0, BLACK);
    setPx(i, 7, BLACK);
    setPx(0, i, BLACK);
    setPx(7, i, BLACK);
  }
  setPx(1, 1, appleColor);

  setPx(2, 3, snakeColor);
  setPx(1, 2, snakeColor);
  setPx(1, 3, snakeColor);
  setPx(3, 3, snakeColor);
  setPx(3, 2, snakeColor);
  setPx(3, 1, snakeColor);
  setPx(4, 1, snakeColor);
  setPx(5, 1, snakeColor);
  setPx(6, 1, snakeColor);
  setPx(6, 2, snakeColor);

  int s1[] = {50};
  int s2[] = {43, 51};
  int s3[] = {36, 44, 52};
  int s4[] = {29, 37, 45, 53};
  int sAll[] = {50, 43, 51, 36, 44, 52, 29, 37, 45, 53};
  resetS(sAll);
  int s = selectedSpeed % 4;
  switch (s) {
    case 1:
      speed = speeds[0];
      for (int i = 0; i < 1; i++) {
        pixels.setPixelColor(s1[i], BLUE);
      }
      break;
    case 2:
      speed = speeds[1];
      for (int i = 0; i < 2; i++) {
        pixels.setPixelColor(s2[i], BLUE);
      }
      break;
    case 3:
      speed = speeds[2];
      for (int i = 0; i < 3; i++) {
        pixels.setPixelColor(s3[i], BLUE);
      }
      break;
    case 0:
      speed = speeds[3];
      for (int i = 0; i < 4; i++) {
        pixels.setPixelColor(s4[i], BLUE);
      }
      break;
  }
  pixels.show();
}

void resetS(int sAll[]) {
  for (int i = 0; i < 10; i++) {
    pixels.setPixelColor(sAll[i], LIGHTBLUE);
  }
}

void spawnApple() {
  xApple = random(8);
  yApple = random(8);
}

void spawnHead() {
  xHead = 3;
  yHead = 0;
  direction = 'd';
  Point *p = new Point(xHead, yHead, direction);
  bodyParts.add(p);
}

void setPx(int x, int y, unsigned long color) {
  int pos = x + y * 8;
  pixels.setPixelColor(pos, color);
}

void setTheme() {
  selectedTheme++;
  int t = selectedTheme % 7;
  switch (t) {
    case 1:
      snakeColor = themes[0][0];
      appleColor = themes[0][1];
      break;
    case 2:
      snakeColor = themes[1][0];
      appleColor = themes[1][1];
      break;
    case 3:
      snakeColor = themes[2][0];
      appleColor = themes[2][1];
      break;
    case 4:
      snakeColor = themes[3][0];
      appleColor = themes[3][1];
      break;
    case 5:
      snakeColor = themes[4][0];
      appleColor = themes[4][1];
      break;
    case 6:
      snakeColor = themes[5][0];
      appleColor = themes[5][1];
      break;
    case 0:
      snakeColor = themes[6][0];
      appleColor = themes[6][1];
      break;
  }
}

void checkSerial() {
  if (Serial.available() > 0) {
    String incomingText = Serial.readStringUntil('#');
    if (running) {
      if (incomingText == "up" && !uBlocked) {
        unblockAll();
        direction = 'u';
        dBlocked = true;
      }
      if (incomingText == "down" && !dBlocked) {
        unblockAll();
        direction = 'd';
        uBlocked = true;
      }
      if (incomingText == "left" && !lBlocked) {
        unblockAll();
        direction = 'l';
        rBlocked = true;
      }
      if (incomingText == "right" && !rBlocked) {
        unblockAll();
        direction = 'r';
        lBlocked = true;
      }
      if (incomingText == "escape") {
        setup();
      }
    } else {
      if (incomingText == "start") {
        countdown(3);
        running = true;
      }
      if (incomingText == "speed") {
        selectedSpeed++;
      }
      if (incomingText == "highscore") {
        getHighscore();
      }
    }
    if (incomingText == "theme") {
      setTheme();
    }
    if (incomingText == "switchIdleMode") {
      switchIdleMode();
    }
  }
}

void switchIdleMode() {
  if (idleMode) {
    idleMode = false;
  } else {
    idleMode = true;
  }
}

void unblockAll() {
  uBlocked = false;
  dBlocked = false;
  lBlocked = false;
  rBlocked = false;
}

void fadeOut() {
  int effect = random(4);
  if (effect == 0) {
    for (int i = 0; i < 8; i++) {
      delay(10);
      for (int j = 0; j < 8; j++) {
        setPx(j, i, BLACK);
        pixels.show();
      }
    }
  }
  if (effect == 1) {
    for (int i = 7; i >= 0; i--) {
      delay(10);
      for (int j = 0; j < 8; j++) {
        setPx(j, i, BLACK);
        pixels.show();
      }
    }
  }
  if (effect == 2) {
    for (int i = 0; i < 8; i++) {
      delay(10);
      for (int j = 0; j < 8; j++) {
        setPx(i, j, BLACK);
        pixels.show();
      }
    }
  }
  if (effect == 3) {
    for (int i = 7; i >= 0; i--) {
      delay(10);
      for (int j = 0; j < 8; j++) {
        setPx(i, j, BLACK);
        pixels.show();
      }
    }
  }
}

void showScore(int score) {
  int eDigit = score % 10;
  int zDigit = (score % 100 - eDigit) / 10;

  int eX = 5;
  int eY = 1;
  int zX = 1;
  int zY = 1;
  for (int l = 0; l < 4; l++) {
    pixels.clear();
    pixels.show();
    delay(400);
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 3; j++) {
        if (numbers[eDigit][i][j] == 1) {
          setPx(eX + j, eY + i, snakeColor);
        }
      }
    }
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 3; j++) {
        if (numbers[zDigit][i][j] == 1) {
          setPx(zX + j, zY + i, snakeColor);
        }
      }
    }
    if (showHighscore) {
      for (int i = 2; i < 6; i++) {
        setPx(i, 7, YELLOW);
      }
    }
    pixels.show();
    delay(400);
  }
  showHighscore = false;
}

void countdown(int seconds) {
  int x = 3;
  int y = 1;
  for (int l = seconds; l >= 0; l--) {
    pixels.clear();
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 3; j++) {
        if (numbers[l][i][j] == 1) {
          setPx(x + j, y + i, snakeColor);
        }
      }
    }
    pixels.show();
    delay(400);
  }
}

void setHighscore() {
  int hScore = EEPROM.read(selectedSpeed % 4);
  if (snakeLength > hScore) {
    showHighscore = true;
    EEPROM.write(selectedSpeed % 4, snakeLength);
    EEPROM.commit();
    delay(10);
  }
}

void getHighscore() {
  int hScore = EEPROM.read(selectedSpeed % 4);
  showHighscore = true;
  delay(10);
  showScore(hScore);
}

void resetData() {
  snakeLength = 1;
  timer = 0;
  unblockAll();
  for (int i = 0; i < bodyParts.size() + 1; i++) {
    Point *deletedPart = bodyParts.pop();
    delete deletedPart;
  }
  bodyParts.clear();
}
