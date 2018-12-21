#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
const int increaseP1 = 12;
const int decreaseP1 = 11;
const int resetP1 = 10;
const int increaseP2 = 9;
const int decreaseP2 = 8;
const int resetP2 = 7;

int p1Life = 20;
int p2Life = 20;
//bool p1ResetFlag = false;
//bool p2ResetFlag = false;

void setup() {
  //  pinMode(increaseP1, INPUT_PULLUP);
  //  pinMode(increaseP2, INPUT_PULLUP);
  //  pinMode(decreaseP1, INPUT_PULLUP);
  //  pinMode(decreaseP2, INPUT_PULLUP);
    pinMode(resetP1, INPUT_PULLUP);
    pinMode(resetP2, INPUT_PULLUP);

  Serial.begin(9600);

  alpha4.begin(0x70);  // pass in the address

  attachInterrupt(digitalPinToInterrupt(increaseP1), ISRIncreaseP1, FALLING);
  attachInterrupt(digitalPinToInterrupt(increaseP2), ISRIncreaseP2, FALLING);
  attachInterrupt(digitalPinToInterrupt(decreaseP1), ISRDecreaseP1, FALLING);
  attachInterrupt(digitalPinToInterrupt(decreaseP2), ISRDecreaseP2, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(resetP1), ISRResetP1, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(resetP2), ISRResetP1, FALLING);
}

char displaybuffer[4] = {'0', '2', ' ', ' '};
char p1onesDigit;
char p1tensDigit;
char p2onesDigit;
char p2tensDigit;

void loop() { 
  if (digitalRead(resetP1) == LOW && digitalRead(resetP2) == LOW) {
    p1Life = 20;
    p2Life = 20;
  }

  p1onesDigit = (p1Life % 10) + 48;
  p1tensDigit = ((p1Life / 10) % 10) + 48;
  p2onesDigit = (p2Life % 10) + 48;
  p2tensDigit = ((p2Life / 10) % 10) + 48;

  displaybuffer[3] = p1onesDigit;
  displaybuffer[2] = p1tensDigit;
  displaybuffer[1] = p2onesDigit;
  displaybuffer[0] = p2tensDigit;

  // set every digit to the buffer
  alpha4.writeDigitAscii(0, displaybuffer[0]);
  alpha4.writeDigitAscii(1, displaybuffer[1]);
  alpha4.writeDigitAscii(2, displaybuffer[2]);
  alpha4.writeDigitAscii(3, displaybuffer[3]);

  // write it out!
  alpha4.writeDisplay();
  delay(200);
}
void ISRIncreaseP1() {
  p1Life++;
}

void ISRDecreaseP1() {
  p1Life--;
}

void ISRIncreaseP2() {
  p2Life++;
}

void ISRDecreaseP2() {
  p2Life--;
}

//void ISRResetP1() {
//  p1ResetFlag = true;
//}
//void ISRResetP2() {
//  p2ResetFlag = true;
//}
