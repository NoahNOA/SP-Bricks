//A simple Brick Breaker sketch for use with the VL53L0X rangefinder
//and three 256-pixel WS2812 addressable LED arrays like the ones available at:
//https://www.smart-prototyping.com/WS2812B-flexible-LED-matrix-8x32
//Written by Noah Zerkin for Smart Prototyping
//Do with it as you will, but I wouldn't recommend running a nuclear power plant, avionics package, or life support system using this code.
//If things go sideways (for instance, if you try to power your LEDs from an inappropriate source and that source catches on fire) I accept no responsibility.

//Requires installation of the Adafruit_VL53L0X, Adafruit_GFX, Adafruit_NeoMatrix, and Adafruit_NeoPixel libraries.
//https://github.com/adafruit/Adafruit_VL53L0X
//https://github.com/adafruit/Adafruit-GFX-Library
//https://github.com/adafruit/Adafruit_NeoMatrix
//https://github.com/adafruit/Adafruit_NeoPixel
//Limor Freid rocks \m/

//Note:
//An Arduino Uno won't cut it for this sketch, or for driving 768 WS2812s in general. This has been tested and works with Arduino Due and NodeMCU.

#include "Adafruit_VL53L0X.h"

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "BallGame.h"
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN 0 //Pin D3 on the NodeMCU
#define FRAMERATE 30

static int frameInterval = 1000 / FRAMERATE;
long nextFrame = 0;
boolean runAsFastAsYouCan = false;

uint16_t range = 0;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// The following matrix declaration information is copied from Adafruit's NeoMatrix tiletest example:
//
// MATRIX DECLARATION:
// Parameter 1 = width of EACH NEOPIXEL MATRIX (not total display)
// Parameter 2 = height of each matrix
// Parameter 3 = number of matrices arranged horizontally
// Parameter 4 = number of matrices arranged vertically
// Parameter 5 = pin number (most are valid)
// Parameter 6 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
//     arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns WITHIN
//     EACH MATRIX proceed in the same order, or alternate lines reverse
//     direction; pick one.
//   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. NEO_TILE_TOP + NEO_TILE_LEFT for the top-left corner.
//   NEO_TILE_ROWS, NEO_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_TILE_PROGRESSIVE, NEO_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).
//   See example below for these values in action.
// Parameter 7 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 pixels)
//   NEO_GRB     Pixels are wired for GRB bitstream (v2 pixels)
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA v1 pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

// Example with three 10x8 matrices (created using NeoPixel flex strip --
// these grids are not a ready-made product).  In this application we'd
// like to arrange the three matrices side-by-side in a wide display.
// The first matrix (tile) will be at the left, and the first pixel within
// that matrix is at the top left.  The matrices use zig-zag line ordering.
// There's only one row here, so it doesn't matter if we declare it in row
// or column order.  The matrices use 800 KHz (v2) pixels that expect GRB
// color data.

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, 1, 3, PIN,
                            NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_ZIGZAG +
                            NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB + NEO_KHZ800);

uint16_t textColor = matrix.Color(0, 255, 0);

Paddle* paddle = new Paddle(5);
Ball* ball = new Ball();
BrickArray* bricks = new BrickArray();
BallGame* game = new BallGame(&matrix, paddle, bricks, ball); //ball); //bricks, ball);


void setup() {
  Serial.begin(115200);

  if (!lox.begin()) {
    while (1);
  }
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(20);
  matrix.setTextColor(textColor);
  game->numPlayers = 1; //Currently unused
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus != 4) {
    range = measure.RangeMilliMeter / 10;
    //matrix.print(range);
  }
  nextFrame = millis() + frameInterval;
}

int cursorX    = matrix.width();

int x0 = 16;
int r = 10;
int paddleCenter = 0;


void loop() {

  if (runAsFastAsYouCan || millis() >= nextFrame) {
    if (!runAsFastAsYouCan) {
      if (millis() - nextFrame <= frameInterval) nextFrame += frameInterval;
      else{
        runAsFastAsYouCan = true;
        Serial.println("Game logic and display cycle exceeds frameInterval.");
        Serial.println("Switching to uncapped framerate.");
      }
    }
    matrix.fillScreen(0);

    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    if (measure.RangeStatus != 4) {
      range = measure.RangeMilliMeter / 10;
    }
    
    paddleCenter = map(range, 6, 30, 0, 32);
    game->updateGame(paddleCenter);
    matrix.show();
  }
}
