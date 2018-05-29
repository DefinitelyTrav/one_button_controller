// MSF_XINPUT - Version: Latest 
#include <xinput.h>

int buttonPin = 2;
int held = 0;
int heldTime = 50;
int released = 0;
int releasedTime = 75;
int buttonPinLast = HIGH;
int buttonPinCurrent = HIGH;

int LED1 = 3;
int LED2 = 4;

char currentCode[6] = {0};
int lastActive[26] = {-1};

bool configModeLast = false;

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
      // Serial.print(code);
      // Serial.print(" is ");
      // Serial.println(isActive);
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
  input(" --.. ", 6, false, false, 'q'),     //R_STICK_LEFT
  input(" ..-- ", 6, false, false, 'r'),     //R_STICK_RIGHT
  input(" .-.. ", 6, false, false, 's'),     //DPAD_UP
  input(" ..-. ", 6, false, false, 't'),     //DPAD_DOWN
  input(" -.-. ", 6, false, false, 'u'),     //DPAD_LEFT
  input(" .-.- ", 6, false, false, 'v'),     //DPAD_RIGHT
  input(" .... ", 6, false, false, 'w'),     //BTN_L3
  input(" ---- ", 6, false, false, 'x'),     //BTN_R3
  input(" . ", 3, false, false, '&'),        //CONFIG MODE
  input(" - ", 3, false, false, '$')          //RECALL
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
  for (int i = 0; i < 26; i++) {
    if (lastActive[i] >= 0) {
      codes[lastActive[i]].isActive = true;
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
    if (held > heldTime) {
      if (codes[24].isActive) {
        addToArray('-');
        // Serial.println(currentCode);
      }
    } else {
      if (codes[24].isActive) {
        addToArray('.');
        // Serial.println(currentCode);
      } else {
        codes[24].isActive = !codes[24].isActive;
      }
    }
    // Serial.println(held);
    // Serial.println(codes[24].isActive);
    held = 0;
    released = 0;
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
    if (buttonPinCurrent == LOW) {
      if (held >= heldTime) {
        if (codes[0].isActive) {
          controller.stickUpdate(STICK_LEFT, 0, -32767);    // LEFT STICK UP
        }
        if (codes[1].isActive) {
          controller.stickUpdate(STICK_LEFT, 0, 32767);   // LEFT STICK DOWN
        }
        if (codes[2].isActive) {
          controller.stickUpdate(STICK_LEFT, -32767, 0);   // LEFT STICK LEFT
        }
        if (codes[3].isActive) {
          controller.stickUpdate(STICK_LEFT, 32767, 0);    // LEFT STICK RIGHT
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
        if (codes[14].isActive) {
          controller.stickUpdate(STICK_RIGHT, 0, -32767);    // RIGHT STICK UP
        }
        if (codes[15].isActive) {
          controller.stickUpdate(STICK_RIGHT, 0, 32767);   // RIGHT STICK DOWN
        }
        if (codes[16].isActive) {
          controller.stickUpdate(STICK_RIGHT, -32767, 0);   // RIGHT STICK LEFT
        }
        if (codes[17].isActive) {
          controller.stickUpdate(STICK_RIGHT, 32767, 0);    // RIGHT STICK RIGHT
        }
        if (codes[18].isActive) {
          controller.dpadUpdate(1,0,0,0);
        }
        if (codes[19].isActive) {
          controller.dpadUpdate(0,1,0,0);
        }
        if (codes[20].isActive) {
          controller.dpadUpdate(0,0,1,0);
        }
        if (codes[21].isActive) {
          controller.dpadUpdate(0,0,0,1);
        }
        if (codes[22].isActive) {
          controller.buttonUpdate(BUTTON_L3, 1);
        }
        if (codes[23].isActive) {
          controller.buttonUpdate(BUTTON_R3, 1);
        }
      }
    } else {
      gp_releaseAll();
    }
    analogWrite(LED1, 0);
    analogWrite(LED2, 0);
  }
  
  // Store last activated buttons in array in case they need to be recalled
  // Do this when we exit config mode
  // Empty the array first, then fill with the new code
  if (codes[24].isActive == false && configModeLast != codes[24].isActive) {
    int j = 0;
    // Clear the array
    for (int i = 0; i < 26; i++) {
      lastActive[i] = -1;
    }
    // See which entries should be added to the array
    // Add items that are active to the array
    for (int i = 0; i < 24; i++) {
      if (codes[i].isActive == true) {
        lastActive[j] = i;
        j++;
      }
    }
    j = 0;
  }
  //Send data
  controller.sendXinput();

  //Receive data
  controller.receiveXinput();
  
  for (int i = 0; i < 25; i++) {
    codes[i].notify();
  }
  buttonPinLast = buttonPinCurrent;
  configModeLast = codes[24].isActive;
  for (int i = 0; i < 25; i++) {
    codes[i].isActiveLast = codes[i].isActive;
  }
}