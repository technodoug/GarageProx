void process_normal_mode(void){

    if (distance <= emergency_distance){
      car_detect = 1;
      if (blink_ctr == 1){
        WriteLED(1023,0,0); //Red
        digitalWrite(RED_LED,LED_ACTIVE);
        digitalWrite(GREEN_LED,~LED_ACTIVE);  
        digitalWrite(BLUE_LED,~LED_ACTIVE);
      }
      else{
        WriteLED(0,0,1023); //Blue
        digitalWrite(RED_LED,~LED_ACTIVE);
        digitalWrite(GREEN_LED,~LED_ACTIVE);  
        digitalWrite(BLUE_LED,LED_ACTIVE);
      }
    #ifdef SERIAL_ENABLE
    Serial.print(",EMERG STOP");
    #endif
    }
    
    else if (distance <= stop_distance){
      car_detect = 1;
      WriteLED(1023,0,0); //Red
      digitalWrite(RED_LED,LED_ACTIVE);
      digitalWrite(GREEN_LED,~LED_ACTIVE);
      digitalWrite(BLUE_LED,~LED_ACTIVE);
      #ifdef SERIAL_ENABLE
      Serial.print(",STOP");
      #endif
    }
    else if (distance <= warning_distance){
      car_detect = 1;
      WriteLED(0,1023,0); //Green
      digitalWrite(RED_LED,~LED_ACTIVE);
      digitalWrite(GREEN_LED,LED_ACTIVE);
      digitalWrite(BLUE_LED,~LED_ACTIVE);
      #ifdef SERIAL_ENABLE
      Serial.print(",WARN");
      #endif
      
    }
    else if (distance <= maximum_distance) {
      car_detect = 1;
      WriteLED(0,0,1023); //Blue
      digitalWrite(RED_LED,~LED_ACTIVE);
      digitalWrite(GREEN_LED,~LED_ACTIVE);
      digitalWrite(BLUE_LED,LED_ACTIVE);
      #ifdef SERIAL_ENABLE
      Serial.print(",MAX");
      #endif      
    }
    
    
    else{
       car_detect = 0;
       WriteLED(0,0,0); //Off
       digitalWrite(RED_LED,~LED_ACTIVE);
       digitalWrite(GREEN_LED,~LED_ACTIVE);
       digitalWrite(BLUE_LED,~LED_ACTIVE);
       #ifdef SERIAL_ENABLE
       Serial.print(",No Car Detected");
       #endif  
      
    }
    
    
}
