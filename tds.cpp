#include <Arduino.h>
#include "config.h"
#include "tds.h"

// to confirm: calibration factor k, from V5 against a reference solution
static const float K = 1.0;

static int samples[SAMPLE_N];
static uint8_t count = 0;
static unsigned long lastSample = 0;
static bool sampling = false;
static float lastPpm = 0;

static float countsToPpm(int adc) {
  float v = adc / 1024.0 * 5.0;
  float ec = K * (133.42 * v * v * v - 255.86 * v * v + 857.39 * v);
  return ec * 0.5;
}

static int median(int *a, uint8_t n) {
  for (uint8_t i = 1; i < n; i++) {
    int key = a[i];
    int j = i - 1;
    while (j >= 0 && a[j] > key) {
      a[j + 1] = a[j];
      j--;
    }
    a[j + 1] = key;
  }

  return a[n / 2];
}

void tdsBegin() {
  pinMode(PIN_TDS, INPUT);
}

void tdsStartSample() {
  count = 0;
  sampling = true;
  lastSample = millis();
}

void tdsUpdate() {
  if (!sampling) return;
  if (millis() - lastSample < SAMPLE_DT_MS) return;

  lastSample = millis();
  samples[count] = analogRead(PIN_TDS);
  count++;

  if (count >= SAMPLE_N) {
    sampling = false;
    lastPpm = countsToPpm(median(samples, SAMPLE_N));
  }
}

bool tdsReady() {
#if SIM_TDS
  return Serial.available() > 0;
#else
  return !sampling && count >= SAMPLE_N;
#endif
}

float tdsPpm() {
#if SIM_TDS
  lastPpm = Serial.parseFloat();
  while (Serial.available()) Serial.read();
#endif
  return lastPpm;
}