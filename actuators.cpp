#include <Arduino.h>
#include <AFMotor.h>
#include <Servo.h>
#include "config.h"
#include "actuators.h"

static AF_DCMotor mixer(PORT_MIXER);
static AF_DCMotor fillPump(PORT_FILL);
static AF_DCMotor filterPump(PORT_FILTER);
static AF_DCMotor returnPump(PORT_RETURN);

static Servo dispenser;

static unsigned long t0 = 0;
static uint8_t dosesLeft = 0;
static bool atOut = false;

// real time, or short fake
static unsigned long phaseLen(unsigned long real) {
#if SIM_TIMING
  (void)real;
  return SIM_PHASE_MS;
#else
  return real;
#endif
}

// attach if needed, go to angle
static void servoTo(int deg) {
  if (!dispenser.attached()) dispenser.attach(PIN_DISPENSER);
  dispenser.write(deg);
}

// pulses off, no holding torque
static void servoStop() {
  dispenser.detach();
}

// shield latches motors, clear them
void actBegin() {
  allOff();
}

// everything dead
void allOff() {
  mixer.run(RELEASE);
  fillPump.run(RELEASE);
  filterPump.run(RELEASE);
  returnPump.run(RELEASE);
  servoStop();
}

void startFill() {
  t0 = millis();
  fillPump.setSpeed(255);
  fillPump.run(FORWARD);
}

bool fillDone() {
  if (millis() - t0 < phaseLen(T_FILL_MS)) return false;

  fillPump.run(RELEASE);
  return true;
}

// swing out, pocket dumps
void startDose() {
  dosesLeft = DOSE_COUNT;
  atOut = true;
  t0 = millis();
  servoTo(DISPENSE_OUT_DEG);
}

// dump, refill, dump. true after last
bool doseDone() {
  if (atOut) {
    if (millis() - t0 < phaseLen(T_DOSE_MS)) return false;

    servoTo(DISPENSE_HOME_DEG);
    atOut = false;
    dosesLeft--;
    t0 = millis();
  }

  if (millis() - t0 < phaseLen(T_DOSE_GAP_MS)) return false;

  if (dosesLeft == 0) {
    servoStop();
    return true;
  }

  atOut = true;
  t0 = millis();
  servoTo(DISPENSE_OUT_DEG);
  return false;
}

void startMix() {
  t0 = millis();
  mixer.setSpeed(255);
  mixer.run(FORWARD);
}

bool mixDone() {
  if (millis() - t0 < phaseLen(T_MIX_MS)) return false;

  mixer.run(RELEASE);
  return true;
}

void startFilter() {
  t0 = millis();
  filterPump.setSpeed(255);
  filterPump.run(FORWARD);
}

bool filterDone() {
  if (millis() - t0 < phaseLen(T_FILTER_MS)) return false;

  filterPump.run(RELEASE);
  return true;
}

void startReturn() {
  t0 = millis();
  returnPump.setSpeed(255);
  returnPump.run(FORWARD);
}

bool returnDone() {
  if (millis() - t0 < phaseLen(T_RETURN_MS)) return false;

  returnPump.run(RELEASE);
  return true;
}