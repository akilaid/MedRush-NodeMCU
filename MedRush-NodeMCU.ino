#include <Wire.h>
#include <ds3231.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <stdlib.h>
#include "blinkLed.h"
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "emptyTrays.h"
#include "saveDoseTime.h"    //saveDoseTime.h header file
#include "doseTimeChecker.h" // doseTimeChecker.h header file
#include <SoftwareSerial.h>

const char * mqttServer = "168.138.30.76";
const int mqttPort = 8883;
const char * mqttUser = "akila";
const char * mqttPassword = "akilamqtt67";

#define LED_PIN 13

//char medicineData[256]; // Global variable to store medicineData
char medicineTimeData[256];

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  NodeMCU.begin(4800);
  Wire.begin();
  DS3231_init(DS3231_CONTROL_INTCN);

  pinMode(D3, INPUT);
  pinMode(D4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  WiFiManager wm;
  bool res;
  res = wm.autoConnect("MedRush", "password");
  if (!res) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("Connected to WiFi");
  }
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  EEPROM.begin(EEPROM_SIZE);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("MedRush IoT Client", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT Server");
    } else {
      Serial.print("Failed to connect to MQTT server. State: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
  
  client.subscribe("user1/medrush/getCmds");
}

void loop() {
  client.loop();
  tempRead();
  printMedicineData();
}

void callback(char * topic, byte * payload, unsigned int length) {
  char msgContent[length + 1];
  strncpy(msgContent, (char * ) payload, length);
  msgContent[length] = '\0';

  char * firstWord = strtok(msgContent, " ");
  if (strcmp(firstWord, "doseTimeUpdates") == 0) {
    char * receivedDoseTimeData = msgContent + strlen(firstWord) + 1;
    updateDoseTime(receivedDoseTimeData);
    NodeMCU.println("time-updated");
    delay(500);
    resetFlags();
  }

  if (strcmp(msgContent, "emptyTrays") == 0) {
    emptyTrays();
    NodeMCU.println("emptyTrays");
  }
}

void tempRead() {
  float temperature = DS3231_get_treg();
  char temperatureStr[10];
  snprintf(temperatureStr, sizeof(temperatureStr), "%.2f", temperature);
  //Serial.println(temperatureStr);

  static String prevTemp = "0"; // Declare prevTemp as static to retain its value between function calls.

  if (strcmp(temperatureStr, prevTemp.c_str()) != 0) {
    prevTemp = temperatureStr;
    client.publish("user1/medrush/temp", temperatureStr);
    Serial.println(temperatureStr);
  }
}
