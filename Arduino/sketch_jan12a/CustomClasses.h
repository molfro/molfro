#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9 // Пін RST MFRC522
#define SS_PIN         10 // Пін SDA MFRC522

class RFIDKey
{
  public:
    RFIDKey(byte *bytes, int bSize)
    {
      ByteValue = bytes;
      ByteSize = bSize;
    };
    byte *ByteValue;
    int ByteSize; 
    bool HasValue;
};

MFRC522 mfrc522(SS_PIN, RST_PIN); //ініцюємо обєкт
 
void ReadRFIDKeySetup() //визви мене в Setup();
{
  mfrc522.PCD_Init();   // Iniцiatо MFRC522
  mfrc522.PCD_DumpVersionToSerial();
{  

RFIDKey *ReadRFIDKey() //викличи мене коли хочеш прочитати ключ  
{  
  if (! mfrc522.PICC_IsNewCardPresent()) // зчитали прикладену картку
  {
    return NULL;
  } 
  if (! mfrc522.PICC_ReadCardSerial())  // Виберіть одну з карток
  {
    return NULL;
  }
  RFIDKey *key = new RFIDKey(mfrc522.uid.uidByte, mfrc522.uid.size);
  key->ByteValue = mfrc522.uid.uidByte;
  key->ByteSize = mfrc522.uid.size;
  return key;
}
 