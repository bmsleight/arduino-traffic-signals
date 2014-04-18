/*
  ats - Arduino Traffic Signals
  Turns an arduino in to a traffic signal controller.
 
 (c) Brendan M. Sleight 2014

#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include "Ats_phase.h"

Ats_phase::Ats_phase() {
}

Ats_phase::~Ats_phase() {
}

void Ats_phase::configure(unsigned char type, int red, int amber, int green, int demand, int detector_pin) {
  _type = type;
  _state = PHASE_RED; // Start on RED
  for (unsigned char ps = 0; ps < PHASE_STEPS; ps++) {
   // dont like 3 as the magic number - but it the where the min time is stored.
   // Use the default setting from phaseTypes configuration 
    _min_times[ps] = phaseTypes[type][ps][3]; 
  }
  _time_on_green_milliseconds = 0;
  _time_on_current_state_milliseconds = 0;
  _time_since_green_milliseconds = 0;
  _demand = DEMAND_NONE;
  
  /* Set up pins */
  _aspect_pins[0] = red;
  _aspect_pins[1] = amber;
  _aspect_pins[2] = green;
  _demand_green_pin = demand;
  _detector_pin = detector_pin;
  for (unsigned char pins = 0; pins < 3; pins++) {
    _set_pin_mode(_aspect_pins[pins], OUTPUT);
  }
  _set_pin_mode(_demand_green_pin, OUTPUT);   
  _set_pin_mode(_detector_pin, INPUT_PULLUP);   

  /* Debugging via serial */
  Serial.print("Configure ");
  Serial.print(_state);
  Serial.println(" ");

}


void Ats_phase::_set_pin_mode(int p, int mode) {
  // Set mode if not zero
  if (p !=0) {
    pinMode(p, mode);
  }
}

void Ats_phase::setMinTimes(int phase_step, int min) {
  _min_times[phase_step] = min; // Need to cheak not bellow minimum.
}

void Ats_phase::demand_set(unsigned char state)  {
  // Want to keep _demand private - but still need an overide to set the demand at start
  // too hard at the moment to set everythign demand and then fight...
  // TO DO!
  _demand = state;

}


unsigned char Ats_phase::demand_return()  {
  // Want to keep _demand private
  return _demand;
}

unsigned char Ats_phase::state(){
  // Dont want state to be public
  return _state;
}

bool Ats_phase::ran_min_green() {
  if((_time_on_green_milliseconds/1000)>=_min_times[PHASE_GREEN]) {
    return true;
  }
  else {
    return false;
  }
}

void Ats_phase::detect()  {
  // Can not be detected if aleady at green
  // or no demand pin to read from
  if ((_state != PHASE_GREEN) && (_demand_green_pin !=0) ) {
    if(digitalRead(_demand_green_pin) == LOW) {
      _demand = DEMAND_GREEN;
      /* Debugging via serial */
      Serial.print("Very demanding");
    }
  }
}


void Ats_phase::tick(int millseconds) {
  /* TICKS */
  if (_state == PHASE_GREEN) {
    _time_on_green_milliseconds = _time_on_green_milliseconds + millseconds;
  }
  else {
    _time_since_green_milliseconds = _time_since_green_milliseconds + millseconds;
    _time_on_green_milliseconds = 0;
  }
  _time_on_current_state_milliseconds = _time_on_current_state_milliseconds + millseconds;

  // Mins handles elewhere (for now)
  // Intergreens handled elewhere (for now)
  // Allowed moves handled ...
  /* TOCKS */
  if ((_state == PHASE_GREEN) && (_demand == DEMAND_RED)) {
    // On Green want Red
    if (ran_min_green()) {
      Serial.println("*.");
      _state = PHASE_POST_GREEN;
      _demand = DEMAND_NONE;
      _time_since_green_milliseconds = 0;
      _time_on_current_state_milliseconds = 0;
    }
  }
  if ((_state == PHASE_RED) && (_demand == DEMAND_GREEN)) {
    // On Red want Green
    if (true) { // No min red but reflects above code
      Serial.println("+.");
      _state = PHASE_POST_RED;
      _demand = DEMAND_NONE;
      _time_on_current_state_milliseconds = 0;
    }
  }
  // Not on green or red, march forward until the red or green
  if ((_state != PHASE_GREEN) && (_state != PHASE_RED)) {
    // Have we done the min time for the part of the Phase state ?
    if ((_time_on_current_state_milliseconds/1000) >= _min_times[_state]) {
      Serial.println("&.");
      // _state has PHASE_STEPS 0..5
      _state ++;
      if (_state >= PHASE_STEPS) {
        _state = 0;
      }
      _time_on_current_state_milliseconds = 0;
    }
  }
  
  _set_outputs();

}


void Ats_phase::_set_outputs() {
  unsigned char aspect_state;
  for (unsigned char pins = 0; pins < 3; pins++) {
    aspect_state = phaseTypes[_type][_state][pins];
    _set_pin_output(_aspect_pins[pins], aspect_state);
  }
  if (_demand == DEMAND_GREEN) {
    _set_pin_output(_demand_green_pin, A_ON___);
    Serial.print("n");
  }
  else {
    _set_pin_output(_demand_green_pin, A_OFF__);
//    Serial.print("m");
  }
}


void Ats_phase::_set_pin_output(int p, unsigned char aspect_state) {
  if (p !=0) {
    switch (aspect_state) {
      case A_ON___:
        digitalWrite(p, HIGH);
        break;
      case A_FLASH:
        digitalWrite(p, HIGH);
        break;
      case A_OFF__:
        digitalWrite(p, LOW);
        break;

//    Serial.println("Insert demand");
    }

  }
}
