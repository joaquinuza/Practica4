/*********
  Mandar automaticamente -> state -> variable -> con ajax
*********/

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "ESP32Time.h"

// Replace with your network credentials
const char* ssid = "RSense";
const char* password = "";
//NTP variables
const long  gmtOffset_sec = 3600;   //Local time is utc+1 (winter time)
const char* ntpServer = "pool.ntp.org";
const int   daylightOffset_sec = 0;
//Program variables
String state="";
// Set LED GPIO
const int ledPin = 32;
// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String printLocalTime()
/* Funtion that formats the timeinfo variable to the next string  
 * Day/Month/Year H:%M:%S
 */
{
  struct tm timeinfo;
  String actual_time = "";
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  actual_time = String(timeinfo.tm_hour) + ":" +
                String(timeinfo.tm_min);
  return actual_time;
}


// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    return state;
  }
  return String();
}
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  state = printLocalTime();

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    state = printLocalTime();
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
//    setTime(0, 0, 0, 1, 1, 2022); 
    state = printLocalTime();
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
