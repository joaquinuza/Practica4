
//SEML standar
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <stdio.h>
#include "WiFi.h"
#include <WiFiClient.h> 
#include <ESP32_FTPClient.h>
#include <time.h>

#define WIFI_NETWORK "RSense"
#define WIFI_PASSWORD ""
#define WIFI_TIMEOUT_MS 20000 // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 30000 // Wait 30 seconds after a failed connection attemp

//FTP SERVER credentials
char ftp_server[] = "155.210.150.77";
char ftp_user[]   = "rsense";
char ftp_pass[]   = "rsense";
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass,5000,2);

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

void keepWiFiAlive(void * parameter) {

  //NTP variables
  const long  gmtOffset_sec = 3600;   //Local time is utc+1 (winter time)
  const char* ntpServer = "pool.ntp.org";
  const int   daylightOffset_sec = 0;

  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      //Update esp32 time from the web
      continue;
    }

    Serial.println("[WIFI] Connecting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();

    // Keep looping while we're not connected and haven't reached the timeout
    while (WiFi.status() != WL_CONNECTED &&
           millis() - startAttemptTime < WIFI_TIMEOUT_MS) {}
    // When we couldn't make a WiFi connection (or the timeout expired)
    // sleep for a while and then retry.
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WIFI] FAILED");
      vTaskDelay(WIFI_RECOVER_TIME_MS / portTICK_PERIOD_MS);
      continue;
    }
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.print("The local IP is ");
    Serial.println(WiFi.localIP());
  }
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
      //SEND FILE OVER FTP
      ftp.OpenConnection();
      ftp.ChangeWorkDir("/rsense/ftp_joaquin");
      ftp.InitFile("Type A");
      ftp.NewFile("grupo1_2.json");
      ftp.Write(json_string);
      ftp.CloseFile();
      ftp.CloseConnection();
      Serial.println("File written");
      cnt = 0;
      sensor_temperatures.clear();
    }
    cnt++;
    Serial.println(cnt);
    vTaskDelayUntil(&xLastWakeTime, xDelay1s);
  }
}


void setup() {
  Serial.begin(9600);
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
  xTaskCreate(keepWiFiAlive, "keepWiFiAlive", 5000, NULL, 2, NULL);
  xTaskCreate(json_ftp_task, "base_task", 2000, NULL, 1, NULL); //If code crashes check stack depth

}

void loop() {
  // put your main code here, to run repeatedly:

}
