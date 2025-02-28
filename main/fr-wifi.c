#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/ip4_addr.h"

#include "fr-wifi.h"

#define FR_WIFI_MAX_IMUM_RETRY  5
#define FR_WIFI_MAX_STA_CONNECT 4

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char* TAG = "FR-WIFI";

static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group = NULL;

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) 
{
  switch (event_id) 
  {
    case WIFI_EVENT_STA_START:
      ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
      esp_wifi_connect();
      break;
    case WIFI_EVENT_STA_CONNECTED:
      ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
      break;
    case WIFI_EVENT_STA_DISCONNECTED:
      ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
      if(s_retry_num < FR_WIFI_MAX_IMUM_RETRY)  {
        esp_wifi_connect();
        s_retry_num++;
        ESP_LOGI(TAG, "retry to connect to the AP");
      } else {
        ESP_LOGI(TAG, "connect to the AP fail");
      }
      break;
    case WIFI_EVENT_AP_STACONNECTED:
      wifi_event_ap_staconnected_t *event = event_data;
      ESP_LOGI(TAG, "station:" MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
      break;
    case WIFI_EVENT_AP_STADISCONNECTED:
      wifi_event_ap_stadisconnected_t *ds_event = event_data;
      ESP_LOGI(TAG, "station:" MACSTR " leave, AID=%d", MAC2STR(ds_event->mac), ds_event->aid);
      break;
    default:
      break;
  }
}

void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  ip_event_got_ip_t *event;

  switch(event_id) 
  {
    case IP_EVENT_STA_GOT_IP:
      event = (ip_event_got_ip_t*)event_data;
      ESP_LOGI(TAG, "got ip:" IPSTR "\n", IP2STR(&event->ip_info.ip));
      s_retry_num = 0;
      xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
      break;
    default:
      break;
  }
}

void wifi_init_softap(esp_netif_t * netif)
{
  //设置网关并开启DHCP服务器
  if(strcmp(FR_SERVER_IP,"192.167.4.1")) {
    int a,b,c,d;
    sscanf(FR_SERVER_IP, "%d.%d.%d.%d", &a, &b, &c, &d);
    esp_netif_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, a, b, c, d);
    IP4_ADDR(&ip_info.gw, a, b, c, d);
    IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif));
    ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));
    ESP_ERROR_CHECK(esp_netif_dhcps_start(netif));
  }
  //配置wifi参数
  wifi_config_t wifi_config = {
    .ap.ssid = FR_WIFI_AP_SSID,
    .ap.password = FR_WIFI_AP_PSWD,
    .ap.ssid_len = strlen((char *)wifi_config.ap.ssid),
    .ap.channel = 1,
    .ap.authmode = WIFI_AUTH_WPA2_PSK,
    .ap.max_connection = FR_WIFI_MAX_STA_CONNECT,
    .ap.beacon_interval = 100,
  };

  if (strlen(FR_WIFI_AP_PSWD) == 0) {
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  }

  if (strlen(FR_WIFI_AP_CHANNEL)) {
    int channel;
    sscanf(FR_WIFI_AP_CHANNEL, "%d", &channel);
    wifi_config.ap.channel = channel;
  }

  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
    FR_WIFI_AP_SSID, FR_WIFI_AP_PSWD);
}

void wifi_init_sta()
{
  wifi_config_t wifi_config = {
    .sta = {
      .ssid = FR_WIFI_STA_SSID,
      .password = FR_WIFI_STA_PSWD,
    }
  };
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_LOGI(TAG, "wifi_init_sta finished.");
  ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
    FR_WIFI_STA_SSID, FR_WIFI_STA_PSWD);  
}

void fr_wifi_init()
{
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

  wifi_mode_t mode = WIFI_MODE_NULL;

  if(strlen(FR_WIFI_AP_SSID) && strlen(FR_WIFI_STA_SSID)) {
    mode = WIFI_MODE_APSTA;
  } else if(strlen(FR_WIFI_AP_SSID)) {
    mode = WIFI_MODE_AP;
  } else if(strlen(FR_WIFI_STA_SSID)) {
    mode = WIFI_MODE_STA;
  }

  if(mode == WIFI_MODE_NULL) {
    ESP_LOGW(TAG, "Neither AP or STA have been configured. WiFi will be off.");
    return;
  }

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());//创建默认事件循环

  s_wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(mode));  

  if (mode & WIFI_MODE_AP) {
    esp_netif_t * ap_netif = esp_netif_create_default_wifi_ap();
    wifi_init_softap(ap_netif);
  }
  if (mode & WIFI_MODE_STA) {
    esp_netif_create_default_wifi_sta();
    wifi_init_sta();
  }

  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));//ps=power save   wifi节能模式
  ESP_LOGI(TAG, "wifi init finished.");

  if (mode & WIFI_MODE_STA) {
    xEventGroupWaitBits(s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY);
  }
  vEventGroupDelete(s_wifi_event_group);
  s_wifi_event_group = NULL;
}