#ifndef TIMER5_H
#include "timer5.h"
#endif

void timer5::beginStepper()
{
  pinMode(46, OUTPUT); // 46:OCR5A
  pinMode(45, OUTPUT); // 45:OCR5B
  pinMode(44, OUTPUT); // 44:OCR5C
  cli();
  TCCR5A |= (0x01 << COM5A0) | (0x02 << COM5B0) | (0x02 << COM5C0) | (0x03 << WGM50);

  TCCR5B = (0x03 << WGM52) | (0x01 << CS50);

  sei();
}

void timer5::setStepperSpeed(uint32_t _frequency)
{
  if(_frequency>=250&&_frequency <= 8000000)
  {
    compare_A = (uint16_t)((clk/_frequency) - 1);
    
    OCR5A = compare_A;
    OCR5B = 0.5*compare_A;
    OCR5C = 0.5*compare_A;
  }
}
void timer5::beginServo(uint32_t _frequency)
{
  pinMode(45, OUTPUT); // 7:OCR4B
  pinMode(44, OUTPUT); // 8:OCR4C

  cli();

  /* Set 16bit PWM fast mode and pwm outputs */
  TCCR5A |= (0x02 << COM5B0) | (0x02 << COM5C0) | (0x03 << WGM50);
  TCCR5A |= (0x01 << COM5A0);

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
  OCR5A = compare_A;

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
  TCCR5B = (0x03 << WGM52) | (clock_select << CS50);

  sei();
  
  /* uncomment for debug purposes */
  //Serial.print("Prescaler:\t"); Serial.println(prescaler);
  //Serial.print("Compare value:\t"); Serial.println(temp_compare);
  return;
}


void timer5::setDutyCycle(int _pin, int _dutyCycle)
{
  switch (_pin)
  {
    case 45: OCR5B = compare_A * _dutyCycle / 100; break;
    case 44: OCR5C = compare_A * _dutyCycle / 100; break;
    default: return;
  }
  return;
}
