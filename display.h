#ifndef DISPLAY_H
#define DISPLAY_H

struct Status {
  const char *stateName;
  float ppm;
  int cycle;
  const char *note;
};

void oledBegin();
void oledRender(const Status &s);

#endif