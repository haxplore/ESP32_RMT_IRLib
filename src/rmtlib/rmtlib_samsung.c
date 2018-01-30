/*
Like NEC but a wee bit different header timings
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


#define SAMSUNG_BITS              32
#define SAMSUNG_HEADER_HIGH_US    5000
#define SAMSUNG_HEADER_LOW_US     5000
#define SAMSUNG_BIT_ONE_HIGH_US    560
#define SAMSUNG_BIT_ONE_LOW_US    1690
#define SAMSUNG_BIT_ZERO_HIGH_US   560
#define SAMSUNG_BIT_ZERO_LOW_US    560
#define SAMSUNG_BIT_END            560
//
#define SAMSUNG_DATA_ITEM_NUM   	34  /*!< code item number: header + 32bit data + end */

const char* SAMSUNG_TAG = "SAMSUNG";

#if SEND_SAMSUNG
/*
 * @brief Generate header value: active 5ms + negative 5ms
 */
void samsung_fill_item_header(rmt_item32_t* item)
{
    rmt_fill_item_level(item, SAMSUNG_HEADER_HIGH_US, SAMSUNG_HEADER_LOW_US);
}

/*
 * @brief Generate data bit 1: positive 0.56ms + negative 1.69ms
 */
void samsung_fill_item_bit_one(rmt_item32_t* item)
{
    rmt_fill_item_level(item, SAMSUNG_BIT_ONE_HIGH_US, SAMSUNG_BIT_ONE_LOW_US);
}

/*
 * @brief Generate data bit 0: positive 0.56ms + negative 0.56ms
 */
void samsung_fill_item_bit_zero(rmt_item32_t* item)
{
    rmt_fill_item_level(item, SAMSUNG_BIT_ZERO_HIGH_US, SAMSUNG_BIT_ZERO_LOW_US);
}

/*
 * @brief Generate end signal: positive 0.56ms
 */
void samsung_fill_item_end(rmt_item32_t* item)
{
    rmt_fill_item_level(item, SAMSUNG_BIT_END, 0x7fff);
}

/*
  @brief Build 32bit waveform.
*/
void samsung_build_items(rmt_item32_t* item, uint32_t cmd_data)
{
  samsung_fill_item_header(item++);
  
  uint32_t mask = 0x80000000;
  for (int j = 0; j < 32; j++) {
    if (cmd_data & mask) {
      samsung_fill_item_bit_one(item);
    } else {
      samsung_fill_item_bit_zero(item);
    }
    item++;
    mask >>= 1;
  }

  samsung_fill_item_end(item);
}


void samsung_tx_init(gpio_num_t gpio_num)
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

void rmtlib_samsung_send(unsigned long data)
{
	vTaskDelay(10);
	samsung_tx_init(RMT_TX_GPIO_NUM);
	
	esp_log_level_set(SAMSUNG_TAG, ESP_LOG_INFO);
	ESP_LOGI(SAMSUNG_TAG, "RMT TX DATA");
	
	size_t size = (sizeof(rmt_item32_t) * SAMSUNG_DATA_ITEM_NUM * RMT_TX_DATA_NUM);
	rmt_item32_t* item = (rmt_item32_t*) malloc(size);
	memset((void*) item, 0, size);
	
	samsung_build_items(item, data);

	int item_num = SAMSUNG_DATA_ITEM_NUM * RMT_TX_DATA_NUM;
	rmt_write_items(RMT_TX_CHANNEL, item, item_num, true);
	rmt_wait_tx_done(RMT_TX_CHANNEL);
	free(item);
}

#endif
