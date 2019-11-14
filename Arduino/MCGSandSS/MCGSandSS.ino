#include<SoftwareSerial.h>
#include <Wire.h>
#include "MutichannelGasSensor.h"
#include "MHZ19.h"
#include "DHT.h"
#include "Seeed_HM330X.h"

HM330X sensor;
u8 buf[30];

float fromPM25[7];

const int sensorIndPin = 11;

SoftwareSerial mySerial(4, 3);

//Things for PPD42NS Particulate Matter Sensor
int pm25Pin = 6;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000; 
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

//Things for CO2 Sensor
const int rx_pin = 10; //Serial rx pin no
const int tx_pin = 9; //Serial tx pin no

MHZ19 *mhz19_uart = new MHZ19(rx_pin,tx_pin);

//Things for the Temperature and Humidity Sensor
#define DHTPIN 2 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

struct sensorData{
  
  float nh3 = 0.0;
  float co = 0.0;
  float no2 = 0.0;
  float c3h8 = 0.0;
  float c4h10 = 0.0;
  float ch4 = 0.0;
  float h2 = 0.0;
  float c2h5oh = 0.0;
  float pm25 = 0.0;
  float co2 = 0.0;
  float temperature = 0.0;
  float humidity = 0.0;
  
};

float parse_result(u8 *data){
  
    u16 value=0;
    for(int i=1;i<8;i++){

         value = (u16)data[i*2]<<8|data[i*2+1];
         fromPM25[i-1] = value;

    }

    //for(int i = 0; i < 7; i++){
    //
    //  Serial.print(i);
    //  Serial.print(": ");
    //  Serial.println(fromPM25[i]);
    //}

    return fromPM25[4];
}

byte outGoingBuffer[sizeof(sensorData)];

sensorData getValuesFromSensors(){

  sensorData gatheredData;

  delay(100);
  //Gathering Data from the Multichannel Gas Sensor
  gatheredData.nh3 = gas.measure_NH3();
  gatheredData.co = gas.measure_CO();
  gatheredData.no2 = gas.measure_NO2();
  gatheredData.c3h8 = gas.measure_C3H8();
  gatheredData.c4h10 = gas.measure_C4H10();
  gatheredData.ch4 = gas.measure_CH4();
  gatheredData.h2 = gas.measure_H2();
  gatheredData.c2h5oh = gas.measure_C2H5OH();
  
  delay(100);
  
  if(sensor.read_sensor_value(buf,29)){
    Serial.println("HM330X read result failed!!!");  
  }
  
  gatheredData.pm25 =(float) parse_result(buf);

  //gatheredData.pm25 = 0.1;
  delay(100);

  gatheredData.co2 = mhz19_uart->getPPM(MHZ19_POTOCOL::UART);

  delay(100);
  
  gatheredData.temperature = dht.readTemperature();
  gatheredData.humidity = dht.readHumidity();

  return gatheredData;
}

void setup(){

  pinMode(sensorIndPin, OUTPUT);
  digitalWrite(sensorIndPin, LOW);

  //Initializing Serial Communication Protocol
  mySerial.begin(9600);
  Serial.begin(9600);  // start serial for output
  mhz19_uart->begin(rx_pin, tx_pin);

  //Setting up CO2 Sensor
  mhz19_uart->setAutoCalibration(false);
  while (mhz19_uart->isWarming()){
    
      Serial.print("MH-Z19 now warming up...  status:");
      Serial.println(mhz19_uart->getStatus());

      digitalWrite(sensorIndPin, LOW);
      delay(500);
      digitalWrite(sensorIndPin, HIGH);
      delay(500);
  }

  //Setting Up MCGS
  Serial.println("power on!");
  gas.begin(0x04);//the default I2C address of the slave is 0x04
  gas.powerOn();
  Serial.print("Firmware Version = ");
  Serial.println(gas.getVersion());

  //Setting up Dust Sensor
  pinMode(pm25Pin , INPUT);

  //Setting up Temperature and Humidity Sensor
  DHT dht(DHTPIN, DHTTYPE);

  // Starting PM25 Sensor
  if(sensor.init()){
        Serial.println("HM330X init failed!!!");
        while(1);
    } 

  digitalWrite(sensorIndPin, HIGH);
  
}

void loop(){

  Serial.println("Gathering Data...");

  sensorData dataToSend = getValuesFromSensors();

  memcpy(&outGoingBuffer, &dataToSend, sizeof(dataToSend));

  Serial.println("Sending Data...");
  
  mySerial.write(outGoingBuffer, sizeof(outGoingBuffer));

  digitalWrite(sensorIndPin, LOW);
  delay(50);
  digitalWrite(sensorIndPin, HIGH);
  delay(50);
  digitalWrite(sensorIndPin, LOW);
  delay(50);
  digitalWrite(sensorIndPin, HIGH);
  delay(50);
  digitalWrite(sensorIndPin, LOW);
  delay(50);
  digitalWrite(sensorIndPin, HIGH);

  Serial.print("NH3: ");
  Serial.println(dataToSend.nh3);
  Serial.print("CO: ");
  Serial.println(dataToSend.co);
  Serial.print("NO2: ");
  Serial.println(dataToSend.no2);
  Serial.print("C3H8: ");
  Serial.println(dataToSend.c3h8);
  Serial.print("C4H10: ");
  Serial.println(dataToSend.c4h10);
  Serial.print("CH4: ");
  Serial.println(dataToSend.ch4);
  Serial.print("H2: ");
  Serial.println(dataToSend.h2);
  Serial.print("C2H5OH: ");
  Serial.println(dataToSend.c2h5oh);
  Serial.print("PM2.5: ");
  Serial.println(dataToSend.pm25);
  Serial.print("CO2: ");
  Serial.println(dataToSend.co2);
  Serial.print("Tmeperature: ");
  Serial.println(dataToSend.temperature);
  Serial.print("Humidity: ");
  Serial.println(dataToSend.humidity);

  Serial.println();
  Serial.println("---------------");
  Serial.println();
  
  delay(5000);
    
}
