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

#ifndef PHASE_H
#define PHASE_H


/* Define some Globals use for a phase */

/* Phase Types */
/* Each phase type will go in the order .... */

#define GREEN 0
#define POST_GREEN 1
#define PRE_RED 2
#define RED 3
#define POST_RED 4
#define PRE_GREEN 5
#define PHASE_STEPS 5      // 0..5 = 6
//#define OUTPUT_PER_PHASE 2 // 0..2 = 3
#define OUTPUT_PER_PHASE 3 // As going to store min time at end

/* Phase types */
#define PHASE_TYPES 3
#define TRAFFIC_PELICAN 0
#define PED_PELICAN 1
#define TRAFFIC_JUNCTION 2
#define PED_JUNCTION 3

// 0 = Off 1 = On 2 = Flashing
static byte phaseTypes[PHASE_TYPES][PHASE_STEPS][OUTPUT_PER_PHASE]=
{
  { // TRAFFIC_PELICAN 0
    // R A G
    {0, 0, 1, 7}, // GREEN
    {0, 0, 1, 0}, // POST_GREEN
    {0, 1, 0, 3}, // PRE_RED
    {1, 0, 0, 0}, // RED
    {0, 2, 0, 0}, // POST_RED
    {0, 2, 0, 0}  // PRE_GREEN
  },
  { // PED_PELICAN 1
    // Red Figure - Green Figure
    {0, 0, 1, 5}, // GREEN
    {0, 0, 2, 0}, // POST_GREEN
    {1, 0, 0, 0}, // PRE_RED
    {1, 0, 0, 0}, // RED
    {1, 0, 0, 0}, // POST_RED
    {1, 0, 0, 0}  // PRE_GREEN
  },
  { // TRAFFIC_JUNCTION 2
    // R A G
    {0, 0, 1, 7}, // GREEN
    {0, 0, 1, 0}, // POST_GREEN
    {0, 1, 0, 3}, // PRE_RED
    {1, 0, 0, 0}, // RED
    {1, 0, 0, 0}, // POST_RED
    {1, 1, 0, 2}  // PRE_GREEN
  },
  { // PED_JUNCTION 3
    // Red Figure - Green Figure
    {0, 0, 1, 5}, // GREEN
    {0, 0, 0, 0}, // POST_GREEN
    {0, 0, 0, 0}, // PRE_RED
    {1, 0, 0, 0}, // RED
    {1, 0, 0, 0}, // POST_RED
    {1, 0, 0, 0}  // PRE_GREEN
  },


};

class Phase {
  public:
    Phase(byte type);
    ~Phase();
    void setPins(int red, int amber, int green, int demand);
    void setMinTimes(int phase_step, int min);

    byte demand; // 0 No demand, 1 - Green, 2 - Red
    int time_on_green_milliseconds;
    int time_since_green_milliseconds;
    void tick(int millseconds);

  private:
    byte _type;
    int _aspect_pins[2]; // RAG
    int _demand_green_pin; // Used show a demand for green e.g. a wait
    int _min_times[PHASE_STEPS];
};



#endif 
