#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "lvgl.h"
#include "HQCoreS3.h"
#include "fr-main.hpp"
#include "fr-wifi.h"
#include "fr-mdns.h"
#include "fr-httpd.hpp"
#include "fr-user.h"

static const char *TAG = "APP_MAIN";

extern "C" void app_main() 
{
	//! Flash
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
			ESP_ERROR_CHECK(nvs_flash_erase());
			ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret); 

	//! Driver
	// bsp_led_init();
	// bsp_btn_init();
	bsp_i2c_init();
	const camera_config_t canCfg = FR_CAM_DEFAULT_CONFIG();
	bsp_cam_init(&canCfg);
	bsp_lvgl_init();
	bsp_sdcard_init();
	bsp_mpu6050_init();
	
	//! APP
	// 创建队列
	QueueHandle_t frameRecogOut = xQueueCreate(2, sizeof(camera_fb_t*));
	QueueHandle_t httpRequest = xQueueCreate(1, sizeof(fr_user_t));

	// 初始化FR各模块
  // fr_user_fmt();
  fr_user_init();
  fr_user_load();
  fr_user_print();
	fr_wifi_init(); // Stack:(2304 + (512)) Core: PRO_CPU_NUM
	fr_mdns_init();
	fr_main_init(frameRecogOut, httpRequest);
	// fr_httpd_init(frameRecogOut, NULL, true, httpRequest); // Stack:4096 Core:PRO_CPU_NUM Prority:5
	fr_httpd_init(frameRecogOut, httpRequest);

	// 创建任务
	// xTaskCreatePinnedToCore(ostk_led_rinbow, 	"LED-RINBOW",       4096, NULL, 5, NULL, APP_CPU_NUM);
	xTaskCreatePinnedToCore(ostk_bsp_lvgl,  				"BSP-LVGL",         4096, NULL, 5, NULL, APP_CPU_NUM);
	xTaskCreatePinnedToCore(ostk_fr_main,           "FR-MAIN",          4096, NULL, 5, NULL, APP_CPU_NUM);
	xTaskCreatePinnedToCore(ostk_fr_httpd,					"FR-HTTPD",					4096, NULL, 6, NULL, APP_CPU_NUM);	
}
