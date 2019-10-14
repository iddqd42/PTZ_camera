/*

Demonstrates simple RX and TX operation.
Any of the Basic_RX examples can be used as a receiver.
Please read through 'NRFLite.h' for a description of all the methods available in the library.

Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

*/

#include <SPI.h>
#include <NRFLite.h>



const static uint8_t RADIO_ID = 1;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 3; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
    uint32_t OnTimeS;
    uint32_t FailedTxCount;
    int16_t value;
};

NRFLite _radio;
RadioPacket _radioData;



#define CLK 5
#define DT 6
#define SW 7
#include "GyverEncoder.h"
Encoder enc1(CLK, DT, SW);


uint32_t Time1 = 0;       // максимальное время работы модема

void setup()
{
    Serial.begin(9600);
    
    Serial.print("Start");
    
    // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
    // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
    // You can run the 'ChannelScanner' example to help select the best channel for your environment.
    // You can assign a different bitrate and channel as shown below.
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0);
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100); // THE DEFAULT
    
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
    
    _radioData.FromRadioId = RADIO_ID;

    
enc1.setType(TYPE2);    // тип энкодера TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
}

void loop()
{


/*  enc1.tick();
if (enc1.isRight()) _radioData.value += 20;      // если был поворот направо, увеличиваем на 1
if (enc1.isLeft()) _radioData.value -= 20;     // если был поворот налево, уменьшаем на 1
if (enc1.isRightH()) _radioData.value += 20;  // если было удержание + поворот направо, увеличиваем на 5
if (enc1.isLeftH()) _radioData.value -= 20; // если было удержание + поворот налево, уменьшаем на 5  
if (enc1.isTurn()) {       // если был совершён поворот (индикатор поворота в любую сторону)
*/
delay(15);
if ( (_radioData.value - analogRead(A0)) > 2 || (_radioData.value - analogRead(A0)) < -2)
   {
   _radioData.value =analogRead(A0);
  if (_radioData.value < 0) _radioData.value = 0;
  if (_radioData.value > 1023) _radioData.value = 1023;
  
Serial.println(_radioData.value);   // выводим значение при повороте

for (int i=0; i<5; i++){
    _radioData.OnTimeMillis = millis();
    _radioData.OnTimeS = millis()/1000;

    Serial.print("Sending ");
    Serial.print(_radioData.OnTimeMillis);
    Serial.print(" ms");

    // By default, 'send' transmits data and waits for an acknowledgement.  If no acknowledgement is received,
    // it will try again up to 16 times.  You can also perform a NO_ACK send that does not request an acknowledgement.
    // The data packet will only be transmitted a single time so there is no guarantee it will be successful.  Any random
    // electromagnetic interference can sporatically cause packets to be lost, so NO_ACK sends are only suited for certain
    // types of situations, such as streaming real-time data where performance is more important than reliability.
    //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)
    //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // THE DEFAULT
    
    if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) // Note how '&' must be placed in front of the variable name.
    {
        Serial.println("...Success");
        i=3;
    }
    else
    {
        Serial.println("...Failed");
        _radioData.FailedTxCount++;
    }
}
}
}
