
int process_timeout (int run_timer){

  static unsigned int led_timeout_ctr = 0;
  int led_timeout = 0;
  
  //timeout processing
    //turn off the LED some time after car is detected
    if(run_timer){
       #ifdef SERIAL_ENABLE
        Serial.print(led_timeout_ctr); 
       #endif
        
        if(++led_timeout_ctr >= MAX_LED_TIMEOUT){
          led_timeout_ctr = MAX_LED_TIMEOUT;
          led_timeout = 1;
          #ifdef SERIAL_ENABLE
          Serial.print(",TIMEOUT");
          #endif
     }
      
    }//end if car detected
    else{// car no longer detected
       led_timeout_ctr = 0;  
       led_timeout = 0;       
       #ifdef SERIAL_ENABLE
       Serial.print(",NO TIMEOUT");
       #endif
    }
  
  return (led_timeout);        
}
#define PRESS_CNT 5
#define HOLD_CNT 10



int process_pushbutton(int buttonstate){
 
 static unsigned int pb_timeout_ctr = 0; 
 
 int button_out = BUTTON_RELEASE;
 
 if (buttonstate == BUTTON_ACTIVE){

   ++pb_timeout_ctr;
   
   if(pb_timeout_ctr >= PRESS_CNT){
     button_out = BUTTON_PRESS;
   }
   
   //this is not working for some reason:
   if(pb_timeout_ctr >= HOLD_CNT){
     button_out = BUTTON_HOLD;
     pb_timeout_ctr = HOLD_CNT;
   }
   
   
 }
 else{
   pb_timeout_ctr = 0;
   button_out = BUTTON_RELEASE;
 }
 
   
 
 
 return (button_out);
 
 
  
}
