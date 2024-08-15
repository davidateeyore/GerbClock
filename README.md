# GerbClock
A simple LCD clock with date and temperature driven from Arduino Uno or earlier

This project has been been on the books for years..The combination of Arduino and DS18B20 temp sensor with DS1307 RTC is cheap cheerful and reliable. The original code was hacked out over 10 years ago and tweaked about since. What I have learned is that different A/N LCD displays handle 'special' chars differently, DS18B20' do _NOT_ like being hooked up backwards! and decoding the output of the DS1307 from scratch was a bit more tricky than I thought...

dt 30MAY2021

At last! I started using a better version of RTCLib, and completely reworked the DS1307 RTC code - much cleaner and easier to display..Enjoy!

dt 21DEC2021

I found a glitch with the version of RTCLib that I was using, causing the month display to be corrupted e.g. 'APR' was displayed as 'AMR'. updating to latest version of RTCLib fixed this now displays 'Apr'.

dt 18APR2022

The clock has been working fine for a while now, the next clock will be a large 7 seg LED model for the Shed!
dt 17MAY2024

the shed unit has changed to a 4 x FC8x8 LEDmatrix display design, I just have to clean up the code and assemble the clock in a 3D printed case..
dt 15AUG2024
