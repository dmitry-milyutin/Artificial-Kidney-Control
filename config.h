#ifndef CONFIG_H
#define CONFIG_H

// ===== SAME EXACT PINS FROM HARDWARE 1 SETUP FILE =====

// e-stop, to GND, pressed = LOW
const uint8_t PIN_ESTOP = 18;
// start, to GND, pressed = LOW
const uint8_t PIN_START = 19;
// TDS board output
const uint8_t PIN_TDS = A2;
// servo signal, SERVO_2 header
const uint8_t PIN_DISPENSER = 9;
// level sensor, unused
const uint8_t PIN_LEVEL = 30;

// stirrer
const uint8_t PORT_MIXER = 3; //M3
// submersible
const uint8_t PORT_FILL = 2;
// diaphragm
const uint8_t PORT_FILTER = 1; //1
// peristaltic
const uint8_t PORT_RETURN = 4;

// ===== TEST SWITCHES =====

// cuts everything to 3 seconds just to see if they work in order,
// but don't recommend for testing because it will just activate it
// for a short period of time
#define SIM_TIMING 0

// manually fill out TDS each stage
#define SIM_TDS 0

// fake phase length, SIM_TIMING only
const unsigned long SIM_PHASE_MS = 3000;

// ===== FILL =====

// submersible, initial -> mixing
// for water it was approximately 15 seconds,
// for cornstarch will be around 40% more
const unsigned long T_FILL_MS = 21000;

//      ===== WHAT TO MEASURE =====

// stirrer run. probably around 20 seconds, MEASURE
const unsigned long T_MIX_MS = 20000;

// motors off before reading, bubbles clear
// probably around 20 seconds, MEASURE
const unsigned long SETTLE_MS = 20000;

// diaphragm, one volume through syringe
// need to MEASURE with filter in and PLUG THE TOP UNUSED HOLE
const unsigned long T_FILTER_MS = 20000;

// peristaltic, mixing -> initial
// around 190 seconds, so that's the longest one
const unsigned long T_RETURN_MS = 190000;

// stop cycling here when the tds test isnt 1. UNCONFIRMED
// if it never reaches this value within the max cycles it just returns anyways
const float PPM_TARGET = 200.0;
// fewest filter passes
const uint8_t MIN_CYCLES = 2;
// most filter passes
const uint8_t MAX_CYCLES = 6;



//             ===== SERVO =====


// pocket sits under the tub here and refills, park it here before start and try to be close
// realistically its only twice there isnt much error that can happen in those two
const int DISPENSE_HOME_DEG = 14;
// pocket inverted, load drops into the chamber
// write() clamps above 180, so 194 and 180 are the same position
const int DISPENSE_OUT_DEG = 180;

// hold at OUT, powder falls out. MEASURE
const unsigned long T_DOSE_MS = 1000;
// hold at HOME, pocket refills from tub. MEASURE
const unsigned long T_DOSE_GAP_MS = 1000;
// pocket loads per run, one load is not enough amylase
const uint8_t DOSE_COUNT = 2;

//          ===== TDS VALUES =====

// probe shorted above this
const float PPM_MAX_VALID = 1000.0;
// probe dry below this
const float PPM_MIN_VALID = 20.0;

// samples per reading, median used
const uint8_t SAMPLE_N = 30;
// gap between samples
const unsigned long SAMPLE_DT_MS = 40;

//           BUTTON and OLED STUFF

// start button hold, debounce
const unsigned long START_HOLD_MS = 500;
// OLED redraw, I2C is slow
const unsigned long OLED_PERIOD_MS = 500;

#endif
