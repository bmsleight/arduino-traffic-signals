/*
 */
 
#include <Ats_phase.h>

#define TOTAL_PHASES 2

Ats_phase phases[TOTAL_PHASES];


void setup() {                
  Serial.begin(9600);    

  phases[0].configure(TRAFFIC_JUNCTION, 14,15,16,0,0); // No Demand light (wait)
  phases[1].configure(PED_JUNCTION, 7,0,8,9,10);  //No Amber
  phases[1].setMinTimes(PHASE_GREEN,5);  
  phases[1].setMinTimes(PHASE_BLACKOUT,4);  
  
  // In ther futre need to treat intergreen correctly
  phases[0].setMinTimes(PHASE_POST_RED,6);  // Crude intergreen
  phases[1].setMinTimes(PHASE_POST_RED,3);  // Crude intergreen
  phases[1].setMinTimes(PHASE_PRE_GREEN,2);  // Crude intergreen
  
  phases[0].demand_set(DEMAND_GREEN);
  phases[1].demand_set(DEMAND_RED);  
}

void loop() {
  for (unsigned char l = 0; l < 30; l++) {
    Serial.print("Phase 0 :");
    Serial.print(phases[0].state() );
    Serial.print(" Phase 1 :");
    Serial.println(phases[1].state() );

    interupt_tick();
    interupt_tick();
 
    interupt_tick();
    interupt_tick();

    interupt_tick();
    interupt_tick();

    interupt_tick();
    interupt_tick();

    interupt_tick();
    interupt_tick();

    decide_movements();
  }

  Serial.println("Flip demand");
  phases[1].demand_set(DEMAND_GREEN);
  phases[0].demand_set(DEMAND_RED); 


/*  interupt_tick();
  delay (100);
  interupt_tick();
  delay (100);
  interupt_tick();
  delay (100);
  interupt_tick();
  delay (100);
  */
}

void interupt_demands() {
  for (unsigned char p = 0; p < TOTAL_PHASES; p++) {  
    phases[p].detect();
  }
}
  
void interupt_tick()
{
  delay (100);

  for (unsigned char p = 0; p < TOTAL_PHASES; p++) {  
    phases[p].tick(100);
  }
}


void decide_movements() {
  // Want to fill this with better logic
  // Move to a stage based signals
  // At the moment only works for two phases
  // One resting and the other run for min
  // Assuming both conflict
  // Want stages, conflict, intergreen later....
  
  // Ped stage run to min.
  if ((phases[1].state() == PHASE_GREEN) && (phases[1].ran_min_green())) {
    Serial.print("Ped stage ran to min");
    phases[1].demand_set(DEMAND_RED);
    phases[0].demand_set(DEMAND_GREEN);    
  }
  // Vehicle has run min and ped demanded
  if ((phases[0].state() == PHASE_GREEN) && (phases[0].ran_min_green()) && (phases[1].demand_return() == DEMAND_GREEN)) {
    Serial.print("Vehicle stage ran to min");
    phases[0].demand_set(DEMAND_RED);
  }


}
