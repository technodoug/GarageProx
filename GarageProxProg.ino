  
/*
Garage Parking Proximity Sensor
D. Artiles
Oct 17, 2013

This will work with either the Megabrite LED or a RGB LEB
DIGISPARK does not support Megabrite

Programmable distance version

*/

#include <EEPROMEx.h>

//data stored in EEPROM
byte configuration_init;
float configuration_distance;
#define INIT_SIGNATURE 0xC7


#define DIGISPARK
//#define SERIAL_ENABLE


#ifdef DIGISPARK
//HC-SR04 Setup
#define trigpin 1 
#define echopin 5

#define RED_LED 2
#define GREEN_LED 3
#define BLUE_LED 4

//no blinking LED support
//#define BLINK_LED 8
//Button input 
#define BUTTON1 0 
#define BUTTON_ACTIVE LOW

#else
//HC-SR04 Setup
#define trigpin 3
#define echopin 2


//Pins
//For discrete LED control
#define RED_LED 5
#define GREEN_LED 6
#define BLUE_LED 7
#define BLINK_LED 8
//Button input A5
#define BUTTON1 19 
#define BUTTON_ACTIVE HIGH


#endif


#define BUTTON_RELEASE 0
#define BUTTON_PRESS 1
#define BUTTON_HOLD 2


#define LED_ACTIVE LOW


//Distances (inches)
#define EMERGENCY_DIST 2     /* absolute */
#define STOP_DIST_DEFAULT 6  /* absolute */
#define WARN_DIST_DELTA 15   /* relative to stop distance */
#define MAX_DIST  48         /* absolute */
#define MIN_DIST 3           /* absolute */

#define MIN_MOTION_COUNT 2
#define MAX_FOREGROUND 2
//100 ms * MAX_FOREGROUND = 5 times per second
#define MAX_LED_TIMEOUT 60*5 // number of seconds times foreground rate
#define PROG_MODE_TIMEOUT 60 *5


//Megabrite Control
//from Sparkfun appnote
#ifndef DIGISPARK
#define clockpin 13 // CI
#define enablepin 10 // EI
#define latchpin 9 // LI
#define datapin 11 // DI
#define NumLEDs 1
 
int LEDChannels[NumLEDs][3] = {0};
#endif

//Operational modes
#define IDLE_MODE 0
#define NORMAL_MODE 1
#define PROGRAM_MODE 2

boolean program_complete = 0;

volatile int mode;

float diff_limit = 1.0;
int foreground_ctr = 0;
volatile boolean tick = 0;
boolean blink_ctr = 0;
unsigned int led_timeout_ctr = 0;
int motion_detected = 0;
boolean car_detect = 0;
boolean led_timeout = 0;

unsigned int duration;
float distance;
float emergency_distance;
float stop_distance;
float warning_distance;
float maximum_distance;
//


void setup(){
 mode = IDLE_MODE; 
 
 #ifdef SERIAL_ENABLE
 Serial.begin(115200);
 #endif
 
#ifndef DIGISPARK
 //Megabrite setup
 pinMode(datapin, OUTPUT);
 pinMode(latchpin, OUTPUT);
 pinMode(enablepin, OUTPUT);
 pinMode(clockpin, OUTPUT);
 SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);
 digitalWrite(latchpin, LOW);
 digitalWrite(enablepin, LOW);
#endif
 
 //HC-SR04 Setup
 pinMode(trigpin, OUTPUT); 
 pinMode(echopin, INPUT); 
 
 //LED setup
 pinMode(RED_LED,OUTPUT);
 pinMode(GREEN_LED,OUTPUT);
 pinMode(BLUE_LED,OUTPUT);
 #ifndef DIGISPARK
 pinMode(BLINK_LED,OUTPUT);
 #endif
 
 //Pushbutton setup
 pinMode(BUTTON1, INPUT); 
 
 #ifdef DIGISPARK
 //digispark is using active low button input
 //set the pull-up resistor on input by writing to it
 //connect switch to ground
 digitalWrite(BUTTON1,HIGH);
 #endif
 
 
 
  //RGB LED setup
 digitalWrite(RED_LED,LED_ACTIVE);
 digitalWrite(GREEN_LED,~LED_ACTIVE);
 digitalWrite(BLUE_LED,~LED_ACTIVE);

 
 //powering up with the button pressed will reset the default
 if (digitalRead(BUTTON1) == BUTTON_ACTIVE){
   
   stop_distance = STOP_DIST_DEFAULT;
   write_config_data(stop_distance);
   
   //wait for button to be released
   while(digitalRead(BUTTON1) == BUTTON_ACTIVE);
   digitalWrite(RED_LED,LED_ACTIVE);
   digitalWrite(GREEN_LED,~LED_ACTIVE);
   digitalWrite(BLUE_LED,LED_ACTIVE);
   delay (1000);

   
 }
 else{

  // recover initial data from EEPROM
  configuration_init = EEPROM.read(0);
  
  if(configuration_init == INIT_SIGNATURE){
     configuration_distance = EEPROM.readFloat(1);
     stop_distance = configuration_distance;
  }
  else
     stop_distance = STOP_DIST_DEFAULT;

   
 }
 
 
 //set up distances
 emergency_distance = EMERGENCY_DIST;
 warning_distance  = stop_distance + WARN_DIST_DELTA;
 maximum_distance = MAX_DIST;






 
 
 
 
 
 
 //RGB LED setup
 digitalWrite(RED_LED,~LED_ACTIVE);
 digitalWrite(GREEN_LED,~LED_ACTIVE);
 digitalWrite(BLUE_LED,~LED_ACTIVE);
 #ifndef DIGISPARK
 digitalWrite(BLINK_LED,HIGH);
 #endif
 
 WriteLED(0,1023,0); //Green
 digitalWrite(GREEN_LED,LED_ACTIVE);
 delay(500);
 digitalWrite(GREEN_LED,~LED_ACTIVE);
 
 //Yellow
 WriteLED(1023,1023,0); //Yellow

 digitalWrite(RED_LED,LED_ACTIVE);
 digitalWrite(GREEN_LED,LED_ACTIVE);
 delay(500);
 digitalWrite(RED_LED,~LED_ACTIVE);
 digitalWrite(GREEN_LED,~LED_ACTIVE);
 

 //Red
 WriteLED(1023,0,0); //Red
 digitalWrite(RED_LED,LED_ACTIVE);
 delay(500);
 digitalWrite(RED_LED,~LED_ACTIVE);

 // blue
 WriteLED(0,0,1023); //Blue
 digitalWrite(BLUE_LED,LED_ACTIVE);
 delay(500);
 digitalWrite(BLUE_LED,~LED_ACTIVE);

 //Off
 WriteLED(0,0,0); //Off
 mode = NORMAL_MODE; 

 #ifndef DIGISPARK
 digitalWrite(BLINK_LED,LOW);
    // initialize Timer1
    cli();          // disable global interrupts
    TCCR1A = 0;     // set entire TCCR1A register to 0
    TCCR1B = 0;     // same for TCCR1B
 
    // set compare match register to desired timer count:
    OCR1A = 1562; //15624; 0.1 sec = 1562
    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
    // enable global interrupts:
    sei(); 


 #else
 //AT Tiny has 8 bit timer 1
    // initialize Timer1
    cli();          // disable global interrupts
    TCCR1 = 0;      // set entire TCCR1 register to 0
    OCR1A = 0;
    // turn on CTC mode:
    
    TCCR1 |= (1 << CTC1);
    // Set bits for 1024 prescaler:
    TCCR1 |= (1 << CS10);
    TCCR1 |= (1 << CS11);
    TCCR1 |= (1 << CS12);
    TCCR1 |= (1 << CS13);
    
    // enable timer compare interrupt:
    TIMSK |= (1 << OCIE1A);

    // enable global interrupts:
    sei();   
 

 #endif
 
}

//Timer 1 ISR
ISR(TIMER1_COMPA_vect)
{
  static int tick_ctr = 0;
    tick = 1;
  if (mode == NORMAL_MODE){    
    if (++tick_ctr >= 10){
      tick_ctr = 0;
       #ifndef DIGISPARK
      digitalWrite(BLINK_LED,HIGH);
      #else
      ;
      #endif
    } 
    else
     #ifndef DIGISPARK
      digitalWrite(BLINK_LED,LOW);
      #else
      ;
      #endif
 
  }
  
  else if (mode == IDLE_MODE){
    if (++tick_ctr >= 50){
      tick_ctr = 0;
       #ifndef DIGISPARK
      digitalWrite(BLINK_LED,HIGH);
      #else
      ;
      #endif
    } 
    else
      #ifndef DIGISPARK
      digitalWrite(BLINK_LED,LOW);
      #else
      ;
      #endif
    
  }
  else
   #ifndef DIGISPARK
    digitalWrite(BLINK_LED, !digitalRead(BLINK_LED));
   #else
   ;
   #endif
  
}



void loop(){


  
  
if(tick){ 
  tick = 0; 
  if(++foreground_ctr == MAX_FOREGROUND){
    
    foreground_ctr = 0; 
    blink_ctr = !blink_ctr;
  
    digitalWrite(trigpin,HIGH);
    delayMicroseconds(1000);
    digitalWrite(trigpin,LOW);
    duration = pulseIn (echopin,HIGH);
    
//    distance = duration  / (29.1*2.0); // cm
//    distance = distance / 2.54; //inches
    
    distance = duration  * 0.0067646; // inches
    
 #ifdef SERIAL_ENABLE    
    Serial.print("Dur ");
    Serial.print(duration);
    Serial.print(" Dist ");
    Serial.print(distance);
 #endif

    motion_detected = motion_detect(diff_limit,distance);



    //determine the current mode
    switch (mode){
        case NORMAL_MODE:
          if (process_timeout(car_detect) == 1)
            mode = IDLE_MODE;
            
          if (process_pushbutton(digitalRead(BUTTON1)) == BUTTON_PRESS){
            mode = PROGRAM_MODE; 
          }  
        
          break;
          
        case PROGRAM_MODE:
          if (program_complete){
            mode = NORMAL_MODE;
            program_complete = 0;
          }
        
          break;
          
        case IDLE_MODE:
          if (process_timeout(car_detect) == 1)
            mode = IDLE_MODE;
          else
            mode = NORMAL_MODE;
          break;

        default:
          mode = NORMAL_MODE;
                
      
    }//end switch mode
    

    
    //process the modes    
    if (mode == NORMAL_MODE){
      process_normal_mode();
    }

    else if (mode == PROGRAM_MODE)
      process_program_mode();
    else{
      proceess_idle_mode();
    }

  
  
} // end if process foreground
  
} // end if tick
} // end loop

void proceess_idle_mode(void){
  #ifdef SERIAL_ENABLE
  Serial.println("Idle");
  #endif
  
  WriteLED(0,0,0); //Off
  digitalWrite(RED_LED,~LED_ACTIVE);
  digitalWrite(GREEN_LED,~LED_ACTIVE);
  digitalWrite(BLUE_LED,~LED_ACTIVE);
  
  
  if (distance > maximum_distance)
    car_detect = 0;
       
  
}

