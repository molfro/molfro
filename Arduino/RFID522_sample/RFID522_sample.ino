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



GTimer adminWasHere(MS);
Storage storage(EE_START_ADDR);

//зберігаємо флаг ( адмін притуляв свою карточку )
bool prevAdmin = false;
bool writeStatus = false;
/*
bool btnpinValue = digitalRead(6);
  digitalRead(BTN_PIN);
*/
void ledSetColor(int state)
{
    switch(state)
    {
    case  LED_STATE_OFF:
      digitalWrite(RED_LED_PIN,LOW); 
      digitalWrite(GREEN_LED_PIN,LOW);
      digitalWrite(BLUE_LED_PIN,LOW);
      break;
    case  LED_STATE_GREEN:
      digitalWrite(RED_LED_PIN,LOW); 
      digitalWrite(GREEN_LED_PIN,HIGH);
      digitalWrite(BLUE_LED_PIN,LOW);
      break;
    case  LED_STATE_RED:
      digitalWrite(GREEN_LED_PIN,LOW);
      digitalWrite(RED_LED_PIN,HIGH);
      digitalWrite(BLUE_LED_PIN,LOW);
      break;
    case LED_STATE_BLUE:
      digitalWrite(BLUE_LED_PIN,HIGH);
      digitalWrite(RED_LED_PIN,LOW);
      break;
    case LED_STATE_VIOLET:
      digitalWrite(BLUE_LED_PIN,HIGH);
      digitalWrite(RED_LED_PIN,HIGH);
      digitalWrite(GREEN_LED_PIN,LOW);
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
  
  adminWasHere.setInterval(10000);
  Serial.print("Saved keys count ");
  Serial.println(storage.savedTagsCount());
  delay(500);
    }


void uidPrint(byte *uidBite, int uidSize)
   {
  for (byte i = 0; i < uidSize; i++) 
      {
     Serial.print(uidBite[i] < 0x10 ? " 0" : " ");
     Serial.print(uidBite[i], HEX);
      }
   }

bool IsAdmin(byte *uidBite, int uidSize)
    {
 return true; 
    }
void openDoor()
       {            // тут вставити код відкривання дверей
   
   /*digitalWrite(DOOR_PIN, HIGH);
   digitalWrite(BUZZER_PIN, HIGH);*/
  Serial.println("відкриваємо двері");
      
       } 

void loop() 
{
  
  RFIDKey *key = ReadRFIDKey();  //uidPrint(mfrc522.uid.uidByte,mfrc522.uid.size)
  
  if(key!=NULL)       //ключ піднесли
       {
                     //перевіряємо чи є в базі
        
      int keyAddress = storage.foundTag(key->ByteValue,key->ByteSize);
   
      if(keyAddress!=-1)  
      {
        Serial.println("keyAddress");
        //є адреса цього ключа, можна давати доступ 
        //ключ є в базі
        ledSetColor(LED_STATE_GREEN);
        
        openDoor();
        
        if(!prevAdmin && IsAdmin(key->ByteValue,key->ByteSize)){
            prevAdmin = true;
            ledSetColor(LED_STATE_BLUE);
            delay(1000);
        }
                
      }else{ 
        //ключа нема в базі
        if(prevAdmin){
            // якщо в режимі адмін
            // зберігаємо ключ
            storage.saveTag(key->ByteValue,key->ByteSize);
            Serial.print("картка збережена!");
            writeStatus=false;
            prevAdmin = false;
            ledSetColor(LED_STATE_VIOLET); 
            delay(1500);
        }else{
            //ключа нема в базі
            ledSetColor(LED_STATE_RED);  
        }
        
        

        
      }
      /*
      uidPrint(key->ByteValue,key->ByteSize);
      Serial.println(" keyAddress");
      delay(500);
      /**/
      // кінеці ключ піднесли

      if(storage.savedTagsCount()==0){
        storage.saveTag(key->ByteValue,key->ByteSize);
        }
   } 
   else 
   {
      // ключа нема в базі
       if(prevAdmin) {
          if(!writeStatus)
          {
              writeStatus = true;
              adminWasHere.reset();
              Serial.print("чекаю на нову картку ");
              Serial.println(adminWasHere.isEnabled());
           }
      }else{
        ledSetColor(LED_STATE_OFF);  

        
      }
    
      
   }


  if ( adminWasHere.isReady())
  {
    Serial.println("timer tick");
    if(prevAdmin){
        Serial.println("не встиг");   
        //adminWasHere.stop();
        writeStatus = false;
        prevAdmin = false;
    }
  }
}
