#ifndef DM860_H
#include "DM860.h"
#endif

DM860* DM860::sEncoder = 0;

void DM860::doHoming()
{
  position = maxSteps;
  dirHigh();
  stop();
  homing = true;

  Serial.println("homing...");

  start();
  while (1)
  {
    if (isMoving() && startHoming())
    {
      stop();
      homing = false;
      current_position = 0;
      break;
    }
    //Serial.println(isMoving());
  }
  Serial.println("done.");
}


void DM860::moveRounds(float rounds,bool dir){
  if (dir == 1) dirHigh();
  else dirLow();
  moveToPosition(int32_t (rounds * pulse_rev * ratio));
}

void DM860::moveToPosition(int32_t pos)
{
  next_position = pos;
  pos = pos - current_position;
  //Serial.println(current_position);
  
  if (pos < 0) dirHigh();
  else dirLow();

  if (pos != 0)
  {
    encoder_en = true;
    start();
  }

}

bool DM860::dirIsHigh()
{
  return dir;
}

bool DM860::dirIsLow()
{
  return !dir;
}
void DM860::setupHoming(int _start, int _end)
{
  if (_start)
  {
    pinMode(_start, INPUT_PULLUP);
    start_homing = _start;
  }
  if (_end)
  {
    pinMode(_end, INPUT_PULLUP);
    end_homing = _end;
  }
}
bool DM860::startHoming()
{
  if (start_homing)
  {
    if (digitalRead(start_homing)) return true;
  }

  return false;
}
bool DM860::endHoming()
{
  if (end_homing)
  {
    if (digitalRead(end_homing)) return true;
  }

  return false;
}

void DM860::begin(int _pin_pulse, int _pin_en, int _pin_dir, int _pin_interrupt,uint32_t _pulse_rev, int _ratio)
{
  pin_pulse = _pin_pulse;
  pin_en = _pin_en;
  pin_dir = _pin_dir;
  pulse_rev = _pulse_rev;
  ratio = _ratio;
  pin_interrupt = _pin_interrupt;
  maxSteps = 0;
  
  for (int i = 0; i < _ratio; i++)
  {
    maxSteps += _pulse_rev;
  }
  //Serial.println(maxSteps);
  
  sEncoder = this;
  attachInterrupt(digitalPinToInterrupt(pin_interrupt), DM860::updateEncoderISR, RISING);

  pinMode(pin_en, OUTPUT);
  pinMode(pin_dir, OUTPUT);
  digitalWrite(pin_dir, dir);
  digitalWrite(pin_en, HIGH);

  beginTimer();
}


void DM860::updateEncoderISR()
{
    if (sEncoder != 0)
        sEncoder->updateEncoder();
}

uint32_t DM860::linearRamp(uint32_t x){
  float m = int32_t(ramp_speed - ramp_v0)/float(ramp_stop - ramp_start);
  int32_t mx = m * x;
  uint32_t vf = ramp_v0 + mx;
  return vf;
}


void DM860::setupLinearRamp(uint32_t _start, uint32_t _stop, uint32_t _tc, uint32_t _vf){
  ramp_start = _start;
  ramp_stop = _stop;
  ramp_tc = _tc;
  ramp_v0 = pwm_frequency;
  ramp_speed = _vf;
  ramp_code = 0;
}

void DM860::updateRamp(uint32_t x){
    
    if(x > ramp_start && x < ramp_stop){
      // Serial.println("#####");
      //Serial.println(x);
      on_ramp = true;
      if((x - ramp_start) % ramp_tc == 0){
        if(ramp_code == 0){
          uint32_t _speed = linearRamp(x-ramp_start);
          if((x + ramp_tc) >= ramp_stop){
            setPWMFrequency(ramp_speed);
          }
          else{
          // Serial.println("#########");
          // Serial.println(_speed);
          setPWMFrequency(_speed);
          // Serial.println("##################");
          // 
          }
          
          Serial.print(current_position);
          Serial.print("------");
          Serial.println(pwm_frequency);
        }

        else if(ramp_code == -1){

        }
      }
    }
    else{ on_ramp = false;}

}


void DM860::updateEncoder(){
  if(encoder_en){
    enc_cnt += 1; 
    if(moving){
      if(dir){
        current_position -= 1;
      }
      else{
        current_position += 1;
      }
    }
    
    if(ramp_code >= 0){
      updateRamp(enc_cnt);
    }
    //   if(ramp_en){
    //     ramp_cnt += 1;
    //     if(ramp_cnt > ramp_dist){
    //       ramp_cnt = 0;
    //       ramp_dist += ramp_dist*(ramp_pen);
    //       Serial.println(ramp_dist);
    //       speed = pwm_frequency + pwm_frequency*ramp_inc;
    //       Serial.println(speed);
    //       setPWMFrequency(pwm_frequency + speed);
    //     }
    //   }
    // }
    
    if(next_position == current_position){
      encoder_en = false;
      enc_cnt = 0;
      
      pause();
    }
  }
}

void DM860::beginTimer()
{
  if (pin_pulse < 9)
  {
    _timer4.beginStepper();
  }
  else
  {
    _timer5.beginStepper();
  }
}

void DM860::speedTimer()
{
  if (pin_pulse < 9)
  {
    _timer4.setStepperSpeed(pwm_frequency);
  }
  else
  {
    _timer5.setStepperSpeed(pwm_frequency);
  }
}

void DM860::setPWMFrequency(uint32_t _pwm_frequency)
{
  pwm_frequency = _pwm_frequency;
  speedTimer();
}

uint32_t DM860::calculateFrequencyFromRPS(float _rps)
{
  return uint32_t((pulse_rev*ratio)/_rps);
}


uint32_t DM860::calculateFrequencyFromRPM(float _rpm)
{
  float rps = (_rpm*60);
  return uint32_t((pulse_rev*ratio)/rps);
}

void DM860::toggleDir()
{
  dir = not(dir);

  digitalWrite(pin_dir, dir);
}

void DM860::dirHigh()
{
  dir = true;
  digitalWrite(pin_dir, dir);

}
void DM860::dirLow()
{
  dir = false;
  digitalWrite(pin_dir, dir);
}

void DM860::start()
{
  //setPWMFrequency(restore_pwm);
  moving = true;
  pinMode(pin_pulse, OUTPUT);

  digitalWrite(pin_en, LOW);
}
bool DM860::isMoving()
{
  return moving;
}

void DM860::pause()
{
  //setPWMFrequency(0);
  moving = false;
  pinMode(pin_pulse, INPUT_PULLUP);

 
}

void DM860::stop()
{
  moving = false;
  digitalWrite(pin_en, HIGH);
}
