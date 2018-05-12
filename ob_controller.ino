// Keyboard - Version: Latest 
#include <Keyboard.h>

int buttonPin = 2;
int held = 0;
int released = 0;
int buttonPinLast = HIGH;
int buttonPinCurrent = HIGH;

int LED1 = 3;
int LED2 = 4;

char activeCode[6] = {'-', '.', '-', '.', '.', '.'};
char currentCode[6] = {0};

bool configMode = false;

// get the last `size` inputs from the currentCode list
void getRecentInputs(char *code, int size)
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
    getRecentInputs(current, hasLength);

    if (memcmp(code, current, hasLength) == 0) {
      isActive = true;
    }
  }
  
  void doPress() {
    if (isActive == true) {
      Keyboard.press(key);
    }
  }
};

input inputs[24] = {
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
  input(" ---- ", 6, false, 'x')      //BTN_R3
};

void toggle(bool &toToggle) {
  toToggle = !toToggle;
}

void addToArray(char toAdd) {
  currentCode[0] = currentCode[1];
  currentCode[1] = currentCode[2];
  currentCode[2] = currentCode[3];
  currentCode[3] = currentCode[4];
  currentCode[4] = currentCode[5];
  currentCode[5] = toAdd;
}

bool isMatch() {
  int matches = 0;
  for (int i = 0; i < 6; i++) {
    if (currentCode[i] == activeCode[i]) {
      matches++;
    }
  }
  if (matches == 6) {
    return true;
  } else {
    return false;
  }
  matches = 0;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  Keyboard.begin();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonPinCurrent = digitalRead(buttonPin);
  if (buttonPinCurrent == LOW) {
    held++;
    delay(1);
  } else {
    released++;
    delay(1);
    if (released == 500) {
      addToArray(' ');
      Serial.println(currentCode);
    }
  }

  if (buttonPinCurrent == HIGH && buttonPinLast == LOW) {
    if (held > 100) {
      addToArray('-');
      Serial.println(currentCode);
    } else {
      addToArray('.');
      Serial.println(currentCode);
    }
    Serial.println(held);
    held = 0;
    released = 0;
  }
  
  if (isMatch()) {
    if (configMode == false) {
      for (int i = 0; i < 24; i++) {
        inputs[i].isActive = false;
      }
    }
    toggle(configMode);
    for (int i=0; i < 6; i++) {
      addToArray(' ');
    }
    Serial.println(configMode);
  }

  if (configMode == true) {
    Keyboard.releaseAll();
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
      analogWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
    }
    for (int i = 0; i < 24; i++) {
      inputs[i].getMatch();
    }
  } else {
    if (buttonPinCurrent == LOW) {
      if (held >= 200) {
        for (int i = 0; i < 24; i++) {
          inputs[i].doPress();
        }
      }
    } else {
      Keyboard.releaseAll();
    }
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
  }
  buttonPinLast = buttonPinCurrent;
}
