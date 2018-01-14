/*
8 bit address and 8 bit command length.
Address and command are transmitted twice for reliability. (32bit)
Extended mode available, doubling the address size.
Pulse distance modulation.
Carrier frequency of 38kHz.
Bit time of 1.125ms or 2.25ms
Each pulse is a 560µs long 38kHz carrier burst (about 21 cycles). 
A logical "1" takes 2.25ms to transmit, while a logical "0" is only half of that, being 1.125ms. 
The recommended carrier duty-cycle is 1/4 or 1/3.
*/
#include "Arduino.h"
#include "RMTLib.h"
#include "esp_log.h"
#include "esp32-hal-rmt.h" 

const char* NEC_TAG = "NEC";

#define NEC_BITS              	32
#define NEC_HEADER_HIGH_US    	9000                         /*!< NEC protocol header: positive 9ms */
#define NEC_HEADER_LOW_US     	4500                         /*!< NEC protocol header: negative 4.5ms*/
#define NEC_BIT_ONE_HIGH_US    	560                         /*!< NEC protocol data bit 1: positive 0.56ms */
//#define NEC_BIT_ONE_LOW_US    	(2250-NEC_BIT_ONE_HIGH_US)   /*!< NEC protocol data bit 1: negative 1.69ms */
#define NEC_BIT_ONE_LOW_US    1690
#define NEC_BIT_ZERO_HIGH_US   	560                         /*!< NEC protocol data bit 0: positive 0.56ms */
//#define NEC_BIT_ZERO_LOW_US   	(1120-NEC_BIT_ZERO_HIGH_US)  /*!< NEC protocol data bit 0: negative 0.56ms */
#define NEC_BIT_ZERO_LOW_US    560
#define NEC_BIT_END            	560                         /*!< NEC protocol end: positive 0.56ms */
#define NEC_BIT_MARGIN         	20                          /*!< NEC parse margin time */
//
#define NEC_DATA_ITEM_NUM   	34  /*!< NEC code item number: header + 32bit data + end */


#if SEND_NEC
/*
 * @brief Generate NEC header value: active 9ms + negative 4.5ms
 */
void nec_fill_item_header(rmt_item32_t* item)
{
    rmt_fill_item_level(item, NEC_HEADER_HIGH_US, NEC_HEADER_LOW_US);
}

/*
 * @brief Generate NEC data bit 1: positive 0.56ms + negative 1.69ms
 */
void nec_fill_item_bit_one(rmt_item32_t* item)
{
    rmt_fill_item_level(item, NEC_BIT_ONE_HIGH_US, NEC_BIT_ONE_LOW_US);
}

/*
 * @brief Generate NEC data bit 0: positive 0.56ms + negative 0.56ms
 */
void nec_fill_item_bit_zero(rmt_item32_t* item)
{
    rmt_fill_item_level(item, NEC_BIT_ZERO_HIGH_US, NEC_BIT_ZERO_LOW_US);
}

/*
 * @brief Generate NEC end signal: positive 0.56ms
 */
void nec_fill_item_end(rmt_item32_t* item)
{
    rmt_fill_item_level(item, NEC_BIT_END, 0x7fff);
}

/*
  @brief Build NEC 32bit waveform.
*/
void nec_build_items(rmt_item32_t* item, uint32_t cmd_data)
{
  nec_fill_item_header(item++);
  
  uint32_t mask = 0x80000000;
  for (int j = 0; j < 32; j++) {
    if (cmd_data & mask) {
      nec_fill_item_bit_one(item);
    } else {
      nec_fill_item_bit_zero(item);
    }
    item++;
    mask >>= 1;
  }

  nec_fill_item_end(item);
}


void nec_tx_init(gpio_num_t gpio_num)
{
	rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.gpio_num = gpio_num;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_duty_percent = RMT_CARRIER_DUTY;
    rmt_tx.tx_config.carrier_freq_hz = RMT_CARRIER_FREQ_38;
    rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = RMT_MODE_TX;
	
	rmt_tx_init(&rmt_tx);
}

// public exports

void RMTLib::sendNEC(unsigned long data)
{
	Serial.println("sendNEC");
	
	// what is the reason for this?
	vTaskDelay(10);
	nec_tx_init(RMTLib::tx_pin);

	esp_log_level_set(NEC_TAG, ESP_LOG_INFO);
	ESP_LOGI(NEC_TAG, "RMT TX DATA");

	size_t size = (sizeof(rmt_item32_t) * NEC_DATA_ITEM_NUM * RMT_TX_DATA_NUM);
	rmt_item32_t* item = (rmt_item32_t*) malloc(size);
	memset((void*) item, 0, size);

	nec_build_items(item, data);

	int item_num = NEC_DATA_ITEM_NUM * RMT_TX_DATA_NUM;
	rmt_write_items(RMT_TX_CHANNEL, item, item_num, true);
	rmt_wait_tx_done(RMT_TX_CHANNEL);
	free(item);
}
#endif