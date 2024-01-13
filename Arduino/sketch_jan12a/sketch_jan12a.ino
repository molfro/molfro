#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>
//#include "CustomClasses.h"
//#include "storage.h"
#include "GyverTimer.h"
 
#define RED_LED_PIN    4 
#define GREEN_LED_PIN  5 
#define BLUE_LED_PIN   6
#define DOOR_PIN 7 // Пін для підключення реле
#define LED_STATE_OFF  0
#define LED_STATE_GREEN  1
#define LED_STATE_RED  2
#define LED_STATE_BLUE  3
#define LED_STATE_VIOLET  4
#define EE_START_ADDR   5 // Ключ для проверки на перше включення

#define RST_PIN         9 // Пін RST MFRC522
#define SS_PIN         10 // Пін SDA MFRC522
GTimer adminWasHere(MS);
class RFIDKey {
public:
  RFIDKey();  // конструктор
    // інші члени класу
};
//RFIDKey myRFIDKey;  // створення об'єкта з використанням конструктора
uint8_t buf[8]; // буфер для міток з EE 
uint16_t address;  
int16_t keyAddress[8];
uint16_t adminKeyAddress[8] = { 0xF3, 0x86, 0x76, 0x11 };  //0xD3526F0D  // 
int16_t storage(EE_START_ADDR);
MFRC522 rfid(SS_PIN, RST_PIN);
// RFIDKey adminKey(byte *uidBite, int uidSize));
bool prevAdmin = false; //зберігаємо флаг (адмін притуляв свою карточку )
bool writeStatus = false;
bool isAdmin(RFIDKey &currentKeyAddress);

void isAdmin(RFIDKey &currentKeyAddress) {
 Serial.print("Поточний Key: ");
  uidPrint(currentKeyAddress.ByteValue, currentKeAddressy.ByteSize);
  Serial.print("Порівнюється з адміністраторським Key:");
  uidPrint(adminKeAddressy.ByteValue, adminKeyAddress.ByteSize);
 return memcmp(currentKeyAddress.ByteValue, adminKeyAddress.ByteValue, adminKeyAddress.ByteSize) == 0;
}

void ledSetColor(int state)
{
  switch(state)
  {
    case  LED_STATE_OFF:
    digitalWrite(RED_LED_PIN, LOW); 
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, LOW);
    break;
    case  LED_STATE_GREEN:
    digitalWrite(RED_LED_PIN, LOW); 
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(BLUE_LED_PIN, LOW);
    break;
    case  LED_STATE_RED:
    digitalWrite(GREEN_LED_PIN,LOW);
    digitalWrite(RED_LED_PIN,HIGH);
    digitalWrite(BLUE_LED_PIN, LOW);
    break;
    case  LED_STATE_BLUE:
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, HIGH);
    break;
    case LED_STATE_VIOLET:
    digitalWrite(GREEN_LED_PIN,HIGH);
    digitalWrite(RED_LED_PIN,HIGH);
    digitalWrite(BLUE_LED_PIN, HIGH);
    break;    
  }
    
}
void setup() 
{
  Serial.begin(9600); 
  SPI.begin();
  pinMode(3,OUTPUT); 
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,INPUT); 
 // keyAddress = ReadRFIDKey();
  rfid.PCD_Init();	// Init MFRC522
  //digitalWrite(DOOR_PIN, LOW);  // контролюємо реле при старті
  //RFIDKeySetup(); 
  adminWasHere.setInterval(10000);
  //mfrc522.PCD_DumpVersionToSerial();	// Показати деталі пристрою для читання карток PCD - MFRC522
  Serial.println(F("Скануємо PICC, щоб побачити UID, SAK, тип і блоки даних..."));
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
/*
void isAdmin(byte *uidBite, int uidSize) // Означення функції IsAdmin
{ 
  //uidPrint(adminKey.ByteValue, adminKey.ByteSize);
  if (memcmp(uidBite, adminKey.uidBite, uidSize) == 0) //return memcmp(byte *uidBite, int uidSize) == 0;
  {        // uidPrint(currentKey.ByteValue, currentKey.ByteSize);
    Serial.print(" Порівнюється з адміністраторським Key: ");
    return true; 
  } 
   else 
  {
   return false;
  }  
}
*/
void openDoor()
{
  digitalWrite(DOOR_PIN, HIGH);//тут вставити код відкривання дверей
  ledSetColor(LED_STATE_GREEN);
  Serial.println("відкриваємо двері");
} 

void loop()
{  
RFIDKey key = RFIDKey();
  //uidPrint(mfrc522.uid.uidByte,mfrc522.uid.size) 
  //byte adminKey = {0xF3, 0x86, 0x76, 0x11}; // 0xD3526F0D  // 0xF3867611Реальний UID адмін  мітки
  //RFIDKey currentKey(adminKey.ByteValue, sizeof(adminKey.ByteSize));
  if (keyAddress != NULL) //без ключа а потім ключ піднесли
  { 
    int16_t keyAddress = storage.foundTag(keyAddress->uidBite, keyAddress->uidSize);//перевіряємо чи є в базі ,     
      if (keyAddress != -1)  // Ключ знайдено в базі, надаємо доступ
      {         
        ledSetColor(LED_STATE_GREEN);
        openDoor();
        if (keyAddress != -1 && !prevAdmin)
        {                            
          //Serial.print("keyAddress");
          Serial.println("Це адміністратор!");
          prevAdmin = true;          
          writeStatus = true;  
        }
      }
      else
      {  
        //ключа нема в базі
        if (prevAdmin) // якщо в режимі адмін  зберігаємо ключ  
        { 
          adminWasHere.setInterval(10000); 
          storage.saveTag(keyAddress->byte, keyAddress->size); // Зберігаємо новий ключ в базі byte *tag, uint8_t size
          Serial.print("картка збережена!");
          //Serial.println(adminWasHere.isEnabled());
          ledSetColor(LED_STATE_VIOLET);                                                              
        }
         else  //ключа нема в базі
        {
            //adminWasHere.reset();
            //writeStatus = false;
            //prevAdmin = false; 
            ledSetColor(LED_STATE_RED); 
            Serial.println("не встиг"); 

        }        
      } 
    }
    else
    { //ключа немає
      if (prevAdmin) 
      {
        if (!writeStatus && adminWasHere.isEnabled())
           {
            writeStatus = true;
            adminWasHere.reset();
            Serial.print("чекаю на нову картку ");
            Serial.println(adminWasHere.isEnabled());
           }
      }
      else
      {
        ledSetColor(LED_STATE_OFF); 
      } 
  

    if (adminWasHere.isReady())
    {
      Serial.println("timer tick");
      if (prevAdmin)
      {
          Serial.println("не встиг");
          adminWasHere.reset();
          writeStatus = false;
          prevAdmin = false;
      }
    }    
  }
}


