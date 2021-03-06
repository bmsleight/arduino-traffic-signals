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
  _time_on_green_milliseconds = 0;
  _time_on_current_state_milliseconds = 0;
  _time_since_green_milliseconds = 0;
  _phase_change = PHASE_CHANGE_NONE;
  _state = PHASE_RED; // Start on RED
  _illuminate = false;
  _demand = false;
  debug_to_serial = false;
}

Ats_phase::~Ats_phase() {
}

void Ats_phase::configure(unsigned char type, int red, int amber, int green, int demand_pin, int detector_pin, bool demand) {
  _type = type;
  // Use the default setting from phaseTypes configuration 
  for (unsigned char ps = 0; ps < PHASE_STEPS; ps++) {
   // dont like 3 as the magic number - but it the where the min time is stored.
    _min_times[ps] = phaseTypes[type][ps][3]; 
  }
  
  /* Set up pins */
  _aspect_pins[0] = red;
  _aspect_pins[1] = amber;
  _aspect_pins[2] = green;
  _demand_green_pin = demand_pin;
  _detector_pin = detector_pin;
  _demand = demand;
  for (unsigned char pins = 0; pins < 3; pins++) {
    _set_pin_mode(_aspect_pins[pins], OUTPUT);
  }
  _set_pin_mode(_demand_green_pin, OUTPUT);   
  _set_pin_mode(_detector_pin, INPUT);   

  if (debug_to_serial) {
    /* Debugging via serial */
    Serial.print("Configure ");
    Serial.print(_state);
    Serial.println(" ");
  }

}


void Ats_phase::_set_pin_mode(int p, int mode) {
  // Set mode if not zero
  if (p !=0) {
    pinMode(p, mode);
    if (mode == INPUT) {
      // Pull up resistors (internal to chip - WARNING: ATMEL only )
      digitalWrite(_detector_pin, HIGH);
    }
  }
}

void Ats_phase::setMinTimes(int phase_step, int min) {
  _min_times[phase_step] = min; // TO DO: Need to cheak not bellow minimum.
}

void Ats_phase::phase_change_set(unsigned char state)  {
  // Want to keep _phase_change private - but still need an overide to set the demand at start
  // too hard at the moment to set everythign demand and then fight...
  // TO DO!
  _phase_change = state;
}


unsigned char Ats_phase::phase_change_return()  {
  // Want to keep _phase_change private
  return _phase_change;
}

unsigned char Ats_phase::state(){
  // Dont want state to be public
  return _state;
}

void Ats_phase::state_set(unsigned char state){
  // Dont want state to be public
  _state = state;
}

void Ats_phase::state_reset_time(){
  // Dont want state to be public
  _time_on_green_milliseconds = 0;
  _time_on_current_state_milliseconds = 0;
  _time_since_green_milliseconds = 0;
}


void Ats_phase::illuminate(bool i) {
  _illuminate = i;
}


bool Ats_phase::ran_min_green() {
  if((_time_on_green_milliseconds/1000)>=_min_times[PHASE_GREEN]) {
    return true;
  }
  else {
    return false;
  }
}

bool Ats_phase::detect()  {
  // Can not be detected if aleady at green
  // or of there is a demand pin to read from
  if ((_state != PHASE_GREEN) && (_detector_pin !=0) ) {
    if(digitalRead(_detector_pin) == LOW) {
      _demand = true;
    }
  }
  return _demand;
}


bool Ats_phase::demanded()  {
  return _demand;
}


void Ats_phase::tick(int millseconds) {
  /* TICKS */
  if (_state == PHASE_GREEN) {
    _time_on_green_milliseconds = _time_on_green_milliseconds + millseconds;
    _demand = false;
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
  if ((_state == PHASE_GREEN) && (_phase_change == PHASE_CHANGE_TO_RED)) {
    // On Green want Red
    if (ran_min_green()) {
      _state = PHASE_POST_GREEN;
      _phase_change = PHASE_CHANGE_NONE;
      _time_since_green_milliseconds = 0;
      _time_on_current_state_milliseconds = 0;
      if (debug_to_serial) {
        Serial.println("Leaving Green");
      }
    }
  }
  if ((_state == PHASE_RED) && (_phase_change == PHASE_CHANGE_TO_GREEN)) {
    // On Red want Green
    if (true) { // No min red but reflects above code
      _state = PHASE_POST_RED;
      _phase_change = PHASE_CHANGE_NONE;
      _time_on_current_state_milliseconds = 0;
      if (debug_to_serial) {
        Serial.println("Leaving Red");
      }
    }
  }
  // Not on green or red, march forward until the red or green
  if ((_state != PHASE_GREEN) && (_state != PHASE_RED)) {
    // Have we done the min time for the part of the Phase state ?
    if ((_time_on_current_state_milliseconds/1000) >= _min_times[_state]) {
      // TO DO: Have we done the intergreen
      // _state has PHASE_STEPS 0..5
      _state = (_state + 1) % PHASE_STEPS;
      _time_on_current_state_milliseconds = 0;
      if (debug_to_serial) {
        Serial.println("Moving to Green/Red");
      }
    }
  }
 
  // Now we have everthing stored - write to the pins
  _set_outputs();
}


void Ats_phase::_set_outputs() {
  unsigned char aspect_state;
  for (unsigned char pins = 0; pins < 3; pins++) {
    aspect_state = phaseTypes[_type][_state][pins];
    _set_pin_output(_aspect_pins[pins], aspect_state);
  }
  if (_demand == true) {
    _set_pin_output(_demand_green_pin, A_ON___ );
  }
  else {
    _set_pin_output(_demand_green_pin, A_OFF__ );
  }
}


void Ats_phase::_set_pin_output(int p, unsigned char aspect_state) {
  if (p != 0 && _illuminate) {
    switch (aspect_state) {
      case A_ON___:
        digitalWrite(p, A_ON___ ^ A_POLARITY);
        break;
      case A_FLASH:
        // Always starts flashing cycle with an ON.
        if ((_time_on_current_state_milliseconds % (FLASH_AFTER_HEARTBEATS*HEARTBEAT_MILLS*2))<(FLASH_AFTER_HEARTBEATS*HEARTBEAT_MILLS)) {
          digitalWrite(p, A_ON___ ^ A_POLARITY );
        }
        else {
          digitalWrite(p, A_OFF__ ^ A_POLARITY);
        }
        break;
      case A_OFF__:
        digitalWrite(p, A_OFF__ ^ A_POLARITY);
        break;
    }
  }
  if (p != 0 && !_illuminate) {
    digitalWrite(p, A_POLARITY);
  }
}

void Ats_phase::serial_debug(unsigned char p) {
  Serial.print("## Phase: ");
  Serial.print(p);
  Serial.print(" ");

  switch (_state) {
    case PHASE_GREEN:
      Serial.print("Green  ");
      break;
    case PHASE_POST_GREEN:
      Serial.print(" Post G");
      break;
    case PHASE_PRE_RED :
      Serial.print(" Pre R ");
      break;
    case PHASE_RED:
      Serial.print("Red    ");
      break;
    case PHASE_POST_RED:
      Serial.print(" Post R");
      break;
    case PHASE_PRE_GREEN:
      Serial.print(" Pre G ");
      break;
  }
  Serial.print(", demand ");
  Serial.print(_demand);
  Serial.print(" ## ");
}
