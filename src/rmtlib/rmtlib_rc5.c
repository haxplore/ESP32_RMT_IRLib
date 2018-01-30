/*
5 bit address + 6 bit command length
Carrier frequency of 36kHz
Bi-phase coding (aka Manchester coding)
Constant bit time of 1.778ms (64 cycles of 36 kHz)
RC5X command 7 bits
*/
#include "esp32_rmt_common.h"
#include "esp32_rmt_remotes.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "esp_log.h"

#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"

#include "freertos/ringbuf.h"

const char* RC5_TAG = "RC5";

#define RC5_BITS              	11 			// 5 + 6

#define RC5_BIT_US    			1778
#define RC5_BIT_HALF_US    		889

#define RC5_BIT_ONE_HIGH_US    	889
#define RC5_BIT_ONE_LOW_US    	889
#define RC5_BIT_ZERO_HIGH_US   	889 
#define RC5_BIT_ZERO_LOW_US    	889
//#define RC5_BIT_ONE_LOW_US    	(1778-RC5_BIT_ONE_HIGH_US)
//#define RC5_BIT_ZERO_LOW_US   	(1778-RC5_BIT_ZERO_HIGH_US)

//
#define RC5_DATA_ITEM_NUM   	15  /*!< RC5 code item number: 2 start + 1 toggle + 5 addr + 6 data + rmt end */


#if SEND_RC5

/*
 * @brief Generate data bit 1
 */
void rc5_fill_item_bit_one(rmt_item32_t* item)
{
    rmt_fill_item_level_me(item, RC5_BIT_HALF_US, true);
}

/*
 * @brief Generate data bit 0
 */
void rc5_fill_item_bit_zero(rmt_item32_t* item)
{
    rmt_fill_item_level_me(item, RC5_BIT_HALF_US, false);
}

/*
  @brief Build waveform
*/
void rc5_build_items(rmt_item32_t* item, uint32_t cmd_data)
{
	// header
  // 1 1 0 - 	[s]ms, mms, 2x start, 1x toggle, (s_ilent,m_ark)
  rc5_fill_item_bit_one(item++);
  rc5_fill_item_bit_one(item++);
  rc5_fill_item_bit_zero(item++);
  
  // parse from left to right 11 bits (0x400)
  uint32_t mask = 0x01;
  mask <<= RC5_BITS - 1;
  //Serial.println(mask, HEX);
  for (int j = 0; j < RC5_BITS; j++) {
    if (cmd_data & mask) {
      rc5_fill_item_bit_one(item);
    } else {
      rc5_fill_item_bit_zero(item);
    }
    item++;
    mask >>= 1;
  }

  // add RMT end block
  rmt_fill_end_item(item);
}


void rc5_tx_init(gpio_num_t gpio_num)
{
	rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.gpio_num = gpio_num;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_duty_percent = 50;
    rmt_tx.tx_config.carrier_freq_hz = RMT_CARRIER_FREQ_36;
    rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    
	rmt_tx_init(&rmt_tx);
}

// public exports

void rmtlib_rc5_send(unsigned long data)
{
	vTaskDelay(10);
	rc5_tx_init(RMT_TX_GPIO_NUM);
	
	esp_log_level_set(RC5_TAG, ESP_LOG_INFO);
	ESP_LOGI(RC5_TAG, "RMT TX DATA");
	
	size_t size = (sizeof(rmt_item32_t) * RC5_DATA_ITEM_NUM * RMT_TX_DATA_NUM);
	rmt_item32_t* item = (rmt_item32_t*) malloc(size);
	memset((void*) item, 0, size);
		
	rc5_build_items(item, data);

	int item_num = RC5_DATA_ITEM_NUM * RMT_TX_DATA_NUM;
	// Serial.println(item_num);
	// for(int j=0; j<item_num;j++)
	// {
		// Serial.println(item[j].val, HEX);
	// }
	rmt_write_items(RMT_TX_CHANNEL, item, item_num, true);
	rmt_wait_tx_done(RMT_TX_CHANNEL);
	free(item);
}
#endif
