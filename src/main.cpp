#include <SPI.h>
#include <NRFLite.h>

NRFLite _radio;
int success;



uint32_t lastTime = 0;

struct RadioPacket
{
    uint32_t time;
    uint32_t pulses8s;
    uint32_t pulses;

};


RadioPacket radioPacket;

void setup()
{
    Serial.begin(9600);
    success = _radio.init(0, D2, D8, NRFLite::BITRATE250KBPS); // Set this radio's Id = 0, along with its CE and CSN pins

    if(success){
        Serial.println("RX Init success");
    } else
    {
        Serial.println("RX Init failed");
    }
    
    
}


void loop()
{
    while (_radio.hasData())
    {
            
        _radio.readData(&radioPacket);
       
       Serial.print("Time RX:");
       Serial.println(radioPacket.time);

    Serial.print("Momentan Power kW:");
    Serial.println(0.36f * radioPacket.pulses8s / (radioPacket.time - lastTime));
    lastTime = radioPacket.time;

    Serial.print("Avg Power kW:");
    Serial.println(0.36f * radioPacket.pulses / radioPacket.time);

    }
}

