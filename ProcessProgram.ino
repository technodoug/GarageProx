#define PROG_MODE_INIT 0
#define PROG_MODE_1 1
#define PROG_MODE_PROGRAM 2
#define PROG_MODE_3 3

void process_program_mode(void){
  
  static int prog_mode = PROG_MODE_INIT;
  static int prog_mode_ctr = 0;
  
   #ifdef SERIAL_ENABLE
   Serial.print(",ProgramMode");
   #endif

    if (++prog_mode_ctr >= PROG_MODE_TIMEOUT){
              Serial.print(",ProgramMode Timeout");
              prog_mode_ctr = 0;
              program_complete= 1;
              prog_mode = PROG_MODE_INIT;    
    
    }
    
    else
    {
   
      switch (prog_mode){
          case PROG_MODE_INIT:
            //Teal
            digitalWrite(RED_LED,~LED_ACTIVE);
            digitalWrite(GREEN_LED,LED_ACTIVE);  
            digitalWrite(BLUE_LED,LED_ACTIVE);
          
            if (process_pushbutton(digitalRead(BUTTON1) == BUTTON_RELEASE)){
              prog_mode = PROG_MODE_1;
            }
            break;
            
          case PROG_MODE_1: 

          
            if ((distance <= (MAX_DIST-WARN_DIST_DELTA)) && distance >= MIN_DIST ){
              //Green LED if within legal distance zone
              digitalWrite(RED_LED,~LED_ACTIVE);
              digitalWrite(GREEN_LED,LED_ACTIVE);  
              digitalWrite(BLUE_LED,~LED_ACTIVE);
              
              //can only program if within the legay range
              if (process_pushbutton(digitalRead(BUTTON1) == BUTTON_PRESS)){
                 prog_mode = PROG_MODE_PROGRAM;
              }
            } 
            else{ //red LED if illegal (too close or too far)
              digitalWrite(RED_LED,LED_ACTIVE);
              digitalWrite(GREEN_LED,~LED_ACTIVE);  
              digitalWrite(BLUE_LED,~LED_ACTIVE);
              #ifdef SERIAL_ENABLE
              Serial.print("Illegal distance: ");
              Serial.println(distance);
              #endif

            }       
            break;
            

          case PROG_MODE_PROGRAM:
            //Teal LED until button is released 
            digitalWrite(RED_LED,~LED_ACTIVE);
            digitalWrite(GREEN_LED,LED_ACTIVE);  
            digitalWrite(BLUE_LED,LED_ACTIVE);
   
           //write the data
            write_config_data(distance); 
           
            emergency_distance = EMERGENCY_DIST; //never changes
            stop_distance  = distance;
            warning_distance  = stop_distance + WARN_DIST_DELTA;
            maximum_distance = MAX_DIST; //never changes

            #ifdef SERIAL_ENABLE
            Serial.print("Stored new distance: ");
            Serial.println(distance);
            #endif

            if (process_pushbutton(digitalRead(BUTTON1) == BUTTON_RELEASE)){
              prog_mode = PROG_MODE_3;
            }
            
            break;
          case PROG_MODE_3:
            //programming done
            //White LED 
            digitalWrite(RED_LED,LED_ACTIVE);
            digitalWrite(GREEN_LED,LED_ACTIVE);  
            digitalWrite(BLUE_LED,LED_ACTIVE);
            program_complete= 1;
            prog_mode = PROG_MODE_INIT;
            break;
            
          default:
            break;
      }//end switch
    }//end else
  
}



void write_config_data (float dist){

     configuration_init = INIT_SIGNATURE;
     EEPROM.write(0, configuration_init);
     EEPROM.writeFloat(1, dist); 

}

