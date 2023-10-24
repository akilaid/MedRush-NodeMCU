#ifndef DOSE_TIME_CHECKER_H
#define DOSE_TIME_CHECKER_H

#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial NodeMCU(D3, D4); // RX, TX

struct ts t;

// Define flags to track whether the commands have been sent
bool morningPillCommandSent = false;
bool afternoonPillCommandSent = false;
bool eveningPillCommandSent = false;

void resetFlags()
{
  // Get the current time
  morningPillCommandSent = false;
  afternoonPillCommandSent = false;
  eveningPillCommandSent = false;
}

void printMedicineData()
{

  DS3231_get(&t);

  int currentHour = t.hour;
  int currentMinute = t.min;

  // Serial.println(currentHour);
  // Serial.println(currentMinute);
  // Call the function to reset the flags at the beginning of the day
  delay(500);

  if (EEPROM.read(EEPROM_ADDRESS_TIME) == 255)
  {
    Serial.println("Address does not exist");
    return;
  }
  else
  {
    EEPROM.get(EEPROM_ADDRESS_TIME, medicineTimeData);
    // Serial.println(medicineTimeData);
    StaticJsonDocument<256> doc;
    deserializeJson(doc, medicineTimeData);
    // serializeJsonPretty(doc, Serial);

    const char *morningValue = doc["morning"];
    const char *afternoonValue = doc["afternoon"];
    const char *eveningValue = doc["evening"];

    int morningHour = atoi(morningValue);
    int morningMinute = atoi(strchr(morningValue, ':') + 1);
    int afternoonHour = atoi(afternoonValue);
    int afternoonMinute = atoi(strchr(afternoonValue, ':') + 1);
    int eveningHour = atoi(eveningValue);
    int eveningMinute = atoi(strchr(eveningValue, ':') + 1);

    // Check if it's time to send the morning pill command
    if (morningHour == currentHour && morningMinute == currentMinute && !morningPillCommandSent)
    {
      //Serial.println("Sending morning pill dispense command to slave");
      // Serial.print(currentHour);
      // Serial.print(":");
      // Serial.print(currentMinute);
      NodeMCU.println("dropPillsM");
      morningPillCommandSent = true; // Set the flag to true
    }
    else if (afternoonHour == currentHour && afternoonMinute == currentMinute && !afternoonPillCommandSent)
    {
      //Serial.println("Sending afternoon pill dispense command to slave");
      // Serial.print(currentHour);
      // Serial.print(":");
      // Serial.print(currentMinute);
      NodeMCU.println("dropPillsA");
      afternoonPillCommandSent = true; // Set the flag to true
    }
    else if (eveningHour == currentHour && eveningMinute == currentMinute && !eveningPillCommandSent)
    {
      //Serial.println("Sending evening pill dispense command to slave");
      // Serial.print(currentHour);
      // Serial.print(":");
      // Serial.println(currentMinute);
      NodeMCU.println("dropPillsE");
      eveningPillCommandSent = true; // Set the flag to true
    }
    else if (currentHour == 0 && currentMinute == 0)
    {
      resetFlags();
    }
  }
}

#endif
