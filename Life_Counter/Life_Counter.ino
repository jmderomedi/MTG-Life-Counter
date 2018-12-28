#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <elapsedMillis.h>

Adafruit_AlphaNum4 p1Alpha4 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 p2Alpha4 = Adafruit_AlphaNum4();

elapsedMillis timeElapsed;

const int increaseP1 = 12;
const int decreaseP1 = 11;
const int resetP1 = 10;
const int increaseP2 = 9;
const int decreaseP2 = 8;
const int resetP2 = 7;
char lifeBuffer[4] = {'2', '0', '2', '0'};
char allPeriodsBuffer[4] = {'*', '*', '*', '*'};
char p1onesDigit;
char p1tensDigit;
char p2onesDigit;
char p2tensDigit;
int keyCount = 0;

int p1Life = 20;
int p2Life = 20;
//bool p1ResetFlag = false;
//bool p2ResetFlag = false;

void setup() {
  /*TODO
     Find out why the external pullup is not working, the pin is always reading as low
     It is the same setup as the four other pins
  */
  pinMode(resetP1, INPUT_PULLUP);
  pinMode(resetP2, INPUT_PULLUP);

  Serial.begin(9600);

  p1Alpha4.begin(0x70);
  p1Alpha4.setBrightness(5);
  p2Alpha4.begin(0x71);
  p2Alpha4.setBrightness(5);
  attachInterrupt(digitalPinToInterrupt(increaseP1), ISRIncreaseP1, FALLING);
  attachInterrupt(digitalPinToInterrupt(increaseP2), ISRIncreaseP2, FALLING);
  attachInterrupt(digitalPinToInterrupt(decreaseP1), ISRDecreaseP1, FALLING);
  attachInterrupt(digitalPinToInterrupt(decreaseP2), ISRDecreaseP2, FALLING);
  /*TODO
     Decide if I want to have the reset buttons on interrupts
  */
  //  attachInterrupt(digitalPinToInterrupt(resetP1), ISRResetP1, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(resetP2), ISRResetP1, FALLING);
}

void loop() {
  /*TODO
     Implement a timer system that reset the life after three seconds of the buttons being pushed
  */
  while (digitalRead(resetP1) == LOW && digitalRead(resetP2) == LOW) {
    keyCount++;
    Serial.println(keyCount);
    if (keyCount > 300) {
      assignBuffer(allPeriodsBuffer);
      p1Alpha4.writeDisplay();
      p2Alpha4.writeDisplay();
      p1Life = 20;
      p2Life = 20;
      keyCount = 0;
    }
    delay(10);
  }
  keyCount = 0;

  intSeperation(p1Life, p2Life);

  assignBuffer(lifeBuffer);
  p1Alpha4.writeDisplay();
  p2Alpha4.writeDisplay();
  //delay(200);
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
void intSeperation(int lifeP1, int lifeP2) {
  p1onesDigit = (lifeP1 % 10) + 48;
  p1tensDigit = ((lifeP1 / 10) % 10) + 48;
  p2onesDigit = (lifeP2 % 10) + 48;
  p2tensDigit = ((lifeP2 / 10) % 10) + 48;

  lifeBuffer[3] = p1onesDigit;
  lifeBuffer[2] = p1tensDigit;
  lifeBuffer[1] = p2onesDigit;
  lifeBuffer[0] = p2tensDigit;
}
void assignBuffer(char * displayBuffer) {
  p1Alpha4.writeDigitAscii(0, displayBuffer[0]);
  p1Alpha4.writeDigitAscii(1, displayBuffer[1]);
  p1Alpha4.writeDigitAscii(2, displayBuffer[2]);
  p1Alpha4.writeDigitAscii(3, displayBuffer[3]);

  p2Alpha4.writeDigitAscii(0, displayBuffer[2]);
  p2Alpha4.writeDigitAscii(1, displayBuffer[3]);
  p2Alpha4.writeDigitAscii(2, displayBuffer[0]);
  p2Alpha4.writeDigitAscii(3, displayBuffer[1]);
}

//void ISRResetP1() {
//  p1ResetFlag = true;
//}
//void ISRResetP2() {
//  p2ResetFlag = true;
//}
