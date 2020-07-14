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
int32_t X_LAST = 0;
int32_t Y_LAST = 0;

unsigned long TIME, PRESS_TIME, RELEASE_TIME;

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  XInput.begin();
  Serial.begin(9600);

  XInput.setAutoSend(true);
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

void STICK_PRESS(XInputControl s, int32_t x, int32_t y, bool b, int t = 100) {
  if (b) {
    if (X_LAST == x && Y_LAST == y) {
      XInput.setJoystick(s, false, false, false, false);
      X_LAST = 0;
      Y_LAST = 0;
    } else {
      XInput.setJoystickX(s, x);
      XInput.setJoystickY(s, y);
      X_LAST = x;
      Y_LAST = y;
    }
  } else {
    XInput.setJoystickX(s, x);
    XInput.setJoystickY(s, y);
    delay(t);
    XInput.setJoystick(s, false, false, false, false);
    X_LAST = 0;
    Y_LAST = 0;
  }
}

void RUN_MACRO(unsigned long long c) {
  int SOFT_PRESS;

  if (XInput.getTrigger(TRIGGER_LEFT)){
    SOFT_PRESS = 30;
  } else {
    SOFT_PRESS = 50;
  }
  
  switch (c) {
    // Button presses
    case 2:         
      BUTTON_PRESS(BUTTON_A, false);
      break;
    case 1: 
      BUTTON_PRESS(BUTTON_B, false);
      break;
    case 211: 
      BUTTON_PRESS(BUTTON_X, false);
      break;
    case 112: 
      BUTTON_PRESS(BUTTON_Y, false);
      break;
    case 2221: 
      BUTTON_PRESS(BUTTON_LB, false);
      break;
    case 212: 
      BUTTON_PRESS(BUTTON_RB, false);
      break;
    case 2111:
      TRIGGER_PRESS(TRIGGER_LEFT, 100, false);
      break;
    case 2121:
      TRIGGER_PRESS(TRIGGER_LEFT, 100, true);
      break;
    case 1112:
      TRIGGER_PRESS(TRIGGER_RIGHT, 100, true);
      break;
    case 11111:
      XInput.releaseAll();
      BUTTON_PRESS(BUTTON_START, false);
      break;
    case 2112:
      DPAD_PRESS(DPAD_UP, false);
      break;
    case 1221:
      DPAD_PRESS(DPAD_DOWN, false);
      break;
    case 1211:
      DPAD_PRESS(DPAD_LEFT, false);
      break;
    case 1121:
      DPAD_PRESS(DPAD_RIGHT, false);
      break;
      
    // Turns
    case 11:
      STICK_PRESS(JOY_LEFT, 0, 50, false, 50);
      break;
    case 22:
      STICK_PRESS(JOY_LEFT, 0, -50, false, 50);
      break;
    case 21:
      STICK_PRESS(JOY_LEFT, -50, 0, false, 50);
      break;
    case 12:
      STICK_PRESS(JOY_LEFT, 50, 0, false, 50);
      break;
    
    // Movement
    case 111:
      STICK_PRESS(JOY_LEFT, 0, 100, true);
      break;
    case 222:
      STICK_PRESS(JOY_LEFT, 0, -100, true);
      break;
    case 221:
      STICK_PRESS(JOY_LEFT, -100, 0, true);
      break;
    case 122:
      STICK_PRESS(JOY_LEFT, 100, 0, true);
      break;
    case 11121:
      STICK_PRESS(JOY_LEFT, -100, 100, true);
      break;
    case 11112:
      STICK_PRESS(JOY_LEFT, 100, 100, true);
      break;
    case 22221:
      STICK_PRESS(JOY_LEFT, -100, -100, true);
      break;
    case 22212:
      STICK_PRESS(JOY_LEFT, 100, -100, true);
      break;
    
    // Slow movement
    case 1111:
      STICK_PRESS(JOY_LEFT, 0, SOFT_PRESS, true);
      break;
    case 2222:
      STICK_PRESS(JOY_LEFT, 0, -SOFT_PRESS, true);
      break;
    case 2211:
      STICK_PRESS(JOY_LEFT, -SOFT_PRESS, 0, true);
      break;
    case 1122:
      STICK_PRESS(JOY_LEFT, SOFT_PRESS, 0, true);
      break;
    case 111121:
      STICK_PRESS(JOY_LEFT, -SOFT_PRESS, SOFT_PRESS, true);
      break;
    case 111112:
      STICK_PRESS(JOY_LEFT, SOFT_PRESS, SOFT_PRESS, true);
      break;
    case 222221:
      STICK_PRESS(JOY_LEFT, -SOFT_PRESS, -SOFT_PRESS, true);
      break;
    case 222212:
      STICK_PRESS(JOY_LEFT, SOFT_PRESS, -SOFT_PRESS, true);
      break;
        
    // Quick Spin
    case 11122:
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

    // Jump slash
    case 1212:
      XInput.setButton(TRIGGER_LEFT, true);
      delay(50);
      XInput.setButton(BUTTON_A, true);
      delay(100);
      XInput.setButton(TRIGGER_LEFT, false);
      XInput.setButton(BUTTON_A, false);
      break;

    // Crouch Stab
    case 11211:
      XInput.setButton(TRIGGER_RIGHT, true);
      delay(50);
      XInput.setButton(BUTTON_B, true);
      delay(100);
      XInput.setButton(TRIGGER_RIGHT, false);
      XInput.setButton(BUTTON_B, false);
      break;

    // Backflip
    case 12121:
        XInput.setButton(TRIGGER_LEFT, true);
        delay(50);
        XInput.setJoystickY(JOY_LEFT, -100);
        XInput.setButton(BUTTON_A, true);
        delay(100);
        XInput.setButton(TRIGGER_LEFT, false);
        XInput.setJoystick(JOY_LEFT, false, false, false, false);
        XInput.setButton(BUTTON_A, false);
        break;
        
    // Sidehop right
    case 12112:
        XInput.setButton(TRIGGER_LEFT, true);
        delay(50);
        XInput.setJoystickX(JOY_LEFT, 100);
        XInput.setButton(BUTTON_A, true);
        delay(100);
        XInput.setButton(TRIGGER_LEFT, false);
        XInput.setJoystick(JOY_LEFT, false, false, false, false);
        XInput.setButton(BUTTON_A, false);
        break;
  }
}
