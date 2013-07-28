/********************

TeensyPi.ino

Version 0.0.12tc
Last Modified 07/28/2013
By Jim Mayhugh

Used with TeensyPi Rev 0.0.10 pc board

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

This software uses multiple libraries that are subject to additional
licenses as defined by the author of that software. It is the user's
and developer's responsibility to determine and adhere to any additional
requirements that may arise.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


*********************/
#include <PID_v1.h>
#include <math.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "Adafruit_MAX31855.h"
#include <OneWire.h>
#include <errno.h>

// #include <MemoryFree.h>

/*
  General Setup
*/

const char* versionStr = "TeensyPi with Thermocouple Version 0.0.12tc, 07/28/2013";

const uint8_t allDebug       = 0x01; 
const uint8_t pidDebug       = 0x02; 
const uint8_t eepromDebug    = 0x04; 
const uint8_t chipDebug      = 0x08; 
const uint8_t serial1Debug   = 0x10; 
const uint8_t serialDebug    = 0x20; 
const uint8_t tcProbeDebug   = 0x40;
const uint8_t ds2762Debug    = 0x80;

uint16_t setDebug = 0x0;  

// define serial commands

const uint8_t getMaxChips        = '1';
const uint8_t showChip           = getMaxChips + 1;       // "2"
const uint8_t getChipCount       = showChip + 1;          // "3"
const uint8_t getChipAddress     = getChipCount + 1;      // "4"
const uint8_t getChipStatus      = getChipAddress + 1;    // "5"
const uint8_t setSwitchState     = getChipStatus + 1;     // "6"
const uint8_t getAllStatus       = setSwitchState + 1;    // "7"
const uint8_t getChipType        = getAllStatus + 1;      // "8"
const uint8_t getAllChips        = getChipType + 1;       // "9" last in this series

const uint8_t getActionArray     = 'A';                   // "A" start of new serial command list
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
const uint8_t readMX             = restoreStructures + 1; // "R"

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
bool showCelsius = FALSE;

uint32_t timer, timer2, mxTimer, startTime, endTime;
const uint32_t updateTime = 250;
const uint32_t ramUpdateTime = 10000;
const uint32_t ds2762UpdateTime = 500;

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
const uint8_t ds2762ID       = 0x30;
const uint8_t dsT31855ID     = 0xAA;
const uint8_t dsPIO_A        = 0x20;
const uint8_t dsPIO_B        = 0x40;

const uint8_t maxChips       = 36; // Maximum number of Chips
const uint8_t maxActions     = 12; // Maximum number of Actions

OneWire  ds(oneWireAddress);

// DS2762 oneWire conversion table - K-Type Thermocouple

const uint16_t kNegTableCnt = 271;
PROGMEM prog_uint16_t kNegTable[kNegTableCnt] =
{
 /*       --0--  --1--  --2--  --3--  --4--  --5--  --6--  --7--  --8--  --9-- */
 /*   0*/     0,    39,    79,   118,   157,   197,   236,   275,   314,   353,
 /* -10*/   392,   431,   470,   508,   547,   586,   624,   663,   701,   739,
 /* -20*/   778,   816,   854,   892,   930,   968,  1006,  1043,  1081,  1119,
 /* -30*/  1156,  1194,  1231,  1268,  1305,  1343,  1380,  1417,  1453,  1490,
 /* -40*/  1527,  1564,  1600,  1637,  1673,  1709,  1745,  1782,  1818,  1854,
 /* -50*/  1889,  1925,  1961,  1996,  2032,  2067,  2103,  2138,  2173,  2208,
 /* -60*/  2243,  2278,  2312,  2347,  2382,  2416,  2450,  2485,  2519,  2553,
 /* -70*/  2587,  2620,  2654,  2688,  2721,  2755,  2788,  2821,  2854,  2887,
 /* -80*/  2920,  2953,  2986,  3018,  3050,  3083,  3115,  3147,  3179,  3211,
 /* -90*/  3243,  3274,  3306,  3337,  3368,  3400,  3431,  3462,  3492,  3523,
 /*-100*/  3554,  3584,  3614,  3645,  3675,  3705,  3734,  3764,  3794,  3823,
 /*-110*/  3852,  3882,  3911,  3939,  3968,  3997,  4025,  4054,  4082,  4110,
 /*-120*/  4138,  4166,  4194,  4221,  4249,  4276,  4303,  4330,  4357,  4384,
 /*-130*/  4411,  4437,  4463,  4490,  4516,  4542,  4567,  4593,  4618,  4644,
 /*-140*/  4669,  4694,  4719,  4744,  4768,  4793,  4817,  4841,  4865,  4889,
 /*-150*/  4913,  4936,  4960,  4983,  5006,  5029,  5052,  5074,  5097,  5119,
 /*-160*/  5141,  5163,  5185,  5207,  5228,  5250,  5271,  5292,  5313,  5333,
 /*-170*/  5354,  5374,  5395,  5415,  5435,  5454,  5474,  5493,  5512,  5531,
 /*-180*/  5550,  5569,  5588,  5606,  5624,  5642,  5660,  5678,  5695,  5713,
 /*-190*/  5730,  5747,  5763,  5780,  5797,  5813,  5829,  5845,  5861,  5876,
 /*-200*/  5891,  5907,  5922,  5936,  5951,  5965,  5980,  5994,  6007,  6021,
 /*-210*/  6035,  6048,  6061,  6074,  6087,  6099,  6111,  6123,  6135,  6147,
 /*-220*/  6158,  6170,  6181,  6192,  6202,  6213,  6223,  6233,  6243,  6252,
 /*-230*/  6262,  6271,  6280,  6289,  6297,  6306,  6314,  6322,  6329,  6337,
 /*-240*/  6344,  6351,  6358,  6364,  6370,  6377,  6382,  6388,  6393,  6399,
 /*-250*/  6404,  6408,  6413,  6417,  6421,  6425,  6429,  6432,  6435,  6438,
 /*-260*/  6411,  6444,  6446,  6448,  6450,  6452,  6453,  6455,  6456,  6457,
 /*-270*/  6458
};

const uint16_t kTableCnt = 1024;
PROGMEM prog_uint16_t kTable[kTableCnt] =
{
  /*       --0--  --1--  --2--  --3--  --4--  --5--  --6--  --7--  --8--  --9-- */
  /*0000*/     0,    39,    79,   119,   158,   198,   238,   277,   317,   357,
  /*0010*/   397,   437,   477,   517,   557,   597,   637,   677,   718,   758,
  /*0020*/   798,   838,   879,   919,   960,  1000,  1040,  1080,  1122,  1163,
  /*0030*/  1203,  1244,  1284,  1326,  1366,  1407,  1448,  1489,  1530,  1570,
  /*0040*/  1612,  1653,  1694,  1735,  1776,  1816,  1858,  1899,  1941,  1982,
  /*0050*/  2023,  2064,  2105,  2146,  2188,  2230,  2270,  2311,  2354,  2395,
  /*0060*/  2436,  2478,  2519,  2560,  2601,  2644,  2685,  2726,  2767,  2810,
  /*0070*/  2850,  2892,  2934,  2976,  3016,  3059,  3100,  3141,  3184,  3225,
  /*0080*/  3266,  3307,  3350,  3391,  3432,  3474,  3516,  3557,  3599,  3640,
  /*0090*/  3681,  3722,  3765,  3806,  3847,  3888,  3931,  3972,  4012,  4054,
  /*0100*/  4096,  4137,  4179,  4219,  4261,  4303,  4344,  4384,  4426,  4468,
  /*0110*/  4509,  4549,  4591,  4633,  4674,  4714,  4756,  4796,  4838,  4878,
  /*0120*/  4919,  4961,  5001,  5043,  5083,  5123,  5165,  5206,  5246,  5288,
  /*0130*/  5328,  5368,  5410,  5450,  5490,  5532,  5572,  5613,  5652,  5693,
  /*0140*/  5735,  5775,  5815,  5865,  5895,  5937,  5977,  6017,  6057,  6097,
  /*0150*/  6137,  6179,  6219,  6259,  6299,  6339,  6379,  6419,  6459,  6500,
  /*0160*/  6540,  6580,  6620,  6660,  6700,  6740,  6780,  6820,  6860,  6900,
  /*0170*/  6940,  6980,  7020,  7059,  7099,  7139,  7179,  7219,  7259,  7299,
  /*0180*/  7339,  7379,  7420,  7459,  7500,  7540,  7578,  7618,  7658,  7698,
  /*0190*/  7738,  7778,  7819,  7859,  7899,  7939,  7979,  8019,  8058,  8099,
  /*0200*/  8137,  8178,  8217,  8257,  8298,  8337,  8378,  8417,  8458,  8499,
  /*0210*/  8538,  8579,  8618,  8659,  8698,  8739,  8778,  8819,  8859,  8900,
  /*0220*/  8939,  8980,  9019,  9060,  9101,  9141,  9180,  9221,  9262,  9301,
  /*0230*/  9343,  9382,  9423,  9464,  9503,  9544,  9585,  9625,  9666,  9707,
  /*0240*/  9746,  9788,  9827,  9868,  9909,  9949,  9990, 10031, 10071, 10112,
  /*0250*/ 10153, 10194, 10234, 10275, 10316, 10356, 10397, 10439, 10480, 10519,
  /*0260*/ 10560, 10602, 10643, 10683, 10724, 10766, 10807, 10848, 10888, 10929,
  /*0270*/ 10971, 11012, 11053, 11093, 11134, 11176, 11217, 11259, 11300, 11340,
  /*0280*/ 11381, 11423, 11464, 11506, 11547, 11587, 11630, 11670, 11711, 11753,
  /*0290*/ 11794, 11836, 11877, 11919, 11960, 12001, 12043, 12084, 12126, 12167,
  /*0300*/ 12208, 12250, 12291, 12333, 12374, 12416, 12457, 12499, 12539, 12582,
  /*0310*/ 12624, 12664, 12707, 12747, 12789, 12830, 12872, 12914, 12955, 12997,
  /*0320*/ 13039, 13060, 13122, 13164, 13205, 13247, 13289, 13330, 13372, 13414,
  /*0330*/ 13457, 13497, 13539, 13582, 13624, 13664, 13707, 13749, 13791, 13833,
  /*0340*/ 13874, 13916, 13958, 14000, 14041, 14083, 14125, 14166, 14208, 14250,
  /*0350*/ 14292, 14335, 14377, 14419, 14461, 14503, 14545, 14586, 14628, 14670,
  /*0360*/ 14712, 14755, 14797, 14839, 14881, 14923, 14964, 15006, 15048, 15090,
  /*0370*/ 15132, 15175, 15217, 15259, 15301, 15343, 15384, 15426, 15468, 15510,
  /*0380*/ 15554, 15596, 15637, 15679, 15721, 15763, 15805, 15849, 15891, 15932,
  /*0390*/ 15974, 16016, 16059, 16102, 16143, 16185, 16228, 16269, 16312, 16355,
  /*0400*/ 16396, 16439, 16481, 16524, 16565, 16608, 16650, 16693, 16734, 16777,
  /*0410*/ 16820, 16861, 16903, 16946, 16989, 17030, 17074, 17115, 17158, 17201,
  /*0420*/ 17242, 17285, 17327, 17370, 17413, 17454, 17496, 17539, 17582, 17623,
  /*0430*/ 17667, 17708, 17751, 17794, 17836, 17879, 17920, 17963, 18006, 18048,
  /*0440*/ 18091, 18134, 18176, 18217, 18260, 18303, 18346, 18388, 18431, 18472,
  /*0450*/ 18515, 18557, 18600, 18643, 18686, 18728, 18771, 18812, 18856, 18897,
  /*0460*/ 18940, 18983, 19025, 19068, 19111, 19153, 19196, 19239, 19280, 19324,
  /*0470*/ 19365, 19408, 19451, 19493, 19536, 19579, 19621, 19664, 19707, 19750,
  /*0480*/ 19792, 19835, 19876, 19920, 19961, 20004, 20047, 20089, 20132, 20175,
  /*0490*/ 20218, 20260, 20303, 20346, 20388, 20431, 20474, 20515, 20559, 20602,
  /*0500*/ 20643, 20687, 20730, 20771, 20815, 20856, 20899, 20943, 20984, 21027,
  /*0510*/ 21071, 21112, 21155, 21199, 21240, 21283, 21326, 21368, 21411, 21454,
  /*0520*/ 21497, 21540, 21582, 21625, 21668, 21710, 21753, 21795, 21838, 21881,
  /*0530*/ 21923, 21966, 22009, 22051, 22094, 22137, 22178, 22222, 22265, 22306,
  /*0540*/ 22350, 22393, 22434, 22478, 22521, 22562, 22606, 22649, 22690, 22734,
  /*0550*/ 22775, 22818, 22861, 22903, 22946, 22989, 23032, 23074, 23117, 23160,
  /*0560*/ 23202, 23245, 23288, 23330, 23373, 23416, 23457, 23501, 23544, 23585,
  /*0570*/ 23629, 23670, 23713, 23757, 23798, 23841, 23884, 23926, 23969, 24012,
  /*0580*/ 24054, 24097, 24140, 24181, 24225, 24266, 24309, 24353, 24394, 24437,
  /*0590*/ 24480, 24523, 24565, 24608, 24650, 24693, 24735, 24777, 24820, 24863,
  /*0600*/ 24905, 24948, 24990, 25033, 25075, 25118, 25160, 25203, 25245, 25288,
  /*0610*/ 25329, 25373, 25414, 25457, 25500, 25542, 25585, 25626, 25670, 25711,
  /*0620*/ 25755, 25797, 25840, 25882, 25924, 25967, 26009, 26052, 26094, 26136,
  /*0630*/ 26178, 26221, 26263, 26306, 26347, 26390, 26432, 26475, 26516, 26559,
  /*0640*/ 26602, 26643, 26687, 26728, 26771, 26814, 26856, 26897, 26940, 26983,
  /*0650*/ 27024, 27067, 27109, 27152, 27193, 27236, 27277, 27320, 27362, 27405,
  /*0660*/ 27447, 27489, 27531, 27574, 27616, 27658, 27700, 27742, 27784, 27826,
  /*0670*/ 27868, 27911, 27952, 27995, 28036, 28079, 28120, 28163, 28204, 28246,
  /*0680*/ 28289, 28332, 28373, 28416, 28416, 28457, 28500, 28583, 28626, 28667,
  /*0690*/ 28710, 28752, 28794, 28835, 28877, 28919, 28961, 29003, 29045, 29087,
  /*0700*/ 29129, 29170, 29213, 29254, 29297, 29338, 29379, 29422, 29463, 29506,
  /*0710*/ 29548, 29589, 29631, 29673, 29715, 29757, 29798, 29840, 29882, 29923,
  /*0720*/ 29964, 30007, 30048, 30089, 30132, 30173, 30214, 30257, 30298, 30341,
  /*0730*/ 30382, 30423, 30466, 30507, 30548, 30589, 30632, 30673, 30714, 30757,
  /*0740*/ 30797, 30839, 30881, 30922, 30963, 31006, 31047, 31088, 31129, 31172,
  /*0750*/ 31213, 31254, 31295, 31338, 31379, 31420, 31461, 31504, 31545, 31585,
  /*0760*/ 31628, 31669, 31710, 31751, 31792, 31833, 31876, 31917, 31957, 32000,
  /*0770*/ 32040, 32082, 32124, 32164, 32206, 32246, 32289, 32329, 32371, 32411,
  /*0780*/ 32453, 32495, 32536, 32577, 32618, 32659, 32700, 32742, 32783, 32824,
  /*0790*/ 32865, 32905, 32947, 32987, 33029, 33070, 33110, 33152, 33192, 33234,
  /*0800*/ 33274, 33316, 33356, 33398, 33439, 33479, 33521, 33561, 33603, 33643,
  /*0810*/ 33685, 33725, 33767, 33807, 33847, 33889, 33929, 33970, 34012, 34052,
  /*0820*/ 34093, 34134, 34174, 34216, 34256, 34296, 34338, 34378, 34420, 34460,
  /*0830*/ 34500, 34542, 34582, 34622, 34664, 34704, 34744, 34786, 34826, 34866,
  /*0840*/ 34908, 34948, 34999, 35029, 35070, 35109, 35151, 35192, 35231, 35273,
  /*0850*/ 35313, 35353, 35393, 35435, 35475, 35515, 35555, 35595, 35637, 35676,
  /*0860*/ 35718, 35758, 35798, 35839, 35879, 35920, 35960, 36000, 36041, 36081,
  /*0870*/ 36121, 36162, 36202, 36242, 36282, 36323, 36363, 36403, 36443, 36484,
  /*0880*/ 36524, 36564, 36603, 36643, 36685, 36725, 36765, 36804, 36844, 36886,
  /*0890*/ 36924, 36965, 37006, 37045, 37085, 37125, 37165, 37206, 37246, 37286,
  /*0900*/ 37326, 37366, 37406, 37446, 37486, 37526, 37566, 37606, 37646, 37686,
  /*0910*/ 37725, 37765, 37805, 37845, 37885, 37925, 37965, 38005, 38044, 38084,
  /*0920*/ 38124, 38164, 38204, 38243, 38283, 38323, 38363, 38402, 38442, 38482,
  /*0930*/ 38521, 38561, 38600, 38640, 38679, 38719, 38759, 38798, 38838, 38878,
  /*0940*/ 38917, 38957, 38996, 39036, 39076, 39115, 39164, 39195, 39234, 39274,
  /*0950*/ 39314, 39353, 39393, 39432, 39470, 39511, 39549, 39590, 39628, 39668,
  /*0960*/ 39707, 39746, 39786, 39826, 39865, 39905, 39944, 39984, 40023, 40061,
  /*0970*/ 40100, 40140, 40179, 40219, 40259, 40298, 40337, 40375, 40414, 40454,
  /*0980*/ 40493, 40533, 40572, 40610, 40651, 40689, 40728, 40765, 40807, 40846,
  /*0990*/ 40885, 40924, 40963, 41002, 41042, 41081, 41119, 41158, 41198, 41237,
  /*1000*/ 41276, 41315, 41354, 41393, 41431, 41470, 41509, 41548, 41587, 41626,
  /*1010*/ 41665, 41704, 41743, 41781, 41820, 41859, 41898, 41937, 41976, 42014,
  /*1020*/ 42053, 42092, 42131, 42169
};
uint8_t addr[8], voltage[2], cjTemp[2], error, sign, i;
int16_t tcVoltage, cjTemperature, tblLo, eePntr, tempC, cjComp;
uint16_t tcBuff, tblHi, testVal;


const uint32_t tempReadDelay = 125;

uint8_t chipAddrArray[chipAddrSize] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

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

uint8_t chipBuffer[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
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

PID *pidArrayPtr[maxPIDs] = {&PID0,&PID1,&PID2,&PID3,&PID4,&PID5,&PID6,&PID7};

// End PID Stuff

//EEPROM Stuff
const int   EEPROMsize       = 2048;   // Cortex M4
const int   EEPROMidAddr     = 0x10;   // ID address to verify a previous EEPROM write
const int   EEPROMccAddr     = 0x20;   // number of chips found during findchips()
const int   EEPROMchipAddr   = 0x40;  // start address of structures
const byte  EEPROMidVal      = 0x55;   // Shows that an EEPROM update has occurred 
bool        eepromReady      = FALSE;
int         eepromSpace, eeResult, EEPROMactionAddr, EEPROMpidAddr, EEPROMtcAddr;


// Adafruit MAX31855K Thermocouple Module Stuff

const int maxMX = 4;

typedef struct
{
  bool mxEnabled;
  double mxTemp;
}chipMXstruct;

chipMXstruct eMX[maxMX]=
{
  { FALSE, 3000.0 },
  { FALSE, 3000.0 },
  { FALSE, 3000.0 },
  { FALSE, 3000.0 }
};

uint8_t mxCnt = 0;

// Multiple MAX31855K Modules may be used by paralleling mxClk and mxDO, and having separate
// mxCSx lines
const int mxDO  = 2;
const int mxCLK = 3;
const int mxCS0 = 4;
const int mxCS1 = 5;
const int mxCS2 = 6;
const int mxCS3 = 7;

// Initialize the Thermocouple
Adafruit_MAX31855 eMX0(mxCLK, mxCS0, mxDO);
Adafruit_MAX31855 eMX1(mxCLK, mxCS1, mxDO);
Adafruit_MAX31855 eMX2(mxCLK, mxCS2, mxDO);
Adafruit_MAX31855 eMX3(mxCLK, mxCS3, mxDO);

Adafruit_MAX31855 *mxArrayPtr[maxMX] = {&eMX0, &eMX1, &eMX2, &eMX3};



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

/*
  for(uint16_t i = 1010; i < kTableCnt; i++)
    {
      Serial.print(F("kTable["));
      Serial.print(i);
      Serial.print(F("] = "));
      Serial.println(pgm_read_word_near(kTable + i));
      delay(500);
    }
*/


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
    
    if((setDebug & eepromDebug) || (setDebug & allDebug))
    { 
      Serial.println(F("Getting EEPROM Data"));
    }

    chipCnt = EEPROM.read(EEPROMccAddr);

    if((setDebug & eepromDebug) || (setDebug & allDebug))
    { 
      Serial.print(F("Getting chipCnt - "));
      Serial.print(chipCnt);
      Serial.println(F(" chips found."));
    }

    readStructures();
    if((setDebug & eepromDebug) || (setDebug & allDebug))
    { 
      Serial.println(F("EEPROM Data Read Completed"));
    }
  
    eepromReady = TRUE;
    
  }else{

    if((setDebug & eepromDebug) || (setDebug & allDebug))
    { 
      Serial.println(F("Getting EEPROM Data"));
    }

    chipCnt = EEPROM.read(EEPROMccAddr);

    if((setDebug & eepromDebug) || (setDebug & allDebug))
    { 
      Serial.print(F("Getting chipCnt - "));
      Serial.print(chipCnt);
      Serial.println(F(" chips found."));
    }

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
    Serial.print( (sizeof(chipMXstruct) / sizeof(byte) ) *maxMX);
    Serial.println(F(" bytes in tc structure Array"));
  }

  Serial1.begin(baudRate);
  Serial.print(F("Serial1 Debug running at "));
  Serial.print(baudRate);
  Serial.println(F(" baud"));
  digitalWrite(waitPin, HIGH);
  digitalWrite(waitLED, HIGH);
  timer = millis();
  mxTimer = millis();
  timer2 = millis();
  
  pidSetup();
}

void loop()
{
  if(timer > (millis() + 5000)) // in case of rollover
  {
    timer = millis();
  }

  checkForSerialMessage();
  
  updateChipStatus(chipX);
  chipX++;
  if(chipX >= maxChips){chipX = 0;}
  
  checkForSerialMessage();
  
  updateActions(actionsCnt);    
  actionsCnt++;
  if(actionsCnt >= maxActions){actionsCnt = 0;}

  checkForSerialMessage();
  
  updatePIDs(pidCnt);
  pidCnt++;
  if(pidCnt >= maxPIDs){pidCnt = 0;}

  checkForSerialMessage();
 
  if(millis() > mxTimer + updateTime)
  {
    updateMX(mxCnt);
    mxCnt++;
    if(mxCnt >= maxMX){mxCnt = 0;}
    mxTimer = millis();
  }
}

void checkForSerialMessage(void)
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
  digitalWrite(waitPin, LOW);
  digitalWrite(waitLED, LOW);

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

  EEPROMactionAddr = eeResult + EEPROMchipAddr + 0x01;

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

  EEPROMpidAddr =  EEPROMactionAddr + eeResult + 0x01;

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
    Serial.println(EEPROMchipAddr, HEX);
  }

  EEPROMtcAddr = eeResult + EEPROMpidAddr + 0x01;

  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("EEPROMtcAddr = 0x"));
    Serial.println(EEPROMtcAddr, HEX);
  }

  eeResult = EEPROM_readAnything(EEPROMtcAddr, eMX);

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
    displayStructure((byte *)(uint32_t) &eMX, sizeof(eMX));
  }
  digitalWrite(waitPin, HIGH);
  digitalWrite(waitLED, HIGH);

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
  EEPROMactionAddr = eeResult + EEPROMchipAddr + 0x01;
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
  EEPROMpidAddr =  EEPROMactionAddr + eeResult + 0x01;
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
  }
  EEPROMtcAddr =  EEPROMpidAddr + eeResult + 0x01;
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("EEPROMtcAddr = 0x"));
    Serial.println(EEPROMtcAddr, HEX);
  }
  eeResult = EEPROM_writeAnything(EEPROMtcAddr, eMX);
  eepromSpace += eeResult;
  if((setDebug & eepromDebug) || (setDebug & allDebug))
  {
    Serial.print(F("Wrote "));
    Serial.print(eeResult);
    Serial.print(F(" bytes to address Ox"));
    Serial.println(EEPROMtcAddr, HEX);
    Serial.print(F("saveStructures() EEPROM Data Write of "));
    Serial.print(eepromSpace);
    Serial.println(F(" bytes Completed - Displaying chip Structures"));
    displayStructure((byte *)(uint32_t) &chip, sizeof(chip));
    Serial.println(F("Displaying action Structures"));
    displayStructure((byte *)(uint32_t) &action, sizeof(action));
    Serial.println(F(" bytes Completed - Displaying PID Structures"));
    displayStructure((byte *)(uint32_t) &ePID, sizeof(ePID));
    Serial.println(F(" bytes Completed - Displaying TC Structures"));
    displayStructure((byte *)(uint32_t) &eMX, sizeof(eMX));
    Serial.println(F("Exiting saveStructures"));
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
 int cntx = 0, cmpCnt, cmpArrayCnt, dupArray = 0;

  ds.reset_search();
  delay(250);

  while (ds.search(chip[cntx].chipAddr))
  {
    
    for(cmpCnt = 0; cmpCnt < cntx; cmpCnt++)
    {
      for(cmpArrayCnt = 0; cmpArrayCnt < chipAddrSize; cmpArrayCnt++)
      {
        if(chip[cntx].chipAddr[cmpArrayCnt] != chip[cmpCnt].chipAddr[cmpArrayCnt])
        {
          break;
        }else if(cmpArrayCnt == chipAddrSize-1){
          dupArray = 1;
        }
      }
    }
    
    if(dupArray == 1)
    {
      dupArray = 0;
      continue;
    }
    
    if(ds.crc8(chip[cntx].chipAddr, chipAddrSize-1) != chip[cntx].chipAddr[chipAddrSize-1]) continue;

    if( (setDebug & chipDebug) || (setDebug & allDebug) )
    {
      Serial.print(F("Chip "));
      Serial.print(cntx);
      Serial.print(F(" = {"));
      
      for( int i = 0; i < chipAddrSize; i++)
      {
        if(chip[cntx].chipAddr[i]>=0 && chip[cntx].chipAddr[i]<16)
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
  delay(250);
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
    
    case getChipStatus: // "5"
    {
      x = atoi((char *) &softSerialBuffer[1]);
      updateChipStatus(x);
      switch(chip[x].chipAddr[0])
      {
        case ds18b20ID:
        case ds2762ID:
        case dsT31855ID:
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
    
    case getAllStatus: // "7"
    {
      for(int x = 0; x < maxChips; x++)
      {
        switch (chip[x].chipAddr[0])
        {
          case ds18b20ID:
          case ds2762ID:
          case dsT31855ID:
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
      for( x = 0; x < maxMX; x++)
      {
        if(eMX[x].mxEnabled == TRUE)
        {
          Serial1.print(F(","));
          Serial1.print(eMX[x].mxTemp,0);
        }else{
          Serial1.print(F(",X"));
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
        case ds18b20ID: // DS18B20 Digital Thermometer
        {
          Serial1.print(F("T"));
        }
        break;
 
        case ds2406ID:// DS2406+ Digital Switch
        {
          Serial1.print(F("S"));
        }
        break;

        case ds2762ID:// DS2762 Thermocouple Sensor
        {
          Serial1.print(F("C"));
        }
        break;
        
        case dsT31855ID:
        {
          Serial1.print(F("K"));
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

    case getAllChips: // "9"
    {
      for(x = 0; x < maxChips; x++)
      {
        if(chip[x].chipAddr[0] != 0x00)
        {
          showChipInfo(x);
        }
      }
    }
    break;
    
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
      // turn off all switches and clear chip structures
      for(x=0; x<maxChips; x++)
      {
        setSwitch(x, ds2406PIOAoff);
        for(int cAcnt = 0;cAcnt < 8; cAcnt++)
        {
          chip[x].chipAddr[cAcnt] = 0x00;
        }
        chip[x].chipStatus = 0;
        chip[x].tempTimer = 0;
      }
      
      // disable and clear actions
      for(x=0; x<maxActions; x++)
      {
        action[x].actionEnabled = FALSE;
        action[x].tempPtr = NULL;
        action[x].tooCold = -255;
        action[x].tcPtr = NULL;
        action[x].tcSwitchLastState = 'F';
        action[x].tcDelay = 0;
        action[x].tcMillis = 0;
        action[x].tooHot = 255;
        action[x].thPtr = NULL;
        action[x].thSwitchLastState = 'F';
        action[x].thDelay = 0;
        action[x].thMillis = 0;
      }

      // disable and clear pid
      for(x=0; x<maxPIDs; x++)
      {
        ePID[x].pidEnabled = FALSE;
        ePID[x].tempPtr = NULL;
        ePID[x].pidSetPoint = 70;
        ePID[x].switchPtr = NULL;
        ePID[x].pidKp = 0;
        ePID[x].pidKi = 0;
        ePID[x].pidKd = 0;
        ePID[x].pidDirection = 0;
        ePID[x].pidwindowStartTime = 5000;
        ePID[x].pidInput = 0;
        ePID[x].pidOutput = 0;
        ePID[x].myPID = NULL;
      }
      
      saveStructures();

      // find new chips
      findChips();
      
      saveStructures();
      
      readStructures();

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
    
    case readMX: // "R"
    {
      x = atoi((char *) &softSerialBuffer[1]);
      Serial1.print(eMX[x].mxTemp);
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
  uint16_t chipCRCval, chipBufferCRC, noCRCmatch =1;
  
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
      
      if((chip[x].tempTimer != 0) && (millis() >= chip[x].tempTimer + tempReadDelay))
      {
        ds.reset();
        ds.select(chip[x].chipAddr);    
        ds.write(0xBE);         // Read Scratchpad
  
        for (int i = 0; i < 9; i++) 
        {
          chipBuffer[i] = ds.read();
        }
        
        if(ds.crc8(chipBuffer, 8) != chipBuffer[8]) break; // CRC invalid, try later
  
      // convert the data to actual temperature
        unsigned int raw = (chipBuffer[1] << 8) | chipBuffer[0];
        if( showCelsius == TRUE)
        {
          chip[x].chipStatus = (int) ((float)raw / 16.0);
        }else{
          chip[x].chipStatus = (int) ((((float)raw / 16.0) * 1.8) + 31.0);
        }
        chip[x].tempTimer = 0;
      }
    }
    break;
    
    case dsT31855ID:
    {
      if(chip[x].tempTimer == 0)
      {
        ds.reset();
        ds.select(chip[x].chipAddr);
        ds.write(0x44);         // start conversion, with parasite power on at the end
        chip[x].tempTimer = millis();
      }
      
      if((chip[x].tempTimer != 0) && (millis() >= chip[x].tempTimer + tempReadDelay))
      {
        ds.reset();
        ds.select(chip[x].chipAddr);    
        ds.write(0xBE);         // Read Scratchpad
  
        for (int i = 0; i < 9; i++) 
        {
          chipBuffer[i] = ds.read();
        }
        
        if(ds.crc8(chipBuffer, 8) != chipBuffer[8]) break; // CRC invalid, try later
  
      // convert the data to actual temperature
        int16_t raw = (chipBuffer[1] << 8) | chipBuffer[0];
        if( showCelsius == TRUE)
        {
          chip[x].chipStatus = (int16_t) ((float)raw * 0.25);
        }else{
          chip[x].chipStatus = (int16_t) ((((float)raw * 0.25) * 1.8) + 31.0);
        }
        chip[x].tempTimer = 0;
      }
    }
    break;
    
    case ds2406ID:
    {
      
      while(noCRCmatch)
      {
        ds.reset();
        ds.select(chip[x].chipAddr);
        ds.write(ds2406MemRd);
        chipBuffer[0] = ds2406MemRd;
        ds.write(0x0); //2406 Addr Low
        chipBuffer[1] = 0;
        ds.write(0x0); //2406 Addr Hgh
        chipBuffer[2] = 0;
        for(int i = 3; i <  13; i++)
        {
          chipBuffer[i] = ds.read();
        }
        ds.reset();

        chipCRCval = ~(ds.crc16(chipBuffer, 11)) & 0xFFFF;
        chipBufferCRC = ((chipBuffer[12] << 8) | chipBuffer[11]) ;
        if((setDebug & chipDebug) || (setDebug & allDebug))
        {
          Serial.print(F("chipBuffer = "));
          for(int i = 0; i < 13; i++)
          {
            if(chipBuffer[i] >= 0 && chipBuffer[i] <= 15)
            {
              Serial.print(F("0X0"));
            }else{
              Serial.print(F("0X"));
            }
            Serial.print(chipBuffer[i], HEX);
            if(i < 13)
            {
              Serial.print(F(", "));
            }else{
              Serial.println();
            }
          }
          Serial.print(F("chip "));
          Serial.print(x);
          Serial.print(F(" chipCRC = 0X"));
          Serial.print(chipCRCval, HEX);
          Serial.print(F(", chipBufferCRC = 0X"));
          Serial.println(chipBufferCRC, HEX);
        }
        
        if(chipBufferCRC == chipCRCval) noCRCmatch = 0;
        
        if(chipBuffer[10] & dsPIO_A)
        {
          chip[x].chipStatus = switchStatusOFF;
        }else{
          chip[x].chipStatus = switchStatusON;
        }
      }
    }
    break;
    
    case ds2762ID:
    {
      if(millis() >= chip[x].tempTimer + ds2762UpdateTime)
      {
        if((setDebug & ds2762Debug) || (setDebug & allDebug))
        {
          startTime = millis();
          Serial.println(F("Enter Read DS2762 Lookup"));
        }
        Read_TC_Volts(x);
        Read_CJ_Temp(x);
        cjComp = pgm_read_word_near(kTable + cjTemperature);
        if((setDebug & ds2762Debug) || (setDebug & allDebug))
        {
          Serial.print(F("kTable["));
          Serial.print(cjTemperature);
          Serial.print(F("] = "));
          Serial.println(pgm_read_word_near(kTable + cjTemperature));
        }
        if(sign == 1)
        {
          if(tcVoltage < cjComp)
          {
            cjComp -= tcVoltage;
          }else{
            cjComp = 0;
          }
        }else{
          cjComp += tcVoltage;
        }
        if((setDebug & ds2762Debug) || (setDebug & allDebug))
        {
          Serial.print(F("cjComp = "));
          Serial.print(cjComp);
          Serial.println(F(" microvolts"));
        }
        tblHi = kTableCnt - 1;
        TC_Lookup();
        if(error == 0)
        {
          if((setDebug & ds2762Debug) || (setDebug & allDebug))
          {
            Serial.print(F("Temp = "));
            Serial.print(eePntr);
            Serial.print(F(" degrees C, "));
            Serial.print(((eePntr * 9) / 5) + 32);
            Serial.println(F(" degrees F"));
          }
          if(showCelsius == TRUE)
          {
            chip[x].chipStatus = eePntr;
          }else{
            chip[x].chipStatus = ((eePntr * 9) / 5) + 32;
          }
        }else{
          if((setDebug & ds2762Debug) || (setDebug & allDebug))
          {
            Serial.println(F("Value Out Of Range"));
          }
        }
        if((setDebug & ds2762Debug) || (setDebug & allDebug))
        {
          endTime = millis();
          Serial.print(F("Exit Read DS2762 Lookup - "));
          Serial.print(endTime - startTime);
          Serial.println(F(" milliseconds"));
          Serial.println();
          Serial.println();
        }
        chip[x].tempTimer = millis() + tempReadDelay;
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
  digitalWrite(waitPin, LOW);
  digitalWrite(waitLED, LOW);

  if((setDebug & serial1Debug) || (setDebug & allDebug))
  {
    Serial.print(F("updating Action: "));
    Serial.print(x);
  }
  
  if(action[x].actionEnabled == TRUE)
  {
    if(action[x].tempPtr->chipStatus <= action[x].tooCold)
    {
      if((setDebug & serial1Debug) || (setDebug & allDebug))
      {
        Serial.println(F(" - TOO COLD"));
      }
    }else if(action[x].tempPtr->chipStatus >= action[x].tooHot){
      if((setDebug & serial1Debug) || (setDebug & allDebug))
      {
        Serial.println(F(" - TOO HOT"));
      }
    }else{
      if((setDebug & serial1Debug) || (setDebug & allDebug))
      {
        Serial.println(F(" - JUST RIGHT"));
      }
    }
    if(action[x].tempPtr->chipStatus <= action[x].tooCold &&
       action[x].tcPtr->chipStatus == switchStatusOFF) // too cold
    {
      if(action[x].tcDelay == 0 || millis() > (action[x].tcMillis + action[x].tcDelay))
      {
        actionSwitchSet((uint8_t *) &action[x].tcPtr->chipAddr, ds2406PIOAon);
        
      }
    }else if(action[x].tempPtr->chipStatus > action[x].tooCold &&
             action[x].tcPtr->chipStatus == switchStatusON){
               
      actionSwitchSet((uint8_t *) &action[x].tcPtr->chipAddr, ds2406PIOAoff);
      action[x].tcMillis = millis();
       
    }

    if(action[x].tempPtr->chipStatus >= action[x].tooHot &&
       action[x].thPtr->chipStatus == switchStatusOFF) //too hot
    {
      if(action[x].thDelay == 0 || millis() > (action[x].thMillis + action[x].thDelay))
      {
        actionSwitchSet((uint8_t *) &action[x].thPtr->chipAddr, ds2406PIOAon);
        
      }
    }else if(action[x].tempPtr->chipStatus < action[x].tooHot &&
             action[x].thPtr->chipStatus == switchStatusON){
               
      actionSwitchSet((uint8_t *) &action[x].thPtr->chipAddr, ds2406PIOAoff);
      action[x].thMillis = millis();
               
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

void updateMX(uint8_t mxCnt)
{
  if((setDebug & tcProbeDebug) || (setDebug & allDebug))
  {
    startTime = millis();
    Serial.println(F("updateMX enter"));
  }
  if(eMX[mxCnt].mxEnabled == TRUE)
  {
    eMX[mxCnt].mxTemp = mxArrayPtr[mxCnt]->readFarenheit();
    if((setDebug & tcProbeDebug) || (setDebug & allDebug))
    {
      Serial.print(F("eMX["));
      Serial.print(mxCnt);
      Serial.print(F("].mxtemp = "));
      if(!isnan(eMX[mxCnt].mxTemp))
      {
        Serial.println(eMX[mxCnt].mxTemp);
      }else{
        Serial.println(F("NaN"));
      }
    }  
  }
  if((setDebug & tcProbeDebug) || (setDebug & allDebug))
  {
    endTime = millis();
    Serial.print(F("updateMX exit - "));
    Serial.print(endTime - startTime);
    Serial.println(F(" milliseconds"));
    Serial.println();
    Serial.println();
  }
}

void Read_TC_Volts(uint8_t x)
{ 
  if((setDebug & ds2762Debug) || (setDebug & allDebug))
  {
    Serial.println(F("Enter Read_TC_Volts"));
  }
  ds.reset();
  ds.select(chip[x].chipAddr);
  ds.write(0x69); //read voltage
  ds.write(0x0e);
  for (i = 0; i < 2; i++)
  {
    voltage[i] = ds.read();
    if((setDebug & ds2762Debug) || (setDebug & allDebug))
    {
      Serial.print(F("voltage["));
      Serial.print(i);
      Serial.print(F("] = 0x"));
      if(voltage[i] < 0x10){Serial.print(F("0"));}
      Serial.print(voltage[i], HEX);
      Serial.print(F(" "));
    }
  }
  if((setDebug & ds2762Debug) || (setDebug & allDebug))
  {
    Serial.println();
  }
  ds.reset();
  tcVoltage = (voltage[0] << 8) + voltage[1];
  tcVoltage >>= 3; 
  if((voltage[0] & 0x80) == 0x80)
  {
    sign = 1;
    tcVoltage |= 0xF000;
    tcVoltage = ~tcVoltage;
    tcVoltage += 1;
  }else{
    sign = 0;
  }
  tcBuff = tcVoltage * 15;
  tcVoltage *= 5;
  tcVoltage >>= 3;
  tcVoltage += tcBuff;
  
  if((setDebug & ds2762Debug) || (setDebug & allDebug))
  {
    Serial.print(F("tcVoltage = "));
    Serial.print(tcVoltage);
    Serial.println(F(" microvolts"));
    Serial.println(F("Exit Read_TC_Volts"));
  }
} 

/* Reads cold junction (device) temperature 
-- each raw bit = 0.125 degrees C 
-- returns tmpCJ in whole degrees C */ 
void Read_CJ_Temp(uint8_t x)
{ 
  if((setDebug & ds2762Debug) || (setDebug & allDebug))
  {
    Serial.println(F("Enter Read_CJ_Temp"));
  }
  ds.reset();
  ds.select(chip[x].chipAddr);
  ds.write(0x69);
  ds.write(0x18); //read cjTemp
  for (i = 0; i < 2; i++)
  {
    cjTemp[i] = ds.read();
    if((setDebug & ds2762Debug) || (setDebug & allDebug))
    {
      Serial.print(F("cjTemp["));
      Serial.print(i);
      Serial.print(F("] = 0x"));
      if(cjTemp[i] < 0x10){Serial.print(F("0"));}
      Serial.print(cjTemp[i], HEX);
      Serial.print(F(" "));
    }
  }
  if((setDebug & ds2762Debug) || (setDebug & allDebug))
  {
    Serial.println();
  }
  ds.reset();
  cjTemperature = (cjTemp[0] << 8) + cjTemp[1];
  if(cjTemperature>=0x8000)
  { 
    cjTemperature = 0;
//    cjdTemperature = 0.0; // disallow negative 
  }else{
//    cjdTemperature =  (double) ((double) cjTemperature) * .125;
    cjTemperature >>= 8;
  } 
  if((setDebug & ds2762Debug) || (setDebug & allDebug))
  {
    Serial.print(F("cjTemperature = "));
    Serial.print(cjTemperature);
    Serial.print(F(" degrees C, "));
    Serial.print(((cjTemperature * 9) / 5) + 32);
    Serial.println(F(" degrees F")); 
    Serial.println(F("Exit Read_CJ_Temp"));
  }
} 

/* Search currently selected TC table for nearest entry 
-- uses modified binary algorithm to find cjComp 
-- high end of search set before calling (tblHi) 
-- successful search sets tempC */ 
void TC_Lookup(void)
{ 
  if((setDebug & ds2762Debug) || (setDebug & allDebug))
  {
    Serial.println(F("Enter TC_Lookup"));
  }
  tblLo=0; // low entry of table 
  tempC=22; // default to room temp
  testVal=pgm_read_word_near(kTable + tblHi); // check max temp
  if(cjComp>testVal)
  { 
    error=1; // out of range 
  }else{ 
    while(1)
    { 
      eePntr=(tblLo+tblHi)/2; // midpoint of search span 
      testVal=pgm_read_word_near(kTable + eePntr); // read value from midpoint
      if((setDebug & ds2762Debug) || (setDebug & allDebug))
      {
        Serial.print(F("testVal = "));
        Serial.print(testVal);
      }
      if(cjComp == testVal)
      {
        if((setDebug & ds2762Debug) || (setDebug & allDebug))
        {
          Serial.println(F(" - TC_Lookup Temp Match"));
        }
        tempC = eePntr;
        return; // found it! 
      }else{
        if(cjComp<testVal)
        {
          if((setDebug & ds2762Debug) || (setDebug & allDebug))
          {
             Serial.println(F(" - testVal too BIG"));
          }
         tblHi=eePntr;
        }else{
          if((setDebug & ds2762Debug) || (setDebug & allDebug))
          {
             Serial.println(F(" - testVal too small"));
          }
         tblLo=eePntr;
        } // search upper half
      }
      if((setDebug & ds2762Debug) || (setDebug & allDebug))
      {
        Serial.print(F("tblHi = "));
        Serial.print(tblHi);
        Serial.print(F(", tblLo = "));
        Serial.println(tblLo);
      }
      if((tblHi-tblLo)<2)
      { // span at minimum 
        if((setDebug & ds2762Debug) || (setDebug & allDebug))
        {
          Serial.println(F("TC_Lookup Temp Span At Minimum"));
        }
        eePntr=tblLo; 
        return; 
      } 
    } 
  }
} 

