#ifndef TIMER4_H
#include "timer4.h"
#endif

#ifndef TIMER5_H
#include "timer5.h"
#endif

#ifndef DM860_H
#define DM860_H

#define LINEAR 0

class DM860
{
  private:
  int pin_pulse;
  int pin_en;
  int pin_dir;
  int pin_interrupt;
  uint32_t pulse_rev;
  uint32_t ratio;
  uint32_t rpm;
  uint32_t frequency;
  bool encoder_en = false;
  
  bool toggle_dir = false;
  
  timer4 _timer4;
  timer5 _timer5;

  uint32_t restore_pwm;
  void beginTimer();
  void speedTimer();
  
  static DM860* sEncoder;
  static void updateEncoderISR();


  public:
  bool on_ramp = false;
  bool moving = false;
  bool dir = false;
  bool homing = false;
  int start_homing = 0;
  int end_homing = 0;
  int ramp_code = -1;
  
  uint32_t pwm_frequency;
  
  uint32_t ramp_start = 0;
  uint32_t ramp_stop = 0;
  uint32_t ramp_tc = 0;
  uint32_t ramp_speed = 0;
  uint32_t ramp_v0 = 0;
  volatile uint32_t ramp_cnt = 0;


  uint32_t maxSteps = 0;
  
  int32_t current_position = 0;
  int32_t next_position = 0;
  volatile uint32_t position = 0;
  volatile uint32_t enc_cnt = 0;
  

  void begin(int _pin_pulse,int _pin_en, int _pin_dir, int _pin_interrupt, uint32_t _pulse_rev, int _ratio);
  void setupLinearRamp(uint32_t _start, uint32_t _stop, uint32_t _tc, uint32_t _vf);
  void updateRamp(uint32_t x);
  uint32_t linearRamp(uint32_t x);
  void setPWMFrequency(uint32_t _pwmFrequency);
  void moveRounds(float rounds,bool dir);
  uint32_t calculateFrequencyFromRPM(float _rpm);
  uint32_t calculateFrequencyFromRPS(float _rps);

  void toggleDir();

  void dirHigh();

  void dirLow();

  bool dirIsHigh();

  bool dirIsLow();
  
  void start();

  void pause();

  void stop();

  bool isMoving();

  void setupHoming(int _start, int _end);

  bool startHoming();

  bool endHoming();

  void moveToPosition(int32_t pos);

  void doHoming();
  
  void updateEncoder();

};
#endif
