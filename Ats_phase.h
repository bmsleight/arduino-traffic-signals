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

/* Define some Globals use for a phase */

/* Phase Types */
/* Each phase type will go in the order .... */

#define PHASE_GREEN 0
#define PHASE_POST_GREEN 1
#define PHASE_BLACKOUT 1
#define PHASE_PRE_RED 2
#define PHASE_RED 3
#define PHASE_POST_RED 4
#define PHASE_PRE_GREEN 5
#define PHASE_STEPS 6
//#define OUTPUT_PER_PHASE 3
#define OUTPUT_PER_PHASE 4 // As going to store min time at end

/* Phase types */
#define PHASE_TYPES 4
#define TRAFFIC_PELICAN 0
#define PED_PELICAN 1
#define TRAFFIC_JUNCTION 2
#define PED_JUNCTION 3

#define ASPECTS 3

/* Demands */
#define DEMAND_NONE 0
#define DEMAND_GREEN 1
#define DEMAND_RED 2



// 0 = Off 1 = On 2 = Flashing
static unsigned char phaseTypes[PHASE_TYPES][PHASE_STEPS][OUTPUT_PER_PHASE]=
{
  { // TRAFFIC_PELICAN 0
    // R A G
    {0, 0, 1, 7}, // PHASE_GREEN
    {0, 0, 1, 0}, // PHASE_POST_GREEN
    {0, 1, 0, 3}, // PHASE_PRE_RED
    {1, 0, 0, 0}, // PHASE_RED
    {0, 2, 0, 0}, // PHASE_POST_RED
    {0, 2, 0, 0}  // PHASE_PRE_GREEN
  },
  { // PED_PELICAN 1
    // Red Figure - Green Figure
    {0, 0, 1, 5}, // PHASE_GREEN
    {0, 0, 2, 0}, // PHASE_POST_GREEN
    {1, 0, 0, 0}, // PHASE_PRE_RED
    {1, 0, 0, 0}, // PHASE_RED
    {1, 0, 0, 0}, // PHASE_POST_RED
    {1, 0, 0, 0}  // PHASE_PRE_GREEN
  },
  { // TRAFFIC_JUNCTION 2
    // R A G
    {0, 0, 1, 7}, // PHASE_GREEN
    {0, 0, 1, 0}, // PHASE_POST_GREEN
    {0, 1, 0, 3}, // PHASE_PRE_RED
    {1, 0, 0, 0}, // PHASE_RED
    {1, 0, 0, 0}, // PHASE_POST_RED
    {1, 1, 0, 2}  // PHASE_PRE_GREEN
  },
  { // PED_JUNCTION 3
    // Red Figure - Green Figure
    {0, 0, 1, 5}, // PHASE_GREEN
    {0, 0, 0, 0}, // PHASE_POST_GREEN
    {0, 0, 0, 0}, // PHASE_PRE_RED
    {1, 0, 0, 0}, // PHASE_RED
    {1, 0, 0, 0}, // PHASE_POST_RED
    {1, 0, 0, 0}  // PHASE_PRE_GREEN
  }
};

class Ats_phase {
  public:
    Ats_phase();
    ~Ats_phase();
    void configure(unsigned char type, int red, int amber, int green, int demand, int detector_pin);
    void setMinTimes(int phase_step, int min);
    void tick(int millseconds);
    void detect();
    void demand();

  private:
    /* Status of phase */
    unsigned char _type;
    unsigned char _state; // PHASE_GREEN 0, PHASE_RED 3 etc....
    unsigned char _demand; // DEMAND_NONE 0, DEMAND_GREEN 1, DEMAND_RED 2
    int _time_on_green_milliseconds;
    int _time_since_green_milliseconds;
    int _min_times[PHASE_STEPS];
    /* Wiring to pins */
    int _aspect_pins[3] ; // RAG
    int _demand_green_pin; // Used show a demand for green e.g. a wait
    int _detector_pin ; // The detector

};



#endif 
