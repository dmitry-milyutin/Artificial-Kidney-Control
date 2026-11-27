#include "config.h"
#include "buttons.h"
#include "tds.h"
#include "display.h"
#include "actuators.h"

enum State { READY, FILL, DOSE, MIX, SETTLE, READ, FILTER,
             RETURN_FLUID, DONE, FAULT, STOPPED };

static const unsigned long MARGIN_MS = 5000;
static const unsigned long READ_TIMEOUT_MS = 60000;

static State state = READY;
static unsigned long tEnter = 0;
static unsigned long tOled = 0;
static int cycle = 0;
static float ppm = 0;
static const char *note = "";

static const char *stateName();

// set state, stamp clock, trace
static void enter(State s) {
  state = s;
  tEnter = millis();
  Serial.print(F("-> "));
  Serial.print(stateName());
  Serial.print(F("  cycle "));
  Serial.println(cycle);
}

// time in current state
static bool elapsed(unsigned long ms) {
  return millis() - tEnter >= ms;
}

// kill all, park
static void fault(const char *reason) {
  allOff();
  note = reason;
  enter(FAULT);
}

static const char *stateName() {
  switch (state) {
    case READY:        return "READY";
    case FILL:         return "FILLING";
    case DOSE:         return "DOSING";
    case MIX:          return "MIXING";
    case SETTLE:       return "SETTLE";
    case READ:         return "READING";
    case FILTER:       return "FILTER";
    case RETURN_FLUID: return "RETURN";
    case DONE:         return "DONE";
    case FAULT:        return "FAULT";
    case STOPPED:      return "E-STOP";
  }
  return "?";
}

void setup() {
  Serial.begin(9600);
  btnBegin();
  tdsBegin();
  actBegin();
  oledBegin();
  Serial.println(F("BOOT"));
}

void loop() {
  if (btnEstop() && state != STOPPED) {
    allOff();
    note = "Hit RESET";
    enter(STOPPED);
  }

  tdsUpdate();

  switch (state) {

    // hold start to begin
    case READY:
      if (btnStartHeld(START_HOLD_MS)) {
        cycle = 0;
        note = "";
        startFill();
        enter(FILL);
      }
      break;

    // submersible, initial -> mixing
    case FILL:
      if (fillDone()) {
        startDose();
        enter(DOSE);
      }
      else if (elapsed(T_FILL_MS + MARGIN_MS)) fault("FILL TIMEOUT");


      break;

    // amylase, once per run
    case DOSE:
      if (doseDone()) {
        startMix();
        enter(MIX);
      }
      else if (elapsed((T_DOSE_MS + T_DOSE_GAP_MS) * DOSE_COUNT + MARGIN_MS)) fault("DOSE TIMEOUT");
      break;

    // stirrer
    case MIX:
      if (mixDone()) enter(SETTLE);
      else if (elapsed(T_MIX_MS + MARGIN_MS)) fault("MIX TIMEOUT");
      break;

    // quiet, bubbles clear
    case SETTLE:
      if (elapsed(SETTLE_MS)) {
        tdsStartSample();
        enter(READ);
      }
      break;

    // sample, then decide
    case READ:
      if (tdsReady()) {
        ppm = tdsPpm();

        if (ppm > PPM_MAX_VALID) fault("PROBE HIGH");
        else if (ppm < PPM_MIN_VALID) fault("PROBE LOW");
        else if (cycle == 0) {
          cycle = 1;
          startFilter();
          enter(FILTER);
        }
        else if (ppm <= PPM_TARGET && cycle >= MIN_CYCLES) {
          note = "TARGET MET";
          startReturn();
          enter(RETURN_FLUID);
        }
        else if (cycle >= MAX_CYCLES) {
          note = "MAX CYCLES";
          startReturn();
          enter(RETURN_FLUID);
        }
        else {
          cycle++;
          startFilter();
          enter(FILTER);
        }
      }
      else if (elapsed(READ_TIMEOUT_MS)) fault("READ TIMEOUT");
      break;

    // diaphragm, one pass through filter
    case FILTER:
      if (filterDone()) enter(SETTLE);
      else if (elapsed(T_FILTER_MS + MARGIN_MS)) fault("FILTER TIMEOUT");
      break;

    // peristaltic, mixing -> initial
    case RETURN_FLUID:
      if (returnDone()) enter(DONE);
      else if (elapsed(T_RETURN_MS + MARGIN_MS)) fault("RETURN TIMEOUT");
      break;

    case DONE:
    case FAULT:
    case STOPPED:
      allOff();
      break;
  }

  if (millis() - tOled >= OLED_PERIOD_MS) {
    tOled = millis();
    Status s = { stateName(), ppm, cycle, note };
    oledRender(s);
  }
}
