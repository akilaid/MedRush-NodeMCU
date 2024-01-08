#ifndef FANCONTROL_H
#define FANCONTROL_H

#include <Arduino.h>
const int fanPin = D5;
const int delayTime = 10; // Time delay in milliseconds between speed changes

void fanControl() {
  // Decrease the speed from 255 to 0
  //  for (int speed = 255; speed >= 0; speed--) {
  //    analogWrite(fanPin, speed);
  //    delay(delayTime);
  //  }
  //
  //  for (int speed = 0; speed <= 255; speed++) {
  //    analogWrite(fanPin, speed);
  //    delay(delayTime);
  //  }
  //  delay(5000);
  analogWrite(fanPin, 255);
//  delay(5000);
//  analogWrite(fanPin, 10);
//  delay(5000);
//  analogWrite(fanPin, 1);
//  delay(5000);
}

#endif
