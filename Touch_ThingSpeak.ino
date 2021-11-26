#include "WiFi.h"
#include "HTTPClient.h"

const char* ssid = "XXXXXXX";
const char* password = "XXXXXXX";

const char* serverName = "https://api.thingspeak.com/update";
String apiKey = "XXXXXXXX";

void setup(){
  pinMode(4,INPUT);
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  Serial.println("Connecting");
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected!");
}

void loop(){
  if(WiFi.status()==WL_CONNECTED){
    HTTPClient http;
    http.begin(serverName);
    int v=touchRead(4);
    String DataSent = "api_key=" + apiKey + "&field1=" + String(v);
    int Response = http.POST(DataSent);
    Serial.print("\nResponse: ");
    Serial.print(v);
    http.end();
  }
}
