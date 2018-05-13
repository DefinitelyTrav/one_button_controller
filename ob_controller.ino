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
  char key;

  input(char c1[6], int l1, bool a1, char k1) {
    memset(code, 0, 6);
    memcpy(code, c1, l1);
    hasLength = l1;
    isActive = a1;
    key = k1;
  }

  void getMatch() {
    // currentCode is the sequence from the button
    // code is this input's sequence, forwards
    // if they match, set isActive to true
    char current[hasLength];
    getRecentcodes(current, hasLength);

    if (memcmp(code, current, hasLength) == 0) {
      isActive = !isActive;
    }
  }
  
  void doPress() {
    if (isActive) {
      Keyboard.press(key);
    }
  }
};

input codes[25] = {
  input(" .. ", 4, false, 'a'),       //L_STICK_UP
  input(" -- ", 4, false, 'b'),       //L_STICK_DOWN
  input(" -. ", 4, false, 'c'),       //L_STICK_LEFT
  input(" .- ", 4, false, 'd'),       //L_STICK_RIGHT
  input(" ... ", 5, false, 'e'),      //BTN_A
  input(" ..- ", 5, false, 'f'),      //BTN_B
  input(" -.. ", 5, false, 'g'),      //BTN_X
  input(" --- ", 5, false, 'h'),      //BTN_Y
  input(" --. ", 5, false, 'i'),      //BTN_R1
  input(" .-. ", 5, false, 'j'),      //BTN_R2
  input(" .-- ", 5, false, 'k'),      //BTN_L1
  input(" -.- ", 5, false, 'l'),      //BTN_L2
  input(" -... ", 6, false, 'm'),     //BTN_ST
  input(" ...- ", 6, false, 'n'),     //BTN_SE
  input(" .--. ", 6, false, 'o'),     //R_STICK_UP
  input(" -..- ", 6, false, 'p'),     //R_STICK_DOWN
  input(" --.. ", 6, false, 'q'),     //R_STICK_LEFT
  input(" ..-- ", 6, false, 'r'),     //R_STICK_RIGHT
  input(" .-.. ", 6, false, 's'),     //DPAD_UP
  input(" ..-. ", 6, false, 't'),     //DPAD_DOWN
  input(" -.-. ", 6, false, 'u'),     //DPAD_LEFT
  input(" .-.- ", 6, false, 'v'),     //DPAD_RIGHT
  input(" .... ", 6, false, 'w'),     //BTN_L3
  input(" ---- ", 6, false, 'x'),      //BTN_R3
  input(" . ", 3, false, '&')          //CONFIG MODE
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
  Serial.begin(9600);
}

void loop() {
  buttonPinCurrent = digitalRead(buttonPin);
  if (buttonPinCurrent == LOW) {
    held++;
    delay(1);
  } else {
    released++;
    delay(1);
    if (released == 300) {
      addToArray(' ');
      Serial.println(currentCode);
    }
  }

  if (buttonPinCurrent == HIGH && buttonPinLast == LOW) {
    if (held > 100) {
      if (codes[24].isActive) {
        addToArray('-');
        Serial.println(currentCode);
      }
    } else {
      if (codes[24].isActive) {
        addToArray('.');
        Serial.println(currentCode);
      } else {
        codes[24].isActive = !codes[24].isActive;
      }
    }
    Serial.println(held);
    held = 0;
    released = 0;
  }

  if (codes[24].isActive) {   // codes[24] is our configuration code
    Keyboard.releaseAll();
    if (!configModeLast) {
      for (int i = 0; i < 24; i++) {
        codes[i].isActive = false;
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
  buttonPinLast = buttonPinCurrent;
  configModeLast = codes[24].isActive;
}
