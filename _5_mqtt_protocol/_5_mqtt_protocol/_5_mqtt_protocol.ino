//SEML standar
#include <ArduinoJson.h>
#include <stdio.h>
#include <time.h>
#include "EspMQTTClient.h"

EspMQTTClient client(
  "RSense",
  "",
  "xxxxxxxxxxxxx",  // MQTT Broker server ip
  "xxxxxxxxxx",   // Can be omitted if not needed
  "xxxxxxxxxxxx",   // Can be omitted if not needed
  "ESP32_TEST",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("/feeds/ESP32_TEMPERATURE", [](const String & payload) {
    Serial.println(payload);
  });
  
  // Publish a message to "mytopic/test"
  client.publish("mytopic/test", "This is a message"); // You can activate the retain flag by setting the third parameter to true

  // Execute delayed instructions
  client.executeDelayed(5 * 1000, []() {
    client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  });
}


// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void json_ftp_task(void *pvParameters) {
  /*Function that captures data every second,forms a json file every 10 seconds
     and sends it to an ftp server
  */
  //Task handling variables
  const TickType_t xDelay1s = pdMS_TO_TICKS (1000);
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  //Task variables
  uint8_t cnt = 0, tm = 10;
  const int SIZE = 10;
  uint16_t arrayTemp[SIZE]; //Array to store 10 values
  String localTime = "";
  // Use arduinojson.org/v6/assistant to compute the capacity.
  const size_t CAPACITY = 100 + JSON_ARRAY_SIZE(10);
  //save json to char
  char json_string[256];
  while (1) {
    if (cnt == 1) {
      localTime = getTime();
    }
    if (cnt >= 1 && cnt < 11) {
      arrayTemp[cnt - 1] = rand() % 1000;
    }

    if (cnt == 11) {
      DynamicJsonDocument sensor_temperatures(CAPACITY);
      sensor_temperatures["bn"] = "temp_sensor_1";
      sensor_temperatures["bt"] = localTime;
      sensor_temperatures["bu"] = "Cel";
      JsonArray values = sensor_temperatures.createNestedArray("v");
      for (int i = 0; i < SIZE; i++) {
        values.add(arrayTemp[i]);
      }
      serializeJson(sensor_temperatures, json_string);
      cnt = 0;
      sensor_temperatures.clear();
    }
    cnt++;
    Serial.println(cnt);
    vTaskDelayUntil(&xLastWakeTime, xDelay1s);
  }
}

void json_mqtt(void *pvParameters){
  
}


void setup() {
  Serial.begin(9600);
  // Optionnal functionnalities of EspMQTTClient : 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
  xTaskCreate(json_mqtt, "base_task", 2000, NULL, 1, NULL); //If code crashes check stack depth
}

void loop() {
  // put your main code here, to run repeatedly:

}
