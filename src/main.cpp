#include <SPI.h>
#include <NRFLite.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <movingAvg.h>
#include "credentials.h"


void mqttCallback(char* topic, byte* payload, unsigned int length);
boolean setup_wifi();
boolean setup_mqtt();



NRFLite _radio;
int success;

// Credentials 
char ssid[] = SSIDNAME;
char password[] = PASSWORD;

// Wifi and MQTT
WiFiClient espClient;


IPAddress mqttServer(192, 168, 1, 200);
PubSubClient mqttClient(mqttServer, 1883, mqttCallback, espClient);
movingAvg mom(4);
movingAvg avg(240);




uint32_t lastTime = 0;
uint32_t now;

struct RadioPacket
{
    uint32_t pulses;
    uint32_t pulsesTotal;
};


RadioPacket radioPacket;

void setup(){

    Serial.begin(9600);

    
    success = _radio.init(0, D2, D8, NRFLite::BITRATE250KBPS); // Set this radio's Id = 0, along with its CE and CSN pins

    if(success){
        Serial.println("RX Init success");
    } else
    {
        Serial.println("RX Init failed");
    }
    
    success = setup_wifi();
    if(!success){
        Serial.println("Wifi setup failed");
    }

    
}


void loop()
{

    if (!mqttClient.connected()) {
        setup_mqtt(); 
     } else {
        // Client connected
        mqttClient.loop();
    }

    while (_radio.hasData()){
        
        now = millis();

        _radio.readData(&radioPacket);
       
        float power = 360 * radioPacket.pulses / (float)(now - lastTime);
        char str[20];
        snprintf(str, sizeof(str),  "%.2f", power);
        mqttClient.publish("stat/emeter/mompower", str);

        lastTime = now;

        Serial.print("Power: ");
        Serial.println(power);
      }
    
}

boolean setup_wifi() {

  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSIDNAME);


  WiFi.begin(ssid, password);

  int attempt = 30;

  while(attempt){
    if(WiFi.status() == WL_CONNECTED){

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
    }
    delay(500);
    attempt--;
  }

return false;
}


boolean setup_mqtt(){

  char clientName[50];


    snprintf (clientName, 50, "%ld", system_get_chip_id());

    if (mqttClient.connect(clientName, MQTTUSER, MQTTPASSWORD)) {
      char str[30];
      strcpy(str, "The Emeter is connected to MQTT ");
      mqttClient.publish("stat/emeter/hello", str);
    } else {

      delay(1000);
      Serial.println("Connecting to MQTT server ...");

    }
  
  return mqttClient.connected();
}


void mqttCallback(char* topic, byte* payload, unsigned int length) {
  
}



