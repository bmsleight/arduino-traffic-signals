#define ledPin 13

volatile long tick_count = 0;


void setup()
{
  Serial.begin(9600); 
  pinMode(ledPin, OUTPUT);
  
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
  OCR1A = HEATBEAT;              // compare match register (16MHz÷256)÷100Hz
  TCCR1B |= (1 << WGM12);   // CTC mode                            
                            // CTC mode. Clear timer on compare match. 
                            // When the timer counter reaches the compare 
                            // match register, the timer will be cleared
                            // So once reaches 625, the ISR(TIMER1_COMPA_vect)
                            // will be run and the the counter will reset
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  tick_count++;
  if (tick_count>=TICK_PER_HEARTBEAT) {
    tick_count = 0;
    digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
  }
}

void loop()
{
  Serial.print("Tick :");
  Serial.println(tick_count);
  delay(1000);
  // your program here...
}

