#include "Ats_phase.h"

Ats_phase::Ats_phase() {
}

Ats_phase::~Ats_phase() {
}

void Ats_phase::configure(unsigned char type, int red, int amber, int green, int demand, int detector_pin) {
  _type = type;
  _state = PHASE_RED; // Start on RED
  for (unsigned char ps = 0; ps < PHASE_STEPS; ps++) {
   // dont like 4 as the magic number - but it the where the min time is store.
   // Use the default setting from phaseTypes configuration 
    _min_times[ps] = phaseTypes[type][ps][3]; 
  }
  _aspect_pins[0] = red;
  _aspect_pins[1] = amber;
  _aspect_pins[2] = green;
  _demand_green_pin = demand;
  _detector_pin = detector_pin;

  _time_on_green_milliseconds = 0;
  _time_since_green_milliseconds = 0;
  _demand = DEMAND_NONE;

  /* Set up pins */
  for (unsigned char pins = 0; pins < 3; pins++) {
    if (_aspect_pins[pins] !=0) {
      pinMode(_aspect_pins[pins], OUTPUT); 
    }
  }
  if (_demand_green_pin !=0) {
    pinMode(_demand_green_pin, OUTPUT);   
  }
  if (_detector_pin !=0) {
    pinMode(_detector_pin, INPUT);   
  }

}

void Ats_phase::setMinTimes(int phase_step, int min) {
  _min_times[phase_step] = min; // Need to cheak not bellow minimum.
}

void Ats_phase::demand()  {
  // Want to keep _demand private - but still need an overide to set the demand at start
  // too hard at the moment to set everythign demand and then fight...
  // TO DO!
  _demand = DEMAND_GREEN;
}


void Ats_phase::tick(int millseconds) {
    
}



void Ats_phase::detect()  {
  unsigned char detection;
  // Can not be detected if aleady at green
  // or aleady Demanded
  // or no demand pin to read from
  if ((_state != PHASE_GREEN) && (_demand != DEMAND_GREEN) && (_demand_green_pin !=0) ) {
    if(digitalRead(_demand_green_pin)) {
      _demand = DEMAND_GREEN;
    }
  }
}