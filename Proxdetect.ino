

  
int motion_detect(float diff_limit, float distance){    
   static float prev_dist = 0;
   static unsigned int motion_count_approach = 0;
   static unsigned int motion_count_recede = 0;
   int motion_detected = 0;
  
    //Motion detection processing
    float diff = prev_dist-distance;
    #ifdef SERIAL_ENABLE
    Serial.print(",diff = ");
    Serial.print (diff);
    #endif
    
    if(diff > diff_limit){
      if (++motion_count_approach > MIN_MOTION_COUNT){
        motion_count_approach = MIN_MOTION_COUNT;
        #ifdef SERIAL_ENABLE
        Serial.print(",Approaching");
        #endif
        motion_detected = 1;
      }
  
    }
    else if (diff < -diff_limit) {
      if (++motion_count_recede > MIN_MOTION_COUNT){
        motion_count_recede = MIN_MOTION_COUNT;
        #ifdef SERIAL_ENABLE
        Serial.print(",Receeding");
        #endif
        motion_detected = -1;
      }
    }
    else{
      motion_count_approach = 0;
      motion_count_recede = 0;
      motion_detected = 0;
      
    }
    
    #ifdef SERIAL_ENABLE
    Serial.println("");
    #endif
    
    prev_dist = distance;

    return (motion_detected);

}

