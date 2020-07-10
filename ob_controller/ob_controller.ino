#include <DebounceInput.h>
#include <XInput.h>

// Pin declarations
const int BUTTON_PIN = 2;
DebouncedInput BUTTON(BUTTON_PIN);

// Assign DOT and DASH numbers so arrays are way simpler.
const int DOT = 1;
const int DASH = 2;

unsigned long DASH_LENGTH = 125; // Value in milliseconds. DOT_LENGTH would be any value < 125.
unsigned long RUN_TIME = 150;    // Value in milliseconds. Time after release until press is read and run.
unsigned long HOLD_TIME = 125;

int MORSE[100];
int MORSE_POS = 0;
int MORSE_MAX = 0;
int CODE_AS_INT = 0;

unsigned long TIME, PRESS_TIME, RELEASE_TIME;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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

    Serial.println(CODE_AS_INT);
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

void RUN_MACRO(int c) {
  switch (c) {
    case 1:
      XInput.setButton(BUTTON_A, true);
      delay(100);
      XInput.setButton(BUTTON_A, false);
      break;
    case 2:
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
      XInput.setButton(BUTTON_B, true);
      delay(100);
      XInput.setJoystick(JOY_LEFT, false, false, false, false);
      XInput.setButton(BUTTON_B, false);
      XInput.setButton(TRIGGER_LEFT, false);
      break;
  }
}
