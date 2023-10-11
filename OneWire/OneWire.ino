void dec2bin(int n);
void dec2bin8(int n);

//https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf

#define ERROR_NO_RESPONSE  1
#define ERROR_RESPONSE_PERIOD  2
#define ERROR_HIGH_PERIOD  3
#define ERROR_DATA_PERIOD  4
#define TIMEOUT_ERROR  1


#define _dht11_pin 2

void dec2bin(int n)
{
    int c, k;

   for (c = 15; c >= 0; c--)
    {
        k = n >> c;

        if (k & 1)
            Serial.print("1");
        else
            Serial.print("0");
    }
} 

void dec2bin8(int n)
{
    int c, k;

   for (c = 7; c >= 0; c--)
    {
        k = n >> c;

        if (k & 1)
            Serial.print("1");
        else
            Serial.print("0");
        }
}

void read_dht11(uint8_t dht11_pin) {

uint32_t Payload = 0;
uint16_t rawHumidity = 0;
uint16_t rawTemperature = 0;
uint8_t  checkSum = 0;

unsigned int x ,j;
  // Phase-1: MCU Sends out Start Signal to DH : Drive Low for 20ms
  digitalWrite(dht11_pin, LOW); 
  GLBL_REG(GLBL_SOFT_REG4) = 0x1;
  delay(20);
  digitalWrite(dht11_pin, HIGH); 
  GLBL_REG(GLBL_SOFT_REG4) = 0x2;

  // Phase-2: change to input mode: Wait for Low
  // MCU will pull up voltage and wait 20-40us for DHT’s response.
  pinMode(dht11_pin, INPUT);
  for(x = 0; x < 5; x++) {
     if(digitalRead(dht11_pin) == 0) break;
  }
  if(x == 5) return ERROR_NO_RESPONSE;
  GLBL_REG(GLBL_SOFT_REG4) = 0x3;
  
  // Phase-3: Once DHT detects the start signal, it will send out a low-voltage-level response signal, which lasts 80us.
  for(x = 0; x < 8; x++) {
     if(digitalRead(dht11_pin) == 1) break;
  } 
  if(x == 8 ) return ERROR_RESPONSE_PERIOD;
  GLBL_REG(GLBL_SOFT_REG4) = 0x4;
  
  // Phase-4: High should for Minimum 80us
  for(x = 0; x < 8; x++) {
     if(digitalRead(dht11_pin) == 0) break;
  } 
  if(x == 8) return ERROR_HIGH_PERIOD;
  GLBL_REG(GLBL_SOFT_REG4) = 0x5;
  
  // Phase-5: 32 bit Payload Phase
  for(j =0; j < 32; j++) {
      Payload = Payload << 1; // Shift 1
      // Phase-5: Data Phase, Start with low for 50us
      for(x = 0; x < 5; x++) {
         if(digitalRead(dht11_pin) == 1) break;
      } 
      if(x == 5) return ERROR_DATA_PERIOD;
      GLBL_REG(GLBL_SOFT_REG4) = 0x6;
  
      // Phase-5: Data Phase, Start with low for 50us
      for(x = 0; x < 10; x++) {
         if(digitalRead(dht11_pin) == 0) break;
      } 
      if(x > 3) {
        Payload |= 1 ; // Shift 1
      } 
      GLBL_REG(GLBL_SOFT_REG4) = 0x7;
  }
      GLBL_REG(GLBL_SOFT_REG5) = Payload;
  
  
  // Phase-6: 8 bit CRC
  for(j =0; j < 8; j++) {
      checkSum = checkSum << 1; // Shift 1
      // Phase-5: Data Phase, Start with low for 50us
      for(x = 0; x < 5; x++) {
         if(digitalRead(dht11_pin) == 1) break;
      } 
      if(x == 5) return ERROR_DATA_PERIOD;
      GLBL_REG(GLBL_SOFT_REG4) = 0x8;
  
      // Phase-5: Data Phase, Start with low for 50us
      for(x = 0; x < 10; x++) {
         if(digitalRead(dht11_pin) == 0) break;
      } 
      if(x > 3) {
        checkSum |= 1 ; // Shift 1
      } 
      GLBL_REG(GLBL_SOFT_REG4) = 0x9;
  }
      
   GLBL_REG(GLBL_SOFT_REG6) = checkSum;

   rawHumidity = (Payload >> 16 ) & 0xFFFF;
   rawTemperature = Payload & 0xFFFF;


}


// Print the Humity, Temp and validate checksum
void printFunc(uint16_t rawHumidity, uint16_t rawTemperature, uint8_t checkSum) {

  uint8_t humi;
  uint8_t humd;
  uint8_t tempi;
  uint8_t tempd; 


Serial.println("Humidity: ");
dec2bin(rawHumidity);
Serial.print("\t");
humi = rawHumidity >> 8;
dec2bin8(humi);
Serial.print("\t");
rawHumidity = rawHumidity << 8;
humd = rawHumidity >> 8;
dec2bin8(humd);
Serial.print("\t");
Serial.print(humi);
Serial.print(".");
Serial.print(humd);
Serial.print("%");
Serial.println("");

Serial.println("Temperature Degree Celcius: ");
dec2bin(rawTemperature);
Serial.print("\t");
tempi = rawTemperature >> 8;
dec2bin8(tempi);
Serial.print("\t");
rawTemperature = rawTemperature << 8;
tempd = rawTemperature >> 8;
//tempd = (byte)rawTemperature;
dec2bin8(tempd);
Serial.print("\t");
Serial.print(tempi);
Serial.print(".");
Serial.print(tempd);
Serial.print("C");
Serial.println("");

Serial.println("Checksum Byte: ");
dec2bin8(checkSum);
Serial.println("");
dec2bin8(tempi + tempd + humi + humd);
Serial.println("");
if((byte)checkSum == (byte)(tempi + tempd + humi + humd)){Serial.print("CHECKSUM_OK");}
else {Serial.print("CHECKSUM_ERROR");}
Serial.println("");
Serial.println("");
Serial.println("");



}

void setup()
{

    Serial.begin(9600);
    pinMode(_dht11_pin, OUTPUT);
    digitalWrite(_dht11_pin, HIGH);
    delay(1);
}

void loop()
{

read_dht11(_dht11_pin); 

}
