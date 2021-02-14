#ifndef TIMER4_H
#include "timer4.h"
#endif
#ifndef ARDUINO_H
#include <Arduino.h>
#endif

void timer4::beginStepper()
{
  pinMode(6, OUTPUT); // 7:OCR4A
  pinMode(7, OUTPUT); // 7:OCR4B
  pinMode(8, OUTPUT); // 8:OCR4C
  cli();
  TCCR4A |= (0x01 << COM4A0) | (0x02 << COM4B0) | (0x02 << COM4C0) | (0x03 << WGM40);

  TCCR4B = (0x03 << WGM42) | (0x01 << CS40);

  sei();
}

void timer4::setStepperSpeed(uint32_t _frequency)
{
  if(_frequency>=250&&_frequency <= 8000000)
  {
    TCCR4A |= (0x01 << COM4A0) | (0x02 << COM4B0) | (0x02 << COM4C0);
    compare_A = (uint16_t)((clk/_frequency) - 1);
    
    OCR4A = compare_A;
    OCR4B = 0.5*compare_A;
    OCR4C = 0.5*compare_A;
  }
  else if (_frequency == 0) TCCR4A &= ~(0x01 << COM4A0) & ~(0x02 << COM4B0) & ~(0x02 << COM4C0);
}
void timer4::beginServo(uint32_t _frequency)
{
  pinMode(7, OUTPUT); // 7:OCR4B
  pinMode(8, OUTPUT); // 8:OCR4C

  cli();

  /* Set 16bit PWM fast mode and pwm outputs */
  TCCR4A |= (0x02 << COM4B0) | (0x02 << COM4C0) | (0x03 << WGM40);
  //TCCR4A |= (0x01 << COM4A0);

  /* determine prescaler and OCR4A values */
  int temp_compare;
  int i;
  int prescaler;
  byte clock_select;

  /* try prescaler values starting from the largest */
  for (i = 4; i >= 0; i--)
  {
    if (i <= 2 ) prescaler = pow(8, i); //prescalers 1, 8, 64
    else if (i == 3) prescaler = 256;
    else prescaler = 1024;

    /* calculate OCR4A */
    temp_compare = (clk / (_frequency * prescaler)) - 1;
    if (temp_compare > 0) break;
  }

  /* store OCR4A value */
  compare_A = (uint16_t)temp_compare;

  /* set OCR4A */
  OCR4A = compare_A;

  /* set CS2 CS1 CS0 for prescaler */
  switch (i)
  {
    case 0: clock_select = 0x1;  break;
    case 1: clock_select = 0x2;  break;
    case 2: clock_select = 0x3;  break;
    case 3: clock_select = 0x4;  break;
    case 4: clock_select = 0x5;  break;
    default: return;
  }
  TCCR4B = (0x03 << WGM42) | (clock_select << CS40);

  sei();
  
  /* uncomment for debug purposes */
  //Serial.print("Prescaler:\t"); Serial.println(prescaler);
  //Serial.print("Compare value:\t"); Serial.println(temp_compare);
  return;
}


void timer4::setDutyCycle(int _pin, int _dutyCycle)
{
  switch (_pin)
  {
    case 7: OCR4B = compare_A * _dutyCycle / 100; break;
    case 8: OCR4C = compare_A * _dutyCycle / 100; break;
    default: return;
  }
  return;
}
