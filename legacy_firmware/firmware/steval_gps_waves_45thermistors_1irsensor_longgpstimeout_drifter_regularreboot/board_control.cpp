#include "board_control.h"

TwoWire ArtemisWire(i2c_port_number);
QWIIC_POWER qwiic_switch;

void blink_LED_n_times(unsigned int number_of_blinks, float frequency_hz){
  wdt.restart();  // we choose to restart at the start and end, but not in loop as a way to make sure not so much blinking that we freeze.

  unsigned long delay_half_blink = static_cast<unsigned long>(1.0 / frequency_hz * 1000.0 / 2.0);
  pinMode(LED, OUTPUT);
  delay(10);

  for (unsigned int i=0; i<number_of_blinks; i++){
    digitalWrite(LED, HIGH);
    delay(delay_half_blink);
    digitalWrite(LED, LOW);
    delay(delay_half_blink);
  }

  wdt.restart();
}

void setup_pins(void){
  pinMode(LED, OUTPUT); // Make the LED pin an output

  turn_gnss_off(); // Disable power for the GNSS
  pinMode(geofencePin, INPUT); // Configure the geofence pin as an input

  turn_iridium_off();
  pinMode(iridiumRI, INPUT); // Configure the Iridium Ring Indicator as an input
  pinMode(iridiumNA, INPUT); // Configure the Iridium Network Available as an input
  pinMode(superCapPGOOD, INPUT); // Configure the super capacitor charger PGOOD input

  pinMode(busVoltageMonEN, OUTPUT); // Make the Bus Voltage Monitor Enable an output
  digitalWrite(busVoltageMonEN, LOW); // Set it low to disable the measurement to save power
  analogReadResolution(14); //Set resolution to 14 bit
}

void turn_on(void){
    wake_up();
}

void turn_gnss_on(void){
  Serial.println(F("turn gnss on"));
  turn_iridium_off();
  delay(100);
  pinMode(gnssEN, OUTPUT); // Configure the pin which enables power for the ZOE-M8Q GNSS
  digitalWrite(gnssEN, LOW); // Enable GNSS power (HIGH = disable; LOW = enable)
  delay(1000);
}

void turn_gnss_off(void){
  Serial.println(F("turn gnss off"));
  pinMode(gnssEN, OUTPUT);
  digitalWrite(gnssEN, HIGH); // Disable GNSS power (HIGH = disable; LOW = enable)
  delay(10);
  pinMode(gnssEN, INPUT_PULLUP); // Configure the pin which enables power for the ZOE-M8Q GNSS
}

void turn_iridium_on(void){
  Serial.println(F("turn iridium on"));
  turn_gnss_off(); 
  delay(100);
  pinMode(superCapChgEN, OUTPUT); // Configure the super capacitor charger enable pin (connected to LTC3225 !SHDN)
  digitalWrite(superCapChgEN, HIGH); // Enable the super capacitor charger
  pinMode(iridiumPwrEN, OUTPUT); // Configure the Iridium Power Pin (connected to the ADM4210 ON pin)
  digitalWrite(iridiumPwrEN, HIGH); // Enable Iridium Power
  delay(1000);
}

void turn_iridium_off(void){
  Serial.println(F("turn iridium off"));
  pinMode(iridiumPwrEN, OUTPUT); // Configure the Iridium Power Pin (connected to the ADM4210 ON pin)
  digitalWrite(iridiumPwrEN, LOW); // Disable Iridium Power (HIGH = enable; LOW = disable)
  pinMode(superCapChgEN, OUTPUT); // Configure the super capacitor charger enable pin (connected to LTC3225 !SHDN)
  digitalWrite(superCapChgEN, LOW); // Disable the super capacitor charger (HIGH = enable; LOW = disable)
  pinMode(iridiumSleep, OUTPUT); // Iridium 9603N On/Off (Sleep) pin
  digitalWrite(iridiumSleep, LOW); // Put the Iridium 9603N to sleep (HIGH = on; LOW = off/sleep)
}

void turn_qwiic_switch_off(void){
  Serial.println(F("turn qwiic switch off"));
   qwiic_switch.pinMode(1, INPUT);
   qwiic_switch.pinMode(2, INPUT);
   delay(50);
   qwiic_switch.powerOff();
   delay(100);
}

void turn_qwiic_switch_on(void){
  Serial.println(F("turn qwiic switch on"));
   qwiic_switch.powerOn();
   delay(50);
   qwiic_switch.pinMode(1, INPUT);
   qwiic_switch.pinMode(2, INPUT);
}

void turn_thermistors_on(void){
 // turn on power to the OneWire sensors
  pinMode(THERMISTORS_POWER_PIN, OUTPUT);
  digitalWrite(THERMISTORS_POWER_PIN, HIGH);
  delay(500); 
}

void turn_thermistors_off(void){
  pinMode(THERMISTORS_ONE_WIRE_PIN, INPUT);
  pinMode(THERMISTORS_POWER_PIN, INPUT);
  delay(100);
}
