/*

Demonstrates simple RX and TX operation.
Any of the Basic_TX examples can be used as a transmitter.
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
#include <EEPROM.h> 
#define address 10

#include <SPI.h>
#include <NRFLite.h>

const static uint8_t RADIO_ID = 3;       // Our radio's id.  The transmitter will send to this id.
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
int16_t i;
NRFLite _radio;
RadioPacket _radioData;

//#include <ServoSmooth.h>
//ServoSmooth servo;



#include <Servo.h>
Servo servo;  // create servo object to control a servo


int newPos = 0;

void setup()
{
    Serial.begin(9600);
    
    Serial.println("Start");

    // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
    // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
    // You can run the 'ChannelScanner' example to help select the best channel for your environment.
    // You can assign a different bitrate and channel as shown below.
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
    
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }

    
    EEPROM.get(address, _radioData.value);
    newPos = map(_radioData.value, 0, 1023, 0, 155); // берём с потенцометра значение 0-180
    
    servo.attach(6);
    servo.write(newPos);
/*    servo.attach(6, 600, 2400);  // 600 и 2400 - длины импульсов, при которых
  // серво поворачивается максимально в одну и другую сторону, зависят от самой серво
  // и обычно даже указываются продавцом. Мы их тут указываем для того, чтобы
  // метод setTargetDeg() корректно отрабатывал диапазон поворота сервы
  
  servo.setSpeed(30);   // ограничить скорость
  servo.setAccel(1);  // установить ускорение (разгон и торможение)
  
  servo.setAutoDetach(true);  // отключить автоотключение (detach) при достижении целевого угла (по умолчанию включено)

  
    
  servo.setTargetDeg(newPos);               // и отправляем на серво
  servo.tick();   // здесь происходит движение серво по встроенному таймеру!
*/
  
}

void loop()
{
 
    while (_radio.hasData())
    {
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

        EEPROM.put(address, _radioData.value);

        String msg = "Radio ";
        msg += _radioData.FromRadioId;
        msg += ", ";
        msg += _radioData.OnTimeMillis;
        msg += " ms, ";
        msg += _radioData.OnTimeS;
        msg += " s, ";
        msg += _radioData.value;
        i = _radioData.value;
        msg += ", ";
        msg += _radioData.FailedTxCount;
        msg += " Failed TX;  |";
        for (i; i>0; i -= 20) { msg += "|"; }

        Serial.println(msg); 
        
        // желаемая позиция задаётся методом setTarget (импульс) или setTargetDeg (угол), далее
  // при вызове tick() производится автоматическое движение сервы
  // с заданным ускорением и ограничением скорости

  newPos = map(_radioData.value, 0, 1023, 0, 155); // берём с потенцометра значение 0-180
  servo.write(newPos);
  
    }


  
}

/*

void loop()
{
  servo.tick();   // здесь происходит движение серво по встроенному таймеру!

  
    while (_radio.hasData())
    {
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

        EEPROM.put(address, _radioData.value);

        String msg = "Radio ";
        msg += _radioData.FromRadioId;
        msg += ", ";
        msg += _radioData.OnTimeMillis;
        msg += " ms, ";
        msg += _radioData.OnTimeS;
        msg += " s, ";
        msg += _radioData.value;
        i = _radioData.value;
        msg += ", ";
        msg += _radioData.FailedTxCount;
        msg += " Failed TX;  |";
        for (i; i>0; i -= 20) { msg += "|"; }

        Serial.println(msg); 
        
        // желаемая позиция задаётся методом setTarget (импульс) или setTargetDeg (угол), далее
  // при вызове tick() производится автоматическое движение сервы
  // с заданным ускорением и ограничением скорости

  newPos = map(_radioData.value, 0, 1023, 0, 155); // берём с потенцометра значение 0-180
  servo.setTargetDeg(newPos);               // и отправляем на серво
  servo.tick();   // здесь происходит движение серво по встроенному таймеру!
    }


  
}*/
