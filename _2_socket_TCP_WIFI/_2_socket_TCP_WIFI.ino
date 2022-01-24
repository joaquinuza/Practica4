#include <WiFi.h>
#include "time.h"


const char* ssid       = "RSense";
const char* password   = "";
//NTP variables
const long  gmtOffset_sec = 3600;   //Local time is utc+1 (winter time)
const char* ntpServer = "pool.ntp.org";
const int   daylightOffset_sec = 0;
//SERVER-CLIENT variables
const IPAddress tcp_ip(192,168,0,100);    //TCP socket ip address
const uint16_t port = 3007;              //TCP socket port
WiFiClient client;  //Create a client object to connect to the tcp-server
String send_string = "Hello";
String received_string = "";

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
  actual_time = String(timeinfo.tm_mday) + "/" + 
                String(timeinfo.tm_mon+1) + "/" + 
                String(timeinfo.tm_year+1900) + "  " + 
                String(timeinfo.tm_hour) + ":" +
                String(timeinfo.tm_min) + ":" + 
                String(timeinfo.tm_sec) + "\n";
  return actual_time;
}

void setup()
{
  Serial.begin(9600);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  Serial.println(WiFi.localIP());
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  client.connect(tcp_ip, port);
  
  
}

void loop()
{
  delay(1000);
  send_string = printLocalTime();
  if(client.available()){   //If there is data in the buffer then..
    received_string = client.readStringUntil('\n');
    }
  if(received_string.indexOf("start") != -1){
    client.print(send_string);  //Send a string to the server
  }
  else if(received_string.indexOf("stop") != -1){
    client.stop();        //Stop tcp gateway
    Serial.println("Ended the client and WiFi conection");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }
}


