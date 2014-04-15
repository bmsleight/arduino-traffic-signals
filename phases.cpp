#include "phases.h"


Phase::Phase(byte type) {
  _type = type;
  _aspect_pins[] = {0,0,0};
  _demand_green_pin = 0;
  _min_times[PHASE_STEPS] = {0,0,0,0,0,0};
}

Phase::~Phase() {
}

//    setPins(int red, int amber, int green, int demand);
//    setMinTimes(int phase_step, int min);

void Phase::setPins(int red, int amber, int green, int demand) {
  _aspect_pins[] = {red,amber,green};
  _demand_green_pin = demand;
}

void Phase::setMinTimes(int phase_step, int min) {
  _min_times[phase_step] = min;
}


void Phase::tick(int millseconds) {
    Serial.print( "tick tock" );
}
