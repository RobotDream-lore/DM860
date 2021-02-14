#ifndef ARDUINO_H
#include <Arduino.h>
#endif

#ifndef TIMER4_H
#define TIMER4_H
class timer4
{
  private:
  byte mode;
  uint16_t compare_A;
  uint32_t clk = 16000000;
  
  public:

  /* funzioni servo per fissare una frequenza e impostare 
  il duty cycle di un'uscita */
  void beginServo(uint32_t _frequency);

  void setDutyCycle(int _pin, int dutyCycle);
  /*****************************************************/

  void beginStepper();

  void setStepperSpeed(uint32_t _frequency);
};
#endif
