#ifndef ACTUATORS_H
#define ACTUATORS_H


// initlaize everything here for actuators
void actBegin();
void allOff();

void startFill();
bool fillDone();

void startDose();
bool doseDone();

void startMix();
bool mixDone();

void startFilter();
bool filterDone();

void startReturn();
bool returnDone();

#endif