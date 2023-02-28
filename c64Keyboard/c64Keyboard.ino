// C64 USB Keyboard mod
// Original by Pyofer
// See original thread @
// http://www.lemon64.com/forum/viewtopic.php?t=55650
//
// Modified to Support restore key & US/EU keymaps by dabone.
//
// Joystick support for keycodes added, and support for second fire button.
//
//
// Wiring is as follows
// 64 Keyboard connector has 20 Pins with a key Pin @ Pin 2.
// Arduino Pro Micro Pinout
// https://cdn.sparkfun.com/assets/9/c/3/c/4/523a1765757b7f5c6e8b4567.png
// Board type should be Arduino Leonardo (or the Pro mini clones)

// Teensy LC Pinout
// https://www.pjrc.com/teensy/teensyLC.html

//KeyBoard Arduino Pro Micro     Leonardo            Teensy LC
//  Pin     Pin     Label         Pin                 Pin
//   20       5       2 - SDA      2                   5
//   19       6       3 - SCL      3                   6
//   18       7       4 - A6       4                   7
//   17       8       5            5                   8
//   16       9       6 - A7       6                   9
//   15       10      7 -          7                   10
//   14       11      8 - A8       8                   11
//   13       12      9 - A9       9                   12
// ------
//   12       13      10 - A10     10                  13
//   11       14      16 - MOSI    11                  14
//   10       15      14 - MISO    12                  15
//   9        16      15 - SCLK    A4                  16
//   8        17      18 - A0      A3                  17
//   7        18      19 - A1      A2                  18
//   6        19      20 - A2      A1                  19
//   5        20      21 - A3      A0                  20
//   4        N/C
//   3        1       1 - TX       1                   1
//   2        N/C
//   1        11      8 - A8       8                   8

// The following pins are optional and not required for keyboard only usage.

//   Joysticks
//  Pins 1,2,3,4,6 Are Both Joysticks to the same input.
//   1        5       2 - SDA                          5
//   2        6       3 - SCL                          6
//   3        7       4 - A6                           7
//   4        8       5                                8
//   6        9       6 - A7                           9
//   9        10      7 -                              10
//  Joy 1
//   8        1       1 - TX                           1
//  Joy 2
//   8        2       0 - RX                           2

#include <HID.h>      // Comment out for Teensey
#include <Keyboard.h> // Comment out for Teensey
#include "keydefs.h"  // Comment out for Teensey

int inChar = 0;
int keyPos = 0;
int keyDown[80];
long lastDebounceTime[80];
int debounceDelay = 50;
int shift = 0;
int Row = 0;
int i;
// Tmp vars
int InputValue;
int RowPin;
// Position of the Hybrid2 Select key in the keymap, CTRL = 72
// int HybridKey2 = 72;
// Convert Row/Col number to associated output pin -- Arduino Pro Micro
//int RowPinMap[8] = {9, 3, 4, 5, 6, 7, 8, 2};
//int ColPinMap[10] = {10, 16, 14, 21, 18, 19, 20, 15, 1, 0};

// Convert Row/Col number to associated output pin -- Teensey LC
//int RowPinMap[8] = {12, 6, 7, 8, 9, 10, 11, 5};
//int ColPinMap[10] = {13, 14, 15, 20, 17, 18, 19, 16, 1, 2};

// Convert Row/Col number to associated output pin -- Leonardo
int RowPinMap[8] = { 9, 3, 4, 5, 6, 7, 8, 2 };
int ColPinMap[10] = { 10, 11, 12, A0, A3, A2, A1, A4, 1, 0 };

// Keyboard Matrix Now Matches real C64 with two more columns.
// Matrix can be found at
// http://sta.c64.org/cbm64kbdlay.html

// Keymap
/**
 * Joystick Keys are the indicated by JoyNumber(Direction), i.e Joy1UP is up on joystick one.
 * Firebuttons are Noted JoyF(Joystick number)- Fire Button number,
 * i.e Joy2F1 Joystick 2, fire button 1, Joy2F2 Joystick 2, fire button 2.
 * Current values for the Joysticks are located at the end of each line.
 * KP stands for numeric keypad inputs.
 */
uint16_t keyMap[80] = {
  //  Del   Return LR   F7   F1    F3  F5  UD      Null      Null
  KEY_DELETE, KEY_ENTER, KEY_RIGHT_CTRL, KEY_RIGHT_GUI, KEY_ESC, KEY_PAGE_UP, KEY_PAGE_UP, KEY_RIGHT_ALT, NULL, NULL,
  //  3     W      A    4    Z     S   E   LSHFT   Joy2Down  Joy1Down
  KEY_3, KEY_W, KEY_A, KEY_4, KEY_Z, KEY_S, KEY_E, KEY_LEFT_SHIFT, KEYPAD_2, KEYPAD_ASTERIX,
  //  5     R      D    6    C     F   T   X       Joy2Left  Joy1Left
  KEY_5, KEY_R, KEY_D, KEY_6, KEY_C, KEY_F, KEY_T, KEY_X, KEYPAD_4, KEYPAD_MINUS,
  //  7     Y      G    8    B     H   U   V       Joy2Right Joy1Right
  KEY_7, KEY_Y, KEY_G, KEY_8, KEY_B, KEY_H, KEY_U, KEY_V, KEYPAD_6, KEYPAD_PLUS,
  //  9     I      J    0    M     K   O   N       Joy2F1    Joy1F1
  KEY_9, KEY_I, KEY_J, KEY_0, KEY_M, KEY_K, KEY_O, KEY_N, KEYPAD_5, KEY_SCROLL_LOCK,
  //  +     P      L    -    .     :   @   ,       Joy2F2    Joy1F2
  KEY_MINUS, KEY_P, KEY_L, KEY_EQUAL, KEY_PERIOD, KEY_SEMICOLON, KEY_LEFT_BRACE, KEY_COMMA, KEYPAD_3, KEYPAD_1,
  //  Pound *      ;    Home RSHFT =   Pi*  /       Restore   Null
  KEY_HOME, KEY_RIGHT_BRACE, KEY_QUOTE, KEY_INSERT, KEY_RIGHT_SHIFT, KEY_BACKSLASH, KEY_END, KEY_SLASH, KEY_BACKSPACE, NULL,
  //  1     BS     CTRL 2    SPC   C=  Q   RunStop Joy2Up    Joy1Up
  KEY_1, KEY_BACKTICK, KEY_TAB, KEY_2, KEY_SPACE, KEY_RIGHT_CTRL, KEY_Q, MODIFIER_FN, KEYPAD_8, KEYPAD_SLASH,
};
// * - Pi on C64 C keyboard appears as up arrow.

// Position of the Modifier FN key in the keymap
byte KeyFn = 77; // RUN/STOP

// FN Modifier keys.
const byte FN_KEYS_COUNT = 11;
uint16_t fnKeyMap[FN_KEYS_COUNT]{
  KEY_F10, KEY_F11, KEY_F12, KEY_UP_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW,
  KEY_DOWN_ARROW,
  KEY_F13, KEY_F14, KEY_F15, KEY_F16,
};
byte fnKeyPos[FN_KEYS_COUNT]{
  // 0 + - Pound Home Delete
  43, 50, 53, 60, 63, 0,
  // Pi
  66,
  // F1 F3 F5 F7
  4, 5, 6, 3
};


void setup() {

  // initialize control over the keyboard
  Keyboard.begin();

  // Set all keys as up
  for (i = 0; i < 80; i++) {
    keyDown[i] = 0;
  }

  for (int Row = 0; Row < 8; Row++) {
    pinMode(RowPinMap[Row], INPUT_PULLUP);
  }

  for (int Col = 0; Col < 10; Col++) {
    // use internal pullups to hold pins high
    pinMode(ColPinMap[Col], INPUT_PULLUP);
  }
}

// main keyboard scanning loop
void loop() {


  // scan through all rows
  for (int Row = 0; Row < 8; Row++) {
    // Map logical row to output pin
    RowPin = RowPinMap[Row];
    // Set output pin to OUTPUT
    pinMode(RowPin, OUTPUT);
    // Set output pin to LOW
    digitalWrite(RowPin, LOW);

    // scan through columns
    for (int Col = 0; Col < 10; Col++) {
      // calculate character map position
      keyPos = Col + (Row * 10);

      inChar = keyMap[keyPos];

      // LOW = Key pressed, HIGH = Key Not Pressed
      InputValue = digitalRead(ColPinMap[Col]);

      // debounce for each key individually
      if ((millis() - lastDebounceTime[keyPos]) > debounceDelay) {

        // if a key is pressed and wasn't already down
        if ((InputValue == LOW) && keyDown[keyPos] == 0) {

          // put the right character in the keydown array
          keyDown[keyPos] = inChar;

          // FN Modifier, replacement lookup
          if (keyDown[KeyFn]) {


            // Update every other "down" key,
            // if it has the FN replacement
            for (i = 0; i < FN_KEYS_COUNT; i++) {
              if (keyDown[fnKeyPos[i]]) {
                Keyboard.release(keyDown[fnKeyPos[i]]);
                keyDown[fnKeyPos[i]] = fnKeyMap[i];
              }
            }
          }


          // reset the debounce delay
          lastDebounceTime[keyPos] = millis();
          // pass the keypress
          if (keyPos != KeyFn) {
            Keyboard.press(keyDown[keyPos]);
          }
        }

        // key is up and a character is stored in the keydown position
        if ((InputValue == HIGH) && keyDown[keyPos] != 0) {
          // reset keybounce delay
          lastDebounceTime[keyPos] = millis();

          if (keyPos != KeyFn) {
            // send key release to OS
            Keyboard.release(keyDown[keyPos]);
          }

          // set keydown array position as up
          keyDown[keyPos] = 0;
        }
      }
    }

    // Set output pin to HIGH
    digitalWrite(RowPin, HIGH);
    // Delay to make sure it has time to go HIGH before switching to INPUT
    delay(1);
    // Set output pin back to INPUT with pullup to make sure it stays HIGH
    pinMode(RowPin, INPUT_PULLUP);
  }
}
