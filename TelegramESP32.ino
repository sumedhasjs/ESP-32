#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>

const char* ssid = "Subramanyam";
const char* password = "9502620512";

#define BOTtoken "2052347033:AAHQjlyML4slVy44y21qc5yD6tz8kYjfccA"  
#define CHAT_ID "760647028"  //replace with your telegram user ID


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


int bot_delay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 2;
bool ledState = LOW;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("Handling New Message");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String user_text = bot.messages[i].text;
    Serial.println(user_text);

    String your_name = bot.messages[i].from_name;

    if (user_text == "/start") {
      String welcome = "Welcome, " + your_name + ".\n";
      welcome += "Use the following commands to control the ESP32.\n\n";
      welcome += "Send /led_on to turn GPIO2 ON \n";
      welcome += "Send /led_off to turn GPIO2 OFF \n";
      welcome += "Send /get_status to request current GPIO2 stateus \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (user_text == "/led_on") {
      bot.sendMessage(chat_id, "LED is now ON", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
    if (user_text == "/led_off") {
      bot.sendMessage(chat_id, "LED is now OFF", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    if (user_text == "/get_status") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LED is ON.", "");
      }
      else{
        bot.sendMessage(chat_id, "LED is OFF.", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + bot_delay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("Got Response!");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
