#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define WLAN_SSID "Subramanyam"
#define WLAN_PASS "9502620512"
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883               
#define AIO_USERNAME "sumedhasjs"
#define AIO_KEY "aio_xGjD82oZpyhyeWLlQdGasHwEHoHV"

int output=2;

WiFiClient client;  
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);   
Adafruit_MQTT_Subscribe LED_Control = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LED_Control");

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(2,OUTPUT);
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  mqtt.subscribe(&LED_Control);
}

uint32_t x=0;

void loop() {
   MQTT_connect();
   Adafruit_MQTT_Subscribe *subscription;
   while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &LED_Control) {
      Serial.print(F("Got: "));
      Serial.println((char *)LED_Control.lastread);
      if (!strcmp((char*) LED_Control.lastread, "ON"))  {
        digitalWrite(2, HIGH);
      }
      else{
        digitalWrite(2, LOW);
      }
    }
  }
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
