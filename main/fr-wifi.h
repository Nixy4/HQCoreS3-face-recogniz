#pragma once

#define FR_WIFI_AP_SSID         "ESP32-FR-WIFI"
#define FR_WIFI_AP_PSWD         "88888888"
#define FR_WIFI_AP_CHANNEL      ""
#define FR_WIFI_STA_SSID        ""
#define FR_WIFI_STA_PSWD        ""
#define FR_SERVER_IP            "192.168.4.1"

#ifdef __cplusplus
extern "C" {
#endif

void fr_wifi_init();

#ifdef __cplusplus
}
#endif