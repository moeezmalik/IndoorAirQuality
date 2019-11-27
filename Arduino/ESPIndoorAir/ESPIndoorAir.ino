#include<SoftwareSerial.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266HTTPClient.h>

const char* ssid     = "wiFi";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "sudoanaconda";     // The password of the Wi-Fi network

#define URX D1
#define UTX D2


SoftwareSerial UART(URX, UTX);

struct Sensor_data{
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

Sensor_data data;

byte buf[sizeof(data)];

void setup() {

  pinMode(D0, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D1, INPUT);
  pinMode(D2, OUTPUT);
  
  // put your setup code here, to run once:

  Serial.begin(9600);
  

  digitalWrite(D0, LOW);


  ////////////////WiFi Setup////////////////////
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    digitalWrite(D0, LOW);
      delay(500);
      digitalWrite(D0, HIGH);
      delay(500);
    Serial.print("Connecting..");
  }

  UART.begin(9600);
  //UART.end()
  Serial.println("Starting");

}

void loop() {
  // put your main code here, to run repeatedly:
    if (UART.available()>0){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      
      UART.readBytes(buf , sizeof(buf));
      
      memcpy(&data , buf , sizeof(buf));
      
      Serial.print("NH3: ");
      Serial.println(data.nh3);
      Serial.print("CO: ");
      Serial.println(data.co);
      Serial.print("NO2: ");
      Serial.println(data.no2);
      Serial.print("C3H8: ");
      Serial.println(data.c3h8);
      Serial.print("C4H10: ");
      Serial.println(data.c4h10);
      Serial.print("CH4: ");
      Serial.println(data.ch4);
      Serial.print("H2: ");
      Serial.println(data.h2);
      Serial.print("C2H5OH: ");
      Serial.println(data.c2h5oh);
      Serial.print("PM2.5: ");
      Serial.println(data.pm25);
      Serial.print("CO2: ");
      Serial.println(data.co2);
      Serial.print("Temperature: ");
      Serial.println(data.temperature);
      Serial.print("Humidity: ");
      Serial.println(data.humidity);
      Serial.println("-------------------------------");
      sendValues();
     }
     Serial.println("I am working");
}


void sendValues(){
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      HTTPClient http;  //Declare an object of class HTTPClient
      String httpString=String("http://35.239.124.91:4322/store?203&")+String(data.nh3)+"&"+String(data.co)+"&"+String(data.no2)+"&"+String(data.ch4)+"&"+String(data.co2)+"&"+String(data.pm25)+"&"+String(data.temperature)+"&"+String(data.humidity)+String("&*****&*****");
      http.begin(httpString);
      int httpCode = http.GET();  //Send the request
      Serial.println(httpString);
      if(httpCode==200){
          digitalWrite(D0,LOW);
          delay(100);
          digitalWrite(D0,HIGH);
          delay(100);
          digitalWrite(D0,LOW);
          delay(100);
          digitalWrite(D0,HIGH);
        }
      
      Serial.println(String(httpCode));
      if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);   //Print the response payload
    }
      http.end();   //Close connection
    }
    delay(5000);    //Send a request every 30 seconds
  }
