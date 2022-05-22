#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9 // Пін RST MFRC522
#define CS_PIN         10 // Пін SDA MFRC522




class RFIDKey{
  public:
    RFIDKey(byte *bytes, int bSize){
      ByteValue = bytes;
      ByteSize = bSize;
      };
    byte *ByteValue;
    int ByteSize; 
    bool HasValue;
  };

MFRC522 mfrc522(CS_PIN, RST_PIN); //ініцюємо обєкт

//визви мене в Setup();
void ReadRFIDKeySetup(){
   mfrc522.PCD_Init();   // Iniцiatо MFRC522
  mfrc522.PCD_DumpVersionToSerial();
  }

//викличи мене коли хочеш прочитати ключ
RFIDKey *ReadRFIDKey(){
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return NULL;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return NULL;
  }

  RFIDKey *key(mfrc522.uid.uidByte,mfrc522.uid.size);
  key->ByteValue = mfrc522.uid.uidByte;
  key->ByteSize = mfrc522.uid.size;

  return key;
  }
