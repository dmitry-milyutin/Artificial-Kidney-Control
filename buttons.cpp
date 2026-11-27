#include <Arduino.h>
#include "config.h"
#include "buttons.h"

static volatile bool estopLatched = false;
static unsigned long startSince = 0;

static void onEstop() {
  estopLatched = true;
}

void btnBegin() {
  pinMode(PIN_ESTOP, INPUT_PULLUP);
  pinMode(PIN_START, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ESTOP), onEstop, FALLING);
}

bool btnEstop() {
  return estopLatched || digitalRead(PIN_ESTOP) == LOW;
}

bool btnStartHeld(unsigned long ms) {
  if (digitalRead(PIN_START) == LOW) {
    if (startSince == 0) startSince = millis();
    return millis() - startSince >= ms;
  }

  startSince = 0;
  return false;
}