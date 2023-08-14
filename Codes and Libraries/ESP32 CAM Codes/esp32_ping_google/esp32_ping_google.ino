#include <WiFi.h>
#include <ESP32Ping.h>
 
const char* ssid = "Router";
const char* password =  "oass@123";
 
void setup() {
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
 
  bool success = Ping.ping("www.google.com", 3);
 
  if(!success){
    Serial.println("Ping failed. Sorry Vanmali Cursed U");
    return;
  }
 
  Serial.println("Ping succesful. Hey Vartak");
 
 
}
 
void loop() { }