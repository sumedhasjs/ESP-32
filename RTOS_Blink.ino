int c1=0;
int c2=0;

TaskHandle_t t1h= NULL;
TaskHandle_t t2h= NULL;

void t1(void *par){
  for(;;){
    Serial.print("Task 1 running on core ");
    Serial.println(xPortGetCoreID());
    Serial.print("Task 1 count:");
    Serial.print(c1++);
    Serial.println();
    vTaskDelay(1000);
   
  }
}
void t2(void *par){
  for(;;){
    Serial.print("Task 2 running on core ");
    Serial.println(xPortGetCoreID());
    Serial.print("Task 2 count:");
    Serial.print(c2++);
    digitalWrite(2,HIGH);
    delay(1000);
    digitalWrite(2,LOW);
    Serial.println();
    vTaskDelay(2000);
  }
}

void setup(){
  Serial.begin(115200);
  xTaskCreatePinnedToCore( t1, "Task 1" , 1000, NULL, 1, NULL,0);
  xTaskCreatePinnedToCore( t2, "Task 2" , 1000, NULL, 1, NULL,1);
  pinMode(2,OUTPUT);
}

void loop(){
  
}
