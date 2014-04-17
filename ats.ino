/*
 */
 
#include <Ats_phase.h>

#define TOTAL_PHASES 2
#define HEARTBEAT_PIN 13

volatile long tick_count = 0;
volatile bool flash = true;

Ats_phase phases[TOTAL_PHASES];



void setup() {
  phases[0].configure(TRAFFIC_JUNCTION, 14,15,16,0,0); // No Demand light (wait)
  phases[1].configure(PED_JUNCTION, 7,0,8,9,10);  //No Amber
  phases[1].setMinTimes(PHASE_GREEN,5);  
  phases[1].setMinTimes(PHASE_BLACKOUT,4);  
  
  // In the future need to treat intergreen correctly
  phases[0].setMinTimes(PHASE_POST_RED,6);  // Crude intergreen
  phases[1].setMinTimes(PHASE_POST_RED,3);  // Crude intergreen
  phases[1].setMinTimes(PHASE_PRE_GREEN,2);  // Crude intergreen
  
  phases[0].demand_set(DEMAND_GREEN);
  phases[1].demand_set(DEMAND_RED);  

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
    delay(1000);
  }

  Serial.println("Flip demand");
  phases[1].demand_set(DEMAND_GREEN);
  phases[0].demand_set(DEMAND_RED); 
}

void interupt_demands() {
  for (unsigned char p = 0; p < TOTAL_PHASES; p++) {  
    phases[p].detect();
  }
}


// This is run every (HEATBEAT_MILLS) ms
ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  tick_count++;
  if (tick_count>=FLASH_AFTER_HEARTBEATS) {
    tick_count = 0;
    digitalWrite(HEARTBEAT_PIN, digitalRead(HEARTBEAT_PIN) ^ 1);
  }

  for (unsigned char p = 0; p < TOTAL_PHASES; p++) {  
    phases[p].tick(HEATBEAT_MILLS);
  }
  decide_movements();
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



void setup_interrupts()  {
  // initialize timer1 
  // http://letsmakerobots.com/content/arduino-101-timers-and-interrupts
  // With added comments by BMS 2014-04-17
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;               // Reset the values
  TCCR1B = 0;
  TCNT1  = 0;
                            // Clock is 16MHz
                            // Target time is 10ms or 100Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  OCR1A = HEATBEAT;         // compare match register (16MHz÷256)÷100Hz
  TCCR1B |= (1 << WGM12);   // CTC mode                            
                            // CTC mode. Clear timer on compare match. 
                            // When the timer counter reaches the compare 
                            // match register, the timer will be cleared
                            // So once reaches 625, the ISR(TIMER1_COMPA_vect)
                            // will be run and the the counter will reset
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts 
}

