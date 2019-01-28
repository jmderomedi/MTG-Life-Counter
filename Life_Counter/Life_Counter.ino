/**
   Author: James Deromedi

   TODO: Add ability to have more than 100 life
               Possibily include the period of alpha for each hundred
               Have seperate LEDs for each hundred
         Add random number roll for players
         Think about how to have more than two players
         Have 'win' and 'lose' print out at the same time
*/
#include <Wire.h>
#include <Snooze.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <elapsedMillis.h>

Adafruit_AlphaNum4 p1Alpha4 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 p2Alpha4 = Adafruit_AlphaNum4();

SnoozeDigital digital;
SnoozeBlock config(digital);

const int INCREASEP1 = 11;
const int DECREASEP1 = 10;
const int RESETP1 = 9;
const int INCREASEP2 = 7;
const int DECREASEP2 = 6;
const int RESETP2 = 4;

char lifeBuffer[4] = {'2', '0', '2', '0'};
char allPeriodsBuffer[4] = {'*', '*', '*', '*'};
char winBuffer[4] = {' ', ' ', ' ', ' '};
char loseBuffer[4] = {' ', ' ', ' ', ' '};

char p1onesDigit;
char p1tensDigit;
char p2onesDigit;
char p2tensDigit;

int keyCount = 0;
int p1Life = 20;
int p2Life = 20;
int who = 0;

bool usbDisable = true;
//long randRollP1;

//------------------------------------------------------------------------------------------------------
void setup() {
  for (int i = 0; i < 46; i++) {    //Sets all unused pins to output to save a tiny bit of power
    if (!(i == INCREASEP1) || !(i == DECREASEP1) || !(i == RESETP1) || !(i == INCREASEP2) || !(i == DECREASEP2) || !(i == RESETP2)) {
      pinMode(i, OUTPUT);
    }
    if (usbDisable) {
      Serial.end();
    }

  }
  /*TODO
     Find out why the external pullup is not working, the pin is always reading as low
     It is the same setup as the four other pins
     Debug why each pin needs software pullups when they contain hardware pullups
            check new voltage from battery
     Finish random generator and add new button for rolling
  */
  //Serial.begin(9600);

  p1Alpha4.begin(0x70);       //Address for player 1 alpha
  p2Alpha4.begin(0x71);       //Address for player 2 alpha

  p1Alpha4.setBrightness(4);        //Brightness for alpha (1-15)
  p2Alpha4.setBrightness(4);        //Anything > 15 == 15

  digital.pinMode(INCREASEP1, INPUT_PULLUP, FALLING);      //Setup sleep module for each button pin
  digital.pinMode(INCREASEP2, INPUT_PULLUP, FALLING);
  digital.pinMode(DECREASEP1, INPUT_PULLUP, FALLING);
  digital.pinMode(DECREASEP2, INPUT_PULLUP, FALLING);
  digital.pinMode(RESETP1, INPUT_PULLUP, FALLING);
  digital.pinMode(RESETP2, INPUT_PULLUP, FALLING);

  //randomSeed(analogRead(0));
}

//------------------------------------------------------------------------------------------------------
void loop() {

  switch (who) {           //Switch case as "sudo" interrupt since sleep doesnt allow ISRs
    case INCREASEP1:
      ISRIncreaseP1();
      break;
    case DECREASEP1:
      ISRDecreaseP1();
      break;
    case INCREASEP2:
      ISRIncreaseP2();
      break;
    case DECREASEP2:
      ISRDecreaseP2();
      break;
    case RESETP1:
      resetButtons();
      break;
    case RESETP2:
      resetButtons();
      break;
    default:
      break;
  }
  printWinner();
  intSeperation(p1Life, p2Life);
  assignBufferP1(lifeBuffer, false);
  assignBufferP2(lifeBuffer);
  p1Alpha4.writeDisplay();
  p2Alpha4.writeDisplay();
  who = Snooze.deepSleep( config );         //Put the teensy into deepSleep and return pin# that woke it
}

//------------------------------------------------------------------------------------------------------
/**
   Finds the first and tenth digit of the players life by using mod
   Adds the appropraite shift for hex value
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
void assignBufferP1(char * displayBuffer, bool p2PrintOut) {
  if (p2PrintOut) {
    p2Alpha4.writeDigitAscii(0, displayBuffer[0]);
    p2Alpha4.writeDigitAscii(1, displayBuffer[1]);
    p2Alpha4.writeDigitAscii(2, displayBuffer[2]);
    p2Alpha4.writeDigitAscii(3, displayBuffer[3]);
    p2Alpha4.writeDisplay();
  } else {
    p1Alpha4.writeDigitAscii(0, displayBuffer[0]);
    p1Alpha4.writeDigitAscii(1, displayBuffer[1]);
    p1Alpha4.writeDigitAscii(2, displayBuffer[2]);
    p1Alpha4.writeDigitAscii(3, displayBuffer[3]);
    p1Alpha4.writeDisplay();
  }
}

void assignBufferP2(char * displayBuffer) {
  p2Alpha4.writeDigitAscii(0, displayBuffer[2]);
  p2Alpha4.writeDigitAscii(1, displayBuffer[3]);
  p2Alpha4.writeDigitAscii(2, displayBuffer[0]);
  p2Alpha4.writeDigitAscii(3, displayBuffer[1]);
  p2Alpha4.writeDisplay();
}
//------------------------------------------------------------------------------------------------------
/**
   Checks if the reset buttons are pressed for 3 seconds and sets the screen to stars and resets the life
*/
bool stateFlag = false;
void resetButtons() {
  while (digitalRead(RESETP1) == LOW && digitalRead(RESETP2) == LOW && !stateFlag) {
    keyCount++;
    if (keyCount > 300) {     //Should take 3 seconds to complete

      resetLife();
      keyCount = 0;
      stateFlag = true;
    }
    delay(10);
  }
  //delay(200);
  keyCount = 0;

  stateFlag = false;       //resets the flag for next time
}

//------------------------------------------------------------------------------------------------------
/**
   Flashes the stars and resets the life
*/
void resetLife() {
  delay(100);
  assignBufferP1(allPeriodsBuffer, false);
  assignBufferP2(allPeriodsBuffer);
  p1Alpha4.writeDisplay();
  p2Alpha4.writeDisplay();
  p1Life = 20;
  p2Life = 20;
  delay(1000);
}

//------------------------------------------------------------------------------------------------------
/**
   Interrupt routine for player one
*/
void ISRIncreaseP1() {
  p1Life++;
}

//------------------------------------------------------------------------------------------------------
/**
   Interrupt routine for player one
*/
void ISRDecreaseP1() {
  p1Life--;
}

//------------------------------------------------------------------------------------------------------
/**
   Interrupt routine for player two
*/
void ISRIncreaseP2() {
  p2Life++;
}

//------------------------------------------------------------------------------------------------------
/**
   Interrupt routine for player two
*/
void ISRDecreaseP2() {
  p2Life--;
}

