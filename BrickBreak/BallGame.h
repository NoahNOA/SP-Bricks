#ifndef BallGame_h
#define BallGame_h
#endif
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <pins_arduino.h>
#include "BallGame.h"
#endif

class Brick {

  public:

    Brick();
    Brick(int cornerX, int cornerY, uint16_t myColor);
    int x;
    int y;
    Adafruit_NeoMatrix* neonMatrix;
    uint16_t color;
    boolean active = true;
    
};

class BrickArray {

  public:

    BrickArray();

    Brick* brickPs[8][4]; 

    void init();
    Adafruit_NeoMatrix* neonMatrix;
  
};

class Paddle {

  public:
    Paddle(int w);
    int delta;
    int lastPosition;
    int center;
    int width;
    Adafruit_NeoMatrix* neonMatrix;

};

class Ball {

  public:

    Ball();
    Ball(int startX, int startY);
    int vX = -1;
    int vY = -2;
    int x = 15;
    int y = 23;
    void deflect();
    Adafruit_NeoMatrix* neonMatrix;
    
};

class BallGame {

  public:

    BallGame(Adafruit_NeoMatrix *thisMatrix, Paddle *pPaddle, BrickArray *pBricks, Ball *pBall, int interval);
    Adafruit_NeoMatrix* neonMatrix;
    Paddle* paddle;
    void updateGame(int paddleCenter);    
    uint8_t numPlayers;
    BrickArray* bricks;
    void drawBricks();
    Ball* ball;
    void updateBall();
    void drawBall();
    void checkCollisions();
    int paddlePos = 0;
    int frameInterval;
    long nextFrame = 0;
};

