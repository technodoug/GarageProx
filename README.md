GarageProx
==========

Initial

Arduino Garage Proximity Sensor

Uses Uno or Digispark and HC-SR04 ultrasonic sensor. Also supports Megabrite on Uno.

Uno version works well, the Digispark is a little off, but works.

Comment out this line for Uno:
  #define DIGISPARK
  
I have this working using Real Bare Bones Board (RBB)

LED output (RGB)

Operation:

Blue LED comes on when within MAX_DIST
Green LED Comes on when within WARN_DIST_DELTA of stop_distance
Red LED comes on at stop_distance
Red/Blue flash when at EMERGENCY_DIST

The default stop_distance is 6 inches, but can be programmed by pressing the button.  Data is stored in EEPROM.
Default distance can be recovered by holding button during power-up.


LED turns off using MAX_LED_TIMEOUT

