/*
 *
 *
 */

#include "Arduino.h"
#include "RMTLib.h"
#include "esp32-hal-rmt.h" 

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif


RMTLib::RMTLib () { 
	tx_pin = RMT_TX_GPIO_NUM;
}
	
void RMTLib::begin(void) {
	
}

void RMTLib::send(void) {
	
}

void RMTLib::setTxPin(unsigned short pin) {
	tx_pin = (gpio_num_t)pin;
	//rmt_set_tx_pin((gpio_num_t)pin);
}

// Private


