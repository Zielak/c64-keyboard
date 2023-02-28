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
// Rows -> output
const byte PIN_ROW[ROW_COUNT] = {8, 9, 10, 11, 12, A0, A1, A2};
// Cols -> input
const byte PIN_COL[COL_COUNT] = {7, 6, 5, 4, 3, 2, 1, 0};
// Restore key -> output
const byte REST = A3;

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

// Previous state of the keyboard
bool pRowDown[ROW_COUNT];
bool pColDown[COL_COUNT];

void setup() {
  // open the serial port:
  Serial.begin(9600);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // And pre-fill row/col default values
  for(int i=0; i<ROW_COUNT; i++){
    pinMode(PIN_ROW[i], OUTPUT);
    rowDown[i] = false;
    pRowDown[i] = false;
  }
  for(int i=0; i<COL_COUNT; i++){
    // Mute COL pins initially
    pinMode(PIN_COL[i], OUTPUT);
    colDown[i] = false;
    pColDown[i] = false;
  }
  
  // initialize control over the keyboard:
  Keyboard.begin();
}

void loop() {

//  digitalWrite(PIN_ROW[0], HIGH);
//
//  if(digitalRead(PIN_COL[0]) == HIGH){
//    digitalWrite(LED_BUILTIN, HIGH);
//  } else {
//    digitalWrite(LED_BUILTIN, LOW);
//  }
  
  isAnyKeyDown = false;
  isFnDown = false;

  // Grab data from all the pins
  for(int r=0; r<ROW_COUNT; r++){
    digitalWrite(PIN_ROW[r], HIGH);

    // For each column
    for(int c=0; c<COL_COUNT; c++){

      // Set all cols pin modes
      for(int i=0; i<COL_COUNT; i++){
        if(i != c){
          pinMode(PIN_COL[i], OUTPUT);
          digitalWrite(PIN_COL[i], HIGH);
        } else {
          pinMode(PIN_COL[i], INPUT);
        }
      }
      
      if(digitalRead(PIN_COL[c]) == LOW){
        if(isAnyKeyDown == false){
          isAnyKeyDown = true;
        }

        // FN key
        if(r == 3 && c == 0) {
          isFnDown = true;
        }

        Serial.println(char(KEY_LAYOUT[r][c]));
      }
    }
    
    digitalWrite(PIN_ROW[r], LOW);
  }

//  for(int r=0; r<1; r++){
//    for(int c=0; c<1; c++){
//      if(rowDown[r] == true && colDown[c] == true) {
//        if(isAnyKeyDown == false) {
//          isAnyKeyDown = true;
//        }
//        Serial.print("PRESSED: [c:");
//        Serial.print(c);
//        Serial.print(", r:");
//        Serial.print(r);
//        Serial.print("] - ");
//        Serial.println(char(KEY_LAYOUT[r][c]));
//      } else {
//        // TODO: could store previous state
//        // to ensure I don't do unneccesary:
//        // Keyboard.release();
//      }
//    }
//  }

  // Light up the LED for testing
  if(isAnyKeyDown == true){
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  delay(50);
  
//  // check for incoming serial data:
//  if (Serial.available() > 0) {
//    // read incoming serial data:
//    char inChar = Serial.read();
//    // Type the next ASCII value from what you received:
//    Keyboard.write(inChar + 1);
//  }
}
