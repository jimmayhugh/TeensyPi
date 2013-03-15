/********************

TeensyPi.ino

Version 0.0.3
Last Modified 03/15/2013
By Jim Mayhugh


*********************/
#include <PID_v1.h>
#include <math.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "OneWire.h"
#include <errno.h>

// #include <MemoryFree.h>

/*
  General Setup
*/

const char* versionStr = "TeensyPi Version 0.0.3";

uint8_t setDebug = 0x0;

const uint8_t allDebug      = 0x01; 
const uint8_t pidDebug      = 0x02; 
const uint8_t eepromDebug   = 0x04; 
const uint8_t chipDebug     = 0x08; 
const uint8_t serial1Debug  = 0x10; 
const uint8_t serialDebug   = 0x20; 

  

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
const uint8_t updateActionArray  = getActionArray + 1;    // "B"
const uint8_t getActionStatus    = updateActionArray + 1; // "C"
const uint8_t getMaxActions      = getActionStatus + 1;   // "D"
const uint8_t setActionSwitch    = getMaxActions + 1;     // "E"
const uint8_t saveToEEPROM       = setActionSwitch + 1;   // "F"
const uint8_t getEEPROMstatus    = saveToEEPROM + 1;      // "G"
const uint8_t getNewSensors      = getEEPROMstatus + 1;   // "H"
const uint8_t masterStop         = getNewSensors + 1;     // "I"
const uint8_t getMaxPids         = masterStop + 1;        // "J"
const uint8_t masterPidStop      = getMaxPids + 1;        // "K"
const uint8_t getPidStatus       = masterPidStop + 1;     // "L"
const uint8_t updatePidArray     = getPidStatus + 1;      // "M"
const uint8_t getPidArray        = updatePidArray + 1;    // "N"
const uint8_t setPidArray        = getPidArray + 1;       // "O"
const uint8_t useDebug           = setPidArray + 1;       // "P"
const uint8_t restoreStructures  = useDebug + 1;          // "Q"

const uint8_t versionID          = 'z';


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


// PID Stuff

const uint8_t maxPIDs = 8;
uint8_t pidCnt = 0;

typedef struct
{
  bool       pidEnabled;
  chipStruct *tempPtr;
  double     pidSetPoint;
  chipStruct *switchPtr;
  double     pidKp;
  double     pidKi;
  double     pidKd;
  int        pidDirection;
  uint32_t   pidWindowSize;
  uint32_t   pidwindowStartTime;
  double     pidInput;
  double     pidOutput;
  PID       *myPID;
}chipPIDStruct;

chipPIDStruct ePID[maxPIDs] =
{
  { FALSE, NULL, 70, NULL, 0, 0, 0, 0, 5000, 0, 0, 0, NULL },
  { FALSE, NULL, 70, NULL, 0, 0, 0, 0, 5000, 0, 0, 0, NULL },
  { FALSE, NULL, 70, NULL, 0, 0, 0, 0, 5000, 0, 0, 0, NULL },
  { FALSE, NULL, 70, NULL, 0, 0, 0, 0, 5000, 0, 0, 0, NULL },
  { FALSE, NULL, 70, NULL, 0, 0, 0, 0, 5000, 0, 0, 0, NULL },
  { FALSE, NULL, 70, NULL, 0, 0, 0, 0, 5000, 0, 0, 0, NULL },
  { FALSE, NULL, 70, NULL, 0, 0, 0, 0, 5000, 0, 0, 0, NULL },
  { FALSE, NULL, 70, NULL, 0, 0, 0, 0, 5000, 0, 0, 0, NULL }
};

//Specify the links and initial tuning parameters
PID PID0(&ePID[0].pidInput,   &ePID[0].pidOutput,  &ePID[0].pidSetPoint,  (double) ePID[0].pidKp,  (double) ePID[0].pidKi,  (double) ePID[0].pidKd,  ePID[0].pidDirection);
PID PID1(&ePID[1].pidInput,   &ePID[1].pidOutput,  &ePID[1].pidSetPoint,  (double) ePID[1].pidKp,  (double) ePID[1].pidKi,  (double) ePID[1].pidKd,  ePID[1].pidDirection);
PID PID2(&ePID[2].pidInput,   &ePID[2].pidOutput,  &ePID[2].pidSetPoint,  (double) ePID[2].pidKp,  (double) ePID[2].pidKi,  (double) ePID[2].pidKd,  ePID[2].pidDirection);
PID PID3(&ePID[3].pidInput,   &ePID[3].pidOutput,  &ePID[3].pidSetPoint,  (double) ePID[3].pidKp,  (double) ePID[3].pidKi,  (double) ePID[3].pidKd,  ePID[3].pidDirection);
PID PID4(&ePID[4].pidInput,   &ePID[4].pidOutput,  &ePID[4].pidSetPoint,  (double) ePID[4].pidKp,  (double) ePID[4].pidKi,  (double) ePID[4].pidKd,  ePID[4].pidDirection);
PID PID5(&ePID[5].pidInput,   &ePID[5].pidOutput,  &ePID[5].pidSetPoint,  (double) ePID[5].pidKp,  (double) ePID[5].pidKi,  (double) ePID[5].pidKd,  ePID[5].pidDirection);
PID PID6(&ePID[6].pidInput,   &ePID[6].pidOutput,  &ePID[6].pidSetPoint,  (double) ePID[6].pidKp,  (double) ePID[6].pidKi,  (double) ePID[6].pidKd,  ePID[6].pidDirection);
PID PID7(&ePID[7].pidInput,   &ePID[7].pidOutput,  &ePID[7].pidSetPoint,  (double) ePID[7].pidKp,  (double) ePID[7].pidKi,  (double) ePID[7].pidKd,  ePID[7].pidDirection);

PID *pidArrayPtr[] = {&PID0,&PID1,&PID2,&PID3,&PID4,&PID5,&PID6,&PID7};

// End PID Stuff

//EEPROM Stuff
const int   EEPROMsize       = 2048;   // Cortex M4
const int   EEPROMidAddr     = 0x10;   // ID address to verify a previous EEPROM write
const int   EEPROMccAddr     = 0x20;   // number of chips found during findchips()
const int   EEPROMchipAddr   = 0x40;  // start address of structures
const byte  EEPROMidVal      = 0x55;   // Shows that an EEPROM update has occurred 
bool        eepromReady      = FALSE;
int         eepromSpace, eeResult, EEPROMactionAddr, EEPROMpidAddr;

void setup()
{
  int x;
  pinMode(waitPin, OUTPUT);
  pinMode(waitLED, OUTPUT);
  digitalWrite(waitPin, LOW);
  digitalWrite(waitLED, LOW);
  Serial.begin(baudRate);
  
  if(setDebug > 0x0)
  {  
    delay(3000);
  }
  
  Serial.print(F("Serial Debug running at "));
  Serial.print(baudRate);
  Serial.println(F(" baud"));


  eeResult = EEPROM.read(EEPROMidAddr);
  
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  { 
    Serial.print(F("eeResult = 0x"));
    Serial.println(eeResult, HEX);
  }
  
  if(eeResult != 0x55)
  {
    if((setDebug & eepromDebug) || (setDebug & allDebug))
    { 
       Serial.println(F("No EEPROM Data"));
    }
  
    eepromReady = FALSE;
    findChips();
    saveStructures();
  }else{

    if((setDebug & eepromDebug) || (setDebug & allDebug))
    { 
      Serial.println(F("Getting EEPROM Data"));
    }

    chipCnt = EEPROM.read(EEPROMccAddr);
    readStructures();
    if((setDebug & eepromDebug) || (setDebug & allDebug))
    { 
      Serial.println(F("EEPROM Data Read Completed"));
    }
  
    eepromReady = TRUE;
    
  }
  
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  { 
    Serial.print( (sizeof(chipStruct) / sizeof(byte) ) * maxChips);
    Serial.println(F(" bytes in chip structure array"));
    Serial.print( (sizeof(chipActionStruct) / sizeof(byte) ) *maxActions);
    Serial.println(F(" bytes in action structure array"));
    Serial.print( (sizeof(chipPIDStruct) / sizeof(byte) ) *maxPIDs);
    Serial.println(F(" bytes in pid structure Array"));
  }

  Serial1.begin(baudRate);
  Serial.print(F("Serial1 Debug running at "));
  Serial.print(baudRate);
  Serial.println(F(" baud"));
  digitalWrite(waitPin, HIGH);
  digitalWrite(waitLED, HIGH);
  timer = millis();
  timer2 = millis();
  
  pidSetup();
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
       
       if((setDebug & serialDebug) || (setDebug & allDebug))
       {
         Serial.write(c);
       }
       
       cnt++;
       
       if((setDebug & serialDebug) || (setDebug & allDebug))
       {
         delay(2);
       }
     }
   }
  
  if(cnt > 0 && serialMessageReady == TRUE)
  {
    if((setDebug & serialDebug) || (setDebug & allDebug))
    {
      Serial.println();
    }
    
    softSerialProcess();
  }
  
  if(timer > (millis() + 5000)) // in case of rollover
  {
    timer = millis();
  }
  
  if(millis() > (timer + 125))
  {
    updateChipStatus(chipX);
    chipX++;
    if(chipX >= maxChips){chipX = 0;}
    timer = millis();
  
    updateActions(actionsCnt);    
    actionsCnt++;
    if(actionsCnt >= maxActions){actionsCnt = 0;}

    updatePIDs(pidCnt);
    pidCnt++;
    if(pidCnt >= maxPIDs){pidCnt = 0;}
  }

/*  
  if(millis() > (timer2 + ramUpdateTime))
  {
    Serial.print(freeMemory());
    Serial.println(F(" Available"));
    timer2 = millis();
  }
*/
}

void pidSetup(void)
{
/// ***** Start PID Setup *****

  for(int x =0; x < maxPIDs; x++)
  {
    ePID[x].myPID = pidArrayPtr[x];
  //tell the PID to range between 0 and the full window size
    ePID[x].myPID->SetOutputLimits(0, ePID[x].pidWindowSize);
    
    ePID[x].myPID->SetTunings(ePID[x].pidKp, ePID[x].pidKi, ePID[x].pidKd);
    
    ePID[x].myPID->SetSampleTime(200);

    if(ePID[x].pidDirection == 1)
    {
      ePID[x].myPID->SetControllerDirection(DIRECT);
    }else{
      ePID[x].myPID->SetControllerDirection(REVERSE);
    }
    
    
  //turn the PID on if variable are non-zero
    if( (ePID[x].pidEnabled == TRUE) &&
        (ePID[x].pidKp != 0) &&
        (ePID[x].pidKi != 0) &&
        (ePID[x].pidKd != 0) &&
        (ePID[x].pidWindowSize != 0)
        )
    {
      ePID[x].myPID->SetMode(AUTOMATIC);
    }else{
      ePID[x].pidEnabled = FALSE;
      ePID[x].myPID->SetMode(MANUAL);
    }
  }

}


void readStructures(void)
{  
  eepromSpace = 0;

  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.println(F("Entering readStructures"));
    Serial.print(F("EEPROMchipAddr = 0x"));
    Serial.println(EEPROMchipAddr, HEX);
  }

  eeResult = EEPROM_readAnything(EEPROMchipAddr, chip);

  eepromSpace += eeResult;

  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("Read "));
    Serial.print(eeResult);
    Serial.print(F(" bytes from address Ox"));
    Serial.println(EEPROMchipAddr, HEX);
  }

  EEPROMactionAddr = (eeResult + EEPROMchipAddr + 0x10) & 0xFFFF0;

  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("EEPROMactionAddr = 0x"));
    Serial.println(EEPROMactionAddr, HEX);
  }

  eeResult = EEPROM_readAnything(EEPROMactionAddr, action);

  eepromSpace += eeResult;

  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("Read "));
    Serial.print(eeResult);
    Serial.print(F(" bytes from address Ox"));
    Serial.println(EEPROMactionAddr, HEX);
  }

  EEPROMpidAddr =  (EEPROMactionAddr + eeResult + 0x10) & 0xFFFF0;

  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("EEPROMpidAddr = 0x"));
    Serial.println(EEPROMpidAddr, HEX);
  }

  eeResult = EEPROM_readAnything(EEPROMpidAddr, ePID);

  eepromSpace += eeResult;

  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("Read "));
    Serial.print(eeResult);
    Serial.print(F(" bytes from address Ox"));
    Serial.println(EEPROMpidAddr, HEX);
    Serial.print(F("readStructures() EEPROM Data Read of "));
    Serial.print(eepromSpace);
    Serial.println(F(" bytes Completed"));
    Serial.println(F("Exiting readStructures"));
    displayStructure((byte *)(uint32_t) &chip, sizeof(chip));
    displayStructure((byte *)(uint32_t) &action, sizeof(action));
    displayStructure((byte *)(uint32_t) &ePID, sizeof(ePID));
  }

  pidSetup();
}


void saveStructures(void)
{  
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.println(F("Entering saveStructures"));
    Serial.print(F("EEPROMchipAddr = 0x"));
    Serial.println(EEPROMchipAddr, HEX);
  }
  eepromSpace = 0;
  digitalWrite(waitPin, LOW);
  digitalWrite(waitLED, LOW);
  EEPROM.write(EEPROMccAddr, chipCnt);
  EEPROM.write(EEPROMidAddr, EEPROMidVal);
  eeResult = EEPROM_writeAnything(EEPROMchipAddr, chip);
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("Wrote "));
    Serial.print(eeResult);
    Serial.print(F(" bytes to address Ox"));
    Serial.println(EEPROMchipAddr, HEX);
  }
  eepromSpace += eeResult;
  EEPROMactionAddr = (eeResult + EEPROMchipAddr + 0x10) & 0xFFFF0;
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("EEPROMactionAddr = 0x"));
    Serial.println(EEPROMactionAddr, HEX);
  }
  eeResult = EEPROM_writeAnything(EEPROMactionAddr, action);
  eepromSpace += eeResult;
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("Wrote "));
    Serial.print(eeResult);
    Serial.print(F(" bytes to address Ox"));
    Serial.println(EEPROMactionAddr, HEX);
  }
  EEPROMpidAddr =  (EEPROMactionAddr + eeResult + 0x10) & 0xFFFF0;
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("EEPROMpidAddr = 0x"));
    Serial.println(EEPROMpidAddr, HEX);
  }
  eeResult = EEPROM_writeAnything(EEPROMpidAddr, ePID);
  eepromSpace += eeResult;
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("Wrote "));
    Serial.print(eeResult);
    Serial.print(F(" bytes to address Ox"));
    Serial.println(EEPROMpidAddr, HEX);
    Serial.print(F("saveStructures() EEPROM Data Write of "));
    Serial.print(eepromSpace);
    Serial.println(F(" bytes Completed"));
    Serial.println(F("Exiting saveStructures"));
    displayStructure((byte *)(uint32_t) &chip, sizeof(chip));
    displayStructure((byte *)(uint32_t) &action, sizeof(action));
    displayStructure((byte *)(uint32_t) &ePID, sizeof(ePID));
  }
  digitalWrite(waitPin, HIGH);
  digitalWrite(waitLED, HIGH);
}

void displayStructure(byte *addr, int size)
{
  int x, y;
  Serial.print(F("0x"));
  Serial.print((uint32_t)addr, HEX);
  Serial.print(F(": ")); 
  for(x = 0, y = 0; x < size; x++)
  {
    if(addr[x] >=0 && addr[x] <= 15)
    {
      Serial.print(F("0x0"));
    }else{
      Serial.print(F("0x"));
    }
    Serial.print(addr[x], HEX);
    y++;
    if(y < 16)
    {
      Serial.print(F(", "));
    }else{
      y = 0;
      Serial.println();
      Serial.print(F("0x"));
      Serial.print((uint32_t)addr + x + 1, HEX);
      Serial.print(F(": ")); 
    }
  }
  Serial.println();
  Serial.println();
}


void updatePIDs(uint8_t pidCnt)
{
  digitalWrite(waitPin, LOW);
  digitalWrite(waitLED, LOW);
  
  if(ePID[pidCnt].pidEnabled == 1)
  {    
    // *** Start PID Loop ***
    ePID[pidCnt].pidInput = (double) ePID[pidCnt].tempPtr->chipStatus;

    if( (setDebug & pidDebug) || (setDebug & allDebug) )
    {
      Serial.println(F("Entering updatePIDs"));
      Serial.print(F("PID #"));
      Serial.println(pidCnt);
      Serial.print(F("ePID["));
      Serial.print(pidCnt);
      Serial.print(F("].pidInput = "));
      Serial.println((double) ePID[pidCnt].pidInput);
      Serial.print(F("ePID["));
      Serial.print(pidCnt);
      Serial.print(F("].pidKp = "));
      Serial.println(ePID[pidCnt].pidKp);
      Serial.print(F("ePID["));
      Serial.print(pidCnt);
      Serial.print(F("].pidKi = "));
      Serial.println(ePID[pidCnt].pidKi);
      Serial.print(F("ePID["));
      Serial.print(pidCnt);
      Serial.print(F("].pidKd = "));
      Serial.println(ePID[pidCnt].pidKd);
      Serial.print(F("ePID["));
      Serial.print(pidCnt);
      Serial.print(F("].pidDirection = "));
      Serial.println(ePID[pidCnt].pidDirection);
      Serial.print(F("ePID["));
      Serial.print(pidCnt);
      Serial.print(F("].pidWindowStartTime = "));
      Serial.println((uint32_t) ePID[pidCnt].pidwindowStartTime);
      Serial.print(F("millis() = "));
      Serial.println((uint32_t) millis());
    }
  
    if(ePID[pidCnt].myPID->Compute())
    {
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.println(F("Compute() returned TRUE"));
      }
    }else{
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.println(F("Compute() returned FALSE"));
      }
    }

    uint32_t now = millis();
    
    if( (setDebug & pidDebug) || (setDebug & allDebug) )
    {
      Serial.print(F("now - ePID[pidCnt].pidwindowStartTime = "));
      Serial.println(now - ePID[pidCnt].pidwindowStartTime);
    }

  /************************************************
   * turn the output pin on/off based on pid output
   ************************************************/
    if(now - ePID[pidCnt].pidwindowStartTime > ePID[pidCnt].pidWindowSize)
    { //time to shift the Relay Window
      ePID[pidCnt].pidwindowStartTime += ePID[pidCnt].pidWindowSize;
    }
  
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("ePID["));
        Serial.print(pidCnt);
        Serial.print(F("].pidOutPut = "));
        Serial.println((double) ePID[pidCnt].pidOutput);
        Serial.print(F("now = "));
        Serial.println(now);
        Serial.print(F("ePID["));
        Serial.print(pidCnt);
        Serial.print(F("].pidwindowStartTime = "));
        Serial.println((double) ePID[pidCnt].pidwindowStartTime);
        Serial.print(F("now - ePID["));
        Serial.print(pidCnt);
        Serial.print(F("].pidwindowStartTime = "));
        Serial.println((double) now - ePID[pidCnt].pidwindowStartTime);
  
        Serial.print((double) ePID[pidCnt].pidOutput);
        
        if(ePID[pidCnt].pidOutput > now - ePID[pidCnt].pidwindowStartTime)
        {
          Serial.print(F(" > "));
        }else{
          Serial.print(F(" < "));
        }
        Serial.println((double) now - ePID[pidCnt].pidwindowStartTime);
      }

    if(ePID[pidCnt].pidOutput > now - ePID[pidCnt].pidwindowStartTime)
    {
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.println(F("Turning Switch ON"));
      }
      actionSwitchSet((uint8_t *) &ePID[pidCnt].switchPtr->chipAddr, ds2406PIOAon);
    }else{
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.println(F("Turning Switch OFF"));
      }
      actionSwitchSet((uint8_t *) &ePID[pidCnt].switchPtr->chipAddr, ds2406PIOAoff);
    }
  // *** End PID Loop ***

    if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("ePID["));
        Serial.print(pidCnt);
        Serial.print(F("].pidOutput = "));
        Serial.println((double) ePID[pidCnt].pidOutput);
        Serial.println(F("Exiting updatePIDs"));
      }

  }else{
    ePID[pidCnt].myPID->SetMode(MANUAL);
  }
  digitalWrite(waitPin, HIGH);
  digitalWrite(waitLED, HIGH);
}

void findChips()
{
 int cntx = 0;
 
  while (ds.search(chip[cntx].chipAddr))
  {

    if( (setDebug & chipDebug) || (setDebug & allDebug) )
    {
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
    }
      
    cntx++;
    delay(750);
  }

  if( (setDebug & chipDebug) || (setDebug & allDebug) )
  {
    Serial.print(cntx);
    Serial.print(F(" Sensor"));
    if(cntx == 1)
    {
      Serial.println(F(" Detected"));
    }else{
       Serial.println(F("s Detected"));
    }
  }
  
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
  int x, ssBufOffset, pidArray, pidSection, pidEnabledVal, pidDirectionVal;
  char *result = NULL, *addrResult = NULL, pidEnd = NULL;
  char delim[] = " ", addrDelim[] = ",";
  int16_t actionEnableTemp, pidEnableVal;
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
            if((setDebug & serial1Debug) || (setDebug & allDebug))
            {
              Serial.print(F("actionArray = "));
              Serial.println(actionArray);
            }
            break;
          }
          case 1:
          {
            actionSection = atoi(result);
            if((setDebug & serial1Debug) || (setDebug & allDebug))
            {
              Serial.print(F("actionSection = "));
              Serial.println(actionSection);
            }
            break;
          }
          
          case 2:
          {
            actionEnableTemp = atoi(result);
            
            if((setDebug & serial1Debug) || (setDebug & allDebug))
            {
              Serial.print(F("actionEnableTemp = "));
              Serial.println(actionEnableTemp);
              Serial.print(F("action["));
              Serial.print(actionArray);
              Serial.print(F("]"));
            }
            
            switch (actionSection)
            {
              case 1:
              {
                if(actionEnableTemp == 1)
                {
                  action[actionArray].actionEnabled = TRUE;
                  
                  if((setDebug & serial1Debug) || (setDebug & allDebug))
                  {
                    Serial.println(F(".actionEnabled is Enabled"));
                  }
                  
                }else{
                  action[actionArray].actionEnabled = FALSE;
                  
                  if((setDebug & serial1Debug) || (setDebug & allDebug))
                  {
                    Serial.println(F(".actionEnabled is Disabled"));
                  }
                }
                
                if((setDebug & serial1Debug) || (setDebug & allDebug))
                {
                  Serial.print(F("action["));
                  Serial.print(actionArray);
                  Serial.print(F("].actionEnabled = "));
                  Serial.println(action[actionArray].actionEnabled);
                }
                
                break;
              }
              
              case 2:
              case 3:
              {
                if(actionSection == 2)
                {
                  action[actionArray].tooCold = actionEnableTemp;
                  
                  if((setDebug & serial1Debug) || (setDebug & allDebug))
                  {
                    Serial.print(F(".tooCold is set to "));
                    Serial.println(actionEnableTemp);
                  }
                  
                }else if( actionSection == 3){
                  action[actionArray].tooHot = actionEnableTemp;
                  
                  if((setDebug & serial1Debug) || (setDebug & allDebug))
                  {
                    Serial.print(F(".tooHot is set to "));
                    Serial.println(actionEnableTemp);
                  }
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
              if((setDebug & serial1Debug) || (setDebug & allDebug))
              {
                Serial.print(F("result = "));
                Serial.println(result);
              }
              actionDelayVal = ((uint32_t) atoi(result));
              
              if((setDebug & serial1Debug) || (setDebug & allDebug))
              {
                Serial.print(F("actionDelayVal = "));
                Serial.println(actionDelayVal);
              }
              
              actionDelayVal *= 1000;
              
              if((setDebug & serial1Debug) || (setDebug & allDebug))
              {
                Serial.print(F("actionDelayVal * 1000 = "));
                Serial.println(actionDelayVal);
                Serial.print(F("action["));
                Serial.print(actionArray);
                Serial.print(F("]."));
              }
              
              if(actionSection == 2)
              {
                action[actionArray].tcDelay = actionDelayVal;
                if(actionDelayVal > 0)
                {
                  action[actionArray].tcMillis = millis();
                }
                
                if((setDebug & serial1Debug) || (setDebug & allDebug))
                {
                  Serial.print(F("tcDelay = "));
                  Serial.println((actionDelayVal / 1000));
                }
                
              }else if (actionSection == 3){
                action[actionArray].thDelay = actionDelayVal;
                if(actionDelayVal > 0)
                {
                  action[actionArray].thMillis = millis();
                }
                
                if((setDebug & serial1Debug) || (setDebug & allDebug))
                {
                  Serial.print(F("thDelay = "));
                  Serial.println(actionDelayVal / 1000);
                }
              }
            }
            break;
          }
          
          case 4:
          {
            if((setDebug & serial1Debug) || (setDebug & allDebug))
            {
              Serial.println(result);
            }
            
            addrResult = strtok( result, addrDelim );
            while(addrResult != NULL)
            {
              addrVal[addrResultCnt] = (uint8_t) strtol(addrResult, NULL, 16);
              
              if((setDebug & serial1Debug) || (setDebug & allDebug))
              {
                Serial.print(F(" "));
                Serial.print(addrVal[addrResultCnt], HEX);
              }
              
              addrResultCnt++;
              addrResult = strtok( NULL, addrDelim );
            }
            
            if(addrVal[0] == 0x0)
            {
              chipAddrCnt = maxChips+10;
            }else{
              chipAddrCnt = matchChipAddress(addrVal);
            }
            
            switch (actionSection)
            {
              case 1:
              {
                if(chipAddrCnt > chipCnt)
                {
                  action[actionArray].tempPtr = NULL;
                }else{
                  action[actionArray].tempPtr = &chip[chipAddrCnt];
                }
                break;
              }
              case 2:
              {
                if(chipAddrCnt > chipCnt)
                {
                  action[actionArray].tcPtr = NULL;
                }else{
                  action[actionArray].tcPtr = &chip[chipAddrCnt];
                }
                break;
              }
              case 3:
              {
                if(chipAddrCnt > chipCnt)
                {
                  action[actionArray].thPtr = NULL;
                }else{
                  action[actionArray].thPtr = &chip[chipAddrCnt];
                }
                break;
              }
            }
            
            if((setDebug & serial1Debug) || (setDebug & allDebug))
            {
              //Serial.print(addrVal, HEX);
              Serial.print(F(", "));
            }
            
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
      if((setDebug & serial1Debug) || (setDebug & allDebug))
      {
        Serial.println(F("case: setActionSwitch"));
        Serial.println(softSerialBuffer);
      }
      
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
          if((setDebug & serial1Debug) || (setDebug & allDebug))
          {
            Serial.println(F("Setting too Cold Switch"));
          }
          
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
          if((setDebug & serial1Debug) || (setDebug & allDebug))
          {
            Serial.println(F("Setting too Hot Switch"));
          }
          
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
          if((setDebug & serial1Debug) || (setDebug & allDebug))
          {
            Serial.println(F("Houston, We HAve A Switch Problem"));
          }
          break;
        }
      }
      Serial1.print(F("\0"));
      break;
    }

    case saveToEEPROM: // "F"
    {
      saveStructures();
      Serial1.print(F("\0"));
      break;
    }
    
    case getEEPROMstatus: // "G"
    {
      if(eepromReady == FALSE)
      {
        Serial1.println(F("FALSE"));
      }else
      {
        Serial1.println(F("TRUE"));
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
      Serial1.print(F("\0"));
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
      Serial1.print(F("\0"));
      break;
    }
    
    case getMaxPids: // "J"
    {
      Serial1.print(maxPIDs);
      Serial1.print(F("\n"));
      break;
    }
    
    case masterPidStop: // "K"
    {
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.println(F("masterPidStop Enter"));
      }
      
      for(x=0;x<maxPIDs;x++)
      {
        ePID[x].pidEnabled = FALSE;
        
        if( (setDebug & pidDebug) || (setDebug & allDebug) )
        {
          Serial.print(F("ePID["));
          Serial.print(x);
          Serial.println(F("].pidEnabled set to FALSE"));
        }
        
        ePID[x].myPID->SetMode(MANUAL);
        
        if( (setDebug & pidDebug) || (setDebug & allDebug) )
        {
          Serial.print(F("ePID["));
          Serial.print(x);
          Serial.println(F("].myPID->SetMode() set to MANUAL"));
        }
        
        if(&ePID[x].switchPtr->chipAddr != NULL)
        {
          actionSwitchSet((uint8_t *) &ePID[x].switchPtr->chipAddr, ds2406PIOAoff);
          if( (setDebug & pidDebug) || (setDebug & allDebug) )
          {
              Serial.print(F("ePID["));
              Serial.print(x);
              Serial.println(F("].switchPtr->chipAddr set to OFF"));
          }
        }
      }
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.println(F("masterPidStop Exit"));
      }
      
      Serial1.print(F("\n"));
      break;
    }
    
    case getPidStatus: // "L"
    {
      getAllPidStatus();
      break;
    }

    case updatePidArray: // "M"
    {
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.println(F("updatePidArray Enter"));
        Serial.println(softSerialBuffer);
      }
      
      result = strtok( softSerialBuffer, delim );
      char* pidArrayPtr      = strtok( NULL, delim );
      char* pidEnabledPtr    = strtok( NULL, delim );
      char* pidTempAddrPtr   = strtok( NULL, delim );
      char* pidSetPointPtr   = strtok( NULL, delim );
      char* pidSwitchAddrPtr = strtok( NULL, delim );
      char* pidKpPtr         = strtok( NULL, delim );
      char* pidKiPtr         = strtok( NULL, delim );
      char* pidKdPtr         = strtok( NULL, delim );
      char* pidDirectionPtr  = strtok( NULL, delim );
      char* pidWindowSizePtr = strtok( NULL, delim );
      
      if(strlen(pidTempAddrPtr) < 39 || strlen(pidSwitchAddrPtr) < 39)
      {
        Serial1.println("Z");
        break;
      }

      pidArray = atoi(pidArrayPtr);
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("pidArray = "));
        Serial.println(pidArray);
        Serial.print(F("pidEnabledPtr = "));
        Serial.println(pidEnabledPtr);
        Serial.print(F("pidTempAddrPtr = "));
        Serial.println(pidTempAddrPtr);
        Serial.print(F("pidSetPointPtr = "));
        Serial.println(pidSetPointPtr);
        Serial.print(F("pidSwitchAddrPtr = "));
        Serial.println(pidSwitchAddrPtr);
        Serial.print(F("pidKpPtr = "));
        Serial.println(pidKpPtr);
        Serial.print(F("pidKiPtr = "));
        Serial.println(pidKiPtr);
        Serial.print(F("pidKdPtr = "));
        Serial.println(pidKdPtr);
        Serial.print(F("pidDirectionPtr = "));
        Serial.println(pidDirectionPtr);
        Serial.print(F("pidWindowSizePtr = "));
        Serial.println(pidWindowSizePtr);
      }
  
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("pidEnabled = "));
        Serial.println(ePID[pidArray].pidEnabled);
      }
      
      asciiArrayToHexArray(pidTempAddrPtr, addrDelim, addrVal);
      
      if(addrVal[0] == 0x0)
      {
        chipAddrCnt = maxChips + 10;
      }else{
        chipAddrCnt = matchChipAddress(addrVal);
      }
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("chipAddrCnt = "));
        Serial.println(chipAddrCnt);
      }
      
      if(chipAddrCnt > chipCnt)
      {
        ePID[pidArray].tempPtr = NULL;
      }else{
        ePID[pidArray].tempPtr = &chip[chipAddrCnt];
      }
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("tempPtr = "));
        Serial.println((uint32_t) ePID[pidArray].tempPtr, HEX);
      }

      ePID[pidArray].pidSetPoint = strtod(pidSetPointPtr, NULL);
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("pidSetPoint = "));
        Serial.println((double) ePID[pidArray].pidSetPoint);
      }
  
      asciiArrayToHexArray(pidSwitchAddrPtr, addrDelim, addrVal);
      
      if(addrVal[0] == 0x0)
      {
        chipAddrCnt = maxChips + 10;
      }else{
        chipAddrCnt = matchChipAddress(addrVal);
      }
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("chipAddrCnt = "));
        Serial.println(chipAddrCnt);
      }
      
      if(chipAddrCnt > chipCnt)
      {
        ePID[pidArray].switchPtr = NULL;
      }else{
        ePID[pidArray].switchPtr = &chip[chipAddrCnt];
      }
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("switchPtr = "));
        Serial.println((uint32_t) ePID[pidArray].switchPtr, HEX);
      }

      ePID[pidArray].pidKp = strtod(pidKpPtr, NULL);
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("pidKp = "));
        Serial.println((double) ePID[pidArray].pidKp);
      }

      ePID[pidArray].pidKi = strtod(pidKiPtr, NULL);
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("pidKi = "));
        Serial.println((double) ePID[pidArray].pidKi);
      }

      ePID[pidArray].pidKd = strtod(pidKdPtr, NULL);
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("pidKd = "));
        Serial.println((double) ePID[pidArray].pidKd);
      }
      
      ePID[pidArray].myPID->SetTunings(ePID[pidArray].pidKp, ePID[pidArray].pidKi, ePID[pidArray].pidKd);

      pidDirectionVal = atoi(pidDirectionPtr);
      if(pidDirectionVal == 0)
      {
        ePID[pidArray].pidDirection = FALSE;
      }else{
        ePID[pidArray].pidDirection = TRUE;
      }
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("pidDirection = "));
        Serial.println(ePID[pidArray].pidDirection);
      }
      
      ePID[pidArray].myPID->SetControllerDirection(ePID[pidArray].pidDirection);

      ePID[pidArray].pidWindowSize = strtol(pidWindowSizePtr, NULL, 10);
      ePID[pidArray].myPID->SetOutputLimits(0, ePID[pidArray].pidWindowSize);
      ePID[pidArray].pidwindowStartTime = millis();

      pidEnabledVal = atoi(pidEnabledPtr);
      if( (pidEnabledVal == 0) ||
          (ePID[pidArray].pidKp == 0) ||
          (ePID[pidArray].pidKi == 0) ||
          (ePID[pidArray].pidKd == 0) ||
          (ePID[pidArray].pidWindowSize == 0) ||
          (ePID[pidArray].switchPtr == NULL) ||
          (ePID[pidArray].tempPtr == NULL)
        )
      {
        ePID[pidArray].pidEnabled = FALSE;
        ePID[pidArray].myPID->SetMode(MANUAL);
      }else{
        ePID[pidArray].pidEnabled = TRUE;
        ePID[pidArray].myPID->SetMode(AUTOMATIC);
      }
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.print(F("pidWindowSize = "));
        Serial.println((double) ePID[pidArray].pidWindowSize);
      }
      break;
    }


    case getPidArray: // "N"
    {
      x = atoi((char *) &softSerialBuffer[1]);
      Serial1.print(ePID[x].pidEnabled);
      Serial1.print(F(" "));
      if(ePID[x].tempPtr == NULL)
      {
        Serial1.print(F("0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00"));
      }else{
        showChipAddress((uint8_t *) &ePID[x].tempPtr->chipAddr);
      }
      Serial1.print(F(" "));
      Serial1.print((int) ePID[x].tempPtr->chipStatus);
      Serial1.print(F(" "));
      Serial1.print((double) ePID[x].pidSetPoint);
      Serial1.print(F(" "));
      if(ePID[x].switchPtr == NULL)
      {
        Serial1.print(F("0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00"));
      }else{
        showChipAddress((uint8_t *) &ePID[x].switchPtr->chipAddr);
      }
      Serial1.print(F(" "));
      Serial1.print((int) ePID[x].switchPtr->chipStatus);
      Serial1.print(F(" "));
      Serial1.print((double) ePID[x].pidKp);
      Serial1.print(F(" "));
      Serial1.print((double) ePID[x].pidKi);
      Serial1.print(F(" "));
      Serial1.print((double) ePID[x].pidKd);
      Serial1.print(F(" "));
      Serial1.print((int) ePID[x].pidDirection);
      Serial1.print(F(" "));
      Serial1.print(ePID[x].pidWindowSize);
      Serial1.print(F("\0"));
      break;
    }
    
    case setPidArray: // "O"
    {
      result = strtok( softSerialBuffer, delim );
      char* pidArrayCtr      = strtok( NULL, delim );
      char* pidEnabledVal    = strtok( NULL, delim );
      
      x = atoi(pidArrayCtr);
      if (atoi(pidEnabledVal) == 0)
      {
        ePID[x].pidEnabled = FALSE;
        ePID[x].myPID->SetMode(MANUAL);
        actionSwitchSet((uint8_t *) &ePID[x].switchPtr->chipAddr, ds2406PIOAoff);
      }else{
        ePID[x].pidEnabled = TRUE;
        ePID[x].pidwindowStartTime = millis();
        ePID[x].myPID->SetMode(AUTOMATIC);
      }
      Serial1.print(F("\0"));
      break;
    }
    
    case useDebug: // "P"
    {
      
      setDebug = atoi((char *) &softSerialBuffer[1]);
      Serial1.print(F("0x"));
      Serial1.print(setDebug, HEX);
      Serial1.print(F("\n\0"));
      break;
    }
 
    case restoreStructures: // "Q"
    {
      readStructures();
      Serial1.print(F("\n\0"));
      break;
    }
    
     case versionID: // "z"
    {
      Serial1.println(versionStr);
      break;
    }
       
  }

  Serial1.print(F("\n"));
//    break;
  
  softSerialBuffer[0]=0x00;
  cnt = 0;
  serialMessageReady = FALSE;
}

void asciiArrayToHexArray(char* result, char* addrDelim, uint8_t* addrVal)
{
  char *addrResult = NULL;
  uint16_t addrResultCnt = 0;
  
  addrResult = strtok( result, addrDelim );
  while(addrResult != NULL)
  {
    addrVal[addrResultCnt] = (uint8_t) strtol(addrResult, NULL, 16);
    
    if( (setDebug & pidDebug) || (setDebug & allDebug) )
    {
      if(addrVal[addrResultCnt] >= 0 && addrVal[addrResultCnt] <= 9)
      {
        Serial.print(F(" 0x0"));
      }else{
        Serial.print(F(" 0x"));
      }
      Serial.print(addrVal[addrResultCnt], HEX);
    }
      
    addrResultCnt++;
    addrResult = strtok( NULL, addrDelim );
   }
   
  if( (setDebug & pidDebug) || (setDebug & allDebug) )
  {
    Serial.println();
  }
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

void getAllPidStatus(void)
{
  uint8_t x;
  
  if( (setDebug & pidDebug) || (setDebug & allDebug) )
  {
  // Serial.println(F("Entering getAllPidStatus(void) "));
  }
  
  // x = atoi((char *) &softSerialBuffer[1]);
  
  for( x = 0; x < maxPIDs; x++ )
  {
    Serial1.print((int) ePID[x].pidEnabled);
    Serial1.print(F(","));
    if(ePID[x].tempPtr == NULL)
    {
      Serial1.print((int) noChipPresent);
    }else{
      Serial1.print((int) ePID[x].tempPtr->chipStatus);
    }
    Serial1.print(F(","));
    Serial1.print((int) ePID[x].pidSetPoint);
    Serial1.print(F(","));
    if(ePID[x].switchPtr == NULL)
    {
      Serial1.print((int) noChipPresent);
    }else{
      Serial1.print((char) ePID[x].switchPtr->chipStatus);
    }
    Serial1.print(F(","));
    Serial1.print((double) ePID[x].pidKp);
    Serial1.print(F(","));
    Serial1.print((double) ePID[x].pidKi);
    Serial1.print(F(","));
    Serial1.print((double) ePID[x].pidKd);
    Serial1.print(F(","));
    Serial1.print((int) ePID[x].pidDirection);
    Serial1.print(F(","));
    Serial1.print((uint32_t) ePID[x].pidWindowSize);
    if( x < (maxPIDs - 1) )
    {
      Serial1.print(F(";"));
    }
  }
  Serial1.print(F("\n"));
  
  if( (setDebug & pidDebug) || (setDebug & allDebug) )
  {
  //Serial.println(F("Exiting getAllPidStatus(void) "));
  }
}

uint8_t matchChipAddress(uint8_t* array)
{
   uint8_t addrMatchCnt, chipAddrCnt;
   
  if( (setDebug & pidDebug) || (setDebug & allDebug) )
  {
   Serial.println(F("matchChipAddress"));
  }
  
  for(addrMatchCnt = 0, chipAddrCnt = 0; ((addrMatchCnt < chipAddrSize) || (chipAddrCnt > chipCnt)); addrMatchCnt++)
  {
    if(array[addrMatchCnt] != chip[chipAddrCnt].chipAddr[addrMatchCnt])
    {
      addrMatchCnt = 0;
      chipAddrCnt++;
      
      if( (setDebug & pidDebug) || (setDebug & allDebug) )
      {
        Serial.println(chipAddrCnt);
      }
  
      continue;
    }
    
    if( (setDebug & pidDebug) || (setDebug & allDebug) )
    {
      Serial.print(array[addrMatchCnt], HEX);
      Serial.print(F(","));
    }
  }
  
  if(chipAddrCnt <= chipCnt)
  {
    if( (setDebug & pidDebug) || (setDebug & allDebug) )
    {
      Serial.print(F("MATCH!! - "));
    }
  }else{

    if( (setDebug & pidDebug) || (setDebug & allDebug) )
    {
      Serial.print(F("NO MATCH!! - "));
    }

    chipAddrCnt = 0xFF;
  }

  if( (setDebug & pidDebug) || (setDebug & allDebug) )
  {
    Serial.println(chipAddrCnt);
  }

  return(chipAddrCnt);
}

void actionSwitchSet(uint8_t* array, uint8_t setChipState)
{
   uint8_t chipAddrCnt;

   if((setDebug & serial1Debug) || (setDebug & allDebug))
   {
     Serial.println(F("actionSwitchSet"));
   }

  chipAddrCnt = matchChipAddress(array);
  
  if(chipAddrCnt != 0xFF)
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
}

void updateActions(uint8_t x)
{
  digitalWrite(waitPin, LOW);
  digitalWrite(waitLED, LOW);

  if((setDebug & serial1Debug) || (setDebug & allDebug))
  {
    Serial.print(F("updating Action: "));
    Serial.print(x);
  }
  
  if(action[x].actionEnabled == TRUE)
  {
    if(action[x].tempPtr->chipStatus <= action[x].tooCold &&
       action[x].tcPtr->chipStatus == switchStatusOFF) // too cold
    {
      if(action[x].tcDelay == 0 || millis() > (action[x].tcMillis + action[x].tcDelay))
      {
        actionSwitchSet((uint8_t *) &action[x].tcPtr->chipAddr, ds2406PIOAon);
        
        if((setDebug & serial1Debug) || (setDebug & allDebug))
        {
          Serial.println(F(" - TOO COLD"));
        }
      }
    }else if(action[x].tempPtr->chipStatus > action[x].tooCold &&
             action[x].tcPtr->chipStatus == switchStatusON){
               
      actionSwitchSet((uint8_t *) &action[x].tcPtr->chipAddr, ds2406PIOAoff);
      action[x].tcMillis = millis();
       
      if((setDebug & serial1Debug) || (setDebug & allDebug))
      {
        Serial.println(F(" - NOT TOO COLD"));
      }
      
    }

    if(action[x].tempPtr->chipStatus >= action[x].tooHot &&
       action[x].thPtr->chipStatus == switchStatusOFF) //too hot
    {
      if(action[x].thDelay == 0 || millis() > (action[x].thMillis + action[x].thDelay))
      {
        actionSwitchSet((uint8_t *) &action[x].thPtr->chipAddr, ds2406PIOAon);
        
        if((setDebug & serial1Debug) || (setDebug & allDebug))
        {
          Serial.println(F(" - TOO HOT"));
        }
  
      }
    }else if(action[x].tempPtr->chipStatus < action[x].tooHot &&
             action[x].thPtr->chipStatus == switchStatusON){
               
      actionSwitchSet((uint8_t *) &action[x].thPtr->chipAddr, ds2406PIOAoff);
      action[x].thMillis = millis();
               
      if((setDebug & serial1Debug) || (setDebug & allDebug))
      {
        Serial.println(F(" - NOT TOO HOT"));
      }
  
    }
  }else{

     if((setDebug & serial1Debug) || (setDebug & allDebug))
     {
       Serial.println(F(" - not enabled"));
     }
     
  }
  digitalWrite(waitPin, HIGH);
  digitalWrite(waitLED, HIGH);
}
