#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
char r;
void setup()
{
  SerialBT.begin("Sumedha's ESP");
  pinMode(2,OUTPUT);
}
void loop(){
  r=SerialBT.read();
  if(r=='1')
  {
    digitalWrite(2,HIGH);
  }
  if(r=='0'){
    digitalWrite(2,LOW);
  }
}
