#include "Seeed_HM330X.h"

HM330X sensor;
u8 buf[30];

int fromPM25[7];


/*parse buf with 29 u8-data*/
err_t parse_result(u8 *data){
    u16 value=0;
    err_t NO_ERROR;
    if(NULL==data)
        return ERROR_PARAM;
    for(int i=1;i<8;i++){

         value = (u16)data[i*2]<<8|data[i*2+1];
         fromPM25[i-1] = value;

    }

    for(int i = 0; i < 7; i++){

    	Serial.print(i);
    	Serial.print(": ");
    	Serial.println(fromPM25[i]);
    }
}


void setup(){
    Serial.begin(9600);
    delay(100);
    Serial.println("Serial start");
    if(sensor.init())
    {
        Serial.println("HM330X init failed!!!");
        while(1);
    }   
}



void loop(){

    if(sensor.read_sensor_value(buf,29)){

        Serial.println("HM330X read result failed!!!");

    }
    parse_result(buf);
    Serial.println(" ");
    Serial.println(" ");
    Serial.println(" ");
    delay(5000);
}
