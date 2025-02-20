#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// void fr_httpd_init(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb, QueueHandle_t http_req);
void fr_httpd_init(const QueueHandle_t frame_i, QueueHandle_t http_req);
void ostk_fr_httpd(void* arg);