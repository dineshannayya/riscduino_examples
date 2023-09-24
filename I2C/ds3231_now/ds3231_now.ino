// now.pde
// Prints a snapshot of the current date and time along with the UNIX time
// Modified by Andy Wickert from the JeeLabs / Ladyada RTC library examples
// 5/15/11
// https://circuitdigest.com/microcontroller-projects/interfacing-ds3231-rtc-with-arduino-and-diy-digital-clock

#include <Wire.h>


// Get all date/time at once to avoid rollover (e.g., minute/second don't match)
static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }


void setup () {
    Serial.begin(9600);
    Wire.begin();
    delay(500);
    Serial.println("Riscduino Ready!");
}

void loop () {
    
    delay(1000);
    
  
  Wire.beginTransmission(0x68);
  Wire.write(0);	// This is the first register address (Seconds)
  			// We'll read from here on for 7 bytes: secs reg, minutes reg, hours, days, months and years.
  Wire.endTransmission();


  Wire.requestFrom(0x68, 7);


  uint32_t second = bcd2bin(Wire.read() & 0x7F);
  uint32_t minute = bcd2bin(Wire.read());
  uint32_t hour = bcd2bin(Wire.read());
  Wire.read();
  uint32_t day = bcd2bin(Wire.read());
  uint32_t month = bcd2bin(Wire.read());
  uint32_t year = bcd2bin(Wire.read()) + 2000;
   
    Serial.print(year, DEC);
    Serial.print('/');
    Serial.print(month, DEC);
    Serial.print('/');
    Serial.print(day, DEC);
    Serial.print(' ');
    Serial.print(hour, DEC);
    Serial.print(':');
    Serial.print(minute, DEC);
    Serial.print(':');
    Serial.print(second, DEC);
    Serial.println();
   
}