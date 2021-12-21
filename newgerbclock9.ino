//
// newgerbclock9.ino
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
// this rev of 5 is to keep unit working, RTCLib has issues with DayofWeek.. dt 29APR2021
// this change to fix day of week sunday error.. dt 30MAY2021
// redo RTCLib to allow DST change, if possible.. dt 16DEC2021
// change time display to use RTClib formatting.. dt 17DEC2021
// this is the cleaned up working version of GerbClock.. dt 21DEC2021

#include <RTClib.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <Wire.h>

RTC_DS1307 rtc;

char daysOfTheWeek[][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define switch1     7                         // for dst change..

#define CONVERT     0x44
#define READSCRATCH 0xBE
#define SKIP_ROM    0xCC
#define MATCH_ROM   0x55

// setup the lines to control an LCD.

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
  Serial.begin(57600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

  clcd.createChar(0, degree);
  pinMode(13, OUTPUT);  		// we'll use the debug LED to output a heartbeat
  pinMode(switch1, INPUT);    	// this is for our switch for Daylight Saving Time, High (+5) via 10k for DST
  Wire.begin();

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

   DateTime now = rtc.now();
   char ThisMonth[] = "MMM";
   // to show abbreviated month name dt..
   char ThisTime[] = "hh:mm:ss";
   // to show colon separated time string..
  //  delay(3000);
  // now, write to the display...
  clcd.begin(20, 4);
  clcd.clear();
  // clcd.home(); // top left..
  clcd.print(" ^<gerbilator.org>^");  // the gerb brand!!
  clcd.setCursor(0,1);  // line 2
  clcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  clcd.print(",");
  clcd.setCursor(11,1);
  clcd.print(now.day(), DEC);
  clcd.print("");
  clcd.print(now.toString(ThisMonth));
  clcd.print("");
  clcd.print(now.year(), DEC);
  clcd.setCursor(0,2);  // Move cursor to third line, position 0
  clcd.print("Local Time: ");

  if (digitalRead(switch1) == HIGH)
	  {
		// for DST, calculate a date which is 0 days, 1 hours, 0 minutes, and 0 seconds into the future
		DateTime future (now + TimeSpan(0,1,0,0));
		clcd.print(future.toString(ThisTime));
		// you could make 'days to Xmas', your anniversary etc with this function.. dt.
	  }
	else
    clcd.print(now.toString(ThisTime));
    clcd.setCursor(0,3);
    clcd.print("Temp: ");

    clcd.print(Whole); // from the DS18B20 convert routine..
    clcd.print(".");
    clcd.print(Fract);
  // Winstar charcode ROM doesn't support degree symbol..so we do a special char of our own!
    clcd.write(byte(0));
    clcd.print("C");
}

// that's it!!
