#include <math.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "OneWire.h"
// #include <MemoryFree.h>

/*
  General Setup
*/

// define DEBUG options
// #define __SERIAL_DEBUG__
// #define __SERIAL1_DEBUG__
// #define __CHIP_DEBUG__
// #define __EEPROM_DEBUG__
// #define __ALL_DEBUG__


// define serial commands

const uint8_t getMaxChips        = '1';
const uint8_t showChip           = getMaxChips + 1;
const uint8_t getChipCount       = showChip + 1;
const uint8_t getChipAddress     = getChipCount + 1;
const uint8_t getChipStatus      = getChipAddress + 1;
const uint8_t setSwitchState     = getChipStatus + 1;
const uint8_t getAllStatus       = setSwitchState + 1;
const uint8_t getChipType        = getAllStatus + 1;
const uint8_t getAllChips        = getChipType + 1; // last in this series

const uint8_t getActionArray     = 'A'; // start of new serial command list
const uint8_t updateActionArray  = getActionArray + 1;
const uint8_t getActionStatus    = updateActionArray + 1;
const uint8_t getMaxActions      = getActionStatus + 1;
const uint8_t setActionSwitch    = getMaxActions + 1;
const uint8_t saveToEEPROM       = setActionSwitch + 1;
const uint8_t getEEPROMstatus    = saveToEEPROM + 1;
const uint8_t getNewSensors      = getEEPROMstatus + 1;
const uint8_t masterStop         = getNewSensors + 1;
// end of serial commands

const uint8_t softSerialError  = 'X';
const uint8_t setSwitchON      = 'N';
const uint8_t setSwitchOFF     = 'F';
const uint8_t switchStatusON   = 'N';
const uint8_t switchStatusOFF  = 'F';
const uint8_t tooHotSwitch     = 'H';
const uint8_t tooColdSwitch    = 'C';
const uint8_t noChipPresent    = 0xFF;


const long baudRate = 115200;
const uint8_t waitPin = 8;
const uint8_t waitLED = 13;

char softSerialBuffer[1024], c;
uint8_t cnt = 0;
uint8_t chipSelected;
uint8_t actionSelected;
uint8_t setChipState;
uint8_t *chipAddrPtr;
bool serialMessageReady = FALSE;
bool actionPtrMatch = FALSE;
bool showCelcius = FALSE;

uint32_t timer, timer2;
const uint32_t updateTime = 250;
const uint32_t ramUpdateTime = 10000;

// OneWire Setup;
const uint8_t oneWireAddress = 9; // OneWire Bus Address
const uint8_t chipAddrSize   = 8; // 64bit OneWire Address
const uint8_t ds2406MemWr    = 0x55;
const uint8_t ds2406MemRd    = 0xaa;
const uint8_t ds2406AddLow   = 0x07;
const uint8_t ds2406AddHi    = 0x00;
const uint8_t ds2406PIOAoff  = 0x3f;
const uint8_t ds2406PIOAon   = 0x1f;
const uint8_t ds2406End      = 0xff;
const uint8_t ds18b20ID      = 0x28;
const uint8_t ds2406ID       = 0x12;
const uint8_t dsPIO_A        = 0x20;
const uint8_t dsPIO_B        = 0x40;

const uint8_t maxChips       = 36; // Maximum number of Chips
const uint8_t maxActions     = 12; // Maximum number of Actions

OneWire  ds(oneWireAddress);

const uint32_t tempReadDelay = 125;

typedef struct
{
  uint8_t   chipAddr[chipAddrSize];
  int16_t   chipStatus;
  uint32_t  tempTimer;
}chipStruct;

chipStruct chip[maxChips] =
{
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 },
  { {0,0,0,0,0,0,0,0}, 0, 0 }
};

typedef struct
{
  bool     actionEnabled;
  chipStruct *tempPtr;
  int16_t  tooCold;
  chipStruct *tcPtr;
  uint8_t  tcSwitchLastState;
  uint32_t  tcDelay;
  uint32_t tcMillis;
  int16_t  tooHot;
  chipStruct *thPtr;
  uint8_t  thSwitchLastState;
  uint32_t  thDelay;
  uint32_t thMillis;
}chipActionStruct;

chipActionStruct action[maxActions] =
{
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 },
  { FALSE, NULL, -255, NULL, 'F', 0, 0, 255, NULL, 'F', 0, 0 }
};

uint8_t chipBuffer[12];
uint8_t chipCnt, chipX = 0, actionsCnt = 0;


//EEPROM Stuff
const int   EEPROMsize       = 2048;   // Cortex M4
const int   EEPROMidAddr     = 0x10;   // ID address to verify a previous EEPROM write
const int   EEPROMccAddr     = 0x20;   // number of chips found during findchips()
const int   EEPROMchipAddr   = 0x100;  // start address of array of chip structures
const int   EEPROMactionAddr = 0x400;  // start address of array of action structures
const byte  EEPROMidVal      = 0x55;   // Shows that an EEPROM update has occurred 
bool        eepromReady      = FALSE;
int         eepromSpace, eeResult;

void setup()
{
  int x;
  pinMode(waitPin, OUTPUT);
  pinMode(waitLED, OUTPUT);
  digitalWrite(waitPin, LOW);
  digitalWrite(waitLED, LOW);
  Serial.begin(baudRate);
  
  #if defined (__SERIAL_DEBUG__) || defined (__SERIAL1_DEBUG__) || defined (__CHIP_DEBUG__) ||defined (__EEPROM_DEBUG__) || defined (__ALL_DEBUG__)
 
    delay(3000);
  #endif
  
  Serial.print(F("Serial Debug running at "));
  Serial.print(baudRate);
  Serial.println(F(" baud"));


  eeResult = EEPROM.read(EEPROMidAddr);
  
  #if defined (__EEPROM_DEBUG__) || defined (__ALL_DEBUG__)
   Serial.print(F("eeResult = 0x"));
   Serial.println(eeResult, HEX);
  #endif
  
  if(eeResult != 0x55)
  {
  #if defined (__EEPROM_DEBUG__ )|| defined (__ALL_DEBUG__)
     Serial.println(F("No EEPROM Data"));
  #endif
  
    eepromReady = FALSE;
    findChips();
  }else{

  #if defined (__EEPROM_DEBUG__) || defined (__ALL_DEBUG__)
     Serial.println(F("Getting EEPROM Data"));
  #endif

    chipCnt = EEPROM.read(EEPROMccAddr);
    EEPROM_readAnything(EEPROMchipAddr, chip); // get chip structures from EEPROM
    EEPROM_readAnything(EEPROMactionAddr, action); // get action structures from EEPROM

  #if defined (__EEPROM_DEBUG__) || defined (__ALL_DEBUG__)
     Serial.println(F("EEPROM Data Read Completed"));
  #endif
  
    eepromReady = TRUE;
  }
  
  #if defined (__EEPROM_DEBUG__) || defined (__ALL_DEBUG__)
   Serial.print(sizeof(chipStruct) / sizeof(byte));
   Serial.println(F(" bytes in chip structure"));
   Serial.print(sizeof(chipActionStruct) / sizeof(byte));
   Serial.println(F(" bytes in action structure"));
  #endif  


  Serial1.begin(baudRate);
  Serial.print(F("Serial1 Debug running at "));
  Serial.print(baudRate);
  Serial.println(F(" baud"));
  digitalWrite(waitPin, HIGH);
  digitalWrite(waitLED, HIGH);
  timer = millis();
  timer2 = millis();
}

void loop()
{
  while(Serial1.available())
  { 
     c = Serial1.read();
     if( (c >= 0x20 && c <= 0x7E) ||
         c == 0 ||
         c == 0x0d ||
         c == 0x0a
       )
     {
       if(c == 0 || c == 0x0d || c == 0x0a)
       {
          c = 0; //null terminate the string
         serialMessageReady = TRUE;
       }
       softSerialBuffer[cnt] = c;
  #if defined (__SERIAL_DEBUG__) || defined (__ALL_DEBUG__)
       Serial.write(c);
  #endif
       cnt++;
  #if defined (__SERIAL_DEBUG__) || defined (__ALL_DEBUG__)
       delay(2);
  #endif
     }
   }
  
  if(cnt > 0 && serialMessageReady == TRUE)
  {
  #if defined (__SERIAL_DEBUG__) || defined (__ALL_DEBUG__)
    Serial.println();
  #endif
    softSerialProcess();
  }
  
//  if(timer > (millis() + 5000)) // in case of rollover
//  {
//    timer = millis();
//  }
  
//  if(millis() > (timer + updateTime))
//  {
    updateChipStatus(chipX);
    chipX++;
    if(chipX >= maxChips){chipX = 0;}
    
    updateActions(actionsCnt);    
    actionsCnt++;
    if(actionsCnt >= maxActions){actionsCnt = 0;}
  
//    timer = millis();
//  }
/*  
  if(millis() > (timer2 + ramUpdateTime))
  {
    Serial.print(freeMemory());
    Serial.println(F(" Available"));
    timer2 = millis();
  }
*/
}


void findChips()
{
 int cntx = 0;
 
  while (ds.search(chip[cntx].chipAddr))
  {

  #if defined (__CHIP_DEBUG__) || defined (__ALL_DEBUG__)
    
     Serial.print(F("Chip "));
     Serial.print(cntx);
     Serial.print(F(" = {"));
    
    for( int i = 0; i < chipAddrSize; i++)
    {
      if(chip[cntx].chipAddr[i]>=0 && chip[cntx].chipAddr[i]<10)
      {
        Serial.print(F("0x0"));
      }else{
        Serial.print(F("0x"));
      }
      Serial.print(chip[cntx].chipAddr[i], HEX);
      if(i < 7){Serial.print(F(","));}
    }
    Serial.println(F("}"));
  #endif    
    cntx++;
    delay(750);
  }

  #if defined (__CHIP_DEBUG__) || defined (__ALL_DEBUG__)
    Serial.print(cntx);
    Serial.print(F(" Sensor"));
    if(cntx == 1)
    {
      Serial.println(F(" Detected"));
    }else{
       Serial.println(F("s Detected"));
    }
  #endif
  
  ds.reset_search();
  chipCnt = cntx;
  if(cntx < maxChips)
  {
    for(;cntx<maxChips;cntx++)
    {
      for(int y=0;y<chipAddrSize;y++)
      {
        chip[cntx].chipAddr[y]=0;
      }
    }
  }
}

void softSerialProcess()
{
  int x, ssBufOffset;
  char *result = NULL, *addrResult = NULL;
  char delim[] = " ", addrDelim[] = ",";
  int16_t actionEnableTemp;
  int16_t resultCnt = 0, addrResultCnt = 0, actionArray, actionSection;
  uint32_t actionDelayVal;
  uint8_t addrVal[chipAddrSize], addrMatchCnt, chipAddrCnt;
  
  switch(softSerialBuffer[0])
  {
    
    case getMaxChips: // "1"
    {
      Serial1.print(maxChips);
      Serial1.print(F("\n"));
    }
    break;
    
    case showChip: // "2"
    {
      x = atoi((char *) &softSerialBuffer[1]);
      showChipInfo(x);
    } 
    break;
    
    case getAllChips: // "9"
    {
      for(x = 0; x < maxChips; x++)
      {
        showChipInfo(x);
      }
    }
    break;
    
    case getChipCount: // "3"
    {
      Serial1.print(chipCnt);
      Serial1.print(F("\n"));
    }
    break;
    
    case getChipAddress: // "4"
    {
      x = atoi((char *) &softSerialBuffer[1]);
      showChipAddress((uint8_t *) &chip[x].chipAddr);
      break;
    }
    
    case getAllStatus: // "7"
    {
      for(int x = 0; x < maxChips; x++)
      {
        switch (chip[x].chipAddr[0])
        {
          case ds18b20ID:
          {
            Serial1.print((int) chip[x].chipStatus);
          }
          break;
          
          case ds2406ID:
          {
              Serial1.print((char) chip[x].chipStatus);
          }
          break;
          
          default:
          {
            Serial1.print(F("Z"));
          }
          break;
        }
        if(x < maxChips -1)
        {
          Serial1.print(F(","));
        }
      }
    Serial1.print(F("\n"));
    }
    break;

    case getChipType: // "8"
    {
      x = atoi((char *) &softSerialBuffer[1]);
      switch(chip[x].chipAddr[0])
      {
        case ds18b20ID:
        {
          Serial1.print(F("T"));
        }
        break;
 
         case ds2406ID:
        {
          Serial1.print(F("S"));
        }
        break;

        default:
        {
          Serial1.print(F("Z"));
        }
        break;
     }
    }
    Serial1.print(F("\n"));
    break;

    case setSwitchState: // "6"
    {
      chipSelected = atoi((char *) &softSerialBuffer[1]);
      if(chipSelected >= 10)
      {
        ssBufOffset = 3;
      }else{
        ssBufOffset = 2;
      }
      if(softSerialBuffer[ssBufOffset] == setSwitchON)
      {
        setChipState = ds2406PIOAon;
      }else{
        setChipState = ds2406PIOAoff;
      }
      setSwitch(chipSelected, setChipState);
      updateChipStatus(chipSelected);
      Serial1.print((char) chip[chipSelected].chipStatus);
      Serial1.print(F("\0"));
    }
    break;
    
    case getChipStatus: // "5"
    {
      x = atoi((char *) &softSerialBuffer[1]);
      updateChipStatus(x);
      switch(chip[x].chipAddr[0])
      {
        case ds18b20ID:
        {
          Serial1.print( (int) chip[x].chipStatus);
        }
        break;

        case ds2406ID:
        {
          Serial1.print( (char) chip[x].chipStatus);
        }
        break;

        default:
        {
          Serial1.print(F("Z"));
        }
        break;
      }
    }

    case getActionArray: // "A"
    {
      x = atoi((char *) &softSerialBuffer[1]);
      Serial1.print(action[x].actionEnabled);
      Serial1.print(F(" "));
      showChipAddress((uint8_t *) &action[x].tempPtr->chipAddr);
      Serial1.print(F(" "));
      Serial1.print(action[x].tooCold);
      Serial1.print(F(" "));
      if(action[x].tcPtr == NULL)
      {
        Serial1.print(F("0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00"));
      }else{
        showChipAddress((uint8_t *) &action[x].tcPtr->chipAddr);
      }
      Serial1.print(F(" "));
      Serial1.print((char) action[x].tcSwitchLastState);
      Serial1.print(F(" "));
      Serial1.print((action[x].tcDelay / 1000));
      Serial1.print(F(" "));
      Serial1.print(action[x].tcMillis);
      Serial1.print(F(" "));
      Serial1.print(action[x].tooHot);
      Serial1.print(F(" "));
      if(action[x].thPtr == NULL)
      {
        Serial1.print(F("0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00"));
      }else{
        showChipAddress((uint8_t *) &action[x].thPtr->chipAddr);
      }
      Serial1.print(F(" "));
      Serial1.print((char) action[x].thSwitchLastState);
      Serial1.print(F(" "));
      Serial1.print((action[x].thDelay / 1000));
      Serial1.print(F(" "));
      Serial1.print(action[x].thMillis);
      Serial1.print(F("\0"));
      break;
    }
    
    case updateActionArray: // "B"
    {
      
      result = strtok( softSerialBuffer, delim );

      while(1)
      {
        result = strtok( NULL, delim );
        if(result == NULL){break;}
        switch (resultCnt)
        {
          case 0: // action
          {
            actionArray = atoi(result);
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
             Serial.print(F("actionArray = "));
             Serial.println(actionArray);
  #endif
            break;
          }
          case 1:
          {
            actionSection = atoi(result);
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
             Serial.print(F("actionSection = "));
             Serial.println(actionSection);
  #endif
            break;
          }
          
          case 2:
          {
            actionEnableTemp = atoi(result);
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
             Serial.print(F("actionEnableTemp = "));
             Serial.println(actionEnableTemp);
             Serial.print(F("action["));
             Serial.print(actionArray);
             Serial.print(F("]"));
  #endif            
            switch (actionSection)
            {
              case 1:
              {
                if(actionEnableTemp == 1)
                {
                  action[actionArray].actionEnabled = TRUE;
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                   Serial.println(F(".actionEnabled is Enabled"));
  #endif
                }else{
                  action[actionArray].actionEnabled = FALSE;
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                 // Serial.println(F(".actionEnabled is Disabled"));
  #endif
                }
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                 Serial.print(F("action["));
                 Serial.print(actionArray);
                 Serial.print(F("].actionEnabled = "));
                 Serial.println(action[actionArray].actionEnabled);
  #endif
                break;
              }
              
              case 2:
              case 3:
              {
                if(actionSection == 2)
                {
                  action[actionArray].tooCold = actionEnableTemp;
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                  Serial.print(F(".tooCold is set to "));
                  Serial.println(actionEnableTemp);
  #endif
                }else if( actionSection == 3){
                  action[actionArray].tooHot = actionEnableTemp;
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                   Serial.print(F(".tooHot is set to "));
                   Serial.println(actionEnableTemp);
  #endif
                }
                break;
              }
            }
            break;
          }
          
          case 3:
          {
            if(actionSection != 1)
            {
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
               Serial.print(F("result = "));
               Serial.println(result);
  #endif
              actionDelayVal = ((uint32_t) atoi(result));
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
               Serial.print(F("actionDelayVal = "));
               Serial.println(actionDelayVal);
  #endif
              actionDelayVal *= 1000;
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
               Serial.print(F("actionDelayVal * 1000 = "));
               Serial.println(actionDelayVal);
               Serial.print(F("action["));
               Serial.print(actionArray);
               Serial.print(F("]."));
  #endif
              if(actionSection == 2)
              {
                action[actionArray].tcDelay = actionDelayVal;
                if(actionDelayVal > 0)
                {
                  action[actionArray].tcMillis = millis();
                }
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                 Serial.print(F("tcDelay = "));
                 Serial.println((actionDelayVal / 1000));
  #endif
              }else if (actionSection == 3){
                action[actionArray].thDelay = actionDelayVal;
                if(actionDelayVal > 0)
                {
                  action[actionArray].thMillis = millis();
                }
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                 Serial.print(F("thDelay = "));
                 Serial.println(actionDelayVal / 1000);
  #endif
              }
            }
            break;
          }
          
          case 4:
          {
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
             Serial.println(result);
  #endif
            addrResult = strtok( result, addrDelim );
            while(addrResult != NULL)
            {
              addrVal[addrResultCnt] = (uint8_t) strtol(addrResult, NULL, 16);
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
               Serial.print(F(" "));
               Serial.print(addrVal[addrResultCnt], HEX);
  #endif
              addrResultCnt++;
              addrResult = strtok( NULL, addrDelim );
            }
            for(addrMatchCnt = 0, chipAddrCnt = 0; addrMatchCnt < chipAddrSize; addrMatchCnt++)
            {
              if(addrVal[addrMatchCnt] != chip[chipAddrCnt].chipAddr[addrMatchCnt])
              {
                addrMatchCnt = 0;
                chipAddrCnt++;
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                 Serial.print(F("chipAddrCnt = "));
                 Serial.println(chipAddrCnt);
                 Serial.print(F("chipCnt = "));
                 Serial.println(chipCnt);
  #endif
                continue;
              }
            }
            if(chipAddrCnt <= chipCnt)
            {
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
               Serial.print(F("MATCH!! - "));
  #endif
              actionPtrMatch = TRUE;
            }else{
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
               Serial.print(F("NO MATCH!! - "));
  #endif
              actionPtrMatch = FALSE;
            }
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
             Serial.println(chipAddrCnt);
  #endif
            switch (actionSection)
            {
              case 1:
              {
                if(actionPtrMatch == TRUE)
                {
                  action[actionArray].tempPtr = &chip[chipAddrCnt];
                }else{
                  action[actionArray].tempPtr = NULL;
                }
                break;
              }
              case 2:
              {
                if(actionPtrMatch == TRUE)
                {
                  action[actionArray].tcPtr = &chip[chipAddrCnt];
                }else{
                  action[actionArray].tcPtr = NULL;
                }
                break;
              }
              case 3:
              {
                if(actionPtrMatch == TRUE)
                {
                  action[actionArray].thPtr = &chip[chipAddrCnt];
                }else{
                  action[actionArray].thPtr = NULL;
                }
                break;
              }
            }
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
              //Serial.print(addrVal, HEX);
              Serial.print(F(", "));
  #endif
            break;
          }
          break;
        }
        resultCnt++;
      }
    }
      
    case getActionStatus: // "C"
    {
      getAllActionStatus();
      break;
    }
    
    case getMaxActions: // "D"
    {
      Serial1.print(maxActions);
      Serial1.print(F("\n"));
    }
    break; 

    case setActionSwitch: // "E"
    {
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
       Serial.println(F("case: setActionSwitch"));
       Serial.println(softSerialBuffer);
  #endif
      actionSelected = atoi((char *) &softSerialBuffer[1]);
      if(actionSelected >= 10)
      {
        ssBufOffset = 3;
      }else{
        ssBufOffset = 2;
      }
      
      if(softSerialBuffer[ssBufOffset+1] == setSwitchON)
      {
        setChipState = ds2406PIOAon;
      }else{
        setChipState = ds2406PIOAoff;
      }
      
      switch (softSerialBuffer[ssBufOffset])
      {
        case tooColdSwitch:
        {
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
           Serial.println(F("Setting too Cold Switch"));
  #endif
          actionSwitchSet((uint8_t *) action[actionSelected].tcPtr->chipAddr, setChipState);
          if(setChipState == ds2406PIOAoff && action[actionSelected].tcDelay > 0)
          {
            action[actionSelected].tcMillis = millis();
          }
          Serial1.print((char) action[actionSelected].tcPtr->chipStatus);
          break;
        }
        
        case tooHotSwitch:
        {
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
           Serial.println(F("Setting too Hot Switch"));
  #endif
          actionSwitchSet((uint8_t *) action[actionSelected].thPtr->chipAddr, setChipState);
          if(setChipState == ds2406PIOAoff && action[actionSelected].thDelay > 0)
          {
            action[actionSelected].thMillis = millis();
          }
          Serial1.print((char) action[actionSelected].thPtr->chipStatus);
          break;
        }
        
        default:
        {
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
           Serial.println(F("Houston, We HAve A Switch Problem"));
  #endif        
          break;
        }
      }
      Serial1.print(F("\0"));
      break;
    }

    case saveToEEPROM: // "F"
    {
      initEEPROM();
      break;
    }
    
    case getEEPROMstatus: // "G"
    {
      if(eepromReady == FALSE)
      {
        Serial1.print(F("FALSE"));
      }else
      {
        Serial1.print(F("TRUE"));
      }
      break;
    }
    
    case getNewSensors: // "H"
    {
      // turn off all switches
      for(x=0; x<maxChips; x++)
      {
        setSwitch(x, ds2406PIOAoff);
      }
      
      // disable and clear actions
      for(x=0; x<maxActions; x++)
      {
        action[x].actionEnabled = FALSE;
        action[x].tempPtr = NULL;
        action[x].tcPtr = NULL;
        action[x].thPtr = NULL;
      }
      // find new chips
      findChips();
      //EEPROM.write(EEPROMidAddr, 0);
      break;
    }
    
    case masterStop: //"I"
    {
      // turn off all switches
      for(x=0; x<maxChips; x++)
      {
        setSwitch(x, ds2406PIOAoff);
      }
      for(x=0; x<maxActions; x++)
      {
        action[x].actionEnabled = FALSE;
      }
      break;
    }
  }
    
    Serial1.print(F("\n"));
//    break;
  
  softSerialBuffer[0]=0x00;
  cnt = 0;
  serialMessageReady = FALSE;
}

void initEEPROM(void)
{
  int x, y, address, value;
  
  digitalWrite(waitPin, LOW);
  digitalWrite(waitLED, LOW);
  // Serial.println(F("Clearing EEPROM Space"));
/*  
  for(int x = 0; x < EEPROMsize; x++)
  {
    EEPROM.write(x, 0xff);
  }
*/
  // Serial.println(F("EEPROM Space Cleared"));
  #if defined (__EEPROM_DEBUG__) || defined (__ALL_DEBUG__)
   Serial.println(F("Writing EEPROM Data"));
  #endif

  EEPROM.write(EEPROMccAddr, chipCnt);
  EEPROM.write(EEPROMidAddr, EEPROMidVal);
  eeResult = EEPROM_writeAnything(EEPROMchipAddr, chip);

  #if defined (__EEPROM_DEBUG__) || defined (__ALL_DEBUG__)
   Serial.print(eeResult);
   Serial.println(F(" bytes written to chip EEPROM"));
  #endif
  eeResult = EEPROM_writeAnything(EEPROMactionAddr, action);

  #if defined (__EEPROM_DEBUG__) || defined (__ALL_DEBUG__)
   Serial.print(eeResult);
   Serial.println(F(" bytes written to action EEPROM"));
   Serial.println(F("EEPROM Data Write Completed"));
  #endif
  eeResult = EEPROM.read(EEPROMidAddr);

  #if defined (__EEPROM_DEBUG__) || defined (__ALL_DEBUG__)
   Serial.print(F("EEPROMidAddr = 0x"));
   Serial.println(eeResult);
 
  for(y = 0, address = EEPROMchipAddr ;  y < maxChips; y++)
  {
    // read a byte from the current address of the EEPROM
     Serial.print(F("0x"));
     Serial.print(address, HEX);
     Serial.print(":\t");
    for(x = 0; x < (sizeof(chipStruct) / sizeof(byte)); x++, address++)
    {
      value = EEPROM.read(address);
      if(value >= 0 && value <= 0x0f)
      {
        Serial.print("0x0");
      }else{
        Serial.print("0x");
      }
      Serial.print(value, HEX);
      Serial.print(F(" "));
    }
    Serial.println();
  }
  
  for(y = 0, address = EEPROMactionAddr ; y < maxActions; y++)
  {
    // read a byte from the current address of the EEPROM
    Serial.print(F("0x"));
    Serial.print(address, HEX);
    Serial.print(":\t");
    for(x = 0; x < (sizeof(chipActionStruct) / sizeof(byte)); x++, address++)
    {
      value = EEPROM.read(address);
      if(value >=0 && value <= 0x0f)
      {
        Serial.print("0x0");
      }else{
        Serial.print("0x");
      }
      Serial.print(value, HEX);
      Serial.print(F(" "));
    }
    Serial.println();
  }
  #endif
  digitalWrite(waitPin, HIGH);
  digitalWrite(waitLED, HIGH);
  eepromReady = TRUE;
}

void getAllActionStatus(void)
{
  uint8_t x;
  
  // x = atoi((char *) &softSerialBuffer[1]);
  for( x = 0; x < maxActions; x++ )
  {
    Serial1.print((int) action[x].actionEnabled);
    Serial1.print(F(","));
    if(action[x].tempPtr == NULL)
    {
      Serial1.print((int) 255);
    }else{
      Serial1.print((int) action[x].tempPtr->chipStatus);
    }
    Serial1.print(F(","));
    if(action[x].tcPtr == NULL)
    {
      Serial1.print((char) noChipPresent);
    }else{
      Serial1.print((char) action[x].tcPtr->chipStatus);
    }
    Serial1.print(F(","));
    if(action[x].thPtr == NULL)
    {
      Serial1.print((char) noChipPresent);
    }else{
      Serial1.print((char) action[x].thPtr->chipStatus);
    }
    Serial1.print(F(","));
    Serial1.print((int) action[x].tooCold);
    Serial1.print(F(","));
    Serial1.print((int) action[x].tooHot);
    if( x < (maxActions - 1) )
    {
      Serial1.print(F(";"));
    }
  }
  Serial1.print(F("\n"));
}

void actionSwitchSet(uint8_t* array, uint8_t setChipState)
{
   uint8_t addrMatchCnt, chipAddrCnt;
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
   Serial.println(F("actionSwitchSet"));
  #endif
  for(addrMatchCnt = 0, chipAddrCnt = 0; addrMatchCnt < chipAddrSize; addrMatchCnt++)
  {
    if(array[addrMatchCnt] != chip[chipAddrCnt].chipAddr[addrMatchCnt])
    {
      addrMatchCnt = 0;
      chipAddrCnt++;
      
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
       Serial.println(chipAddrCnt);
  #endif
  
      continue;
    }
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
     Serial.print(array[addrMatchCnt], HEX);
     Serial.print(F(","));
  #endif
  }
  if(chipAddrCnt <= chipCnt)
  {

  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
     Serial.print(F("MATCH!! - "));
  #endif

    actionPtrMatch = TRUE;
  }else{

  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
     Serial.print(F("NO MATCH!! - "));
  #endif
  
    actionPtrMatch = FALSE;
  }

  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
   Serial.println(chipAddrCnt);
  #endif
  
  if(actionPtrMatch == TRUE)
  {
    setSwitch(chipAddrCnt, setChipState);
  }
}

void showChipAddress( uint8_t* array)
{
  for( int i = 0; i < chipAddrSize; i++)
  {
    Serial1.print(F("0x"));
    if(array[i]>=0x00 && array[i]<=0x0F)
    {
      Serial1.print(F("0"));
    }
    Serial1.print(array[i], HEX);
    if(i < 7){Serial1.print(F(","));}
  }
    
}

void showChipInfo(int x)
{
  showChipAddress((uint8_t *) &chip[x].chipAddr);
  Serial1.print(F(" "));
  if(chip[x].chipAddr[0] == 0x12)
  {
    Serial1.print((char) chip[x].chipStatus);
  }else{
    Serial1.print((int) chip[x].chipStatus);
  }
  Serial1.print(F(" \n"));
}

void setSwitch(uint8_t x, uint8_t setChipState)
{
  if(chip[x].chipAddr[0] == 0x12)
  {
    ds.reset();
    ds.select(chip[x].chipAddr);
    ds.write(ds2406MemWr);
    ds.write(ds2406AddLow);
    ds.write(ds2406AddHi);
    ds.write(setChipState);
    for ( int i = 0; i < 6; i++)
    {
      chipBuffer[i] = ds.read();
    }
    ds.write(ds2406End);
    ds.reset();
    updateChipStatus(x);
  }
}

void updateChipStatus(int x)
{
  digitalWrite(waitPin, LOW);
  digitalWrite(waitLED, LOW);
  switch(chip[x].chipAddr[0])
  {
    case ds18b20ID:
    {
      if(chip[x].tempTimer == 0)
      {
        ds.reset();
        ds.select(chip[x].chipAddr);
        ds.write(0x4E); // write to scratchpad;
        ds.write(0x00); // low alarm
        ds.write(0x00); // high alarm
        ds.write(0x1F); // configuration register - 9 bit accuracy (0.5deg C)
        ds.reset();
        ds.select(chip[x].chipAddr);
        ds.write(0x44,1);         // start conversion, with parasite power on at the end
        chip[x].tempTimer = millis();
      }
/*    
      delay(125);     // for 9 bit accuracy
      // we might do a ds.depower() here, but the reset will take care of it.
*/    
      if((chip[x].tempTimer != 0) && (millis() >= chip[x].tempTimer + tempReadDelay))
      {
        ds.reset();
        ds.select(chip[x].chipAddr);    
        ds.write(0xBE);         // Read Scratchpad
  
        for (int i = 0; i < 4; i++) 
        {
          chipBuffer[i] = ds.read();
        }
  
      // convert the data to actual temperature
        unsigned int raw = (chipBuffer[1] << 8) | chipBuffer[0];
        if( showCelcius == TRUE)
        {
          chip[x].chipStatus = (int) ((float)raw / 16.0);
        }else{
          chip[x].chipStatus = (int) ((((float)raw / 16.0) * 1.8) + 31.0);
        }
        chip[x].tempTimer = 0;
      }
    }
    break;
    
    case ds2406ID:
    {
      ds.reset();
      ds.select(chip[x].chipAddr);
      ds.write(ds2406MemRd);
      ds.write(0x0); //2406 Addr Low
      ds.write(0x0); //2406 Addr Hgh
      for(int i = 0; i <  10; i++)
      {
        chipBuffer[i] = ds.read();
      }
      ds.reset();
      if(chipBuffer[7] & dsPIO_A)
      {
        chip[x].chipStatus = switchStatusOFF;
      }else{
        chip[x].chipStatus = switchStatusON;
      }
    }
    break;
    
    default:
    {
      chip[x].chipStatus = noChipPresent;
    }
  break; 
  }
  digitalWrite(waitPin, HIGH);
  digitalWrite(waitLED, HIGH);
}

void updateActions(uint8_t x)
{

  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
   Serial.print(F("updating Action: "));
   Serial.print(x);
  #endif
  if(action[x].actionEnabled == TRUE)
  {
    if(action[x].tempPtr->chipStatus <= action[x].tooCold &&
       action[x].tcPtr->chipStatus == switchStatusOFF) // too cold
    {
      if(action[x].tcDelay == 0 || millis() > (action[x].tcMillis + action[x].tcDelay))
      {
        actionSwitchSet((uint8_t *) &action[x].tcPtr->chipAddr, ds2406PIOAon);
        
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
           Serial.println(F(" - TOO COLD"));
  #endif        
      }
    }else if(action[x].tempPtr->chipStatus > action[x].tooCold &&
             action[x].tcPtr->chipStatus == switchStatusON){
               actionSwitchSet((uint8_t *) &action[x].tcPtr->chipAddr, ds2406PIOAoff);
               action[x].tcMillis = millis();
               
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                  Serial.println(F(" - NOT TOO COLD"));
  #endif
  
    }

    if(action[x].tempPtr->chipStatus >= action[x].tooHot &&
       action[x].thPtr->chipStatus == switchStatusOFF) //too hot
    {
      if(action[x].thDelay == 0 || millis() > (action[x].thMillis + action[x].thDelay))
      {
        actionSwitchSet((uint8_t *) &action[x].thPtr->chipAddr, ds2406PIOAon);
        
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
           Serial.println(F(" - TOO HOT"));
  #endif
  
      }
    }else if(action[x].tempPtr->chipStatus < action[x].tooHot &&
             action[x].thPtr->chipStatus == switchStatusON){
               actionSwitchSet((uint8_t *) &action[x].thPtr->chipAddr, ds2406PIOAoff);
               action[x].thMillis = millis();
               
  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
                  Serial.println(F(" - NOT TOO HOT"));
  #endif
  
    }
  }else{

  #if defined (__SERIAL1_DEBUG__) || defined (__ALL_DEBUG__)
     Serial.println(F(" - not enabled"));
  #endif 
  }
}

