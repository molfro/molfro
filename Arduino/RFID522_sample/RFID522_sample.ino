#include <Buzzer.h>

#include "CustomClasses.h"
#include "storage.h"
#include "GyverTimer.h"

#define DOOR_PIN       7  // Пін реле замка
#define BUZZER_PIN     3  // Пін БУССЕР

#define RED_LED_PIN    4 
#define GREEN_LED_PIN  5 
#define BLUE_LED_PIN   6
#define LED_STATE_OFF  0

#define LED_STATE_GREEN  1
#define LED_STATE_RED    2
#define LED_STATE_BLUE   3
#define LED_STATE_VIOLET 4

#define DOOR_STATE_ON    4
#define EE_START_ADDR    5   // Ключ для проверки на перше включення

#define ADMIN_KEY 0xD3526F0D  // Ключ адміна
#define DURATION_LED_LIGHT  1000
#define DURATION_LED_ADMIN  10000




Storage storage(EE_START_ADDR);


// для зберігання часу початку події
long time_start_led_blue = -10000;
unsigned long time_start_led_green = 0;
unsigned long time_start_led_red = 0;
unsigned long time_start_led_violet = 0;
unsigned long time_start_admin_mode = 0;


//зберігаємо флаг ( адмін притуляв свою карточку )
bool prevAdmin = false;
bool writeStatus = false;

bool doorOpenStatus = false;
unsigned long door_open_time;

void ledControl(){
     digitalWrite(RED_LED_PIN,LOW); 
      digitalWrite(GREEN_LED_PIN,LOW);
      digitalWrite(BLUE_LED_PIN,LOW);
   unsigned long curTime = millis();

  if(curTime < time_start_led_blue ){
      digitalWrite(BLUE_LED_PIN,HIGH);
  }
  if(curTime < time_start_led_green ){
      digitalWrite(GREEN_LED_PIN,HIGH);
  }
  if(curTime < time_start_led_red ){
      digitalWrite(RED_LED_PIN,HIGH);
  }
  if(curTime < time_start_led_violet ){
      digitalWrite(BLUE_LED_PIN,HIGH);
      digitalWrite(RED_LED_PIN,HIGH);
  }
  
}

/// 
void doTimerEvents(){
   // виключаємо адміністратора
   if(millis() >( time_start_admin_mode + DURATION_LED_ADMIN)){
    if(prevAdmin){
        Serial.println("Час очікування карти вийшов!");   
        writeStatus = false;
        prevAdmin = false;
      }
    }

    if(millis() >( door_open_time + DURATION_LED_ADMIN)){
          // зупиняємо подачу напругу на реле замка
        if(doorOpenStatus==true){    
          doorOpenStatus = false;
          digitalWrite(DOOR_PIN, LOW);    
          Serial.println("Закриваємо двері"); 
        }
    }
  }

/// встановлюємо час початку світіння світлодіода
void ledSetColor(int state, int duration=0)
{
  if(duration==0)
    duration = DURATION_LED_LIGHT;
    switch(state)
    {
    case  LED_STATE_OFF:
      time_start_led_blue=0;
      time_start_led_green=0;
      time_start_led_red=0;
      time_start_led_violet=0;
      break;
    case  LED_STATE_GREEN:
      time_start_led_green = millis()+ duration;
      break;
    case  LED_STATE_RED:
      time_start_led_red = millis()+ duration;

      break;
    case LED_STATE_BLUE:
      time_start_led_blue = millis()+ duration;

      break;
    case LED_STATE_VIOLET:
      time_start_led_violet = millis()+ duration;

      break;
    }
}
void setup() {
  Serial.begin(9600); 
  SPI.begin();
  pinMode(3,OUTPUT); 
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  
  ReadRFIDKeySetup();
  

  Serial.print("В базі збережено ");
  Serial.print(storage.savedTagsCount());
  Serial.println(" карток");
    }


void uidPrint(byte *uidBite, int uidSize)
   {
  for (byte i = 0; i < uidSize; i++) 
      {
     Serial.print(uidBite[i] < 0x10 ? " 0" : " ");
     Serial.println(uidBite[i], HEX);
      }
   }

bool IsAdmin(byte *uidBite, int uidSize)
    {
 return true; 
    }

    
void SetAdminMode(){
  prevAdmin = true;
  ledSetColor(LED_STATE_BLUE, DURATION_LED_ADMIN);
  Serial.println("Михайло!");
  time_start_admin_mode = millis();
  }

void AddNewCard(byte *uidBite, int uidSize){
  storage.saveTag(uidBite,uidSize);
  Serial.println("картка збережена!");
  writeStatus=false;
  prevAdmin = false;
  ledSetColor(LED_STATE_VIOLET);
  }  
void openDoor(){            // тут вставити код відкривання дверей

  if(doorOpenStatus==false){    
    doorOpenStatus = true;
    door_open_time = millis();     // запоминаємо час відкриття замка
    digitalWrite(DOOR_PIN, HIGH);    // подаємо напругу на реле замка
    Serial.println("відкриваємо двері"); 
    }
  } 

void loop() 
{
  ledControl();
  doTimerEvents();
  
  RFIDKey *key = ReadRFIDKey();  //uidPrint(mfrc522.uid.uidByte,mfrc522.uid.size)

  if(key!=NULL)       //ключ піднесли
       {
                     //перевіряємо чи є в базі
        
      int keyAddress = storage.foundTag(key->ByteValue,key->ByteSize);
   
      if(keyAddress!=-1)  
      {
        
        //є адреса цього ключа, можна давати доступ 
        //ключ є в базі
        ledSetColor(LED_STATE_GREEN);

        openDoor();
        
        if(!prevAdmin && IsAdmin(key->ByteValue,key->ByteSize)){
            SetAdminMode();
        }
                
      }else{ 
        //ключа нема в базі
        if(prevAdmin){
            // якщо в режимі адмін
            // зберігаємо ключ
            AddNewCard(key->ByteValue,key->ByteSize);
             
        }else{
            //ключа нема в базі
            ledSetColor(LED_STATE_RED);  
        }
        
      }

      //   ключ піднесли для зчитування 
      // зберігаємо якщо нема жодного в базі
      if(storage.savedTagsCount()==0){
        storage.saveTag(key->ByteValue,key->ByteSize);
        }
   } 
   else 
   {
      // ключа нема біля зчитувача
       if(prevAdmin) {
          if(!writeStatus)
          {
              writeStatus = true;
              Serial.print("Чекаю на нову картку ");
              Serial.print(DURATION_LED_ADMIN/1000);
              Serial.println(" секунд");
          }
      }else{
        ledSetColor(LED_STATE_OFF);  
      }
   }

}
