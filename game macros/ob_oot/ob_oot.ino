#include <XInput.h>
#include <runOnMorse.h>

runOnMorse runOnMorse(1);

const bool STICK_X = true;
const bool STICK_Y = false;

unsigned long HOLD_TIME = 125;

int32_t X_LAST = 0;
int32_t Y_LAST = 0;

bool HOLD_NEXT = false;

void setup() {
  
  XInput.begin();

  XInput.setAutoSend(true);
  XInput.setTriggerRange(0, 100);
  XInput.setJoystickRange(-100, 100);
}

void loop() {
  RUN_MACRO(runOnMorse.getCode());
}

void BUTTON_PRESS(uint8_t b, bool f) {
  if (f){
    XInput.setButton(b, !XInput.getButton(b));
  } else {
    XInput.setButton(b, true);
    delay(100);
    XInput.setButton(b, false);
  }
  HOLD_NEXT = false;
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
  HOLD_NEXT = false;
}

void DPAD_PRESS(XInputControl p, bool b) {
  if (b) {
    XInput.setDpad(p, !XInput.getDpad(p));
  } else {
    XInput.setDpad(p, true);
    delay(100);
    XInput.setDpad(p, false);
  }
  HOLD_NEXT = false;
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
    //HOLD_NEXT
    case 22222:
      if (!HOLD_NEXT) {
        HOLD_NEXT = true;
      }
      break;
    
    // Button presses
    case 2:         
      BUTTON_PRESS(BUTTON_A, HOLD_NEXT);
      break;
    case 1: 
      BUTTON_PRESS(BUTTON_B, HOLD_NEXT);
      break;
    case 211: 
      BUTTON_PRESS(BUTTON_X, HOLD_NEXT);
      break;
    case 112: 
      BUTTON_PRESS(BUTTON_Y, HOLD_NEXT);
      break;
    case 2221: 
      BUTTON_PRESS(BUTTON_LB, HOLD_NEXT);
      break;
    case 212: 
      BUTTON_PRESS(BUTTON_RB, HOLD_NEXT);
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
      DPAD_PRESS(DPAD_UP, HOLD_NEXT);
      break;
    case 1221:
      DPAD_PRESS(DPAD_DOWN, HOLD_NEXT);
      break;
    case 1211:
      DPAD_PRESS(DPAD_LEFT, HOLD_NEXT);
      break;
    case 1121:
      DPAD_PRESS(DPAD_RIGHT, HOLD_NEXT);
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
