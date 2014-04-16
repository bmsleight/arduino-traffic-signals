/*
 */
 
#include <Ats_phase.h>

#define TOTAL_PHASES 2

Ats_phase phases[TOTAL_PHASES];


void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(13, OUTPUT);     

  phases[1].configure(TRAFFIC_JUNCTION, 14,15,16,0,0); // No Demand light (wait)
  phases[2].configure(PED_JUNCTION, 7,0,8,9,10);  //No Amber
  phases[2].setMinTimes(PHASE_GREEN,5);  
  phases[2].setMinTimes(PHASE_BLACKOUT,4);  
  phases[1].demand();
  
}

void loop() {
  digitalWrite(13, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(1000);              // wait for a second
}

void interupt_demands() {
  for (unsigned char p = 0; p < TOTAL_PHASES; p++) {  
    phases[p].detect();
  }
}
  
void interupt_tick()
{


}


void decide_movements() {
  // Want to fill this with better logic
  // Move to a stage based signals
  // At the moment only works for two phases
  // One resting and the other run for min
  // Assuming both conflict
  // Want stages, conflict, intergreen later....
  
  
}
