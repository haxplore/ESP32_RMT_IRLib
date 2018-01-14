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

#include "esp32-hal-rmt.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************/

/*
* Configure SEND
*/
void rmt_tx_init(rmt_config_t* rmt_config_tx)
{
    rmt_config(rmt_config_tx);
    rmt_driver_install(rmt_config_tx->channel, 0, 0);	
}

/*
* Configure RECEIVE
*/
void rmt_rx_init()
{
    rmt_config_t rmt_rx;
    rmt_rx.channel = RMT_RX_CHANNEL;
    rmt_rx.gpio_num = RMT_RX_GPIO_NUM;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = rmt_item32_TIMEOUT_US / 10 * (RMT_TICK_10_US);
    rmt_config(&rmt_rx);
    rmt_driver_install(rmt_rx.channel, 1000, 0);
}

/*
 * @brief Set TX pin and channel
 */
void rmt_set_tx_pin(gpio_num_t gpio_num)
{
	//rmt_set_pin(rmt_channel_t channel, rmt_mode_t mode, gpio_num_t gpio_num)
	rmt_set_pin(RMT_TX_CHANNEL, RMT_MODE_TX, gpio_num);
}

/*
 * @brief Build register value of waveform for PDM one data bit
 */
inline void rmt_fill_item_level(rmt_item32_t* item, int high_us, int low_us)
{
    item->level0 = 1;
    item->duration0 = (high_us) / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = (low_us) / 10 * RMT_TICK_10_US;
}

/*
 * @brief Build register value for end item
 */
inline void rmt_fill_end_item(rmt_item32_t* item)
{
    item->level0 = 0;
    item->duration0 = 0;
    item->level1 = 0;
    item->duration1 = 0;
}

/*
 * @brief Build register value of waveform for Manchester Encoding (bi-phase modulation) one data bit
 */
inline void rmt_fill_item_level_me(rmt_item32_t* item, int length_us, bool start_idle)
{
	if (start_idle)
	{
		item->level0 = 0;
		item->level1 = 1;
	}
	else
	{
		item->level0 = 1;
		item->level1 = 0;
	}
	item->duration0 = (length_us) / 10 * RMT_TICK_10_US;    
    item->duration1 = (length_us) / 10 * RMT_TICK_10_US;
}

#ifdef __cplusplus
}
#endif
