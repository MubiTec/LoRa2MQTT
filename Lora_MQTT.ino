#include <SPI.h>
#include <LoRa.h>

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "xxx";
const char* password = "xxx";
const char* mqtt_server = "192.168.178.71";

/* create an instance of PubSubClient client */
WiFiClient espClient;
PubSubClient client(espClient);

int c;
int i;
int k;
long lastMsg = 0;
char topic[50];
char topic_RSSI[50];
char payload[50];
char msg[50];
char RSSI_[50];
long activ=0;

#define ss 18
#define rst 14
#define dio0 26

void mqttconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Client";
    if (client.connect(clientId.c_str())) {
    } else {
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(9600);
client.publish("lora/restart", "false");
    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  client.setServer(mqtt_server, 1884);  
  while (!Serial);
  Serial.println("LoRa Rx Test");
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

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  WiFi.begin(ssid, password);
delay(500);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  // try to parse packet
    if (!client.connected()) {
    mqttconnect();
  }
    client.loop();
  if (millis()>=3600000){

  client.publish("lora/restart", "true");
  delay(500);
  ESP.restart();
  }

  if (millis()-activ>=60000){
    Serial.println("con");
  client.publish("lora/connected", "true");
  activ=millis();
  }
  
  int wifi_retry = 0;
  while(WiFi.status() != WL_CONNECTED && wifi_retry < 5 ) {
  wifi_retry++;
  Serial.println("Reboot__");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  delay(500);
  }
  if(wifi_retry >= 5) {
      ESP.restart();
  }
  memset(msg, 0, 50);
  memset(topic, 0, 50);
  memset(payload, 0, 50);
  memset(topic_RSSI, 0, 50);
  //Serial.println("LoRa Rx Test");
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // read packet
    c=0;
    while (LoRa.available()) {
      msg[c] += (char)LoRa.read();
      c += 1;
    }



c -= 1;
if (msg[0]=='$' && msg[1]=='%' && msg[2]=='&'){
  for (k = 3; k <= c; k++) {
      if (msg[k]=='=')
      {
        break;
      }
      topic[k-3] += (char)msg[k];
  }

  for (i = k + 1; i <= c; i++) {
      payload[i-k-1] += (char)msg[i];
  }

  client.publish(topic, payload);

  for (i = k-1; i >= 0; i--) {
    if (topic[i]=='/' ){
      break;
    }}
  for(k=0;k<=i;k++){
      topic_RSSI[k]+= (char)topic[k];
  }

   
    topic_RSSI[i+1] = 'R';
    topic_RSSI[i+2] = 'S';
    topic_RSSI[i+3] = 'S';
    topic_RSSI[i+4] = 'I';

itoa(LoRa.packetRssi(), RSSI_, 10);
client.publish(topic_RSSI,RSSI_);

  }
  }
  }
