#include <DebounceInput.h>
#include <XInput.h>

// Pin declarations
const int BUTTON_PIN = 2;
DebouncedInput BUTTON(BUTTON_PIN);

// Assign DOT and DASH numbers so arrays are way simpler.
const int DOT = 1;
const int DASH = 2;

const bool STICK_X = true;
const bool STICK_Y = false;

unsigned long DASH_LENGTH = 125; // Value in milliseconds. DOT_LENGTH would be any value < 125.
unsigned long RUN_TIME = 150;    // Value in milliseconds. Time after release until press is read and run.
unsigned long HOLD_TIME = 125;

int MORSE[100];
int MORSE_POS = 0;
int MORSE_MAX = 0;
unsigned long long CODE_AS_INT = 0;

unsigned long TIME, PRESS_TIME, RELEASE_TIME;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  XInput.setTriggerRange(0, 100);
  XInput.setJoystickRange(-100, 100);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Assign variables
  TIME = millis();
  
  BUTTON.read();

  // Determine whether button press was dot or dash.
  if (BUTTON.falling()) {
    // Store the time when the button is pressed.
    PRESS_TIME = 0;
    PRESS_TIME = TIME;

    // Debug
    //Serial.println("BUTTON PRESSED");
  } else if (BUTTON.rising()) {
    // Store the time when the button is released.
    RELEASE_TIME = 0;
    RELEASE_TIME = TIME;
    // Store DOT or DASH in MORSE.
    MORSE[MORSE_POS] = DOT_OR_DASH();
    MORSE_POS++;

    // Debug
    //Serial.println("BUTTON RELEASED");
    DEBUG_MORSE();
  }

  // If time since release is greater than RUN_TIME, do the needful.
  if (TIME - RELEASE_TIME == RUN_TIME && BUTTON.high()) {
    // Store the current position of MORSE as MORSE_MAX.
    // Subtract one so we have the true max.
    MORSE_MAX = MORSE_POS;
    Serial.print("MORSE MAX: ");
    Serial.println(MORSE_MAX);

    // Dump the array into an integer
    for (int i = 0; i < MORSE_MAX; i++) {
      CODE_AS_INT = 10 * CODE_AS_INT + MORSE[i];
    }

    //Serial.println(CODE_AS_INT);
    RUN_MACRO(CODE_AS_INT);

    // Reset array.
    memset(MORSE, 0, sizeof(MORSE));
    MORSE_POS = 0;
    MORSE_MAX = 0;
    CODE_AS_INT = 0;
    
    RELEASE_TIME = 0;

    // Debug
    XInput.begin();
    Serial.println("RUN");
   
  }
}

int DOT_OR_DASH() {
  unsigned long t = RELEASE_TIME - PRESS_TIME;
  if (t >= DASH_LENGTH) {
    return DASH;
  } else {
    return DOT;
  }
}

void DEBUG_MORSE() {
  for (int i = 0; i < 100; i++) {
    Serial.print(MORSE[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void BUTTON_PRESS(uint8_t b, bool f) {
  if (f){
    XInput.setButton(b, !XInput.getButton(b));
  } else {
    XInput.setButton(b, true);
    delay(100);
    XInput.setButton(b, false);
  }
}

void TRIGGER_PRESS(XInputControl t, int32_t v, bool b) {
  int32_t c = XInput.getTrigger(t);
  if (b) {
    if (c != 0) {
      XInput.setTrigger(t, 0);
    } else {
      XInput.setTrigger(t, v);
    }
  } else {
    XInput.setTrigger(t, v);
    delay(100);
    XInput.setTrigger(t, 0);
  }
}

void DPAD_PRESS(XInputControl p, bool b) {
  if (b) {
    XInput.setDpad(p, !XInput.getDpad(p));
  } else {
    XInput.setDpad(p, true);
    delay(100);
    XInput.setDpad(p, false);
  }
}

void STICK_PRESS(XInputControl s, int32_t v, bool a, bool b, int t = 100) {
  if (a) {
    int32_t c = XInput.getJoystickX(s);
    if (b) {
      if (c != 0) {
        XInput.setJoystickX(s, 0);
      } else {
        XInput.setJoystickX(s, v);
      }
    } else {
      XInput.setJoystickX(s, v);
      delay(t);
      XInput.setJoystickX(s, 0);
    }
  } else {
    int32_t c = XInput.getJoystickY(s);
    if (b) {
      if (c != 0) {
        XInput.setJoystickY(s, 0);
      } else {
        XInput.setJoystickY(s, v);
      }
    } else {
      XInput.setJoystickY(s, v);
      delay(t);
      XInput.setJoystickY(s, 0);
    }
  }
}

void RUN_MACRO(unsigned long long c) {
  switch (c) {
    // Button presses
    case 1:         
      BUTTON_PRESS(BUTTON_A, false);
      break;
    case 2: 
      BUTTON_PRESS(BUTTON_B, false);
      break;
    case 11: 
      BUTTON_PRESS(BUTTON_X, false);
      break;
    case 22: 
      BUTTON_PRESS(BUTTON_Y, false);
      break;
    case 21: 
      BUTTON_PRESS(BUTTON_LB, false);
      break;
    case 12: 
      BUTTON_PRESS(BUTTON_RB, false);
      break;
    case 221:
      TRIGGER_PRESS(TRIGGER_LEFT, 100, false);
      break;
    case 122:
      TRIGGER_PRESS(TRIGGER_RIGHT, 100, false);
      break;
    case 212:
      BUTTON_PRESS(BUTTON_START, false);
      break;
    case 222:
      BUTTON_PRESS(BUTTON_BACK, false);
      break;
    case 2111:
      BUTTON_PRESS(BUTTON_L3, false);
      break;
    case 1112:
      BUTTON_PRESS(BUTTON_R3, false);
      break;
    case 1111:
      DPAD_PRESS(DPAD_UP, false);
      break;
    case 2222:
      DPAD_PRESS(DPAD_DOWN, false);
      break;
    case 2121:
      DPAD_PRESS(DPAD_LEFT, false);
      break;
    case 1212:
      DPAD_PRESS(DPAD_RIGHT, false);
      break;
    // Quick Spin
    case 1212121212:
      XInput.setButton(TRIGGER_LEFT, true);
      XInput.setJoystick(JOY_LEFT, true, false, false, false);
      delay(25);
      XInput.setJoystick(JOY_LEFT, true, false, true, false);
      delay(25);
      XInput.setJoystick(JOY_LEFT, false, false, true, false);
      delay(25);
      XInput.setJoystick(JOY_LEFT, false, true, true, false);
      delay(25);
      XInput.setJoystick(JOY_LEFT, false, true, false, false);
      delay(25);
      XInput.setJoystick(JOY_LEFT, false, true, false, true);
      delay(25);
      XInput.setJoystick(JOY_LEFT, false, false, false, true);
      delay(25);
      XInput.setJoystick(JOY_LEFT, true, false, false, true);
      XInput.setButton(BUTTON_B, true);
      delay(25);
      XInput.setJoystick(JOY_LEFT, true, false, false, false);
      delay(100);
      XInput.setJoystick(JOY_LEFT, false, false, false, false);
      XInput.setButton(BUTTON_B, false);
      XInput.setButton(TRIGGER_LEFT, false);
      break;
  }
}
