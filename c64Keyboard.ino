/*
  C64 keyboard -> USB keyboard

  For the Arduino Leonardo, Micro or Due?

  Reads a byte from C64 pins

  created 5 Jan 2021
  by Darek Greenly
*/

#include "Keyboard.h"

const byte ROW_COUNT = 8;
const byte COL_COUNT = 8;
const unsigned char NIL = 0xFD;
const unsigned char KEY_FN = 0xFE;

// Pins
const byte KEY_PIN_COUNT = ROW_COUNT * COL_COUNT;
const byte PIN_ROW[ROW_COUNT] = {8, 9, 10, 11, 12, A0, A1, A2};
const byte PIN_COL[COL_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7};
const byte REST = A3; // RESTORE key pin

const unsigned char KEY_LAYOUT[ROW_COUNT][COL_COUNT] = {
  {'1', '3', '5', '7', '9', '-', KEY_HOME, KEY_DELETE },
  {'`', 'w', 'r', 'y', 'i', 'p', ']', KEY_RETURN },
  {KEY_TAB, 'a', 'd', 'g', 'j', 'l', '\'', KEY_RIGHT_CTRL },
  {KEY_FN, KEY_LEFT_SHIFT, 'x', 'v', 'n', ',', '/', KEY_LEFT_ALT},
  {' ', 'z', 'c', 'b', 'm', '.', KEY_RIGHT_SHIFT, KEY_ESC},
  {KEY_LEFT_CTRL, 's', 'f', 'h', 'k', ';', '\\', KEY_PAGE_UP},
  {'q', 'e', 't', 'u', 'o', '[', KEY_END, KEY_PAGE_DOWN},
  {'2', '4', '6', '8', '0', '=', KEY_INSERT, KEY_RIGHT_GUI}
};

//const char SHIFT_LAYOUT[8][8] = {
//  {'!', '#', '%', '&', '(', '_', NIL, NIL },
//  {'~', 'W', 'R', 'Y', 'I', 'P', '}', NIL },
//  {NIL, 'A', 'D', 'G', 'J', 'L', '"', NIL },
//  {NIL, NIL, 'X', 'V', 'N', '<', '?', NIL},
//  {' ', 'Z', 'C', 'B', 'M', '>', NIL, NIL},
//  {NIL, 'S', 'F', 'H', 'K', ':', '|', NIL},
//  {'Q', 'E', 'T', 'U', 'O', '{', NIL, NIL},
//  {'@', '$', '^', '*', ')', '+', NIL, NIL}
//};

// NIL here means: don't change, copy from main layout
const unsigned char FN_LAYOUT[ROW_COUNT][COL_COUNT] = {
  {KEY_F1, KEY_F3, KEY_F5, KEY_F7, KEY_F9, KEY_F11, KEY_UP_ARROW, KEY_RIGHT_ARROW},
  {NIL, NIL, NIL, NIL, NIL, NIL, NIL, NIL},
  {NIL, NIL, NIL, NIL, NIL, NIL, NIL, NIL},
  {NIL, NIL, NIL, NIL, NIL, NIL, NIL, NIL},
  {NIL, NIL, NIL, NIL, NIL, NIL, NIL, NIL},
  {NIL, NIL, NIL, NIL, NIL, NIL, NIL, NIL},
  {NIL, NIL, NIL, NIL, NIL, NIL, KEY_DOWN_ARROW, NIL},
  {KEY_F2, KEY_F4, KEY_F6, KEY_F8, KEY_F10, KEY_F12, KEY_LEFT_ARROW, NIL}
};

// Current state of the keyboard
bool rowDown[ROW_COUNT];
bool colDown[COL_COUNT];
bool isFnDown = false;
bool isAnyKeyDown = false;

void setup() {
  // open the serial port:
  // Serial.begin(9600);

  // All pins default to input
  // And pre-fill row/col default values
  for(int i=0; i<ROW_COUNT; i++){
    pinMode(PIN_ROW[i], INPUT_PULLUP);
    rowDown[i] = false;
  }
  for(int i=0; i<COL_COUNT; i++){
    pinMode(PIN_COL[i], INPUT_PULLUP);
    colDown[i] = false;
  }

  // initialize control over the keyboard:
  Keyboard.begin();
}

void updateAnyKeyDown() {
  if(isAnyKeyDown == false) {
    isAnyKeyDown = true;
  }
}

void loop() {
  isAnyKeyDown = false;

  // Grab data from all the pins
  for(int i=0; i<ROW_COUNT; i++){
    rowDown[i] = digitalRead(PIN_ROW[i]);
    if(rowDown[i]) updateAnyKeyDown();
  }
  for(int i=0; i<COL_COUNT; i++){
    colDown[i] = digitalRead(PIN_COL[i]);
    if(colDown[i]) updateAnyKeyDown();
  }

  isFnDown = rowDown[3] == true && colDown[0] == true;

  for(int r=0; r<ROW_COUNT; r++){
    for(int c=0; c<COL_COUNT; c++){
      if(rowDown[r] == true && colDown[c] == true) {
        
      } else {
        // TODO: could store previous state
        // to ensure I don't do unneccesary:
        // Keyboard.release();
      }
    }
  }
  
//  // check for incoming serial data:
//  if (Serial.available() > 0) {
//    // read incoming serial data:
//    char inChar = Serial.read();
//    // Type the next ASCII value from what you received:
//    Keyboard.write(inChar + 1);
//  }
}
