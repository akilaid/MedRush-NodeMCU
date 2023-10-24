#ifndef SAVE_DOSE_TIME_H
#define SAVE_DOSE_TIME_H

#include <Arduino.h>

#include <EEPROM.h>

#define EEPROM_SIZE 512 // Specify the size of the EEPROM data
#define EEPROM_ADDRESS_TIME 256 // Starting address for medicineTimeData (different from EEPROM_ADDRESS_DATA)

extern char medicineTimeData[256]; // Declare the medicineTimeData variable as an external variable

void updateDoseTime(const char * receivedDoseTimeData) {
  // Store receivedDoseTimeData in medicineTimeData variable
  strcpy(medicineTimeData, receivedDoseTimeData);
  // Store medicineTimeData in EEPROM
  EEPROM.put(EEPROM_ADDRESS_TIME, medicineTimeData);
  EEPROM.commit();
  Serial.print("Updated medicineTimeData: ");
  Serial.println(medicineTimeData);
  blinkLed();
}

#endif
