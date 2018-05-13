// Keyboard - Version: Latest 
#include <Keyboard.h>

int buttonPin = 2;
int held = 0;
int released = 0;
int buttonPinLast = HIGH;
int buttonPinCurrent = HIGH;

int LED1 = 3;
int LED2 = 4;

char currentCode[6] = {0};

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
      if (key != '&') {
        isActive = true;
      } else {
        isActive = false;
      }
    }
  }
  
  void doPress() {
    if (isActive) {
      Keyboard.press(key);
    }
  }
  
  void notify() {
    if (isActive != isActiveLast) {
      // Serial.print(key);
      // Serial.print(" is ");
      // Serial.println(isActive);
    }
  }
};

input codes[25] = {
  input(" .. ", 4, false, false, 'a'),       //L_STICK_UP
  input(" -- ", 4, false, false, 'b'),       //L_STICK_DOWN
  input(" -. ", 4, false, false, 'c'),       //L_STICK_LEFT
  input(" .- ", 4, false, false, 'd'),       //L_STICK_RIGHT
  input(" ... ", 5, false, false, 'e'),      //BTN_A
  input(" ..- ", 5, false, false, 'f'),      //BTN_B
  input(" -.. ", 5, false, false, 'g'),      //BTN_X
  input(" --- ", 5, false, false, 'h'),      //BTN_Y
  input(" --. ", 5, false, false, 'i'),      //BTN_R1
  input(" .-. ", 5, false, false, 'j'),      //BTN_R2
  input(" .-- ", 5, false, false, 'k'),      //BTN_L1
  input(" -.- ", 5, false, false, 'l'),      //BTN_L2
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
  input(" ---- ", 6, false, false, 'x'),      //BTN_R3
  input(" . ", 3, false, false, '&')          //CONFIG MODE
};

void addToArray(char toAdd) {
  currentCode[0] = currentCode[1];
  currentCode[1] = currentCode[2];
  currentCode[2] = currentCode[3];
  currentCode[3] = currentCode[4];
  currentCode[4] = currentCode[5];
  currentCode[5] = toAdd;
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  Keyboard.begin();
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
      if (released == 300) {
        addToArray(' ');
        // Serial.println(currentCode);
      }
    }
  }

  if (buttonPinCurrent == HIGH && buttonPinLast == LOW) {
    if (held > 100) {
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
    Keyboard.releaseAll();
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
      digitalWrite(LED1, LOW);
    } else {
      digitalWrite(LED1, HIGH);
    }
    if (currentCode[5] == '-') {
      digitalWrite(LED2, LOW);
    } else {
      digitalWrite(LED2, HIGH);
    }
    if (currentCode[5] == ' ') {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
    }
    
    for (int i = 0; i < 25; i++) {
      codes[i].getMatch();
    }
    
  } else {
    if (buttonPinCurrent == LOW) {
      if (held >= 100) {
        for (int i = 0; i < 24; i++) {  // Do not do the keypress for codes[24]
          codes[i].doPress();
        }
      }
    } else {
      Keyboard.releaseAll();
    }
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
  }
  for (int i = 0; i < 25; i++) {
    codes[i].notify();
  }
  buttonPinLast = buttonPinCurrent;
  configModeLast = codes[24].isActive;
  for (int i = 0; i < 25; i++) {
    codes[i].isActiveLast = codes[i].isActive;
  }
}
