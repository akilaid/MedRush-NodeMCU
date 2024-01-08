#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <stdlib.h>
#include "blinkLed.h"
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "emptyTrays.h"
#include "saveDoseTime.h"    //saveDoseTime.h header file
#include "doseTimeChecker.h" // doseTimeChecker.h header filel
#include <SoftwareSerial.h>
#include "fanControl.h"
#include <DHT.h>

DHT dht;
const char *mqttServer = "168.138.30.76";
const int mqttPort = 8883;
const char *mqttUser = "akila";
const char *mqttPassword = "akilamqtt67";

// const char * mqttServer = "sg.akilaid.tech";
// const int mqttPort = 8883;
// const char * mqttUser = "akila";
// const char * mqttPassword = "Medrush345ewqA";

// char medicineData[256]; // Global variable to store medicineData
char medicineTimeData[256];

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600);
  NodeMCU.begin(4800);
  dht.setup(D7); //dht11 pin

  pinMode(D3, INPUT);
  pinMode(D4, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  WiFiManager wm;
  bool res;
  res = wm.autoConnect("MedRush", "password");
  if (!res)
  {
    Serial.println("Failed to connect");
  }
  else
  {
    Serial.println("Connected to WiFi");
  }
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  EEPROM.begin(EEPROM_SIZE);

  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");
    if (client.connect("MedRush IoT Client", mqttUser, mqttPassword))
    {
      Serial.println("Connected to MQTT Server");
    }
    else
    {
      Serial.print("Failed to connect to MQTT server. State: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  client.subscribe("user1/medrush/getCmds");
  client.publish("user1/medrush/temp", "00");
}

void loop()
{
  client.loop();
  tempRead();
  printMedicineData();
  fanControl();
}

void callback(char *topic, byte *payload, unsigned int length)
{
  char msgContent[length + 1];
  strncpy(msgContent, (char *)payload, length);
  msgContent[length] = '\0';

  char *firstWord = strtok(msgContent, " ");
  if (strcmp(firstWord, "doseTimeUpdates") == 0)
  {
    char *receivedDoseTimeData = msgContent + strlen(firstWord) + 1;
    updateDoseTime(receivedDoseTimeData);
    NodeMCU.println("time-updated");
    delay(500);
    resetFlags();
  }

  if (strcmp(msgContent, "emptyTrays") == 0)
  {
    emptyTrays();
    NodeMCU.println("emptyTrays");
  }
}

void tempRead()
{
  delay(dht.getMinimumSamplingPeriod());
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  static float prevTemp = 0.0;
  static float prevHumidity = 0.0;

  if (temperature != prevTemp || humidity != prevHumidity)
  {
    prevTemp = temperature;
    prevHumidity = humidity;

    // Reduce 20 from actual humidity level
    humidity -= 20;

    char message[20]; // Make sure the array is large enough to hold both temperature and humidity values
    snprintf(message, sizeof(message), "%d,%d", static_cast<int>(temperature), static_cast<int>(humidity));
    
    client.publish("user1/medrush/temp", message);
    Serial.println(message);
  }
}
