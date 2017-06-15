#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include "BallGame.h"

Brick::Brick(int cornerX, int cornerY, uint16_t myColor) {
  //  Serial.println("Initializing brick");
  x = cornerX;
  y = cornerY;
  color = myColor;

}

Brick::Brick() {

}

BrickArray::BrickArray() {

}

//A couple of alternate brick color assigment schemes
/*
  void BrickArray::init() {
  Serial.println("Initializing brick array");
  for (int i = 0; i < 8 ; i++) {
    for (int j = 0; j < 4; j++) {

      if ((i % 2) && (j % 2)) {
        brickPs[i][j] = new Brick(i * 4, j * 2, neonMatrix->Color(map(i, 0, 8, 80, 255) , map(j, 0, 4, 255, 80), map(i + j, 0, 12, 80, 255)));
      }
      else if ((i % 2) && !(j % 2)) {
        brickPs[i][j] = new Brick(i * 4, j * 2, neonMatrix->Color(map(i, 8, 0, 80, 255) , map(j, 0, 4, 255, 80), map(i + j, 0, 12, 80, 255)));
      }
      else if (!(i % 2) && (j % 2)) {
        brickPs[i][j] = new Brick(i * 4, j * 2, neonMatrix->Color(map(i, 0, 8, 80, 255) , map(j, 4, 0, 255, 80), map(i + j, 0, 12, 80, 255)));
      }
      else {
        brickPs[i][j] = new Brick(i * 4, j * 2, neonMatrix->Color(255 , 0, 255));
      }
    }
  }
  }
*/
/*
  void BrickArray::init() {
  Serial.println("Initializing brick array");
  for (int i = 0; i < 8 ; i++) {
    for (int j = 0; j < 4; j++) {
      uint8_t r = random(0,255);
      uint8_t g = random(0,255-r);
      uint8_t b = random(0,255-(r+g));
      brickPs[i][j] = new Brick(i * 4, j * 2, neonMatrix->Color(r , g, b));

    }
  }
  }
*/

void BrickArray::init() {
  //Serial.println("Initializing brick array");
  for (int i = 0; i < 8 ; i++) {
    for (int j = 0; j < 4; j++) {
      uint8_t r = random(80, 255);
      uint8_t g = random(0, 255);
      uint8_t b = random(0, 255);
      brickPs[i][j] = new Brick(i * 4, j * 2, neonMatrix->Color(r , g, b));

    }
  }
}

Paddle::Paddle(int w) {
  width = w;
}

Ball::Ball() {


}

Ball::Ball(int startX, int startY) {


}

BallGame::BallGame(Adafruit_NeoMatrix* thisMatrix, Paddle *pPaddle, BrickArray *pBricks, Ball *pBall) { // Ball *pBall) { //BrickArray *pBricks, Ball *pBall) {

  Serial.println("Initializing game objects");
  neonMatrix = thisMatrix;
  paddle = pPaddle;
  bricks = pBricks;
  ball = pBall;
  paddle->neonMatrix = neonMatrix;
  bricks->neonMatrix = neonMatrix;
  ball->neonMatrix = neonMatrix;
  bricks->init();


}

void BallGame::drawBricks() {
  for (int i = 0; i < 8 ; i++) {
    for (int j = 0; j < 4; j++) {
      if (bricks->brickPs[i][j]->active) {
        neonMatrix->writeFillRect(bricks->brickPs[i][j]->x, bricks->brickPs[i][j]->y, 4, 2, bricks->brickPs[i][j]->color);
      }
    }
  }
}

//checkCollisions walks the BallGame's BrickArray (bricks) looking for active bricks adjacent to the ball object.
void BallGame::checkCollisions() {
  boolean collisionOccured = false;
  for (int i = 0; i < 8 ; i++) {
    for (int j = 0; j < 4; j++) {
      if (bricks->brickPs[i][j]->active) {
        if ((ball->x >= bricks->brickPs[i][j]->x - 1 && ball->x <= bricks->brickPs[i][j]->x + 5 ) && (ball->y >= bricks->brickPs[i][j]->y - 1 && (ball->y <= (bricks->brickPs[i][j]->y + 2)))) {
          bricks->brickPs[i][j]->active = false;
          if (ball->x == bricks->brickPs[i][j]->x - 1 || ball->x == bricks->brickPs[i][j]->x + 4) {
            ball->vX *= -1;
          }
          ball->vY *= -1;
          collisionOccured = true;
        }
      }
      if (collisionOccured) {
        break;
      }
    }
  }
}


void BallGame::updateBall() {
  ball->x += ball->vX;
  ball->y += ball->vY;
  
  if (ball->x >= neonMatrix->width() - 1 && ball->vX > 0) {
    ball->vX *= -1;
  }
  
  if (ball->x <= 1 && ball->vX < 0) {
    ball->vX *= -1;
  }
  
  if (ball->y >= neonMatrix->height() && ball->vY > 0) {
    if (ball->x >= paddlePos - 3 && ball->x <= paddlePos + 3) {
      ball->vY *= -1;
    }
    
    //I threw this in just to clearly denote the end of one game and beginning of the next. Not a remotely permanent solution.
    else {
      ball->x = 16;
      ball->y = neonMatrix->height() - 1;
      ball->vY *= -1;
      bricks->init();
      neonMatrix->fillScreen(0);
      neonMatrix->setCursor(12, 8);
      neonMatrix->print("3");
      neonMatrix-> show();
      delay(500);
      neonMatrix->fillScreen(0);
      neonMatrix->setCursor(12, 8);
      neonMatrix->print("2");
      neonMatrix-> show();
      delay(500);
      neonMatrix->fillScreen(0);
      neonMatrix->setCursor(12, 8);
      neonMatrix->print("1");
      neonMatrix-> show();
      delay(500);
      neonMatrix->fillScreen(0);
      neonMatrix->setCursor(12, 8);
      neonMatrix->print("GO!");
      neonMatrix-> show();
      delay(300);
    }
    
  }
  if (ball->y <= 1 && ball->vY < 0) {
    ball->vY *= -1;
  }
  if (ball->y <= 8 && ball->x <= neonMatrix->width()) {
    checkCollisions();
  }
}

void BallGame::drawBall() {
  neonMatrix->writePixel(ball->x, ball->y, neonMatrix->Color(255, 255, 255));
}

void BallGame::updateGame(int paddleCenter) {
  paddlePos = paddleCenter;
  neonMatrix->drawLine(paddlePos - 3, 23, paddlePos + 3, 23, neonMatrix->Color(255, 255, 255));
  drawBricks();
  if (paddlePos < 36) {
    updateBall();
  }
  drawBall();

}
