//Ardino Code to demonstrate and ATM system using ESP32 and Telegram Bot

//Including the libraries required
#include<stdio.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>
#include "HTTPClient.h"
#include <EEPROM.h> //EEPROM library to store the balance amount and denominations of Rs.2000/-, Rs.1000/-  and Rs500/-
#define EEPROM_SIZE 1 // Giving the size as 1 byte

int eepromArr[4]={250,5,10,10}; //EEPROM array that stores the balance amount, denominations of Rs.2000/-, Rs.1000/-  and Rs500/-

//defining my Network Credentials
const char* ssid = "XXXXXXXXX";
const char* password = "XXXXXXXX";

//defining thingspeak urls and apiKeys to write over the values
const char* serverName = "https://api.thingspeak.com/update";
String apiKey1 = "5X3AKK3QUULTMXY8"; //APIKey to display balance amout
String apiKey2 = "L1B1FJCSJ83AAOIH"; // API Key to diaplay denominations of Rs.2000/-
String apiKey3 = "G7TE9M14662B9BIR"; // API Key to display denominations of Rs.1000/-
String apiKey4 = "EQA99804UZFIHQ9X"; // API Key to display denominations of Rs.500/-


//defining Telegram Bot token and Bot user's ID
#define BOTtoken "2120511023:AAENHO_epEfpz6CPP9RVEYFiIjvkhWwTFUU"  //got using BotFather
#define CHAT_ID "760647028"  //got using IDBot


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int bot_delay = 1000;
unsigned long lastIterationOfBot;

int pinEnter(){ //A function to read the touch pin values and return the OTP touched
  int pinValues[8]={4,15,13,12,14,27,33,32}; //Definining the pins whose touch values are to be monitored
  int digitValues[8]={2,3,4,5,6,7,8,9}; //Digits corresponding to the touch pins
  int i=0; //iterator through pins to check which one is touched
  int j=0; // iterator to get two digits in total which will be the OTP entered
  int touchPin=0; //Will see which digits are touched and comes up with the OTP touched
  delay(10000);  // Giving delay to give user sufficient time to search for the pins if necessary
  for(j=0;j<2;j++){
    for(i=0;i<8;i++){    
      if(touchRead(13)<80 && touchRead(15)<80){ //Managing the case where the D13 and D15 touch pins are touched together to give input of digit '1'
          Serial.print("\nDigit:"); //Printing the digit in serial monitor
          Serial.print('1');
          delay(10000);
          touchPin=(touchPin*10) + 1;
          break;
      }
      if(touchRead(15)<80 && touchRead(4)<80){ //Managing the case where the D4 and D15 touch pins are touched together to give input of digit '0'
          Serial.print("\nDigit:");  //Printing the digit in serial monitor
          Serial.print('0');
          delay(10000);
          touchPin=(touchPin*10) + 0;
          break;
      } 
      if(touchRead(pinValues[i])<80){ //Managing all other cases i.e. from digit '2' till '9'
          Serial.print("\nDigit:"); 
          Serial.print(digitValues[i]);//Printing the digit in serial monitor
          delay(10000);
          touchPin=(touchPin*10) + digitValues[i];
          break;
        
      }
    }
  }
  Serial.print("\nSo, the OTP is "); //To view the OTP on Serial Monitor
  Serial.print(touchPin);//Printing the OTP entered in serial monitor
  Serial.print("\n");
  return touchPin; //Returning the sum of the touched OTP to compare with the generated one
}

int amountEnter(){ // A function to find out the amount inputted using touch pins and return the withdraw amount
  int pinValues[8]={4,15,13,12,14,27,33,32};//Definining the pins whose touch values are to be monitored
  int digitValues[8]={2,3,4,5,6,7,8,9}; //Digits corresponding to the touch pins
  int i=0;//iterator through pins to check which one is touched
  int j=0;// iterator to get two digits in total which will be the OTP entered
  int amountReceived=0; //Will see which digits are touched and comes up with the amount inputted
  delay(10000);  // Giving delay to give user sufficient time to search for the pins if necessary
  for(j=0;j<5;j++){
    for(i=0;i<8;i++){    
      if(touchRead(13)<80 && touchRead(15)<80){ //Managing the case where the D13 and D15 touch pins are touched together to give input of digit '1'
          Serial.print("\nDigit:"); 
          Serial.print('1');//Printing the digit in serial monitor
          delay(10000);
          amountReceived=(amountReceived*10) + 1;
          break;
      }
      if(touchRead(15)<80 && touchRead(4)<80){ //Managing the case where the D4 and D115 touch pins are touched together to give input of digit '0'
          Serial.print("\nDigit:"); 
          Serial.print('0');//Printing the digit in serial monitor
          delay(10000);
          amountReceived=(amountReceived*10) + 0;
          break;
      } 
      if(touchRead(pinValues[i])<80){ //Managing all other cases i.e. from digit '2' till '9'
          Serial.print("\nDigit:"); 
          Serial.print(digitValues[i]);//Printing the digit in serial monitor
          delay(10000);
          amountReceived=(amountReceived*10) + digitValues[i];
          break;
      }
    }
  }
  Serial.print("\nSo, the amount to be withdrawn is ");
  Serial.print(amountReceived); //Printing the amount to be withdrawn in serial monitor
  Serial.print("\n");
  return amountReceived; //Returning the sum of the touched amount to withdraw
}

int withdrawAmount1(int amountToBeWithdrawn, String chat_id){// Function to fint out the number of 2000 notes required for us to satisfy the amount asked
  int remainingAmount=amountToBeWithdrawn;
  int numberOfNotesOf2000=0, numberOfNotesOf1000=0, numberOfNotesOf500=0;
  int numberOfRemainingNotesOf2000=0, numberOfRemainingNotesOf1000=0, numberOfRemainingNotesOf500=0;
  if((amountToBeWithdrawn<=25000) && (amountToBeWithdrawn>=0) && (amountToBeWithdrawn%500==0)){
    numberOfNotesOf2000=remainingAmount/2000; //checking for number of 2000 notes required
    remainingAmount%=2000;
    if(numberOfNotesOf2000>5){ //checking if the notes requires are greater than the notes availabole of 2000 denomination
      remainingAmount+=(numberOfNotesOf2000-5)*2000; //adding the extra to remaining amount so that it can be covered by 1000 and 500 denomination
      numberOfNotesOf2000=5;
    }
  }
  return numberOfNotesOf2000;
}

int withdrawAmount2(int amountToBeWithdrawn, String chat_id){ // Function to fint out the number of 1000 notes required for us to satisfy the amount asked
  int remainingAmount=amountToBeWithdrawn;
  int numberOfNotesOf2000=0, numberOfNotesOf1000=0, numberOfNotesOf500=0;
  int numberOfRemainingNotesOf2000=0, numberOfRemainingNotesOf1000=0, numberOfRemainingNotesOf500=0;
  if((amountToBeWithdrawn<=25000) && (amountToBeWithdrawn>=0) && (amountToBeWithdrawn%500==0)){
    numberOfNotesOf2000=remainingAmount/2000;//checking for number of 2000 notes required
    remainingAmount%=2000;
    if(numberOfNotesOf2000>5){//checking if the notes requires are greater than the notes availabole of 2000 denomination
      remainingAmount+=(numberOfNotesOf2000-5)*2000;//adding the extra to remaining amount so that it can be covered by 1000 and 500 denomination
      numberOfNotesOf2000=5;
    }
    numberOfNotesOf1000=remainingAmount/1000;//checking for number of 1000 notes required
    remainingAmount%=1000;
    if(numberOfNotesOf1000>10){//checking if the notes requires are greater than the notes availabole of 1000 denomination
      remainingAmount+=(numberOfNotesOf1000-10)*1000;//adding the extra to remaining amount so that it can be covered by 500 denomination
      numberOfNotesOf1000=10;
    }
  }
  return numberOfNotesOf1000;
}

int withdrawAmount3(int amountToBeWithdrawn, String chat_id){ // Function to fint out the number of 500 notes required for us to satisfy the amount asked
  int remainingAmount=amountToBeWithdrawn;
  int numberOfNotesOf2000=0, numberOfNotesOf1000=0, numberOfNotesOf500=0;
  int numberOfRemainingNotesOf2000=0, numberOfRemainingNotesOf1000=0, numberOfRemainingNotesOf500=0;
  if((amountToBeWithdrawn<=25000) && (amountToBeWithdrawn>=0) && (amountToBeWithdrawn%500==0)){
    numberOfNotesOf2000=remainingAmount/2000;//checking for number of 2000 notes required
    remainingAmount%=2000;
    if(numberOfNotesOf2000>5){//checking if the notes requires are greater than the notes availabole of 2000 denomination
      remainingAmount+=(numberOfNotesOf2000-5)*2000;//adding the extra to remaining amount so that it can be covered by 1000 and 500 denomination
      numberOfNotesOf2000=5;
    }
    numberOfNotesOf1000=remainingAmount/1000;//checking for number of 1000 notes required
    remainingAmount%=1000;
    if(numberOfNotesOf1000>10){//checking if the notes requires are greater than the notes availabole of 1000 denomination
      remainingAmount+=(numberOfNotesOf1000-10)*1000;//adding the extra to remaining amount so that it can be covered by 500 denomination
      numberOfNotesOf1000=10;
    }
    numberOfNotesOf500=remainingAmount/500;//checking for number of 500 notes required
  }
  return numberOfNotesOf500; //Number of 500 notes required
}


  

void handleNewMessages(int numNewMessages) {
  Serial.println("\nHandling New Message");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id); //Check if the chat id entered and chat id fro which message is sent is same or not
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String user_text = bot.messages[i].text;
    Serial.println(user_text);

    String your_name = bot.messages[i].from_name;

    if (user_text == "/start") {  //Start message which consists of login option and also refers to each pins for the convienience of the user
      int blinkLED=0; //Welcoming the user into the ATM by blinking LED thrice
        while(blinkLED<3){ //loop to blin thrice
          digitalWrite(2,HIGH); //Turning on the GPIO2 LED
          delay(500); //Specifying some delay
          digitalWrite(2,LOW); //Turning off the GPIO2 LED
          delay(500); //Specifying some delay
          blinkLED++;
       }
      String welcome = "Welcome, " + your_name + ".\n"; //Welcome message
      welcome += "Use the following commands to control the ESP32.\n\n";
      welcome += "Send /login to login to your account \n";
      welcome += "Please note that the following are the touch pins corresponding to each digit. \n";
      welcome += "D4 pin and D15 pin touched together corresponds to '0' \n";
      welcome += "D13 pin and D15 pin touched together corresponds to '1' \n";
      welcome += "D4 pin corresponds to '2' \n";
      welcome += "D15 pin corresponds to '3' \n";
      welcome += "D13 pin corresponds to '4' \n";
      welcome += "D12 pin corresponds to '5' \n";
      welcome += "D14 pin corresponds to '6' \n";
      welcome += "D27 pin corresponds to '7' \n";
      welcome += "D33 pin corresponds to '8' \n";
      welcome += "D32 pin corresponds to '9' \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (user_text == "/login") {
      String myOTP = String(random(100)); //Generated a random OTP below 100 and send it to the user to input it using touch pins
      bot.sendMessage(chat_id, "Your OTP is " + myOTP+ ". \nPlease enter it to confirm your login.", "");
      int otpTyped = pinEnter(); //Calling th function which takes touch pin values and generates otp
      bot.sendMessage(chat_id, "The OTP you typed is " + String(otpTyped), "");  
      //Comparing the generated OTP and entered OTP. If user enters correctly, he/she can access their account to withdraw money or check their balance.
      if(String(otpTyped)==  myOTP){  // Displaying sucess of OTP by blinking LED thrice
         int blinkLED=0;
         while(blinkLED<3){ //loop to blin thrice
          digitalWrite(2,HIGH); //Turning on the GPIO2 LED
          delay(500); //Specifying some delay
          digitalWrite(2,LOW); //Turning off the GPIO2 LED
          delay(500); //Specifying some delay
          blinkLED++;
         }
         bot.sendMessage(chat_id, "The OTP you typed matches. \nYou can now access your withdraw and view balace option. " , "");
         bot.sendMessage(chat_id, "Send /balance to check your balance \nSend /withdraw to withdraw money \n " , "");
      }
      else{ //If user enters the wrong OPT, he/she is given a chance to login again with a new OTP
        bot.sendMessage(chat_id, "The OTP you typed is incorrect. Try again.\n/login " , "");
        
      }
    }
    if (user_text == "/withdraw") {//If the user sucessfully logins, he/she can access the withdraw option.
      bot.sendMessage(chat_id, "How much money do you want to withdraw? \nPlease enter it in a 5 digit form. \nAdd zero(s) prior if required.", "");
      int amountToBeWithdrawn = amountEnter(); // The function takes the input from the touch pins and based on that we send it to the function that calculates the number of notes of each denominations that are required
      int Denomination[3]={withdrawAmount1(amountToBeWithdrawn,chat_id),withdrawAmount2(amountToBeWithdrawn,chat_id),withdrawAmount3(amountToBeWithdrawn,chat_id)}; //making an array of notes required of 2000/-,1000/- and 500/-
      if((amountToBeWithdrawn!=0) && (withdrawAmount1(amountToBeWithdrawn,chat_id)==0) &&(withdrawAmount2(amountToBeWithdrawn,chat_id)==0) && (withdrawAmount3(amountToBeWithdrawn,chat_id)==0)){
        bot.sendMessage(chat_id, "Please enter a positive value that is less than 25,000 and is a multiple of 500. Use /withdraw to try again.","");// If the function takes an input which is no feasible, then the constraints on withdrawal amount are displayed and users can try to withdraw again
      }
      else{
        int originalBalanceAmount =EEPROM.get(1,eepromArr[0])*100; // else, if the input amount is feasible, then we firstly calculate the  balance left in the account and send messages to user about the amount he/she have debitted and have left in their account
        int currentBalanceAmount = originalBalanceAmount - amountToBeWithdrawn;
        Serial.print(currentBalanceAmount);
        eepromArr[0]=currentBalanceAmount/100;//update cuurent balance to EEPROM
        
        bot.sendMessage(chat_id, "Your amount of "+ String(amountToBeWithdrawn) + " is deducted from you account. \nPlease check your cash for the following denominations. \nThere will be "+ String(Denomination[0])+ " note(s) of 2000, " +String(Denomination[1]) +" note(s) of 1000, and " + String(Denomination[2]) +" note(s) of 500."  , "");
        int blinkLED=0; //Portraying sucessful transaction by blinking LED thrice
        while(blinkLED<3){ //loop to blin thrice
          digitalWrite(2,HIGH); //Turning on the GPIO2 LED
          delay(500); //Specifying some delay
          digitalWrite(2,LOW); //Turning off the GPIO2 LED
          delay(500); //Specifying some delay
          blinkLED++;
        }
        bot.sendMessage(chat_id, "Amount Withdrawn: " + String(amountToBeWithdrawn)+ "\nBalance Amount: " +String(currentBalanceAmount),"");
        bot.sendMessage(chat_id, "Send /balance to check your balance again \nSend /withdraw to withdraw money again \n " , ""); // Also giving user the option of withdrawing more money if required and also checking their balance once again if overlooked
        bot.sendMessage(chat_id, "Thanks for choosing our Bank. \nIt was lovely doing business with you.","");
        eepromArr[1]-=Denomination[0]; // Using EEPROM to update the values of denominations and balance amount
        eepromArr[2]-=Denomination[1];
        eepromArr[3]-=Denomination[2];
     
      }
    }
    
    
    if (user_text == "/balance") { // when user types this, he will be able to see his/her account balance
      int balanceAmount =EEPROM.read(0)*100; // since the EEPROM space is defined as 1 byte, we are using the first 3 digits and taking the last 3 digits as zero as denominations are all a multiple of 500
      bot.sendMessage(chat_id, "Your balance is " + String(balanceAmount),"");
      
    }
  }
}


void setup() {
  pinMode(2,OUTPUT); //Using pin 2 for blinking LED
  pinMode(4, INPUT); //Mentioning all the pin values that will be used for touch
  pinMode(15,INPUT);
  pinMode(13,INPUT);
  pinMode(12,INPUT);
  pinMode(14,INPUT);
  pinMode(27,INPUT);
  pinMode(33,INPUT);
  pinMode(32,INPUT);
  Serial.begin(115200); //Setting the baud rate
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); // Accessing the Wi-Fi 
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  while (WiFi.status() != WL_CONNECTED) {  //Connecting to WiFi
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  EEPROM.begin(EEPROM_SIZE); // declaring EEPROM addresses for balance amount and denominations of Rs.2000/-, Rs.1000/-  and Rs500/- and specifying their size as 1 byte
  EEPROM.put(1,eepromArr[0]);
  
  EEPROM.put(2,eepromArr[1]);
  
  EEPROM.put(3,eepromArr[2]);
  
  EEPROM.put(4,eepromArr[3]);
  
} 


void loop() {
  
  if (millis() > lastIterationOfBot + bot_delay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("\nGot Response!"); //Updating the Serial Monitor whenever a new response is received
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastIterationOfBot = millis();

    
  }
  if (WiFi.status() == WL_CONNECTED) { 
    HTTPClient http;
    http.begin(serverName);
    // Calling balance amount and denominations of Rs.2000/-, Rs.1000/-  and Rs500/- from EEPROM to plot on Thingspeak  
    int balanceAmount = EEPROM.get(1,eepromArr[0])*100; 
    int Denomination2000= EEPROM.get(2,eepromArr[1]);
    int Denomination1000=EEPROM.get(3,eepromArr[2]);
    int Denomination500=EEPROM.get(4,eepromArr[3]);
    
    String DataSent1 = "api_key=" + apiKey1 + "&field1=" + String(balanceAmount); //Link to access numerical wideget of balance Amount
    http.POST(DataSent1); //Sending balanceAmount values to Thingspeak
    delay(50);
    String DataSent2 = "api_key=" + apiKey2 + "&field1=" + String(Denomination2000); //Link to access numerical wideget of Denomination of 2000
    http.POST(DataSent2);//Sending 2000 Denomination values to Thingspeak
    delay(50);
    String DataSent3 = "api_key=" + apiKey3 + "&field1=" + String(Denomination1000); //Link to access numerical wideget of Denomination of 1000
    http.POST(DataSent3);//Sending 1000 Denomination values to Thingspeak
    delay(50);
    String DataSent4 = "api_key=" + apiKey4 + "&field1=" + String(Denomination500);  //Link to access numerical wideget of Denomination of 500
    http.POST(DataSent4);//Sending 500 Denomination values to Thingspeak
    delay(50);
    //Printing the values on Serial Monitor
    Serial.print("\nBalance Amount: "); 
    Serial.print(balanceAmount);
    Serial.print("\nDenomination of 2000: "); 
    Serial.print(Denomination2000);
    Serial.print("\nDenomination of 1000: "); 
    Serial.print(Denomination1000);
    Serial.print("\nDenomination of 500: "); 
    Serial.print(Denomination500);
    http.end();
  }
}
