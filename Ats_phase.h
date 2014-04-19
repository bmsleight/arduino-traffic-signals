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

#ifndef ATS_PHASE_H
#define ATS_PHASE_H

/* Need to inlcude Arduino libraries to read pins */
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/* Timing for interrupts */
// Can run as low as 10ms, but that may not give time for 
//  debugging via the serial port at 9,600 Baud.
#define HEARTBEAT_MILLS 100
// compare match register HEARTBEAT = (16MHz/256)*(HEARTBEAT_MILLS/1000) 
#define HEARTBEAT 6250
#define FLASH_AFTER_HEARTBEATS 4  
// 60 /2 (on and off) = 30
// 30 / ( FLASH_AFTER_HEARTBEATS * HEARTBEAT_MILLS/1000)
// Makes 75 cycles per minute

/* Define some Globals use for a phase */

/* Phase Types */
/* Each phase type will go in the order .... */

#define PHASE_GREEN 0
#define PHASE_POST_GREEN 1
#define PHASE_BLACKOUT 1 // Same as POST_GREEN - but easier for reading
#define PHASE_PRE_RED 2
#define PHASE_RED 3
#define PHASE_POST_RED 4
#define PHASE_PRE_GREEN 5
#define PHASE_STEPS 6
//#define OUTPUT_PER_PHASE 3
#define OUTPUT_PER_PHASE 4 // As going to store min time at end

/* Phase_Change */
#define PHASE_CHANGE_NONE 0
#define PHASE_CHANGE_TO_GREEN 1
#define PHASE_CHANGE_TO_RED 2


// 0 = Off 1 = On 2 = Flashing

#define A_OFF__ 0
#define A_ON___ 1
#define A_FLASH 2

/* Phase types */
#define PHASE_TYPES 4
/* which of type ... */
#define TRAFFIC_PELICAN 0
#define PED_PELICAN 1
#define TRAFFIC_JUNCTION 2
#define PED_JUNCTION 3

static unsigned char phaseTypes[PHASE_TYPES][PHASE_STEPS][OUTPUT_PER_PHASE]=
{
  // Type of signal
  // State, State, State, Min time
  // e.g. When at green for Traffic Phase at Pelican
  // Red Aspect off, Amber off, Green On, Minimum 7 Seconds
 
  { // TRAFFIC_PELICAN 
    // Red      AMBER   GREEN
    {A_OFF__, A_OFF__, A_ON___, 7}, // PHASE_GREEN
    {A_OFF__, A_OFF__, A_ON___, 0}, // PHASE_POST_GREEN
    {A_OFF__, A_ON___, A_OFF__, 3}, // PHASE_PRE_RED
    {A_ON___, A_OFF__, A_OFF__, 0}, // PHASE_RED
    {A_OFF__, A_FLASH, A_OFF__, 0}, // PHASE_POST_RED
    {A_OFF__, A_FLASH, A_OFF__, 0}  // PHASE_PRE_GREEN
  },
  { // PED_PELICAN 
    //Red Fig    -      Green Fig
    {A_OFF__, A_OFF__, A_ON___, 5}, // PHASE_GREEN
    {A_OFF__, A_OFF__, A_FLASH, 0}, // PHASE_POST_GREEN
    {A_ON___, A_OFF__, A_OFF__, 0}, // PHASE_PRE_RED
    {A_ON___, A_OFF__, A_OFF__, 0}, // PHASE_RED
    {A_ON___, A_OFF__, A_OFF__, 0}, // PHASE_POST_RED
    {A_ON___, A_OFF__, A_OFF__, 0}  // PHASE_PRE_GREEN
  },
  { // TRAFFIC_JUNCTION 
    // Red      AMBER   GREEN
    {A_OFF__, A_OFF__, A_ON___, 7}, // PHASE_GREEN
    {A_OFF__, A_OFF__, A_ON___, 0}, // PHASE_POST_GREEN
    {A_OFF__, A_ON___, A_OFF__, 3}, // PHASE_PRE_RED
    {A_ON___, A_OFF__, A_OFF__, 0}, // PHASE_RED
    {A_ON___, A_OFF__, A_OFF__, 0}, // PHASE_POST_RED
    {A_ON___, A_ON___, A_OFF__, 2}  // PHASE_PRE_GREEN
  },
  { // PED_JUNCTION 
    //Red Fig    -      Green Fig
    {A_OFF__, A_OFF__, A_ON___, 5}, // PHASE_GREEN
    {A_OFF__, A_OFF__, A_OFF__, 5}, // PHASE_POST_GREEN
    {A_OFF__, A_OFF__, A_OFF__, 0}, // PHASE_PRE_RED
    {A_ON___, A_OFF__, A_OFF__, 0}, // PHASE_RED
    {A_ON___, A_OFF__, A_OFF__, 0}, // PHASE_POST_RED
    {A_ON___, A_OFF__, A_OFF__, 0}  // PHASE_PRE_GREEN
  }
};

class Ats_phase {
  public:
    Ats_phase();
    ~Ats_phase();
    void configure(unsigned char type, int red, int amber, int green, int demand_pin, int detector_pin);
    void setMinTimes(int phase_step, int min);
    void tick(int millseconds);
    void detect();
    void phase_change_set(unsigned char state);
    unsigned char phase_change_return();
    unsigned char state();
    bool ran_min_green();
    bool demanded();
    bool debug_to_serial;
    void serial_debug(unsigned char p);

  private:
    /* Status of phase */
    unsigned char _type;
    unsigned char _state; // PHASE_GREEN 0, PHASE_RED 3 etc....
    unsigned char _phase_change; // PHASE_CHANGE_NONE 0, PHASE_CHANGE_TO_GREEN 1, PHASE_CHANGE_TO_GREEN_RED 2
    unsigned long _time_on_green_milliseconds;
    unsigned long _time_since_green_milliseconds;
    unsigned long _time_on_current_state_milliseconds;
    int _min_times[PHASE_STEPS];
    bool _demand;

    /* Wiring to pins */
    int _aspect_pins[3] ; // RAG
    int _demand_green_pin; // Used show a demand for green e.g. a wait
    int _detector_pin ; // The detector
    void _set_pin_mode(int p, int mode);
    void _set_pin_output(int p, unsigned char aspect_state);

    void _set_outputs();
};

#endif 

