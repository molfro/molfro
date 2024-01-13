     
#include <EEPROM.h>

class Storage
{
  private:
    const int STORE_RESULT_DECLINE = 0; // Отказ
    const int STORE_RESULT_SUCCESS = 1; // Успішно
    const int STORE_RESULT_SAVED = 2;   // Збережено
    const int STORE_RESULT_DELITED = 3;  //Видалено
    const int EE_KEY = 100;
    const int MAX_TAGS = 10;
    int startAddress;
    uint8_t savedTags = 0;
    uint8_t buf[8];
    void init() 
    {
      if (EEPROM.read(startAddress) != EE_KEY) 
      {
        for (uint16_t i=0; i < EEPROM.length(); i++) 
        {
          EEPROM.update(i, 0x00);  // спустошуєм EEPROM       
          EEPROM.update(startAddress, EE_KEY); // пишем байт ключ
        } 
        else 
        {       
          savedTags = EEPROM.read(startAddress + 1); // Читаємо кількість міток        
          bool compareUIDs(uint8_t *in1, uint8_t *in2, uint8_t size); 
        } 
      } 
        for (uint8_t i= 0; i < size; i++) 
        {
          if  (in1[i] != in2[i])
          {
            return false;
          }  
        return true;
        }
    }     
  public:
    Storage(int startAddress)
    {
      this->startAddress = startAddress;
      init();
    }
    int foundTag(byte *tag, uint8_t size)
    {
      uint8_t buf[8]; // буфер для міток з EE 
      uint16_t address; // адреса
        for (uint8_t i= 0; i < savedTags; i++)          // пошук по всих мітках
        { 
          address = (i * 8) + startAddress + 2;       // зчитуємо адресу
          EEPROM.get(address, buf);                    // читаємо мітку
          if (compareUIDs(tag, buf, size))  // порівнюємо
          {
            return address; 
          }
        }  
        return -1;  // Якщо не знайдено - мінус 1 
        digitalWrite("мітка не присутня в базі");   
    } 
    int saveTag(byte *tag, uint8_t size)
    {
      int16_t tagAddr = foundTag(tag, size);      
      if(tagAddr<0)
      {
        uint16_t newTagAddr = (savedTags * 8) + startAddress +2;
        for (uint16_t i  = 0; i < size; i++) 
          EEPROM.update(i + newTagAddr, tag[i]);  
          EEPROM.update(startAddress + 1, ++savedTags); // вписеємо норву мітку 
          return STORE_RESULT_SAVED; 
      } 
      else 
      {
        return STORE_RESULT_DECLINE; 
      }
    } 
    int deleteTag(uint8_t *tag, uint8_t size)
    {
      int16_t tagAddr = foundTag(tag, size);    // пошук мітки та считуєм адресу
      uint16_t newTagAddr = (savedTags * 8) + startAddress +2;
      if  (tagAddr >= 0)  // Мітку знайдено і видаляємо
      {                      
        for (uint8_t i = 0; i < 8; i++)       // по байтам
        { 
          EEPROM.update(tagAddr +i, 0x00);         // зтираємо мітку
          EEPROM.read((newTagAddr - 8) +i); //  переміщення мітки 
          EEPROM.update((newTagAddr - 8) +i, 0x00); // зтираємо кінець
          EEPROM.update(startAddress + 1, --savedTags); //зменшуємо кількість міток 
          return STORE_RESULT_DELITED;    // індикація
        }
      return STORE_RESULT_DECLINE;
      }  
    }
  }
};
 