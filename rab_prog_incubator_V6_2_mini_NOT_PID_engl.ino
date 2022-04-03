#include <DS1307new.h>                                          // библиотека часов реального времени
#include <OLED_I2C.h>                                           // библиотека дисплея OLED_I2C 0,96`
#include <Wire.h>                                               // библиотека работы с протоколом I2C
#include <OneWire.h>                                            // библиотека протокола OneWire для работы с датчиком температуры
#include "DHT.h"                                                // библиотека для работы с датчиком вляжности DHT11 или DHT22
#include <EEPROM.h>                                             // библиотека для работы с энергонезависимой памятью EEPROM

#define DHTPIN 1                                                // датчик влажности на 1 цифровом входе
#define DHTTYPE DHT22                                           // типа датчика вляжности DHT22 

#define DS18B20PIN 0                                            // датчик температуры DS18b20 на 0 входе
#define DS18S20_ID 0x10
#define EEPROM_ADDR 0x50                                        // EEPROM по адресу 0х50

const int ResistantKey[6] {0, 83, 250, 368, 493, 570};          // объявление массива с записанными в него возвратными напряжениями после срабатывания кнопок клавиатуры

OneWire ds(DS18B20PIN);                                         // инициализация DS18b20 на 0 входе   
OLED  myOLED(SDA, SCL, 8);                                      // инициализация дисплея OLED
extern uint8_t SmallFont[];                                     // загрузка мелкого шрифта
//extern uint8_t RusFont[];                                     // загрузка кирилического шрифта
extern uint8_t MediumNumbers[];                                 // загрузка среднего шрифта цифр
extern uint8_t MegaNumbers[];                                   // загрузка большого шрифта цифр
DHT dht(DHTPIN, DHTTYPE);                                       // инициализация DHT на 1 выходе

//*************************************************        объявление глобальных переменных и деректив      *************************************************
//*************************************************    переменные и дерективы  для работы с температурой    *************************************************
double TempIncu  = 37.4;                                        // объявление глобальной переменной температуры инкубации и ее первоначального значения
double deltaTemp = 0.2;                                         // объявление глобальной переменной дельта температуры инкубации и ее первоначального значения
double TempDifference = 1;                                      // объявление глобальной переменной разности температур включения укоренного нагрева
int TimeSWH = 30;                                               // объявление глобальной переменной времени автоматического поочередного переключения обогревающих элементов ( в сек )
double TempFan   = 37.9;                                        // объявление глобальной переменной температуры аварийной продувки и ее первоначального значения
double TnowDS    = 37.5;                                        // объявление глобальной переменной реальной температуры на данный момент в инкубаторе на DS18b20
bool FanTempFlag = 1;                                           // объявление глобальной переменной состояния активности продувки инкубатора по перегреву
//************************************************* переменные и дерективы для работы с температурой в меню *************************************************
int bankLoad = 0;                                               // номер банка с которого будут грузиться первоначальные настройки
#define maxTempIncu 60                                          // объявление дериктивы максимального значения температуры для меню
#define minTempIncu 15                                          // объявление дериктивы минимального значения температуры для меню
#define maxDTemp 5                                              // объявление дериктивы максимального значения дельта температуры для меню
#define minDTemp 0.1                                            // объявление дериктивы минимального значения дельта температуры для меню
#define FadeTemp 0.1                                            // объявление дериктивы шага изменения тепертуры в меню
#define maxTimeSWH 300                                          // объявление дириктивы максимального значения времени между переключениями обогревателей для меню
#define minTimeSWH 20                                           // объявление дириктивы минимального значения времени между переключениями обогревателей для меню   
#define FadeTimeSW 1                                            // объявление дириктивы шага изменения времени автоматического переключения между элементами обогрева для меню                              
//*************************************************     переменные и дерективы  для работы с влажностью     *************************************************
int HumiIncu = 56;                                              // объявление глобальной переменной влажности инкубации и ее первоначального значения
int deltaHum = 1;                                               // объявление глобальной переменной дельта H инкубации и ее первоначального значения
int HnowDH= 56;                                                 // объявление глобальной переменной ральной влажности на данный момент в инкубаторе на DHT11
//*************************************************  переменные и дерективы для работы с влажностью в меню  *************************************************
#define MaxHumIncu 90                                           // объявление дериктивы максимального значения влажности для меню
#define MinHumIncu 10                                           // объявление дериктивы минимального значения влажности для меню
#define minDHum 2                                               // объявление дериктивы максимального значения дельтаH для меню
#define maxDHum 10                                              // объявление дериктивы минимального значения дельтаH для меню
#define FadeHum 2                                               // объявление дериктивы шага изменения влажности
//*************************************************  переменные и дерективы для  работы с продувкой от СО2  *************************************************
int TimeFan = 10;                                               // объявление глобальной переменной длительности проветривания инкубатора от СО2 в секундах
int TimeIntervalFan = 5;                                        // объявление глобальной переменной интервала между проветриваниями инкубатора от СО2 в минутах
bool FanTimeFlag = 1;                                           // объявление глобальной переменной состояния активности продувки инкубатора от СО2
//************************************************переменные и дерективы для работы с продувкой от СО2 в меню************************************************
#define maxTimeFan 180                                          // объявление дериктивы максимального время проветривания инкубатора от СО2 для меню в секундах
#define minTimeFan 5                                            // объявление дериктивы минимального время проветривания инкубатора от СО2 для меню в секундах
#define FadeTimeFan 5                                           // объявление дериктивы шага изменения значения времени работы вентилятора в меню в секундах
#define maxTimeIntervalFan 300                                  // объявление дериктивы максимального интервала между включениями проветривания инкубатора от СО2 для меню в минутах
#define minTimeIntervalFan 1                                    // объявление дериктивы минимального интервала между включениями проветривания инкубатора от СО2 для меню в минутах
#define FadeTimeIntervalFan 1                                   // объявление дериктивы шага изменения значения интервала между включениями проветривания инкубатора от СО2 в меню в минутах
//*************************************************      переменные и дерективы для с переворота лотка      *************************************************
int Rot = 8;                                                    // объявление глобальной переменной колличества переворотов яиц в инкубаторе в сутки
const int RotPerDay[9] = {0,1,2,3,4,6,8,12,24};                 // объявление массива колличество возможных переворотов яиц в сутки
bool RotFlag = 0;                                               // объявление глобальной переменной состояния активности переворота при переходе между состояниями
bool RotWork = 0;                                               // объявление глобальной переменной состояния активности переворота 
bool RotFlagTime = 0;                                           // объявление глобальной переменной состояния активности переворота по времени
bool RotFlagButt = 0;                                           // объявление глобальной переменной состояния активности переворота по кнопке
int RotPosition = 2;                                            // объявление глобальной переменной положения лотка яиц ( 1 - еще в зоне действия первого конечника, 2 - уже не в 1 но еще и не во 2 конечнике, 3 - в зоне действия 3 конечника
//*************************************************переменные и дерективы для назначения портов входа/выхода*************************************************/*
#define PinBUT 15                                               // подключение кнопок меню на 1 аналоговом входе
#define PinHT1 13                                               // подключение элемента 1 обогрева на выход 13
#define PinHT2 12                                               // подключение элемента 2 обогрева на выход 13
#define PinROT 11                                               // подключение двигателя переворота на выход 11
#define PinHUM 10                                               // подключение увлажнителя на выход 10
#define PinLS2 9                                                // подключение конечника #2 лотка яиц на 9 вход  
#define PinLS1 8                                                // подключение конечника #1 лотка яиц на 8 вход
#define PinFAN 6                                                // подключение обдува на выход 6
bool Hotter;                                                    // переменная отвечающая за почередность включения элементов обогрева                         
//*************************************************переменные и дерективы для клавиатуры, меню и сост блока *************************************************
bool OnOfDevice = 1;                                            // объявление глобальной переменной состояния блока (включен/выключен)
int MainMenu = 1;                                               // объявление глобальной переменной для главного меню
int Marker = 1;                                                 // объявление глобальной переменной положения маркера
int SimvolMarker = 1;                                           // объявление глобальной переменной положения маркера необходимого для редактирования даты и времени
bool WriteMarker = 0;                                           // объявление глобальной переменной маркера редактирования параметров
int NumberButton;                                               // объявление глобальной переменной номера нажатой кнопки кнопочного модуля
int a, b, c, d;                                                 // объявление глобальных переменных необходимых для вертикального сдвига строчек в меню
unsigned long currentMillis;
unsigned long StartMillis = 0;                                  // счетчик прошедшего времени для AutoStartMenu
#define interval 5000                                           // задержка автовозврата к StartMenu 5сек
unsigned long ds18b20readMillis = 0;                            // счетчик прошедшего времени для интервала чтения с датчика температуры
#define ds18b20interval 1000                                    // опрос датчика температуры каждую сек
unsigned long dht11readMillis = 0;                              // счетчик прошедшего времени для интервала чтения с датчика влажности
#define dht11interval 2000                                      // опрос датчика влажности каждые 2 сек
unsigned long TimeFaning = 0;                                   // счетчик прошедшего времени для продувки инкубатора от СО2
unsigned long TimeIntervalFaning = 0;                           // счетчик прошедшего времени для интервала между продувками инкубатора от СО2
unsigned long TimeFaningInterval = TimeFan * 1000;              // длительность работы вентилятора при продувке инкубатора от СО2 
unsigned long TimeIntervalFaningInterval=TimeIntervalFan*60000; // длительность интервала между продувками инкубатора от СО2
unsigned long TimeIntervalSWH = TimeSWH * 1000;                 // автоматическое поочередное переключение обогревающих элементов
unsigned long TimerSWH = 0;                                     // счетчик времени автоматического поочередного переключения ламп обогрева
int NOWyear;                                                    // объявление глобальной переменной для сохранения года при редактировании даты и времени
int NOWmonth;                                                   // объявление глобальной переменной для сохранения месяца при редактировании даты и времени
int NOWday;                                                     // объявление глобальной переменной для сохранения дня при редактировании даты и времени
int NOWhour;                                                    // объявление глобальной переменной для сохранения часа при редактировании даты и времени
int NOWminute;                                                  // объявление глобальной переменной для сохранения минуты при редактировании даты и времени
int NOWsecond;                                                  // объявление глобальной переменной для сохранения секунды при редактировании даты и времени
int Bank = 0;                                                   // объявление глобальной переменной номера ячейки для загрузки или сохранения параметров инкубации
int i = 6;                                                      // объявление глобальной переменной номера ячейки массива возможного колличества переворотов в сутки

void setup ()
{
  //Serial.begin(9600);                                         // инициализация общения по UART со скоростью 9600бод
  Wire.begin();                                                 // инициализация шини I2C
  dht.begin();                                                  // инициализация датчика влажности
  myOLED.begin();                                               // инициализация дисплея
 
  pinMode(PinFAN, OUTPUT);                                      // установка выхода 6  МК в режим выхода - обдув
  pinMode(PinBUT,  INPUT);                                      // установка выхода 15 МК в режим входа  - кнопки меню
  pinMode(PinLS1,  INPUT);                                      // установка выхода 8  МК в режим входа  - конечник лотка #1 
  pinMode(PinLS2,  INPUT);                                      // установка выхода 9  МК в режим входа  - конечник лотка #2
  pinMode(PinHT2, OUTPUT);                                      // установка режима 10 МК в режим выхода - обогрев рзагонный
  pinMode(PinROT, OUTPUT);                                      // установка режима 11 МК в режим выхода - переворот
  pinMode(PinHUM, OUTPUT);                                      // установка режима 12 МК в режим выхода - увлажнение
  pinMode(PinHT1, OUTPUT);                                      // установка режима 13 МК в режим выхода - обогрев рабочий
  
  //RTC.stopClock();                
  //RTC.fillByYMD ( 2016 , 3 , 1 ); 
  //RTC.fillByHMS ( 14 , 40 , 0 );  
  //RTC.setTime();                  
  //RTC.startClock();               
 
//*************************************чтение из EEPROM первоначальных установок настройки инкубатора****************************************
  /*
  myOLED.clrScr();                               //  _________________ 
  myOLED.setFont(SmallFont);                     // |                 |
  //myOLED.setFont(RusFont);                     // |    LOADING      |
  myOLED.print ( "LOADING", CENTER, CENTER );    // |                 |
  myOLED.update();                               //  -----------------
  */
  
  bankLoad = EEPROM.read ( 100 ); delay;                                                   // чтение номера банка с которого должна производиться загрузка данных
  int BL; byte lowByte; byte higtByte;
  BL = bankLoad * 20 + 0;     lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte температуры инкубации *100 из ячейки "0"
  BL = bankLoad * 20 + 1;     higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte температуры инкубации *100 из ячейки "1"
    TempIncu = ( ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 ) ) / 100.00 ;
  BL = bankLoad * 20 + 2;     lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte дельта температуры инкубации *100 из ячейки "2"
  BL = bankLoad * 20 + 3;     higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte дельта температуры инкубации *100 из ячейки "3"
    deltaTemp = ( ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 ) ) / 100.00 ;
  BL = bankLoad * 20 + 4;     lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte значения резницы температур включения форсажного нагрева *100 из ячейки "4"
  BL = bankLoad * 20 + 5;     higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte значения резницы температур включения форсажного нагрева *100 из ячейки "5"
    TempDifference = ( ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 ) ) / 100.00 ;
  BL = bankLoad * 20 + 6;     TimeSWH  = EEPROM.read(BL);                 delay (100);     // чтение интервала между автоматическим переключением элементов обогрева из ячейки "6"
  BL = bankLoad * 20 + 8;     HumiIncu = EEPROM.read(BL);                 delay (100);     // чтение влажности инкубации из ячейки "8"
  BL = bankLoad * 20 + 9;     deltaHum = EEPROM.read(BL);                 delay (100);     // чтение дельты влажности инкубации из ячейки "9"
  BL = bankLoad * 20 + 10;    Rot      = EEPROM.read(BL);                 delay (100);     // чтение времени через которое происходит переворот яиц из ячейки "10"
  BL = bankLoad * 20 + 11;    lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte температуры продувки *100 из ячейки "11"
  BL = bankLoad * 20 + 12;    higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte температуры продувки *100 из ячейки "12"
    TempFan = ( ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 ) ) / 100.00 ;
  BL = bankLoad * 20 + 13;    lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte времени вентиляции инкубатора от СО2 из ячейки "13"
  BL = bankLoad * 20 + 14;    higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte времени вентиляции инкубатора от СО2 из ячейки "14"
    TimeFan = ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 );
  BL = bankLoad * 20 + 15;    lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte интервала между вентиляциями инкубатора от СО2 из ячейки "15"
  BL = bankLoad * 20 + 16;    higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte интервала между вентиляциями инкубатора от СО2 из ячейки "16"
    TimeIntervalFan = ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 );
  TimeIntervalSWH = TimeSWH * 1000;                                                        // вычисление интервала между автоматическим переключением элементов обогрева 
  TimeFaningInterval = TimeFan * 1000;                                                     // вычисление длительности работы вентилятора при продувке инкубатора от СО2 
  TimeIntervalFaningInterval = TimeIntervalFan * 60000;                                    // вычисление длительности интервала между продувками инкубатора от СО2
//***************************************            чтение из EEPROM состояния блока ВКЛЮЧЕН/ВЫКЛЮЧЕН            *******************************************
  OnOfDevice  = EEPROM.read(112); delay (100);                  // чтение состояния блока ВКЛЮЧЕН/ВЫКЛЮЧЕН
//***************************************  первоначальное выключение всех исполнительных устройств после загрузки *******************************************
  digitalWrite (PinFAN, HIGH);                                  // первоначальная подача логической "1" ( состояние -выключено- ) на выход №6  ( обдув         ) при загрузке 
  digitalWrite (PinHT2, HIGH);                                  // первоначальная подача логической "1" ( состояние -выключено- ) на выход №10 ( обогрев разгон) при загрузке   
  digitalWrite (PinROT, HIGH);                                  // первоначальная подача логической "1" ( состояние -выключено- ) на выход №11 ( переворот     ) при загрузке  
  digitalWrite (PinHUM, HIGH);                                  // первоначальная подача логической "1" ( состояние -выключено- ) на выход №12 ( увлажнение    ) при загрузке  
  digitalWrite (PinHT1, HIGH);                                  // первоначальная подача логической "1" ( состояние -выключено- ) на выход №13 ( обогрев рабоч ) при загрузке 
//***************************************                            приветствие                                  *******************************************
//  myOLED.clrScr();
//  myOLED.update();                                 //  _________________      
//  myOLED.print( "control unit of",    CENTER, 0 ); // | control unit of |
//  myOLED.print( "incubator",          CENTER, 8 ); // |   incubator     |
//  myOLED.print( "by Sereda A A",      CENTER, 18); // |  by Sereda A A  |
//  myOLED.print( "vers.  6_3",         CENTER, 26); // |   vers.  6_3    |
//  myOLED.update();                                 //  -----------------
//  delay (3000);
//  myOLED.clrScr();
//  myOLED.update();
}
//***************************************                 обработка нажатия кнопок клавиатуры                     *******************************************
void PressKeyMenu ()
{
  NumberButton = 0;
  int buttons_Menu = analogRead(PinBUT);
  //Serial.println (  buttons_Menu );
  if ( buttons_Menu < 15 ) NumberButton = 1; else { for ( int i = 1; i <=5; i++ ) { int r = ResistantKey [ i ]; if ( buttons_Menu > ( r - 30 ) && buttons_Menu < ( r + 30 ) ) { NumberButton = i + 1; break; } } }
  delay (50);
}

//***************************************                      прорисовка больших часов                           *******************************************
void TimePrintBig ()
{
  myOLED.setFont(MegaNumbers);
  PrintZerro ( RTC.hour, 4,  13, 30  );
  myOLED.print ( "/" ,   CENTER, 13  );
  PrintZerro ( RTC.minute, 75, 13, 100 );
}
void PrintZerro ( int z, int x, int y , int xx )
{
  if ( z < 10 ) { myOLED.print ( "0", x, y ); myOLED.printNumI ( z, xx, y ); }
  else myOLED.printNumI ( z, x, y ); 
}
//***************************************                      прорисовка малой даты                              *******************************************
void DatePrintLit ()
{
  myOLED.setFont(SmallFont);
  PrintZerro ( RTC.day,      41, 56, 47 );
  myOLED.print     ( ".",        53, 56 );
  PrintZerro ( RTC.month,    59, 56, 65 );
  myOLED.print     ( ".",        71, 56 );
  myOLED.printNumI ( RTC.year,   77, 56 );
  //myOLED.setFont   ( RusFont );
  
  //switch (RTC.dow){
  //case 1:     myOLED.print( "MO", 23, 56); break;     // "MO"
  //case 2:     myOLED.print( "TU", 23, 56); break;     // "TU"
  //case 3:     myOLED.print( "WE", 23, 56); break;     // "WE"
  //case 4:     myOLED.print( "TH", 23, 56); break;     // "TH"
  //case 5:     myOLED.print( "FR", 23, 56); break;     // "FR"
  //case 6:     myOLED.print( "SA", 23, 56); break;     // "SA"
  //case 7:     myOLED.print( "SU", 23, 56); break;}    // "SU"
  
}
//***************************************                       прорисовка малых часов                            *******************************************
void TimePrintLit()
{
  myOLED.setFont(SmallFont);
  PrintZerro ( RTC.hour,     41, 56, 47 );
  myOLED.print     ( ":" ,       53, 56 );
  PrintZerro ( RTC.minute,   59, 56, 65 ); 
  myOLED.print     ( ":" ,       71, 56 );
  PrintZerro ( RTC.second,   77, 56, 83 );
}
//***************************************                       прорисовка большой тепературы                     *******************************************
void TempPrintBig()
{
  //myOLED.setFont   ( RusFont );
  myOLED.print     ( "TEMPERATURE", CENTER, 0 );        // TEMPERATERE
  //myOLED.drawBitmap ( 0, 16, gradusnic, 19, 40 );
  myOLED.setFont   ( MegaNumbers );
  //myOLED.printNumF ( float( TnowDS ), 1, 20, 13 );
  myOLED.printNumF ( float( TnowDS ), 1, CENTER, 13 );
}
//***************************************                       прорисовка  малой  тепературы                     *******************************************
/*
 void TempPrintLit()
{
  myOLED.setFont   ( RusFont );
  myOLED.print     ( "NTVGTHFNEHF", LEFT, 0 );
  myOLED.setFont ( SmallFont );
  myOLED.printNumF ( float( TnowDS ), 1, 72, 0 );
}
*/
//***************************************                       прорисовка большой влажности                      *******************************************
void HumiPrintBig()
{
  //myOLED.setFont ( RusFont );
  myOLED.print ( "HUMIDITY", CENTER, 0 );          // HUMIDITY
  //myOLED.drawBitmap ( 0, 20, kaplya, 19, 33 );
  myOLED.setFont ( MegaNumbers );
  //myOLED.printNumI ( HnowDH, 20, 13 );
  myOLED.printNumI ( HnowDH, CENTER, 13 );
}
//***************************************                       прорисовка  малой  влажности                      *******************************************

/*
void HumiPrintLit()
{
  myOLED.setFont ( RusFont );
  myOLED.print ( "DKF:YJCNM", LEFT, 8 );
  myOLED.setFont ( SmallFont );
  myOLED.printNumI ( HnowDH, 72, 8 );
}
*/
//***************************************               сохранение текущего времени для редактриования            *******************************************
void SaveTime ()
{
  NOWyear = RTC.year; NOWmonth = RTC.month; NOWday = RTC.day; NOWhour = RTC.hour; NOWminute = RTC.minute; NOWsecond = RTC.second;
}
//***************************************               запись отредактированного времени и даты в РТЦ            *******************************************
void SetupTime ()
{
  RTC.getTime();  RTC.stopClock();
  RTC.fillByYMD ( NOWyear , NOWmonth , NOWday );      delay (250); 
  RTC.fillByHMS ( NOWhour , NOWminute , NOWsecond );  delay (250); 
  RTC.setTime();  delay(200);  RTC.startClock();
}
//***************************************                   запись настроек инкубатора в ЕЕПРОМ                   *******************************************
void SaveToEEPROM ( int BankSave )                                                         
{
    int TIC = int ( TempIncu * 100 ); byte lowByte = ( ( TIC >> 0 ) & 0xFF ); byte higtByte = ( ( TIC >> 8 ) & 0xFF ); int BS;
  BS = BankSave * 20 + 0;     EEPROM.write(BS, lowByte);                  delay (100);     // запись lowByte температуры инкубации * 100 в ячейку "0" банка "bank"
  BS = BankSave * 20 + 1;     EEPROM.write(BS, higtByte);                 delay (100);     // запись higtByte температуры инкубации * 100 в ячейку "1" банка "bank"
    TIC = int ( deltaTemp * 100 ); lowByte = ( ( TIC >> 0 ) & 0xFF ); higtByte = ( ( TIC >> 8 ) & 0xFF ); 
  BS = BankSave * 20 + 2;     EEPROM.write(BS, lowByte);                  delay (100);     // запись lowByte дельта температуры инкубации * 100 в ячейку "2" банка "bank"
  BS = BankSave * 20 + 3;     EEPROM.write(BS, higtByte);                 delay (100);     // запись higtByte дельта темпертаруры инкубации * 100 в ячейку "3" банка "bank"
    TIC = int ( TempDifference * 100 ); lowByte = ( ( TIC >> 0 ) & 0xFF ); higtByte = ( ( TIC >> 8 ) & 0xFF ); 
  BS = BankSave * 20 + 4;     EEPROM.write(BS, lowByte);                  delay (100);     // запись lowByte значения резницы температур включения форсажного нагрева * 100 в ячейку "4" банка "bank"
  BS = BankSave * 20 + 5;     EEPROM.write(BS, higtByte);                 delay (100);     // запись higtByte значения резницы температур включения форсажного нагрева * 100 в ячейку "5" банка "bank"
  BS = BankSave * 20 + 6;     EEPROM.write(BS, TimeSWH);                  delay (100);     // запись интервала между автоматическим переключением элементов обогрева в ячейку "6" банка "bank"
  BS = BankSave * 20 + 8;     EEPROM.write(BS, HumiIncu);                 delay (100);     // запись влажности инкубации в ячейку "8" банка "bank"
  BS = BankSave * 20 + 9;     EEPROM.write(BS, deltaHum);                 delay (100);     // запись дельты влажности инкубации в ячейку "9" банка "bank"
  BS = BankSave * 20 + 10;    EEPROM.write(BS, Rot);                      delay (100);     // запись колличества переворотов лотка яиц в сутки в ячейку "10" банка "bank"
    int TICc = int ( TempFan * 100 ); lowByte = ( ( TICc >> 0 ) & 0xFF ); higtByte =  ( ( TICc >> 8 ) & 0xFF );
  BS = BankSave * 20 + 11;    EEPROM.write(BS, lowByte);                  delay (100);     // запись lowByte температуры продувки * 100 в ячейку "11" банка "bank"
  BS = BankSave * 20 + 12;    EEPROM.write(BS, higtByte);                 delay (100);     // запись higtByte температуры продувки * 100 в ячейку "12" банка "bank"
    lowByte = ( ( TimeFan >> 0 ) & 0xFF ); higtByte =  ( ( TimeFan >> 8 ) & 0xFF );
  BS = BankSave * 20 + 13;    EEPROM.write(BS, lowByte);                  delay (100);     // запись lowByte времени вентиляции инкубатора от СО2 в ячейку "13" банка "bank"
  BS = BankSave * 20 + 14;    EEPROM.write(BS, higtByte);                 delay (100);     // запись higtByte времени вентиляции инкубатора от СО2 в ячейку "14" банка "bank"
    lowByte = ( ( TimeIntervalFan >> 0 ) & 0xFF ); higtByte =  ( ( TimeIntervalFan >> 8 ) & 0xFF );
  BS = BankSave * 20 + 15;    EEPROM.write(BS, lowByte);                  delay (100);     // запись lowByte интервала между вентиляциями инкубатора от СО2 в ячейку "15" банка "bank"
  BS = BankSave * 20 + 16;    EEPROM.write(BS, higtByte);                 delay (100);     // запись higtByte интервала между вентиляциями инкубатора от СО2 в ячейку "16" банка "bank"
 }
//***************************************                   загрузка настроек инкубатора из ЕЕПРОМ                *******************************************
void LoadFromEEPROM ( int BankLoad )                                                       
{
  int BL; byte lowByte; byte higtByte;
  BL = BankLoad * 20 + 0;     lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte температуры инкубации *100 из ячейки "0"
  BL = BankLoad * 20 + 1;     higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte температуры инкубации *100 из ячейки "1"
    TempIncu = ( ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 ) ) / 100.00 ;
  BL = BankLoad * 20 + 2;     lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte дельта температуры инкубации *100 из ячейки "2"
  BL = BankLoad * 20 + 3;     higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte дельта температуры инкубации *100 из ячейки "3"
    deltaTemp = ( ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 ) ) / 100.00 ;
  BL = BankLoad * 20 + 4;     lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte значения резницы температур включения форсажного нагрева *100 из ячейки "4"
  BL = BankLoad * 20 + 5;     higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte значения резницы температур включения форсажного нагрева *100 из ячейки "5"
    TempDifference = ( ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 ) ) / 100.00 ;
  BL = BankLoad * 20 + 6;     TimeSWH  = EEPROM.read(BL);                 delay (100);     // чтение интервала между автоматическим переключением элементов обогрева из ячейки "6"
  BL = BankLoad * 20 + 8;     HumiIncu = EEPROM.read(BL);                 delay (100);     // чтение влажности инкубации из ячейки "8"
  BL = BankLoad * 20 + 9;     deltaHum = EEPROM.read(BL);                 delay (100);     // чтение дельты влажности инкубации из ячейки "9"
  BL = BankLoad * 20 + 10;    Rot      = EEPROM.read(BL);                 delay (100);     // чтение времени через которое происходит переворот яиц из ячейки "10"
  BL = BankLoad * 20 + 11;    lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte температуры продувки *100 из ячейки "11"
  BL = BankLoad * 20 + 12;    higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte температуры продувки *100 из ячейки "12"
    TempFan = ( ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 ) ) / 100.00 ;
  BL = BankLoad * 20 + 13;    lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte времени вентиляции инкубатора от СО2 из ячейки "13"
  BL = BankLoad * 20 + 14;    higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte времени вентиляции инкубатора от СО2 из ячейки "14"
    TimeFan = ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 );
  BL = BankLoad * 20 + 15;    lowByte  = EEPROM.read(BL);                 delay (100);     // чтение lowByte интервала между вентиляциями инкубатора от СО2 из ячейки "15"
  BL = BankLoad * 20 + 16;    higtByte = EEPROM.read(BL);                 delay (100);     // чтение higtByte интервала между вентиляциями инкубатора от СО2 из ячейки "16"
    TimeIntervalFan = ( ( lowByte << 0 ) & 0xFF ) + ( ( higtByte << 8 ) & 0xFF00 );
  TimeIntervalSWH = TimeSWH * 1000;                                                        // вычисление интервала между автоматическим переключением элементов обогрева 
  TimeFaningInterval = TimeFan * 1000;                                                     // вычисление длительности работы вентилятора при продувке инкубатора от СО2 
  TimeIntervalFaningInterval = TimeIntervalFan * 60000;                                    // вычисление длительности интервала между продувками инкубатора от СО2
}

//***************************************                           курсирование по меню                          *******************************************
void SlideMenu ()
{
  switch ( NumberButton ){
    case  1: { OnOfDevice = !OnOfDevice; 
               if ( OnOfDevice != 0 ) { 
                 MainMenu = 1; 
                 myOLED.setBrightness ( 255 );
                 EEPROM.write ( 112, 1); 
                 delay (500); } 
               else { 
                 MainMenu = 0; 
                 myOLED.setBrightness ( 10 );
                 digitalWrite (PinFAN, HIGH); 
                 digitalWrite (PinHT2, HIGH); 
                 digitalWrite (PinROT, HIGH); 
                 digitalWrite (PinHUM, HIGH); 
                 digitalWrite (PinHT1, HIGH);
                 EEPROM.write ( 112, 0); 
                 delay (500); } break;}
    case  2: { switch ( MainMenu ) {
                 case  1: { MainMenu = 5; break; }
                 case  2: { MainMenu = 5; break; }
                 case  5: { MainMenu = 1; break; }
                 case 10: { MainMenu = 5; break; }
                 case 20: { MainMenu = 5; break; }
                 case 30: { MainMenu = 5; break; }
                 case 40: { MainMenu = 5; break; }
                 case 50: { MainMenu = 5; break; }
                 case 60: { MainMenu = 5; break; } }
                 delay (50); break; Marker = 1; WriteMarker = 0;}
    case  3: { switch ( MainMenu ) {
                 case  5: { Marker--; if ( Marker < 1 ) Marker = 1; WriteMarker = 0; break; }
                 case 10: { if      ( WriteMarker == 0 ) { Marker --; if ( Marker < 1 ) Marker = 1; a = 0; b = 0; c = 0; WriteMarker = 0; }
                            else if ( WriteMarker == 1 ) { 
                              switch ( Marker ){
                                case 1: { TempIncu  = TempIncu  + FadeTemp; if ( TempIncu  >= maxTempIncu ) TempIncu  = maxTempIncu; break; }
                                case 2: { deltaTemp = deltaTemp + FadeTemp; if ( deltaTemp >= maxDTemp    ) deltaTemp = maxDTemp;    break; }
                                case 3: { TempDifference = TempDifference + FadeTemp; if ( TempDifference >= maxDTemp ) TempDifference = maxDTemp; break; }
                                case 4: { TimeSWH = TimeSWH + FadeTimeSW; if ( TimeSWH >= maxTimeSWH ) TimeSWH = maxTimeSWH; break;}
                                case 5: { TempFan   = TempFan   + FadeTemp; if ( TempFan   >= maxTempIncu ) TempFan   = maxTempIncu; break; }}} break;}
                 case 20: { if      ( WriteMarker == 0 ) { Marker --; if ( Marker < 1 ) Marker = 1; a = 0; b = 0; WriteMarker = 0; }
                            else if ( WriteMarker == 1 ) {
                              switch ( Marker ){
                                case 1: { HumiIncu  = HumiIncu  + FadeHum;  if ( HumiIncu  >= MaxHumIncu  ) HumiIncu  = MaxHumIncu;  break; }
                                case 2: { deltaHum  = deltaHum  + FadeHum;  if ( deltaHum  >= maxDHum     ) deltaHum  = maxDHum;     break; }}} break;}
                 case 30: { if      ( WriteMarker == 0 ) { Marker --; if ( Marker < 1 ) Marker = 1; a = 0; b = 0; WriteMarker = 0; }
                            else if ( WriteMarker == 1 ) { i ++; if ( i > 8 ) i = 8; Rot = RotPerDay [i];                          } break; }
                 case 40: { if      ( WriteMarker == 0 ) { Marker --; if ( Marker < 1 ) Marker = 1; a = 0; b = 0; WriteMarker = 0; }
                            else if ( WriteMarker == 1 ) { 
                              switch ( Marker ){
                                case 1: { TimeFan         = TimeFan         + FadeTimeFan;         if ( TimeFan         >= maxTimeFan         ) TimeFan         = maxTimeFan;         TimeFaningInterval = TimeFan * 1000;                  break; }
                                case 2: { TimeIntervalFan = TimeIntervalFan + FadeTimeIntervalFan; if ( TimeIntervalFan >= maxTimeIntervalFan ) TimeIntervalFan = maxTimeIntervalFan; TimeIntervalFaningInterval = TimeIntervalFan * 60000; break; }}} break; }
                 case 50: { if      ( WriteMarker == 0 ) { Marker --; if ( Marker < 1 ) Marker = 1; a = 0; b = 0; WriteMarker = 0; } 
                            else if ( WriteMarker == 1 ) {
                              switch ( Marker ){
                                case 1: {
                                  switch ( SimvolMarker ){
                                    case 1: { NOWhour   ++; if ( NOWhour   > 24   ) NOWhour   = 24;   break; }
                                    case 2: { NOWminute ++; if ( NOWminute > 59   ) NOWminute = 59;   break; }
                                    case 3: { NOWsecond ++; if ( NOWsecond > 59   ) NOWsecond = 59;   break; }} break; }
                                case 2: {
                                  switch ( SimvolMarker ){
                                    case 1: { NOWyear   ++; if ( NOWyear   > 2030 ) NOWyear   = 2030; break; }
                                    case 2: { NOWmonth  ++; if ( NOWmonth  > 12   ) NOWmonth  = 12;   break; }
                                    case 3: { NOWday    ++; if ( NOWday    > 31   ) NOWday    = 31;   break; }} break; }}} break;}
                 case 60: { if      ( WriteMarker == 0 ) { Marker --; if ( Marker < 1 ) Marker = 1; a = 0; b = 0; WriteMarker = 0; }
                            else if ( WriteMarker == 1 ) { 
                              switch ( Marker ){
                                case 1: { Bank ++; if ( Bank > 4 ) Bank = 4; break; }
                                case 2: { Bank ++; if ( Bank > 4 ) Bank = 4; break; }
                                case 3: { bankLoad ++; if ( bankLoad > 4 ) bankLoad = 4; break; }}} break;}} break;}
    case  4: { switch ( MainMenu ) {
                 case  5: { Marker++; if ( Marker > 6 ) Marker = 6; a = 0; b = 0; WriteMarker = 0; break; }
                 case 10: { if      ( WriteMarker == 0 ) { Marker ++; if ( Marker > 5 ) Marker = 5; a = 0; b = 0; c = 0; WriteMarker = 0; }
                            else if ( WriteMarker == 1 ) {
                              switch ( Marker ) {
                                case 1: { TempIncu  = TempIncu  - FadeTemp; if ( TempIncu  <= minTempIncu ) TempIncu  = minTempIncu; break; }
                                case 2: { deltaTemp = deltaTemp - FadeTemp; if ( deltaTemp <= minDTemp    ) deltaTemp = minDTemp;    break; }
                                case 3: { TempDifference = TempDifference - FadeTemp; if ( TempDifference <= minDTemp ) TempDifference = minDTemp; break; }
                                case 4: { TimeSWH = TimeSWH - FadeTimeSW; if ( TimeSWH <= minTimeSWH ) TimeSWH = minTimeSWH; break;}
                                case 5: { TempFan   = TempFan   - FadeTemp; if ( TempFan   <= minTempIncu ) TempFan   = minTempIncu; break; }}} break; }
                 case 20: { if      ( WriteMarker == 0 ) { Marker ++; if ( Marker > 2 ) Marker = 2; a = 0; b = 0; WriteMarker = 0; } 
                            else if ( WriteMarker == 1 ) {
                              switch ( Marker ) {
                                case 1: { HumiIncu  = HumiIncu  - FadeHum;  if ( HumiIncu  <= MinHumIncu  ) HumiIncu  = MinHumIncu;  break; }
                                case 2: { deltaHum  = deltaHum  - FadeHum;  if ( deltaHum  <= minDHum     ) deltaHum  = minDHum;     break; }}} break;}
                 case 30: { if      ( WriteMarker == 0 ) { Marker ++; if ( Marker > 1 ) Marker = 1; a = 0; b = 0; WriteMarker = 0; } 
                            else if ( WriteMarker == 1 ) { i --; if ( i < 0 ) i = 0; Rot = RotPerDay [i];                          } break; }
                 case 40: { if      ( WriteMarker == 0 ) { Marker ++; if ( Marker > 2 ) Marker = 2; a = 0; b = 0; WriteMarker = 0; }
                            else if ( WriteMarker == 1 ) { 
                              switch ( Marker ){
                                case 1: { TimeFan         = TimeFan         - FadeTimeFan;         if ( TimeFan         <= minTimeFan         ) TimeFan         = minTimeFan;         TimeFaningInterval = TimeFan * 1000;                  break; }
                                case 2: { TimeIntervalFan = TimeIntervalFan - FadeTimeIntervalFan; if ( TimeIntervalFan <= minTimeIntervalFan ) TimeIntervalFan = minTimeIntervalFan; TimeIntervalFaningInterval = TimeIntervalFan * 60000; break; }}} break; }
                 case 50: { if      ( WriteMarker == 0 ) { Marker ++; if ( Marker > 2 ) Marker = 2; a = 0; b = 0; WriteMarker = 0; } 
                            else if ( WriteMarker == 1 ) {
                              switch ( Marker ){
                                case 1: {
                                  switch ( SimvolMarker ){
                                    case 1: { NOWhour   --; if ( NOWhour   < 0    ) { NOWhour   = 23; NOWday --;}    break; }
                                    case 2: { NOWminute --; if ( NOWminute < 0    ) { NOWminute = 59; NOWhour --;}   break; }
                                    case 3: { NOWsecond --; if ( NOWsecond < 0    ) { NOWsecond = 59; NOWminute --;} break; }} break; }
                                case 2: {
                                  switch ( SimvolMarker ){
                                    case 1: { NOWyear   --; if ( NOWyear   < 2016 ) { NOWyear   = 2016;}             break; }
                                    case 2: { NOWmonth  --; if ( NOWmonth  < 0    ) { NOWmonth  = 12; NOWyear --;}   break; }
                                    case 3: { NOWday    --; if ( NOWday    < 0    ) { NOWday    = 31; NOWmonth --;}  break; }} break; }}} break;}
                 case 60: { if      ( WriteMarker == 0 ) { Marker ++; if ( Marker > 3 ) Marker = 3; a = 0; b = 0; WriteMarker = 0; }
                            else if ( WriteMarker == 1 ) {
                              switch ( Marker ){
                                case 1: { Bank --; if ( Bank < 0 ) Bank = 0; break; }
                                case 2: { Bank --; if ( Bank < 0 ) Bank = 0; break; }
                                case 3: { bankLoad --; if ( bankLoad < 0 ) bankLoad = 0; break; }}} break;}} break;}
    case  5: { switch ( MainMenu ) {
                 case  5: {
                  switch ( Marker ) {
                    case 1: { MainMenu = 10; break; }
                    case 2: { MainMenu = 20; break; }
                    case 3: { MainMenu = 30; break; }
                    case 4: { MainMenu = 40; break; } 
                    case 5: { MainMenu = 50; break; }
                    case 6: { MainMenu = 60; break; }} Marker = 1; a = 0; b = 0; WriteMarker = 0; break;} 
                 case 10: {
                  switch ( Marker ) {
                    case 1: { if ( WriteMarker == 0 ) { a = 20; b = 20; c = 20; d = 20; } else if ( WriteMarker == 1 ) { a = 0; b = 0; c = 0;  d = 0; } WriteMarker = !WriteMarker; break; } 
                    case 2: { if ( WriteMarker == 0 ) { a = 0;  b = 20; c = 20; d = 20; } else if ( WriteMarker == 1 ) { a = 0; b = 0; c = 0;  d = 0; } WriteMarker = !WriteMarker; break; } 
                    case 3: { if ( WriteMarker == 0 ) { a = 0;  b = 0;  c = 20; d = 20; } else if ( WriteMarker == 1 ) { a = 0; b = 0; c = 0;  d = 0; } WriteMarker = !WriteMarker; break; }
                    case 4: { if ( WriteMarker == 0 ) { a = 0;  b = 0;  c = 0;  d = 20; } else if ( WriteMarker == 1 ) { a = 0; b = 0; c = 0;  d = 0; } WriteMarker = !WriteMarker; break; }
                    case 5: { if ( WriteMarker == 0 ) { a = -8; b= -8;  c = -8; d = -8; } else if ( WriteMarker == 1 ) { a = 0; b = 0; c = 0;  d = 0; } WriteMarker = !WriteMarker; break; }} break;}
                 case 20: {
                  switch ( Marker ) {
                    case 1: { if ( WriteMarker == 0 ) { a = 20; b = 0; } else if ( WriteMarker == 1 ) { a = 0; b = 0; } WriteMarker = !WriteMarker; break; } 
                    case 2: { if ( WriteMarker == 0 ) { a =  0; b = 0; } else if ( WriteMarker == 1 ) { a = 0; b = 0; } WriteMarker = !WriteMarker; break; }} break;} 
                 case 30: {
                    if ( WriteMarker == 0 ) { a = 20; b = 0;  WriteMarker = 1; } else if ( WriteMarker == 1 ) { a = 0; b = 0; WriteMarker = 0;           } break;}
                 case 40: {
                  switch ( Marker ) {
                    case 1: { if ( WriteMarker == 0 ) { a = 20; b = 0; } else if ( WriteMarker == 1 ) { a = 0; b = 0; } WriteMarker = !WriteMarker; break; } 
                    case 2: { if ( WriteMarker == 0 ) { a =  0; b = 0; } else if ( WriteMarker == 1 ) { a = 0; b = 0; } WriteMarker = !WriteMarker; break; }} break;} 
                 case 50: {
                  switch ( Marker ) {
                    case 1: { if      ( WriteMarker == 0 ) { a = 20; b = 20; WriteMarker = !WriteMarker; SaveTime (); SimvolMarker = 1;} 
                              else if ( WriteMarker == 1 ) {  
                                switch ( SimvolMarker ){
                                  case 1: { SimvolMarker = 2;                                a = 20; b = 20; break; }
                                  case 2: { SimvolMarker = 3;                                a = 20; b = 20; break; }
                                  case 3: { SimvolMarker = 1; WriteMarker = !WriteMarker; SetupTime ();    a = 0;  b = 0;  break; }}} break;}
                    case 2: { if      ( WriteMarker == 0 ) { a = 0;  b = 0;  WriteMarker = !WriteMarker; SaveTime (); SimvolMarker = 1;} 
                              else if ( WriteMarker == 1 ) { 
                                switch ( SimvolMarker ){ 
                                  case 1: { SimvolMarker = 2;                                a = 0;  b = 0;  break; }
                                  case 2: { SimvolMarker = 3;                                a = 0;  b = 0;  break; }
                                  case 3: { SimvolMarker = 1; WriteMarker = !WriteMarker; SetupTime ();    a = 0;  b = 0;  break; }}} break;}} break;}
                 case 60: {
                  switch ( Marker ) {
                    case 1: { if ( WriteMarker == 0 ) { a = 20; b = 20; } else if ( WriteMarker == 1 ) { a = 0; b = 0; SaveToEEPROM   ( Bank ); } WriteMarker = !WriteMarker; break; } 
                    case 2: { if ( WriteMarker == 0 ) { a =  0; b = 20; } else if ( WriteMarker == 1 ) { a = 0; b = 0; LoadFromEEPROM ( Bank ); } WriteMarker = !WriteMarker; break; }
                    case 3: { if ( WriteMarker == 0 ) { a =  0; b =  0; } else if ( WriteMarker == 1 ) { a = 0; b = 0; EEPROM.write ( 100, bankLoad ); delay ( 100 ); } WriteMarker = !WriteMarker; break; }} break;}} break;}}
}

//***************************************                              прорисовка меню                            *******************************************
void MenuDisp ()
{
  switch ( MainMenu ) {
    case 0: { 
      //myOLED.setFont(RusFont);
      myOLED.print( "OFF",            CENTER,   5 ); //OFF
      TimePrintBig();
      DatePrintLit(); 
      break;}
    case 1: { 
      TimePrintBig();
      //TempPrintLit();
      //HumiPrintLit();
      DatePrintLit();
      break;}
    case 2: {
      TimePrintLit();
      TempPrintBig();
      break;}
    case 3: {
      TimePrintLit();
      HumiPrintBig();
      break;}
    case 5: {
      //myOLED.setFont(RusFont);                                //  __________________
      myOLED.print( "SETTING",                  LEFT+15,  8 );  // |  SETTING         |
      if ( Marker == 1 ) myOLED.invertText(true);               // | TEMPERATURE      |
      myOLED.print( "TEMPERATURE",              LEFT+10, 16 );  // | HUMIDITY         |
      myOLED.invertText(false);                                 // | ROTATE           |
      if ( Marker == 2 ) myOLED.invertText(true);               // | PURGE            |
      myOLED.print( "HUMIDITY",                 LEFT+10, 24 );  // | TIME SETTING     |
      myOLED.invertText(false);                                 // | SEVE/LOAD        |
      if ( Marker == 3 ) myOLED.invertText(true);               //  ------------------
      myOLED.print( "ROTATE",                   LEFT+10, 32 ); 
      myOLED.invertText(false);
      if ( Marker == 4 ) myOLED.invertText(true);
      myOLED.print( "PURGE",                    LEFT+10, 40 ); 
      myOLED.invertText(false);
      if ( Marker == 5 ) myOLED.invertText(true);
      myOLED.print( "TIME SETTING",             LEFT+10, 48 ); 
      myOLED.invertText(false);
      if ( Marker == 6 ) myOLED.invertText(true);
      myOLED.print( "SAVE/LOAD",                LEFT+10, 56 ); 
      myOLED.invertText(false);
      break;}
    case 10: {                                                    //                              Marker = 1              Marker = 2              Marker = 3              Marker = 4              Marker = 5
      //myOLED.setFont(RusFont);                                  //  __________________      __________________      __________________      __________________      __________________      __________________
      myOLED.print( "TEMPERATURE",              LEFT+15,  8   );  // |  TEMPERATURE     |    |  TEMPERATURE     |    |  TEMPERATURE     |    |  TEMPERATURE     |    |  TEMPERATURE     |    |  TEMPERATURE     |
      if ( Marker == 1 ) myOLED.invertText(true);                 // | Т INCU           |    | Т INCU           |    | Т INCU           |    | Т INCU           |    | Т INCU           |    | Т INCU           |    
      myOLED.print( "T INCU",                   LEFT+10, 16   );  // | DELTA Т          |    |  37.5*C          |    | DELTA Т          |    | DELTA Т          |    | DELTA Т          |    | DELTA Т          |
      myOLED.invertText(false);                                   // | FORSE HOTTING    |    | DELTA Т          |    |  0.5*C           |    | FORSE HOTTING    |    | FORSE HOTTING    |    | FORSE HOTTING    |
      if ( Marker == 2 ) myOLED.invertText(true);                 // | TIME SWITCH      |    | FORSE HOTTING    |    | FORSE HOTTING    |    |  2*C             |    | TIME SWITCH      |    | TIME SWITCH      |
      myOLED.print( "DELTA T",                  LEFT+10, 24+a );  // | Т PURGE          |    | TIME SWITCH      |    | TIME SWITCH      |    | TIME SWITCH      |    |  30s              |    | Т PURGE          |
      myOLED.invertText(false);                                   // |                  |    | Т PURGE          |    | Т PURGE          |    | Т PURGE          |    | Т PURGE          |    |  38.2*С          |
      if ( Marker == 3 ) myOLED.invertText(true);                 //  ------------------      ------------------      ------------------      ------------------      ------------------      ------------------
      myOLED.print( "FORSE HOTTING",            LEFT+10, 32+b ); 
      myOLED.invertText(false);
      if ( Marker == 4 ) myOLED.invertText(true);
      myOLED.print( "TIME SWITCH",              LEFT+10, 40+c ); 
      myOLED.invertText(false);
      if ( Marker == 5 ) myOLED.invertText(true);
      myOLED.print( "T PURGE",                  LEFT+10, 48+d ); 
      myOLED.invertText(false);
      if ( WriteMarker == 1 ) { 
        switch ( Marker ){
          case 1: { myOLED.setFont(MediumNumbers); myOLED.printNumF ( float( TempIncu       ), 1, 20, 24 ); /*myOLED.setFont(RusFont);*/ myOLED.print ( "*C",  66, 29 ); break;}
          case 2: { myOLED.setFont(MediumNumbers); myOLED.printNumF ( float( deltaTemp      ), 1, 20, 32 ); /*myOLED.setFont(RusFont);*/ myOLED.print ( "*C",  55, 39 ); break;}
          case 3: { myOLED.setFont(MediumNumbers); myOLED.printNumF ( float( TempDifference ), 1, 20, 40 ); /*myOLED.setFont(RusFont);*/ myOLED.print ( "*C",  55, 45 ); break;}
          case 4: { myOLED.setFont(MediumNumbers); myOLED.printNumI (        TimeSWH,             20, 48 ); /*myOLED.setFont(RusFont);*/ myOLED.print ( "s",   47, 54 ); break;} 
          case 5: { myOLED.setFont(MediumNumbers); myOLED.printNumF ( float( TempFan        ), 1, 20, 48 ); /*myOLED.setFont(RusFont);*/ myOLED.print ( "*C",  66, 54 ); break;}}}
      break;}
    case 20: {                                                    //                              Marker = 1              Marker = 2  
      //myOLED.setFont(RusFont);                                  //  __________________      __________________      __________________
      myOLED.print( "HUMIDITY",                 LEFT+15,  8   );  // |  HUMIDITY        |    |  HUMIDITY        |    |  HUMIDITY        |
      if ( Marker == 1 ) myOLED.invertText(true);                 // | Н INCU           |    | Н INCU           |    | Н INCU           |
      myOLED.print( "H INCU",                   LEFT+10, 16   );  // | DELTA Н          |    |  50              |    | DELTA Н          |
      myOLED.invertText(false);                                   // |                  |    | DELTA Н          |    |  2               |
      if ( Marker == 2 ) myOLED.invertText(true);                 // |                  |    |                  |    |                  |
      myOLED.print( "DELTA H",                  LEFT+10, 24+a );  // |                  |    |                  |    |                  |
      myOLED.invertText(false);                                   // |                  |    |                  |    |                  |
      if ( WriteMarker == 1 ) {                                   //  ------------------      ------------------      ------------------
      switch ( Marker ){
        case 1: { myOLED.setFont(MediumNumbers); myOLED.printNumI ( HumiIncu, 20, 24 ); break;}
        case 2: { myOLED.setFont(MediumNumbers); myOLED.printNumI ( deltaHum, 20, 32 ); break;}}}
      break;}
    case 30: {                                                    //                              Marker = 1  
      //myOLED.setFont(RusFont);                                  //  __________________      __________________
      myOLED.print( "ROTATE",                   LEFT+15,  8   );  // |  ROTATE          |    |  ROTATE          |
      if ( Marker == 1 ) myOLED.invertText(true);                 // | ROTATE           |    | ROTATE           |
      myOLED.print( "ROTATE",                   LEFT+10, 16   );  // |                  |    |  8 times a day   |
      myOLED.invertText(false);                                   // |                  |    |                  |
      switch ( Marker ){                                          // |                  |    |                  |
        case 1: { if ( WriteMarker == 1 ) {                       // |                  |    |                  |
          myOLED.setFont(MediumNumbers);                          // |                  |    |                  |
          myOLED.printNumI ( Rot, 20, 24 );                       //  ------------------      ------------------
          //myOLED.setFont(RusFont); 
          myOLED.print( "times a day", 47, 27 );} break;}}
      break;}
    case 40: {
      //myOLED.setFont(SmallFont);                                //                              Marker = 1              Marker = 2  
      //myOLED.setFont(RusFont);                                  //  __________________      __________________      __________________
      myOLED.print( "PURGE",                    LEFT+15,  8   );  // |  PURGE           |    |  PURGE           |    |  PURGE           |
      if ( Marker == 1 ) myOLED.invertText(true);                 // | DURATION         |    | DURATION         |    | DURATION         |
      myOLED.print( "DURATION",                 LEFT+10, 16   );  // | INTERVAL         |    |  10 sec          |    | INTERVAL         |    
      myOLED.invertText(false);                                   // |                  |    | INTERVAL         |    |  10 min          |
      if ( Marker == 2 ) myOLED.invertText(true);                 // |                  |    |                  |    |                  |
      myOLED.print( "INTERVAL",                 LEFT+10, 24+a );  // |                  |    |                  |    |                  |
      myOLED.invertText(false);                                   // |                  |    |                  |    |                  |
      if ( WriteMarker == 1 ) {                                   //  ------------------      ------------------      ------------------
      switch ( Marker ){
        case 1: { myOLED.setFont(MediumNumbers); myOLED.printNumI ( TimeFan,         20, 24 ); /*myOLED.setFont(RusFont);*/ myOLED.print( "sec", 47, 27 ); break;}
        case 2: { myOLED.setFont(MediumNumbers); myOLED.printNumI ( TimeIntervalFan, 20, 32 ); /*myOLED.setFont(RusFont);*/ myOLED.print( "min",  47, 35 ); break;}}}
      break;}
    case 50: {                                                    //                              Marker = 1              Marker = 2     
      //myOLED.setFont(RusFont);                                  //  __________________      __________________      __________________  
      myOLED.print( "TIME SETTING",             LEFT+15,  8   );  // |  TIME SETING     |    |  TIME SETING     |    |  TIME SETING     |
      if ( Marker == 1 ) myOLED.invertText(true);                 // |  SET TIME        |    |  SET TIME        |    |  SET TIME        |
      myOLED.print( "SET TIME",                 LEFT+10, 16   );  // |  SET DATE        |    |  00.00.00        |    |  SET DATE        |
      myOLED.invertText(false);                                   // |                  |    |  SET DATE        |    |  2016.09.13      |
      if ( Marker == 2 ) myOLED.invertText(true);                 // |                  |    |                  |    |                  |
      myOLED.print( "SET DATE",                 LEFT+10, 24+a );  // |                  |    |                  |    |                  |
      myOLED.invertText(false);                                   // |                  |    |                  |    |                  |
      switch ( Marker ){                                          //  ------------------      ------------------      ------------------
        case 1: { if ( WriteMarker == 1 ) { myOLED.setFont(MediumNumbers); 
          if ( SimvolMarker == 1 ) { myOLED.invertText(true); if ( NOWhour   < 10 ) { myOLED.print ( "0" , 5,   24); myOLED.printNumI ( NOWhour ,  17,  24); } else myOLED.printNumI ( NOWhour ,  5,   24); myOLED.invertText(false);}
          else {                                              if ( NOWhour   < 10 ) { myOLED.print ( "0" , 5,   24); myOLED.printNumI ( NOWhour ,  17,  24); } else myOLED.printNumI ( NOWhour ,  5,   24);                          }
          myOLED.print ( ".", 29, 24); 
          if ( SimvolMarker == 2 ) { myOLED.invertText(true); if ( NOWminute < 10 ) { myOLED.print ( "0" , 41,  24); myOLED.printNumI ( NOWminute, 53,  24); } else myOLED.printNumI ( NOWminute, 41,  24); myOLED.invertText(false);}
          else {                                              if ( NOWminute < 10 ) { myOLED.print ( "0" , 41,  24); myOLED.printNumI ( NOWminute, 53,  24); } else myOLED.printNumI ( NOWminute, 41,  24);                          }
          myOLED.print ( "." ,65, 24); 
          if ( SimvolMarker == 3 ) { myOLED.invertText(true); if ( NOWsecond < 10 ) { myOLED.print ( "0" , 77,  24); myOLED.printNumI ( NOWsecond, 89,  24); } else myOLED.printNumI ( NOWsecond, 77,  24); myOLED.invertText(false);} 
          else {                                              if ( NOWsecond < 10 ) { myOLED.print ( "0" , 77,  24); myOLED.printNumI ( NOWsecond, 89,  24); } else myOLED.printNumI ( NOWsecond, 77,  24);                          }}break;}
        case 2: { if ( WriteMarker == 1 ) { myOLED.setFont(MediumNumbers); 
          if ( SimvolMarker == 1 ) { myOLED.invertText(true);                                                        myOLED.printNumI ( NOWyear ,   5,  32);                                                myOLED.invertText(false);}
          else {                                                                                                     myOLED.printNumI ( NOWyear ,   5,  32);                                                                         }
          myOLED.print ( ".", 53, 32); 
          if ( SimvolMarker == 2 ) { myOLED.invertText(true); if ( NOWmonth  < 10 ) { myOLED.print ( "0" , 65,  32); myOLED.printNumI ( NOWmonth,  77,  32); } else myOLED.printNumI ( NOWmonth,  65,  32); myOLED.invertText(false);}
          else {                                              if ( NOWmonth  < 10 ) { myOLED.print ( "0" , 65,  32); myOLED.printNumI ( NOWmonth,  77,  32); } else myOLED.printNumI ( NOWmonth,  65,  32);                          }
          myOLED.print ( "." ,89, 32); 
          if ( SimvolMarker == 3 ) { myOLED.invertText(true); if ( NOWday    < 10 ) { myOLED.print ( "0" , 101, 32); myOLED.printNumI ( NOWday,    113, 32); } else myOLED.printNumI ( NOWday,    101, 32); myOLED.invertText(false);}
          else {                                              if ( NOWday    < 10 ) { myOLED.print ( "0" , 101, 32); myOLED.printNumI ( NOWday,    113, 32); } else myOLED.printNumI ( NOWday,    101, 32);                          }} break;}}
      break;}
    case 60: {                                                    //                              Marker = 1              Marker = 2              Marker = 3          
      //myOLED.setFont(RusFont);                                  //  __________________      __________________      __________________      __________________
      myOLED.print( "SEVE/LOAD",                LEFT+15,  8   );  // |  SEVE/LOAD       |    |  SEVE/LOAD       |    |  SEVE/LOAD       |    |  SEVE/LOAD       |
      if ( Marker == 1 ) myOLED.invertText(true);                 // | SAVE             |    | SAVE             |    | SAVE             |    | SAVE             |
      myOLED.print( "SAVE",                     LEFT+10, 16   );  // | LOAD             |    |  in 0 cell       |    | LOAD             |    | LOAD             | 
      myOLED.invertText(false);                                   // | CELL             |    | LOAD             |    |  of 0 cell       |    | CELL             |
      if ( Marker == 2 ) myOLED.invertText(true);                 // |                  |    | CELL             |    | CELL             |    |  of 0 cell       |
      myOLED.print( "LOAD",                     LEFT+10, 24+a );  // |                  |    |                  |    |                  |    |                  |
      myOLED.invertText(false);                                   // |                  |    |                  |    |                  |    |                  |
      if ( Marker == 3 ) myOLED.invertText(true);                 //  ------------------      ------------------      ------------------      ------------------
      myOLED.print( "CELL",                     LEFT+10, 32+b );
      myOLED.invertText(false);                                 
      if ( WriteMarker == 1 ) {                                   
      switch ( Marker ){
        case 1: { /*myOLED.setFont(RusFont);*/ myOLED.print( "in", 10, 27 ); myOLED.setFont(MediumNumbers); myOLED.printNumI ( Bank,     30, 24 ); /*myOLED.setFont(RusFont);*/ myOLED.print( "cell", 45, 27 ); break;}
        case 2: { /*myOLED.setFont(RusFont);*/ myOLED.print( "of", 10, 35 ); myOLED.setFont(MediumNumbers); myOLED.printNumI ( Bank,     30, 32 ); /*myOLED.setFont(RusFont);*/ myOLED.print( "cell", 45, 35 ); break;}
        case 3: { /*myOLED.setFont(RusFont);*/ myOLED.print( "of", 10, 43 ); myOLED.setFont(MediumNumbers); myOLED.printNumI ( bankLoad, 30, 40 ); /*myOLED.setFont(RusFont);*/ myOLED.print( "cell", 45, 43 ); break;}}}
      break;}
      }
}
//***************************************                         чтение температуры с датчика                    *******************************************
void TempDsRead ()    
{ 
  if ( currentMillis - ds18b20readMillis > ds18b20interval)
  {
    ds18b20readMillis = currentMillis;
    int Whole, Fract, Tc_100, TReading; 
    byte data[2]; 
    ds.reset(); ds.write(0xCC); ds.write(0x44); ds.reset(); ds.write(0xCC); ds.write(0xBE);
    data[0] = ds.read(); data[1] = ds.read(); TReading = ( data [1] << 8 ) + data [0];
    Tc_100 = (6 * TReading) + TReading / 4;
    TnowDS = Tc_100;
    TnowDS = TnowDS / 100;
  }
}
//***************************************                         просчет включения обогрева                      *******************************************
void Hotting ()                                                 
{
  //bool Simbol1 = 0;
  //bool Simbol2 = 0;
  if (currentMillis - TimerSWH > TimeIntervalSWH)
  {
    Hotter = !Hotter;
    TimerSWH = currentMillis;
  }
  int HOT;
  if ( TnowDS < ( TempIncu - ( deltaTemp / 2 ) ) ) { if ( ( TempIncu - TnowDS ) >= TempDifference ) HOT = 2; else HOT = 1; }
  else if ( TnowDS >= ( TempIncu + ( deltaTemp / 2 ) ) ) HOT = 0;
  switch ( HOT ){ 
    case 0: { digitalWrite ( PinHT1, HIGH ); digitalWrite ( PinHT2, HIGH ); Hotter = !Hotter; /*Simbol1 = 0; Simbol2 = 0;*/ break; }
    case 1: { 
      switch ( Hotter ){
        case 0: { /* Hotter = !Hotter; */ digitalWrite ( PinHT1, LOW ); digitalWrite ( PinHT2, HIGH ); /*Simbol1 = 1; Simbol1 = 0;*/ break;}
        case 1: { /* Hotter = !Hotter; */ digitalWrite ( PinHT2, LOW ); digitalWrite ( PinHT1, HIGH ); /*Simbol1 = 0; Simbol2 = 1;*/ break;}} break;}
    case 2: { digitalWrite ( PinHT1, LOW ); digitalWrite ( PinHT2, LOW ); /*Simbol1 = 1; Simbol2 = 1;*/ break;}}
//if ( Simbol1 == 1 ) myOLED.invertText ( true );
//  myOLED.print ( "T1", 58, 0 );
//  myOLED.invertText ( false );
//if ( Simbol2 == 1 ) myOLED.invertText ( true );
//  myOLED.print ( "T2", 78, 0 );
//  myOLED.invertText ( false );  
}
//***************************************                          чтение влажности с датчика                     *******************************************
void HumiDhRead ()
{
  if ( currentMillis - dht11readMillis > dht11interval)
  {
    dht11readMillis = currentMillis;
    HnowDH = dht.readHumidity();
  }
}
//***************************************                         просчет включения увлажнения                    *******************************************
void Humiditing ()
{
  //bool Simbol = 0;
  if ( HnowDH < ( HumiIncu - ( deltaHum / 2 ) ) ) { digitalWrite ( PinHUM, LOW );}// Simbol = 1; } 
  else if ( HnowDH >= (HumiIncu + ( deltaHum / 2 ) ) ) { digitalWrite ( PinHUM, HIGH );}// Simbol = 0; }
  //if ( Simbol == 1 ) myOLED.invertText ( true );
  //myOLED.print ( "H", 118, 0 );
  //myOLED.invertText ( false ); 
}
//***************************************                          просчет включения продувки                     *******************************************
void StartFan () 
{
  //bool Simbol = 0;
  if ( TnowDS >= ( TempFan + ( 0.2 / 2 ) ) ) FanTempFlag = 1; 
  else if ( TnowDS < ( TempFan - ( 0.2 / 2 ) ) ) FanTempFlag = 0; 
  switch ( FanTimeFlag ){
    case 1: { /*
              myOLED.setFont(RusFont); 
              myOLED.printNumI ( ( currentMillis - TimeFaning ) / 1000, RIGHT, 40);
              myOLED.printNumI ( TimeFaningInterval / 1000, RIGHT, 50);*/
              if ( ( currentMillis - TimeFaning ) > TimeFaningInterval ) { FanTimeFlag = !FanTimeFlag; TimeIntervalFaning = currentMillis; } break; }
    case 0: { /*
              myOLED.setFont(RusFont); 
              myOLED.printNumI ( ( currentMillis - TimeIntervalFaning ) / 1000, RIGHT, 40); 
              myOLED.printNumI ( ( TimeIntervalFaningInterval ) / 1000, RIGHT, 50);*/
              if ( ( currentMillis - TimeIntervalFaning ) > TimeIntervalFaningInterval) { FanTimeFlag = !FanTimeFlag; TimeFaning = currentMillis; } break; }}
  if ( FanTempFlag == 1 || FanTimeFlag == 1 ) { digitalWrite ( PinFAN, LOW); /*Simbol = 1;*/ } else  { digitalWrite ( PinFAN, HIGH ); }//Simbol = 0; }
  //if ( Simbol == 1 ) myOLED.invertText ( true );
  //myOLED.print ( "C", 108, 0 );
  //myOLED.invertText ( false );
}
//***************************************   вычисление момента переворота и собственно сам переворот лотка        *******************************************
void Rotating () 
{
  //bool Simbol = 0;
  if ( Rot != 0 ) { int ti = 24 / Rot; for ( int t = 0 ; t < 24 ; t = t + ti ) { if ( RTC.hour == t && RTC.minute == 0 ) { if ( RotFlag == 0 ) { RotFlagTime = 1; RotFlag = !RotFlag; break; } } } }
  if ( RTC.minute > 0 ) RotFlag = 0; 
  if ( NumberButton == 6 ) RotFlagButt = 1;
  unsigned char LS1 = digitalRead ( PinLS1 );  unsigned char LS2 = digitalRead ( PinLS2 );  delay (200);  
  if ( LS1 != 0 && LS2 != 0 ) RotPosition = 2;  
  switch ( RotPosition ){
    case 1: { if ( RotFlagButt == 1 ) {
                switch ( RotWork ){
                  case 1: { digitalWrite ( PinROT, HIGH ); /*Simbol = 0;*/ RotFlagButt = !RotFlagButt; RotWork = !RotWork; break; }
                  case 0: { digitalWrite ( PinROT, LOW  ); /*Simbol = 1;*/ RotFlagButt = !RotFlagButt; RotWork = !RotWork; break; } } }
              if ( RotFlagTime == 1 ) { digitalWrite ( PinROT, LOW ); /*Simbol = 1;*/ RotFlagTime = !RotFlagTime; RotWork = 1;}
              if ( RotWork     == 1 ) { digitalWrite ( PinROT, LOW ); /*Simbol = 1;*/ } break;}                                                                 // положение лотка "еще в зоне 1го конечника"
    case 2: { if ( RotWork     == 1 ) { digitalWrite ( PinROT, LOW ); /*Simbol = 1;*/ }
              if ( RotFlagButt == 1 ) {
                switch ( RotWork ){
                  case 1: { digitalWrite ( PinROT, HIGH ); /*Simbol = 0;*/ RotFlagButt = !RotFlagButt; RotWork = !RotWork; break; }
                  case 0: { digitalWrite ( PinROT, LOW  ); /*Simbol = 1;*/ RotFlagButt = !RotFlagButt; RotWork = !RotWork; break; } } }
              if ( RotFlagTime == 1 && RotWork == 0 ) { digitalWrite ( PinROT, LOW  ); /*Simbol = 1;*/ RotFlagTime = !RotFlagTime; RotWork = !RotWork; }
              if ( LS1 == 0 || LS2 == 0 ) { RotPosition = 3 ; digitalWrite ( PinROT, HIGH ); /*Simbol = 0;*/ RotWork = 0; } break;}                             // положение лотка "между 1м и 2м конечником"
    case 3: { if ( RotFlagButt == 1 || RotFlagTime == 1 ) { RotWork = 1; RotPosition = 1; } break;}}                                                        // положение лотка "в зоне действия 2го конечника"
  //if ( Simbol == 1 ) myOLED.invertText ( true );
  //myOLED.print ( "R", 98, 0 );
  //myOLED.invertText ( false );
}

void loop ()
{
  myOLED.clrScr();
  RTC.getTime();
  PressKeyMenu ();
  SlideMenu ();
  if ( OnOfDevice == 0 ){ MainMenu = 0; MenuDisp ();} 
  else 
  {
    currentMillis = millis();
    TempDsRead ();  Hotting (); StartFan (); 
    HumiDhRead ();  Humiditing ();
    Rotating ();
    if ( currentMillis - StartMillis > interval){ StartMillis = currentMillis;
      switch ( MainMenu ){
        case 1:  MainMenu ++; break;
        case 2:  MainMenu ++; break;
        case 3:  MainMenu = 1; break;}}
    MenuDisp ();
  }
  myOLED.update();
}
