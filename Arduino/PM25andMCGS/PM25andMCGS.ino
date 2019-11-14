#include "Seeed_HM330X.h"
#include <Wire.h>
#include "MutichannelGasSensor.h"

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

    gas.begin(0x04);
    gas.powerOn();
    Serial.print("Firmware Version = ");
    Serial.println(gas.getVersion());   
}



void loop(){

	Serial.print("NH3: ");
	Serial.println(gas.measure_NH3());

	Serial.print("CO: ");
	Serial.println(gas.measure_CO());

	Serial.print("NO2: ");
	Serial.println(gas.measure_NO2());

	Serial.print("C3H8: ");
	Serial.println(gas.measure_C3H8());

	Serial.print("C4H10: ");
	Serial.println(gas.measure_C4H10());

	Serial.print("CH4: ");
	Serial.println(gas.measure_CH4());

	Serial.print("H2: ");
	Serial.println(gas.measure_H2());

	Serial.print("C2H5OH: ");
	Serial.println(gas.measure_C2H5OH());

    if(sensor.read_sensor_value(buf,29)){

        Serial.println("HM330X read result failed!!!");

    }
    parse_result(buf);
    Serial.println(" ");
    Serial.println(" ");
    Serial.println(" ");
    delay(5000);
}
