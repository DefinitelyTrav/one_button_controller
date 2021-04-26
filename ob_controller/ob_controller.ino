#include <XInput.h>
#include <runOnMorse.h>
#include <GameMacros.h>

runOnMorse runOnMorse(2);

// GameMacros GameMacros(OOT);    // The Legend of Zelda: Ocarina of Time
GameMacros GameMacros(FG);        // Fall Guys

void setup() {
  XInput.begin();

  XInput.setAutoSend(true);
  XInput.setTriggerRange(0, 100);
  XInput.setJoystickRange(-100, 100);
}

void loop() {
  GameMacros.runMacro(runOnMorse.getCode());
}
