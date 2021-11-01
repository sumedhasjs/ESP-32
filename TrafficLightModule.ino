
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>

const char* ssid = "XXXXXXXXXXXXX";
const char* password = "XXXXXXXXXXX";

#define BOTtoken "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  
#define CHAT_ID "XXXXXXXXXXX"  

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


int bot_delay = 1000;
unsigned long lastTimeBotRan;

#define RedLedPin 4
#define YellowLedPin 5
#define GreenLedPin 18

bool ledState = LOW;

void handleNewMessages(int numNewMessages) {
  Serial.println("Handling New Message");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String user_text = bot.messages[i].text;
    Serial.println(user_text);

    String your_name = bot.messages[i].from_name;

    if (user_text == "/start") {
      String welcome = "Welcome, " + your_name + ".\n";
      welcome += "Use the following commands to control the ESP32.\n\n";
      welcome += "Send /red_led_on to turn Red LED ON \n";
      welcome += "Send /red_led_off to turn Red LED OFF \n";
      welcome += "Send /yellow_led_on to turn Yellow LED ON \n";
      welcome += "Send /yellow_led_off to turn Yellow LED OFF \n";
      welcome += "Send /green_led_on to turn Green LED ON \n";
      welcome += "Send /green_led_off to turn Green LED OFF \n";
      welcome += "Send /get_status to request current LED stateus \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (user_text == "/red_led_on") {
      bot.sendMessage(chat_id, " Red LED is now ON", "");
      ledState = HIGH;
      digitalWrite(RedLedPin, ledState);
    }
    
    if (user_text == "/red_led_off") {
      bot.sendMessage(chat_id, "Red LED is now OFF", "");
      ledState = LOW;
      digitalWrite(RedLedPin, ledState);
    }

    if (user_text == "/yellow_led_on") {
      bot.sendMessage(chat_id, "Yellow LED is now ON", "");
      ledState = HIGH;
      digitalWrite(YellowLedPin, ledState);
    }
    
    if (user_text == "/yellow_led_off") {
      bot.sendMessage(chat_id, "Yellow LED is now OFF", "");
      ledState = LOW;
      digitalWrite(YellowLedPin, ledState);
    }

    if (user_text == "/green_led_on") {
      bot.sendMessage(chat_id, "Green LED is now ON", "");
      ledState = HIGH;
      digitalWrite(GreenLedPin, ledState);
    }
    
    if (user_text == "/green_led_off") {
      bot.sendMessage(chat_id, "Green LED is now OFF", "");
      ledState = LOW;
      digitalWrite(GreenLedPin, ledState);
    }
    
    if (user_text == "/get_status") {
      if (digitalRead(RedLedPin)){
        bot.sendMessage(chat_id, "Red LED is ON.", "");
      }
      else{
        bot.sendMessage(chat_id, "Red LED is OFF.", "");
      }
      if (digitalRead(YellowLedPin)){
        bot.sendMessage(chat_id, "Yellow LED is ON.", "");
      }
      else{
        bot.sendMessage(chat_id, "Yellow LED is OFF.", "");
      }
      if (digitalRead(GreenLedPin)){
        bot.sendMessage(chat_id, "Green LED is ON.", "");
      }
      else{
        bot.sendMessage(chat_id, "Green LED is OFF.", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RedLedPin, OUTPUT);
  digitalWrite(RedLedPin, ledState);

  pinMode(YellowLedPin, OUTPUT);
  digitalWrite(YellowLedPin, ledState);

  pinMode(GreenLedPin, OUTPUT);
  digitalWrite(GreenLedPin, ledState);
  
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
