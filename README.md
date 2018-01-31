# ESP32 RMT peripheral IR library

Arduino friendly IR library utilizing ESP32 RMT peripheral

Protocols supported (send, limited receive):
- NEC
- SAMSUNG
- RC5

### What's new
IR code is now decoded correctly (NEC only for now).

However, couple of items in the RMT ringbuffer report incorrect duration sometimes.
Always data items at index 24 and 32, e.g. duration is 580ms instead of 1680. Not sure what is going on.
Tried different IR receivers, patch cables. Will try with another ESP32.

Need to do some more testing before implementing decode for the other protocols.

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

#### Version 0.3
- More reliable receive code (NEC only)
- Added extra debugging

#### Version 0.2a
- Added initial receive code for NEC (IR code is decoded but bits are reversed)
- Massive code refactoring (planning to support Arduino IDE + ESP-IDF toolchain with Eclipse)
- Fixed RMT_CLK_DIV, 80 instead of 100

#### Version 0.2
- GPIO can now be set at the beginning

#### Version 0.1
Initial version
- Arduino library with example sketch
- Send only

## Credits
Based on ideas from
- Arduino-IRremote library
- ESP-IDF infrared_nec_main.c example
- Various RMT examples on github
