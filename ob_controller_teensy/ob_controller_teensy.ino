// MSF_XINPUT - Version: Latest 
#include <xinput.h>

int buttonPin = 2;

int held = 0;
int heldTime = 50;
int longHold = 150;
int released = 0;
int releasedTime = 75;

int buttonPinLast = HIGH;
int buttonPinCurrent = HIGH;

int LED1 = 3;
int LED2 = 4;

char currentCode[6] = {0};
int lastActive[26] = {-1};

int speakerPin = 8;
int note1 = 659;
int note2 = 1047;
int note3 = 1760;
int delay1 = 100;
int delay2 = 100;

bool configModeLast = false;

int buttonQueue [25][25] = {};
int buttonQueuePos = 0;
int buttonQueueTotal = 0;

int buttonGroup1 [25][25] = {};
int buttonGroup2 [25][25] = {};
int buttonGroup3 [25][25] = {};
int buttonGroup4 [25][25] = {};
int buttonGroup5 [25][25] = {};
int buttonGroup6 [25][25] = {};
int buttonGroup7 [25][25] = {};
int buttonGroup8 [25][25] = {};
int buttonGroup9 [25][25] = {};
int buttonGroup10 [25][25] = {};

int16_t leftAnalog[2] = {0,0};
int16_t rightAnalog[2] = {0,0};
int POVSwitch[4] = {0,0,0,0};

// get the last `size` codes from the currentCode list
void getRecentcodes(char *code, int size)
{
  int j = 0;
  for (int i = 6 - size; i < 6; i++) {
    code[j++] = currentCode[i];
  }
}

class input {
  public:
  char code[6];   //Code
  int hasLength;  //Length
  bool isActive;  //isActive
  bool isActiveLast;
  char key;

  input(char c1[6], int l1, bool a1, bool a2, char k1) {
    memset(code, 0, 6);
    memcpy(code, c1, l1);
    hasLength = l1;
    isActive = a1;
    isActiveLast = a2;
    key = k1;
  }

  void getMatch() {
    // currentCode is the sequence from the button
    // code is this input's sequence, forwards
    // if they match, set isActive to true
    char current[hasLength];
    getRecentcodes(current, hasLength);

    if (memcmp(code, current, hasLength) == 0) {
      if (key != '&' || key != '$') {
        isActive = !isActive;
        addToArray(' ');
      } 
      if (key == '&') {
        isActive = false;
      }
      if (key == '$') {
        recall();
      }
    }
  }
  
  void notify() {
    if (isActive != isActiveLast) {
      Serial.print(code);
      Serial.print(" is ");
      Serial.println(isActive);
    }
  }
};

input codes[26] = {
  input(" .. ", 4, false, false, 'a'),       //L_STICK_UP
  input(" -- ", 4, false, false, 'b'),       //L_STICK_DOWN
  input(" .- ", 4, false, false, 'c'),       //L_STICK_LEFT
  input(" -. ", 4, false, false, 'd'),       //L_STICK_RIGHT
  input(" ... ", 5, false, false, 'e'),      //BTN_DOWN
  input(" -.. ", 5, false, false, 'f'),      //BTN_RIGHT
  input(" ..- ", 5, false, false, 'g'),      //BTN_LEFT
  input(" --- ", 5, false, false, 'h'),      //BTN_UP
  input(" --. ", 5, false, false, 'i'),      //BTN_RIGHT SHOULDER
  input(" .-. ", 5, false, false, 'j'),      //BTN_RIGHT TRIGGER
  input(" .-- ", 5, false, false, 'k'),      //BTN_LEFT SHOULDER
  input(" -.- ", 5, false, false, 'l'),      //BTN_LEFT TRIGGER
  input(" -... ", 6, false, false, 'm'),     //BTN_ST
  input(" ...- ", 6, false, false, 'n'),     //BTN_SE
  input(" .--. ", 6, false, false, 'o'),     //R_STICK_UP
  input(" -..- ", 6, false, false, 'p'),     //R_STICK_DOWN
  input(" ..-- ", 6, false, false, 'q'),     //R_STICK_LEFT
  input(" --.. ", 6, false, false, 'r'),     //R_STICK_RIGHT
  input(" .-.. ", 6, false, false, 's'),     //DPAD_UP
  input(" ..-. ", 6, false, false, 't'),     //DPAD_DOWN
  input(" .-.- ", 6, false, false, 'u'),     //DPAD_LEFT
  input(" -.-. ", 6, false, false, 'v'),     //DPAD_RIGHT
  input(" .... ", 6, false, false, 'w'),     //BTN_L3
  input(" ---- ", 6, false, false, 'x'),     //BTN_R3
  input(" . ", 3, false, false, '&'),        //CONFIG MODE
  input(" - ", 3, false, false, '$')         //RECALL
};

void addToArray(char toAdd) {
  currentCode[0] = currentCode[1];
  currentCode[1] = currentCode[2];
  currentCode[2] = currentCode[3];
  currentCode[3] = currentCode[4];
  currentCode[4] = currentCode[5];
  currentCode[5] = toAdd;
}

void recall() {
  int currentRow = 0;
  int currentColumn = 0;

  if (!checkButtonsStatus()) {
    buttonQueue[23][24] = 1;
  }
  if (checkButtonsStatus()) {
    //  Find empty row
    for (int i = 0; i < 25; i++) {
      if (buttonQueue[i][0] != 255) {
        currentRow++;
        buttonQueueTotal++;
      } else {
        break;
      }
    }
    
    //  Check which buttons are true and store them in an array
    for (int i = 0; i < 24; i++) {
      if (codes[i].isActive == true) {
        //  Add button to array
        buttonQueue[currentRow][currentColumn] = i;
        //  Set isActive false to prevent repeat readings
        codes[i].isActive = false;
        //  Increment currentColumn
        currentColumn++; 
      }
    }
  }
}

XINPUT controller(NO_LED);

void gp_releaseAll() {
  // Release all buttons
  uint8_t buttonStatus[11];
  for (int i = 0; i < 11; i++) {
    buttonStatus[i] = 0;
  }
  controller.buttonArrayUpdate(buttonStatus);

  // Reset Dpad
  controller.dpadUpdate(0,0,0,0);

  // Reset triggers
  controller.triggerUpdate(0,0);

  //Reset sticks
  controller.stickUpdate(STICK_LEFT, 0, 0);
  controller.stickUpdate(STICK_RIGHT, 0, 0);
}

bool checkButtonsStatus() {
  int v = 0;
  for (int i = 0; i < 24; i++) {
    if (codes[i].isActive) {
      v++;
    }
  }
  if (v > 0) {
    return true;
  } else {
    return false;
  }
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // Serial.begin(9600);
}

void loop() {
  buttonPinCurrent = digitalRead(buttonPin);
  if (buttonPinCurrent == LOW) {
    held++;
    if (codes[24].isActive) {
      if (held == longHold) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+100) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+200) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+300) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+400) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+500) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+600) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+700) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+800) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+900) {
        tone(speakerPin, note3, delay1/2);
      }
      if (held == longHold+1000) {
        tone(speakerPin, note3, delay1/2);
        delay(delay1/2);
        tone(speakerPin, note2, delay1);
      }
    }
    delay(1);
  } else {
    released++;
    delay(1);
    if (codes[24].isActive) {
      if (released == releasedTime) {
        addToArray(' ');
        // Serial.println(currentCode);
      }
    }
  }

  if (buttonPinCurrent == HIGH && buttonPinLast == LOW) {
    if (held <= heldTime){
      if (codes[24].isActive) {
        addToArray('.');
        tone(speakerPin, note1, delay1/2);
        // Serial.println(currentCode);
      } else {
        codes[24].isActive = !codes[24].isActive;
      }
    }
    
    if (held > heldTime && held < longHold) {
      if (codes[24].isActive) {
        addToArray('-');
        tone(speakerPin, note2, delay1*2);
        // Serial.println(currentCode);
      }
    }
    
    // Button group one
    if (held >= longHold && held < longHold+100) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonGroup1[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
                buttonGroup1[0][currentColumn] = i;
                currentColumn++;
              }
            }
            buttonGroup1[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
                buttonGroup1[i][j] = buttonQueue[i][j];
              }
            }
            buttonGroup1[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup1[i][j];
            }
          }
          buttonQueueTotal = buttonGroup1[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }
    
    // Button group two
    if (held >= longHold+100 && held < longHold+200) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonGroup2[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
                buttonGroup2[0][currentColumn] = i;
                currentColumn++;
              }
            }
            buttonGroup2[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
                buttonGroup2[i][j] = buttonQueue[i][j];
              }
            }
            buttonGroup2[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup2[i][j];
            }
          }
          buttonQueueTotal = buttonGroup2[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }

    // Button group three
    if (held >= longHold+200 && held < longHold+300) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
             buttonGroup3[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
               buttonGroup3[0][currentColumn] = i;
                currentColumn++;
              }
            }
           buttonGroup3[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
               buttonGroup3[i][j] = buttonQueue[i][j];
              }
            }
           buttonGroup3[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup3[i][j];
            }
          }
          buttonQueueTotal = buttonGroup3[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }

    // Button group four
    if (held >= longHold+300 && held < longHold+400) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
             buttonGroup4[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
               buttonGroup4[0][currentColumn] = i;
                currentColumn++;
              }
            }
           buttonGroup4[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
               buttonGroup4[i][j] = buttonQueue[i][j];
              }
            }
           buttonGroup4[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup4[i][j];
            }
          }
          buttonQueueTotal = buttonGroup4[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }

    // Button group five
    if (held >= longHold+400 && held < longHold+500) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
             buttonGroup5[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
               buttonGroup5[0][currentColumn] = i;
                currentColumn++;
              }
            }
           buttonGroup5[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
               buttonGroup5[i][j] = buttonQueue[i][j];
              }
            }
           buttonGroup5[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup5[i][j];
            }
          }
          buttonQueueTotal = buttonGroup5[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }

    // Button group six
    if (held >= longHold+500 && held < longHold+600) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
             buttonGroup6[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
               buttonGroup6[0][currentColumn] = i;
                currentColumn++;
              }
            }
           buttonGroup6[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
               buttonGroup6[i][j] = buttonQueue[i][j];
              }
            }
           buttonGroup6[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup6[i][j];
            }
          }
          buttonQueueTotal = buttonGroup6[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }

    // Button group seven
    if (held >= longHold+600 && held < longHold+700) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
             buttonGroup7[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
               buttonGroup7[0][currentColumn] = i;
                currentColumn++;
              }
            }
           buttonGroup7[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
               buttonGroup7[i][j] = buttonQueue[i][j];
              }
            }
           buttonGroup7[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup7[i][j];
            }
          }
          buttonQueueTotal = buttonGroup7[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }

    // Button group eight
    if (held >= longHold+700 && held < longHold+800) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
             buttonGroup8[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
               buttonGroup8[0][currentColumn] = i;
                currentColumn++;
              }
            }
           buttonGroup8[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
               buttonGroup8[i][j] = buttonQueue[i][j];
              }
            }
           buttonGroup8[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup8[i][j];
            }
          }
          buttonQueueTotal = buttonGroup8[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }

    // Button group nine
    if (held >= longHold+800 && held < longHold+900) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
             buttonGroup9[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
               buttonGroup9[0][currentColumn] = i;
                currentColumn++;
              }
            }
           buttonGroup9[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
               buttonGroup9[i][j] = buttonQueue[i][j];
              }
            }
           buttonGroup9[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup9[i][j];
            }
          }
          buttonQueueTotal = buttonGroup9[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }

    // Button group ten
    if (held >= longHold+900 && held < longHold+1000) {
      if (codes[24].isActive) {
        if (checkButtonsStatus() || buttonQueue[0][0] != 255) {
          // Clear the group
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
             buttonGroup10[i][j] = 255;
            }
          }
          // If there are any buttons added, set them so in the group
          if (checkButtonsStatus()) {
            int currentColumn = 0;
            for (int i = 0; i < 24; i++) {
              if (codes[i].isActive) {
               buttonGroup10[0][currentColumn] = i;
                currentColumn++;
              }
            }
           buttonGroup10[24][24] = 1;
          }
          // If there are any buttons in the queue, add them to the group
          // (This will overwrite any buttons added to the group previously)
          if (buttonQueueTotal != 0) {
            for (int i = 0; i < 25; i++) {
              for (int j = 0; j < 25; j++) {
               buttonGroup10[i][j] = buttonQueue[i][j];
              }
            }
           buttonGroup10[24][24] = buttonQueueTotal;
          }
        } 
        if (!checkButtonsStatus() && buttonQueue[0][0] == 255) {
          // Fill the button queue (after clearing it)
          // Clear button queue
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = 255;
            }
          }
          // Fill the queue and renew the total
          for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 25; j++) {
              buttonQueue[i][j] = buttonGroup10[i][j];
            }
          }
          buttonQueueTotal = buttonGroup10[24][24];
          buttonQueue[24][24] = 255; // Clear out the extra total
        }
      }
    }
    
    // Serial.println(held);
    // Serial.println(codes[24].isActive);
    held = 0;
    released = 0;
  }

  
  // Clear button queue when we enter config mode
  if (codes[24].isActive == true && configModeLast != codes[24].isActive) {
    for (int i = 0; i < 25; i++) {
      for (int j = 0; j < 25; j++) {
        buttonQueue[i][j] = 255;
      }
    }
    buttonQueuePos = 0;
    buttonQueueTotal = 0;
    buttonQueue[23][24] = 0;
  }

  if (codes[24].isActive) {   // codes[24] is our configuration code
    gp_releaseAll();
    if (!configModeLast) {
      // Deactivate all buttons
      for (int i = 0; i < 24; i++) {
        codes[i].isActive = false;
      }
      // Dump the array 
      for (int i=0; i < 6; i++) {
        addToArray(' ');
      }
    }
    
    // Indicate whether a dot (.), dash (-), or space was entered via LED
    if (currentCode[5] == '.') {
      analogWrite(LED1, 0);
    } else {
      analogWrite(LED1, 1);
    }
    if (currentCode[5] == '-') {
      analogWrite(LED2, 0);
    } else {
      analogWrite(LED2, 1);
    }
    if (currentCode[5] == ' ') {
      analogWrite(LED1, 1);
      analogWrite(LED2, 1);
    }
    
    for (int i = 0; i < 26; i++) {
      codes[i].getMatch();
    }
    
  } else {
    //  Set isActive for current position to true
    //  when the button is pressed
    if (buttonPinCurrent == LOW && buttonPinLast == HIGH) {
      //  Check columns of current row and set isActive to true
      for (int i = 0; i < 25; i++) {
        if (buttonQueue[buttonQueuePos][i] != 255) {
          codes[buttonQueue[buttonQueuePos][i]].isActive = true;
        } else {
          break;
        }
      }
    }

    //  While the button is held, send the input
    if (buttonPinCurrent == LOW && buttonPinLast == LOW) {
      if (held >= heldTime) {
        if (codes[0].isActive || codes[1].isActive) {
          if (codes[0].isActive) {
            leftAnalog[1] = 32767;    // LEFT STICK UP
          }
          if (codes[1].isActive) {
            leftAnalog[1] = -32767;    // LEFT STICK DOWN
          }
        } else {
          leftAnalog[1] = 0;
        }
        if (codes[2].isActive || codes[3].isActive) {
          if (codes[2].isActive) {
            leftAnalog[0] = -32767;    // LEFT STICK LEFT
          }
          if (codes[3].isActive) {
            leftAnalog[0] = 32767;    // LEFT STICK RIGHT
          }
        } else {
          leftAnalog[0] = 0;
        }
        if (codes[4].isActive) {
          controller.buttonUpdate(BUTTON_A, 1);
        }
        if (codes[5].isActive) {
          controller.buttonUpdate(BUTTON_B, 1);
        }
        if (codes[6].isActive) {
          controller.buttonUpdate(BUTTON_X, 1);
        }
        if (codes[7].isActive) {
          controller.buttonUpdate(BUTTON_Y, 1);
        }
        if (codes[8].isActive) {
          controller.buttonUpdate(BUTTON_RB, 1);
        }
        if (codes[9].isActive) {
          controller.singleTriggerUpdate(TRIGGER_RIGHT, 255);
        }
        if (codes[10].isActive) {
          controller.buttonUpdate(BUTTON_LB, 1);
        }
        if (codes[11].isActive) {
          controller.singleTriggerUpdate(TRIGGER_LEFT, 255);
        }
        if (codes[12].isActive) {
          controller.buttonUpdate(BUTTON_START, 1);
        }
        if (codes[13].isActive) {
          controller.buttonUpdate(BUTTON_BACK, 1);
        }
        if (codes[14].isActive || codes[15].isActive) {
          if (codes[14].isActive) {
            rightAnalog[1] = 32767;    // RIGHT STICK UP
          }
          if (codes[15].isActive) {
            rightAnalog[1] = -32767;    // RIGHT STICK DOWN
          }
        } else {
          rightAnalog[1] = 0;
        }
        if (codes[16].isActive || codes[17].isActive) {
          if (codes[16].isActive) {
            rightAnalog[0] = -32767;    // RIGHT STICK LEFT
          }
          if (codes[17].isActive) {
            rightAnalog[0] = 32767;    // RIGHT STICK RIGHT
          }
        } else {
          rightAnalog[0] = 0;
        }
        if (codes[18].isActive) {
          POVSwitch[0] = 1;
        } else {
          POVSwitch[0] = 0;
        }
        if (codes[19].isActive) {
          POVSwitch[1] = 1;
        } else {
          POVSwitch[1] = 0;
        }
        if (codes[20].isActive) {
          POVSwitch[2] = 1;
        } else {
          POVSwitch[2] = 0;
        }
        if (codes[21].isActive) {
          POVSwitch[3] = 1;
        } else {
          POVSwitch[3] = 0;
        }
        if (codes[22].isActive) {
          controller.buttonUpdate(BUTTON_L3, 1);
        }
        if (codes[23].isActive) {
          controller.buttonUpdate(BUTTON_R3, 1);
        }
        controller.stickUpdate(STICK_LEFT, leftAnalog[0], leftAnalog[1]);
        controller.stickUpdate(STICK_RIGHT, rightAnalog[0], rightAnalog[1]);
        controller.dpadUpdate(POVSwitch[0],POVSwitch[1],POVSwitch[2],POVSwitch[3]); 
      }
    }

    //  When the button is released, increment row to total
    if (buttonPinCurrent == HIGH && buttonPinCurrent != buttonPinLast) {
      gp_releaseAll();
      if (buttonQueue[0][0] != 255) {
        if (buttonQueue[23][24] == 0) {
          for (int i = 0; i < 24; i++) {
            codes[i].isActive = false;
          } 
          if (buttonQueuePos != buttonQueueTotal-1) {
            buttonQueuePos++;
          }
        } 
        if (buttonQueue[23][24] == 1) {
          for (int i = 0; i < 24; i++) {
            codes[i].isActive = false;
          } 
          if (buttonQueuePos != buttonQueueTotal-1) {
            buttonQueuePos++;
          } else {
            buttonQueuePos = 0;
          }
        }
      }
    }
    analogWrite(LED1, 0);
    analogWrite(LED2, 0);
  }
  
  //Play sound when entering and exiting configmode
  if (codes[24].isActive == true && configModeLast != codes[24].isActive) {
    tone(speakerPin, note1, delay1);
    delay(delay1);
    tone(speakerPin, note2, delay2);
  }
  if (codes[24].isActive == false && configModeLast != codes[24].isActive) {
    tone(speakerPin, note2, delay1);
    delay(delay1);
    tone(speakerPin, note1, delay2);
  }
  
  //Send data
  controller.sendXinput();

  //Receive data
  controller.receiveXinput();
  
  buttonPinLast = buttonPinCurrent;
  configModeLast = codes[24].isActive;
  for (int i = 0; i < 25; i++) {
    codes[i].isActiveLast = codes[i].isActive;
  }
}
