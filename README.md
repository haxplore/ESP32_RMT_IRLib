# ESP32 RMT peripheral IR library

Arduino friendly IR library utilizing ESP32 RMT peripheral

Protocols supported (send only):
- NEC
- SAMSUNG
- RC5

### What's new
- Massive code refactoring (planning to support Arduino IDE + ESP-IDF toolchain with Eclipse)
- Added initial receive code for NEC (IR code is decoded but bits are in the wrong order at the moment)
- Fixed RMT_CLK_DIV, 80 instead of 100

## Example

```
#include <RMTLib.h>

RMTLib RMTLib;

const int TX_PIN = 27;

void setup()
{
  Serial.begin(115200);
  
  RMTLib.setTxPin(TX_PIN);
}

void loop()
{
  Serial.println("sending command");

  RMTLib.sendNEC(0x5743C03F);
  delay(5000);
}
```

## TODO
- Repeat/toggle functionality
- Receive and decode
- Refactoring current prototype level code

## Planned
- Examples to send/decode IR code through WIFI (Web, MQTT or CoAP)

## History

#### Version 0.2a
- Initial version of receive code

#### Version 0.2
- GPIO can now be set at the beginning

#### Version 0.1
Initial version
- Arduino library with example sketch
- Send only

## Credits
Based on 
- Arduino-IRremote library
- ESP-IDF infrared_nec_main.c example
- Various RMT examples on github.com
