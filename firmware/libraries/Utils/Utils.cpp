#include "Utils.h"


void error_loop(uint16_t freq){
  uint16_t period = (int)(1000/freq);
  while(1){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(period);
  }
}


void debug_loop(int time_1, int time_2)
{
  pinMode(LED_BUILTIN, OUTPUT);
  while (1)
  {
    digitalWriteFast(LED_BUILTIN, HIGH);
    delay(time_1);
    digitalWriteFast(LED_BUILTIN, LOW);
    delay(100);
    digitalWriteFast(LED_BUILTIN, HIGH);
    delay(time_2);
    digitalWriteFast(LED_BUILTIN, LOW);
    delay(100);
  }
}