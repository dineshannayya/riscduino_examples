//Example Reference: https://www.electronicwings.com/arduino/pwm-in-arduino_

int led = 6;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT); // declare pwm pin to be an output:
  pinMode(5, OUTPUT); // declare pwm pin to be an output:
  pinMode(6, OUTPUT); // declare pwm pin to be an output:
  pinMode(9, OUTPUT); // declare pwm pin to be an output:
  pinMode(10, OUTPUT); // declare pwm pin to be an output:
  pinMode(11, OUTPUT); // declare pwm pin to be an output:
}

void loop() {
  //analogWrite(led, brightness); // set the brightness of led
  //digitalWrite(led, HIGH); // set the brightness of led
  analogWrite(3, brightness); // set the brightness of led - Worked
  analogWrite(5, brightness); // set the brightness of led
  analogWrite(6, brightness); // set the brightness of led
  analogWrite(9, brightness); // set the brightness of led
  analogWrite(10, brightness); // set the brightness of led
  analogWrite(11, brightness); // set the brightness of led

 
  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }  
  delay(100);  // wait for 30 milliseconds to see the dimming effect
  //digitalWrite(led, LOW); // set the brightness of led

}