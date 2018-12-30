#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <elapsedMillis.h>

Adafruit_AlphaNum4 p1Alpha4 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 p2Alpha4 = Adafruit_AlphaNum4();

const int increaseP1 = 12;
const int decreaseP1 = 11;
const int resetP1 = 10;
const int increaseP2 = 9;
const int decreaseP2 = 8;
const int resetP2 = 7;
char lifeBuffer[4] = {'2', '0', '2', '0'};
char allPeriodsBuffer[4] = {'*', '*', '*', '*'};
char winBuffer[4] = {' ', ' ', ' ', ' '};
char youWinArray[11] = {'Y', 'O', 'U', ' ', 'W', 'I', 'N', '!', ' ', ' ', ' '};
char loseBuffer[4] = {' ', ' ', ' ', ' '};
char youLoseArray[12] = {'Y', 'O', 'U', ' ', 'L', 'O', 'S', 'E', '!', ' ', ' ', ' '};
char p1onesDigit;
char p1tensDigit;
char p2onesDigit;
char p2tensDigit;
int keyCount = 0;
//long randRollP1;
int p1Life = 20;
int p2Life = 20;

void setup() {
  /*TODO
     Find out why the external pullup is not working, the pin is always reading as low
     It is the same setup as the four other pins
     Finish random generator and add new button for rolling
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
  //randomSeed(analogRead(0));
}

//------------------------------------------------------------------------------------------------------
void loop() {
  resetButtons();
  intSeperation(p1Life, p2Life);
  assignBuffer(lifeBuffer);
  p1Alpha4.writeDisplay();
  p2Alpha4.writeDisplay();
  youLose();
  youWin();
  resetLife();
}

//------------------------------------------------------------------------------------------------------
/**
 * Interrupt routine for player one
 */
void ISRIncreaseP1() {
  p1Life++;
}

//------------------------------------------------------------------------------------------------------
/**
 * Interrupt routine for player one
 */
void ISRDecreaseP1() {
  p1Life--;
}

//------------------------------------------------------------------------------------------------------
/**
 * Interrupt routine for player two
 */
void ISRIncreaseP2() {
  p2Life++;
}

//------------------------------------------------------------------------------------------------------
/**
 * Interrupt routine for player two
 */
void ISRDecreaseP2() {
  p2Life--;
}

//------------------------------------------------------------------------------------------------------
/**
 * 
 */
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

//------------------------------------------------------------------------------------------------------
/**TODO: Split this function into two functions for player 1 and player 2
   Assigns the buffer for the screens to be written out
*/
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

//------------------------------------------------------------------------------------------------------
/**
   Checks if the reset buttons are pressed for 3 seconds and sets the screen to stars and resets the life
*/
void resetButtons() {
  while (digitalRead(resetP1) == LOW && digitalRead(resetP2) == LOW) {
    keyCount++;
    if (keyCount > 300) {

      resetLife();
      keyCount = 0;
    }
    delay(10);
  }
  keyCount = 0;
}

//------------------------------------------------------------------------------------------------------
/**
   Flashes the stars and resets the life
*/
void resetLife() {
  delay(100);
  assignBuffer(allPeriodsBuffer);
  p1Alpha4.writeDisplay();
  p2Alpha4.writeDisplay();
  p1Life = 20;
  p2Life = 20;
  delay(50);
}

//------------------------------------------------------------------------------------------------------
/**TODO: Combine youLose and youWin functions since they follow the same logic
   Checks if either player 1 or player 2 has zero life and prints out 'you lose' to the loser
*/
void youLose() {
  if (p1Life == 0) {
    for (int i = 0; i < 12; i++) {
      loseBuffer[0] = loseBuffer[1];
      loseBuffer[1] = loseBuffer[2];
      loseBuffer[2] = loseBuffer[3];
      loseBuffer[3] = youLoseArray[i];

      p1Alpha4.writeDigitAscii(0, loseBuffer[0]);
      p1Alpha4.writeDigitAscii(1, loseBuffer[1]);
      p1Alpha4.writeDigitAscii(2, loseBuffer[2]);
      p1Alpha4.writeDigitAscii(3, loseBuffer[3]);
      p1Alpha4.writeDisplay();
      delay(100);
    }
  } else if (p2Life == 0) {
    for (int i = 0; i < 12; i++) {
      loseBuffer[0] = loseBuffer[1];
      loseBuffer[1] = loseBuffer[2];
      loseBuffer[2] = loseBuffer[3];
      loseBuffer[3] = youLoseArray[i];

      p2Alpha4.writeDigitAscii(0, loseBuffer[0]);
      p2Alpha4.writeDigitAscii(1, loseBuffer[1]);
      p2Alpha4.writeDigitAscii(2, loseBuffer[2]);
      p2Alpha4.writeDigitAscii(3, loseBuffer[3]);
      p2Alpha4.writeDisplay();
      delay(100);
    }
  }
}

//------------------------------------------------------------------------------------------------------
/**
   Checks if player 1 or player 2 have zero life and prints out 'you win' to the winner
*/
void youWin() {
  if (p1Life == 0) {
    for (int i = 0; i < 11; i++) {
      winBuffer[0] = winBuffer[1];
      winBuffer[1] = winBuffer[2];
      winBuffer[2] = winBuffer[3];
      winBuffer[3] = youWinArray[i];

      p2Alpha4.writeDigitAscii(0, winBuffer[0]);
      p2Alpha4.writeDigitAscii(1, winBuffer[1]);
      p2Alpha4.writeDigitAscii(2, winBuffer[2]);
      p2Alpha4.writeDigitAscii(3, winBuffer[3]);
      p2Alpha4.writeDisplay();
      delay(100);
    }
  } else if (p2Life == 0) {
    for (int i = 0; i < 11; i++) {
      winBuffer[0] = winBuffer[1];
      winBuffer[1] = winBuffer[2];
      winBuffer[2] = winBuffer[3];
      winBuffer[3] = youWinArray[i];

      p1Alpha4.writeDigitAscii(0, winBuffer[0]);
      p1Alpha4.writeDigitAscii(1, winBuffer[1]);
      p1Alpha4.writeDigitAscii(2, winBuffer[2]);
      p1Alpha4.writeDigitAscii(3, winBuffer[3]);
      p1Alpha4.writeDisplay();
      delay(100);
    }
  }
}

