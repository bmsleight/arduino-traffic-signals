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
 
#include <Ats_phase.h>

/* Set up the volatile counters for ticks and flashing on/off */
volatile long tick_count = 0;
volatile bool flash = true;
#define HEARTBEAT_PIN 13

/* define the phases  */
#define TOTAL_PHASES 2
Ats_phase phases[TOTAL_PHASES];



void setup() {
  phases[0].configure(TRAFFIC_JUNCTION, 2,3,4,0,0); // 0,0 No Demand light (wait), no Detector
  phases[1].configure(PED_JUNCTION, 5,0,6,7,12);  // 0 - No Amber Pin
  phases[1].setMinTimes(PHASE_GREEN,5);
  phases[1].setMinTimes(PHASE_BLACKOUT,4);
  
  // In the future need to treat intergreen correctly
  phases[0].setMinTimes(PHASE_POST_RED,6);  // Crude intergreen
  phases[1].setMinTimes(PHASE_POST_RED,3);  // Crude intergreen
  phases[1].setMinTimes(PHASE_PRE_GREEN,2);  // Crude intergreen
  
  // Insert a starting demand
  phases[0].demand_set(DEMAND_RED);
  phases[1].demand_set(DEMAND_GREEN);  

  setup_interrupts();
  Serial.begin(9600);
  pinMode(HEARTBEAT_PIN, OUTPUT);

}


void loop() {
  for (unsigned char l = 0; l < 30; l++) {
    Serial.print("Phase 0 :");
    Serial.print(phases[0].state() );
    Serial.print(" Phase 1 :");
    Serial.println(phases[1].state() );
    Serial.print(" Phase 0  Demand:");
    Serial.println(phases[0].demand_return() );
    Serial.print(" Phase 1  Demand:");
    Serial.println(phases[1].demand_return() );
    // demand_return()  
    delay(1000);
  }

  Serial.println("Insert demand");
  phases[0].demand_set(DEMAND_RED); 
  phases[1].demand_set(DEMAND_GREEN);
}

void interupt_demands() {
  for (unsigned char p = 0; p < TOTAL_PHASES; p++) {  
    phases[p].detect();
  }
}


// This is run every (HEATBEAT_MILLS) ms
ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  // Add to tick and flip (flash) the HEARTBEAT_PIN after FLASH_AFTER_HEARTBEATS ticks
  heartbeat();
  
  // Deal with each phase - stay on Red/Green or move to Red/Green
  // e,g if on Red-Amber, move to green
  // increments time counter for time on current state - e.g. Green min times.
  phase_changes();
  
  // Decide if a phase should from mov Red/Green to Green/Red
  // If on Red with a demand to green, go to Red-Amber
  decide_movements();
}

void heartbeat() {
  // Add to tick and flip (flash) the HEARTBEAT_PIN after FLASH_AFTER_HEARTBEATS ticks
  tick_count++;
  flash = !flash;
  if (tick_count>FLASH_AFTER_HEARTBEATS) {
    tick_count = 0;
    digitalWrite(HEARTBEAT_PIN, flash);
  }
}

void phase_changes() {
  // Deal with each phase - stay on Red/Green or move to Red/Green
  // increments time counter for time on current state - e.g. Green min times by HEATBEAT_MILLS
  for (unsigned char p = 0; p < TOTAL_PHASES; p++) {  
    phases[p].tick(HEATBEAT_MILLS);
  }
}  


void decide_movements() {
  // Decide if a phase should from mov Red/Green to Green/Red
  
  // Want to fill this with better logic
  // Move to a stage based signals
  // At the moment only works for two phases
  // One resting and the other run for min
  // Assuming both conflict
  // Want stages, conflict, intergreen later....
  
  // Ped stage run to min.
  if ((phases[1].state() == PHASE_GREEN) && (phases[1].ran_min_green())) {
    Serial.println("Ped stage ran to min");
    phases[1].demand_set(DEMAND_RED);
    phases[0].demand_set(DEMAND_GREEN);    
  }
  // Vehicle has run min and ped demanded
  if ((phases[0].state() == PHASE_GREEN) && (phases[0].ran_min_green()) && (phases[1].demand_return() == DEMAND_GREEN)) {
    Serial.println("Vehicle stage ran to min");
    phases[0].demand_set(DEMAND_RED);
  }

}



void setup_interrupts()  {
  // initialize timer1 
  // http://letsmakerobots.com/content/arduino-101-timers-and-interrupts
  // With added comments by BMS 2014-04-17
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;               // Reset the values
  TCCR1B = 0;               // in the registers
  TCNT1  = 0;
                            // Clock is 16MHz
                            // Target time is 10ms or 100Hz
  TCCR1B |= (1 << CS12);    // Set 256 prescaler - See datasheet
  OCR1A = HEATBEAT;         // compare match register (16MHz÷256)÷100Hz
                            // See Ats_phase.h - #define HEATBEAT 625 
                            //
  TCCR1B |= (1 << WGM12);   // CTC mode                            
                            // CTC mode. Clear timer on compare match. 
                            // When the timer counter reaches the compare 
                            // match register, the timer will be cleared
                            // So once reaches 625, the ISR(TIMER1_COMPA_vect)
                            // will be run and the the counter will reset
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts 
}

  
