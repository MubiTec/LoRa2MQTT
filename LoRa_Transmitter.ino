#include <SPI.h>
#include <LoRa.h>

#define ss 18
#define rst 14
#define dio0 26

int sensor=19;
int counter = 0;
int val=0;
int val_old=5;
long activ=0;
long last_temp=0;

#include "DHT.h"
//here we use 23 of ESP32 to read data
#define DHTPIN 33
//our sensor is DHT11 type
#define DHTTYPE DHT11
//create an instance of DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(sensor, INPUT);
  //while (!Serial);
  Serial.println("LoRa Sender");
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(ss, rst, dio0);
  
  if (!LoRa.begin(868E6)) {
    Serial.println("Start of LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.crc();
}

void loop() {
  if (millis()-activ>=300000){
  val_old=5;
  activ=millis();
  }
  if (millis()>=3600000){
  delay(500);
  ESP.restart();
  }

  val=digitalRead(sensor);
  Serial.print(val);
  Serial.print(val_old);
  Serial.print("Sending packet: ");
  Serial.println(counter);
  // send packet
  Serial.print(val!=val_old); 
  if (val!=val_old)
  {
    
  LoRa.beginPacket();
  LoRa.print("$%&");
  LoRa.print("lora/garage/opened");
  if (val==HIGH)
  {
    LoRa.print("=true");
  }else
   {LoRa.print("=false");
  }
  LoRa.endPacket();
  } 
  counter++;
  val_old=val;

if (((millis() - last_temp) >= 5*60*1000) || last_temp==0){
last_temp = millis();
  //use the functions which are supplied by library.
float h = dht.readHumidity();
// Read temperature as Celsius (the default)
float t = dht.readTemperature();
  LoRa.beginPacket();
  LoRa.print("$%&");
  LoRa.print("lora/garage/temperature=");
  LoRa.print(t);
  LoRa.endPacket();
  LoRa.beginPacket();
  LoRa.print("$%&");
  LoRa.print("lora/garage/humidity=");
  LoRa.print(h);
  LoRa.endPacket();
}

// Check if any reads failed and exit early (to try again).
//if (isnan(h) || isnan(t)) {
//Serial.println(t);
//return;
//}  
  
  delay(250);
}
