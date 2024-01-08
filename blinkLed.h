#ifndef BLINKLED_H
#define BLINKLED_H

#include <Arduino.h>
int LED_PIN = D0;
void blinkLed() {
  digitalWrite(LED_PIN, LOW);
  delay(50);
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  delay(50);
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
}

#endif
