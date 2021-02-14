#include "DM860.h"

#define PUL 7
#define DIR 4
#define EN 5
#define INTERRUPT 2

#define HOMING_PIN 30

DM860 motor;

uint32_t start_time = 0;
int stop_cnt = 0;

void setup(){
  Serial.begin(9600);
  pinMode(HOMING_PIN, INPUT_PULLUP);
  motor.begin(PUL,EN,DIR,INTERRUPT,3200,1);
  motor.setPWMFrequency(500);
//  motor.dirLow();
//  motor.start();
//  while(motor.isMoving()){
//      if(digitalRead(HOMING_PIN)){
//          motor.stop();
//      }
//  }  
  
  start_time = millis();
}

void loop(){
  if(millis() - start_time > 300000){
    //motor.pause();
    //start_time = millis();
    //delay(1000);
    motor.stop();
//    while(millis() - start_time < 2000){
//      delay(10);
//    }
//    start_time = millis();
  }
  else{
    delay(3000);
    motor.dirLow();
    motor.start();
    
    while(stop_cnt < 5){
      delay(10);
      if(digitalRead(HOMING_PIN)){
        stop_cnt += 1;
      }
    }
    Serial.println("Homed");
    motor.current_position = 0;
    stop_cnt = 0;
    motor.pause();
    delay(1000);
    motor.moveToPosition(-3500);
    while(motor.isMoving()){
      Serial.println("Moving");
      delay(1);
    }
  }
}
