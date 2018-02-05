#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp32_rmt_remotes.h"

void rmt_example_nec_tx_task(void *pvParameter)
{
	unsigned long int ircode = 0x5743C03F; //NEC
	//rmtlib_samsung_send()
	for(;;) {
		//rmtlib_nec_send(0x5743C03);
		rmtlib_nec_send(ircode);
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}

}

void rmt_example_nec_rx_task(void *pvParameter)
{
	for(;;) {
		//rmtlib_rc5_receive();
		rmtlib_samsung_receive();
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
		ESP_LOGI("Task", "________________");
	}
}

void app_main(void)
{
	esp_log_level_set("MAIN", ESP_LOG_INFO);
    //xTaskCreate(rmt_example_nec_tx_task, "rmt_nec_tx_task", 2048, NULL, 10, NULL);
    xTaskCreate(rmt_example_nec_rx_task, "rmt_nec_rx_task", 2048, NULL, 10, NULL);
}
