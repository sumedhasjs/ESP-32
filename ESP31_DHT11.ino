#include "DHT.h" //including the DHT and the WiFi libraries
#include "WiFi.h"
#include "HTTPClient.h"
#define DHTPIN 4 //Specified the pin which will be connected to the output of DHT 11
#define DHTTYPE DHT11 //Specifying the type of DHT used

float h;
float t;
const char* ssid = "XXXXXXXXXX"; //WiFi credentials
const char* password =  "XXXXXXXXX";
const char* serverName = "https://api.thingspeak.com/update"; 
String apiKey1 = "XXXXXXXXXXX"; //Api keys of thingspeak
String apiKey2 = "XXXXXXXXXXX";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password); //connecting to wifi
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.print("Connected!");
  Serial.println(F("DHT11 test!"));

  dht.begin();
}

void loop() {
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverName);
  // Wait a few seconds between measurements.
    delay(2000);
  
    h = dht.readHumidity();
    // Read temperature as Celsius
    t = dht.readTemperature();
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    String HumidityDataSent = "api_key=" + apiKey1 + "&field1=" + String(h);
    String TempDataSent = "api_key=" + apiKey2 + "&field1=" + String(t);

    http.POST(HumidityDataSent);
    http.POST(TempDataSent);
  

  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println("");
 
}
