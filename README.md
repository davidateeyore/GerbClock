# GerbClock
A simple LCD clock with date and temperature driven from Arduino Uno or earlier

This project has been been on the books for years..The combination of Arduino and DS18B20 temp sensor with DS1307 RTC is cheap cheerful and reliable. The original code was hacked out over 10 years ago and tweaked about since. What I have learned is that different A/N LCD displays handle 'special' chars differently, DS18B20' do _NOT_ like being hooked up backwards! and decoding the output of the DS1307 from scratch was a bit more tricky than I thought...
dt 30MAY2021

At last! I started using a better version of RTCLib, and completely reworked the DS1307 RTC code - much cleaner and easier to display..Enjoy!
dt 21DEC2021
