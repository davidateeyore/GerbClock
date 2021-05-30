//
// newgerbclock5.ino
//
// MyTemperatureClockCalendar machine
// by David Thomas (gerbilator.org)
// this sketch runs on any Arduino NG/clone or later..
// and uses the DS1307 RTC chip and the DS18B20 temp sensor.
// the code is based on the DS1307 demo by Jon McPhalen (www.jonmcphalen.com)
// who based his code on work by others -- 29 DEC 2007
// -- 08 APR 2009 - mods by dt - gerbilator.org
// --- change to std LiquidCrystal lib dt ..
// --- add DS1820 stuff dt..
// --- and you can use both 1Wire (tm) and I2C devices at the same time!!! 
// --- more todo's added .. 21JAN2014 dt
// SDA pin is Analog4
// SCL pin is Analog5
//
// changed to suit newer IDE (1.0.5) and rewrite of lib calls..dt 25MAY2014
// (only wanted to change the time!! :-]) 
// new display module: WINSTAR WH2004A doesn't display degrees symbol
// find char chart...done..
//
// added switch from +5 to pin D7, to signal DST.. dt 16oct2014
// clean up comments.. dt 02oct2016
// DST change still not working properly.. dt 03DEC2018

// kill dst if statement; not working correctly; and delay loop.. dt 23MAY2018
// fix up degrees display (do special char).. dt 24MAY2018
// fix dst if statement move to loop display of time.. dt 27OCT2018
// change hours display routine to print2Hex() with revised if statement and
// change degree symbol (experimental).. dt 28OCT2018
// recompile with later lib versions.. dt 07APR2019
// fix print2Hex, printDec2 functions.. dt 08DEC2019
// disable dst switch..still not right.. dt 09DEC2019

#include <RTClib.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <Wire.h>

 
#define DS1307      0xD0 >> 1                 // shift required by Wire.h (silly...)
#define switch1     7                         // for dst change..
// DS1307 clock registers
#define R_SECS      0
#define R_MINS      1
#define R_HRS       2
#define R_WDAY      3
#define R_DATE      4
#define R_MONTH     5
#define R_YEAR      6
#define R_SQW       7

#define CONVERT     0x44
#define READSCRATCH 0xBE
#define SKIP_ROM    0xCC
#define MATCH_ROM   0x55

byte second = 0x00;                             // default to 01 JAN 2000, midday
byte minute = 0x00;
byte hour = 0x12;
byte Wday = 0x01;
byte day = 0x01;
byte month = 0x01;
byte year = 0x00;
byte ctrl = 0x00;
byte dsthour = 0x00; // to display for daylight saving time switch

// setup the lines to control an LCD.  
// todo: daylight saving time?? done 16oct2014 yayy!!
// todo: different RTC..
// todo: serial LCD..
// todo: clock setting controls..

LiquidCrystal clcd(12, 11, 10, 5, 4, 3, 2);
// rs, rw, enable, d4, d5, d6, d7
byte degree[8] = {
  B00110,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000,
};
 

OneWire  ds(8);  // The DS18B20 is connected to pin 8  

void setup() { 
  clcd.createChar(0, degree);
  pinMode(13, OUTPUT);  // we'll use the debug LED to output a heartbeat
  pinMode(switch1, INPUT);    // this is for our switch for Daylight Saving Time, High (+5) via 10k for DST
  Wire.begin();
  Serial.begin(9600);

  second = 0x00;    // the time we are setting...
  minute = 0x53;    // e.g., 3.29pm
  hour   = 0x15;
  Wday   = 0x04;    // sunday = 1, ... saturday = 7
  day    = 0x29;
  month  = 0x04;    // april 
  year   = 0x21;    // 2019
  ctrl   = 0x00;    // disable SQW output
  
// setClock();          // used only to set the DS1307 registers
// should be reenabled only to set the time if the RTC batt goes flat..  
}

void setClock()
{
 Wire.beginTransmission(DS1307);
 Wire.write(R_SECS);
 Wire.write(second);
 Wire.write(minute);
 Wire.write(hour);
 Wire.write(Wday);
 Wire.write(day);
 Wire.write(month);
 Wire.write(year);
 Wire.write(ctrl);
 Wire.endTransmission();
}

void getClock()
{
 Wire.beginTransmission(DS1307);
 Wire.write(R_SECS);
 Wire.endTransmission();
 Wire.requestFrom(DS1307, 8);
 second = Wire.read();
 minute = Wire.read();
 hour   = Wire.read();
 // disabled the code below - causes strange time display!!
 // the DST switch.. just plug D7 into +5V!
 //if (digitalRead(switch1) == LOW)
 //  {
 //     hour   = Wire.read() + 0x01; // dst setting    
 //  }
 Wday   = Wire.read();
 day    = Wire.read();
 month  = Wire.read();
 year   = Wire.read();
 ctrl   = Wire.read();
}

byte bcd2Dec(byte bcdVal)
{
 //return bcdVal / 16 * 10 + bcdVal % 16;
 return( ((bcdVal >> 4) * 10) + (bcdVal & 0xF) );
 // better function..
}

void printHex2(byte hexVal)
{
 if (hexVal < 0x10){
   clcd.print("0");
 }
 clcd.print(hexVal, HEX);
}

void printDec2(byte decVal)
{
 if (decVal < 10){
   clcd.print("0");
 }
 clcd.print(decVal, DEC);
}

void printDayName(byte d)
{
 switch (d) {
   case 0:
     clcd.print("Sunday");
     break;
   //case 1:
   //  clcd.print("notday");
   //  break;   
   case 1:
     clcd.print("Monday");
     break;
   case 2:
     clcd.print("Tuesday");
     break;
   case 3:
     clcd.print("Wednesday");
     break;
   case 4:
     clcd.print("Thursday");
     break;
   case 5:
     clcd.print("Friday");
     break;
   case 6:
     clcd.print("Saturday");
     break;
   default:
     clcd.print("?unknown?");
 }
}

void printMonthName(byte m)
{
 switch (m) {
   case 1:
     clcd.print("JAN");
     break;
   case 2:
     clcd.print("FEB");
     break;
   case 3:
     clcd.print("MAR");
     break;
   case 4:
     clcd.print("APR");
     break;
   case 5:
     clcd.print("MAY");
     break;
   case 6:
     clcd.print("JUN");
     break;
   case 7:
     clcd.print("JUL");
     break;
   case 8:
     clcd.print("AUG");
     break;
   case 9:
     clcd.print("SEP");
     break;
   case 10:
     clcd.print("OCT");
     break;
   case 11:
     clcd.print("NOV");
     break;
   case 12:
     clcd.print("DEC");
     break;
   default:
     clcd.print("???");
 }
}

void loop()
 {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  char buff[15];
  
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

  if ( !ds.search(addr))   // Search for 1-wire device
    {
      ds.reset_search();
    }
  
  if ( OneWire::crc8( addr, 7) != addr[7])   // Check CRC is valid
    {
        Serial.print("CRC is not valid!\n");
    }
  
  if ( addr[0] != 0x28) {                    // Make sure it is a DS18B20 device
      Serial.print("Device is not a DS18B20 family device.\n");
    
    }

  ds.reset();                               // Reset device 
  ds.select(addr);                          // Select device
  ds.write(CONVERT,1);                      // Issue Convert command 
  

  delay(500);                              //  750ms is enough..
    
  present = ds.reset();                    // Reset device
  ds.select(addr);                         // Select device 
  ds.write(READSCRATCH);                   // Read Scratchpad

 
  for ( i = 0; i < 9; i++) {               // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
    }
  
  
  if(OneWire::crc8( data, 8) == data[8])  // Check CRC is valid
    {
     // CRC is ok
     // for 18B20...
     LowByte = data[0];
     HighByte = data[1];
     TReading = (HighByte << 8) + LowByte;
     SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25
  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;
    }

  if (SignBit) // If it's negative
  {
     Serial.print("-");
  }
  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }
  Serial.print(Fract);
  Serial.print("\n");
  
  // and now read the clock chip...
  getClock();
  //no need to rush
 delay(250); // we already have 750ms delay in temp sensor routine...
  clcd.begin(20, 4);
  clcd.clear();
  // clcd.home(); // top left..
  clcd.print(" $<gerbilator.org>&");  // the gerb brand!!
  clcd.setCursor(0,1);  // line 2
  printDayName(bcd2Dec(Wday));
  clcd.print(",");
  clcd.setCursor(10,1); 
  //printDayName(bcd2Dec(wkDay));
  //clcd.print("  ");

 printHex2(day);
 //clcd.print(" ");
 printMonthName(bcd2Dec(month));
 clcd.print("20");
 printHex2(year);
 //clcd.print(".");
 clcd.setCursor(0,2);  // Move cursor to third line, position 0
 clcd.print("Local Time: ");
 
 if (digitalRead(switch1) == HIGH)
  {
    dsthour = (hour + 1);  // to force hour increment..
    printHex2(dsthour);
  }
  else
 printHex2(hour);
 clcd.print(":");
 printHex2(minute);
 clcd.print(":");
 printHex2(second);
 //lcd.setCursor(9,1);
 clcd.setCursor(0,3);
 clcd.print("Temp: ");
 
 clcd.print(Whole); // from the DS18B20 convert routine..
 clcd.print(".");
 clcd.print(Fract);
 //clcd.setCursor(10,4);
 // Winstar charcode ROM doesn't support degree symbol..so we do special char of our own!
 clcd.write(byte(0)); 
 clcd.print("C");
 //clcd.home();
}
// that's it!!
