#include "string.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "fr-user.h"
#include "esp_spiffs.h"

static const char* TAG = "FR-USER";

fr_user_t user_list[FR_USER_MAX];

void fr_user_init()
{
  nvs_handle_t nvs_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if (err == ESP_OK) {
    size_t required_size = sizeof(user_list);
    err = nvs_get_blob(nvs_handle, "user_list", user_list, &required_size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
      // NVS 中没有用户数据，存储一些默认数据进去
      for (int i = 0; i < FR_USER_MAX; i++) {
        snprintf(user_list[i].name, sizeof(user_list[i].name), "User%d", i + 1);
        user_list[i].face_id = 0;
      }
      err = nvs_set_blob(nvs_handle, "user_list", user_list, sizeof(user_list));
      if (err == ESP_OK) {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
          ESP_LOGE(TAG, "提交默认用户数据到 NVS 失败");
        }
      } else {
        ESP_LOGE(TAG, "保存默认用户数据到 NVS 失败");
      }
    } else if (err != ESP_OK) {
      ESP_LOGE(TAG, "从 NVS 加载用户数据失败");
    }
    nvs_close(nvs_handle);
  } else {
    ESP_LOGE(TAG, "打开 NVS 句柄失败");
  }
}

void fr_user_fmt()
{
  nvs_handle_t nvs_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if (err == ESP_OK) {
    memset(user_list, 0, sizeof(user_list));
    err = nvs_set_blob(nvs_handle, "user_list", user_list, sizeof(user_list));
    if (err == ESP_OK) {
      err = nvs_commit(nvs_handle);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "提交格式化用户数据到 NVS 失败");
      }
    } else {
      ESP_LOGE(TAG, "保存格式化用户数据到 NVS 失败");
    }
    nvs_close(nvs_handle);
  } else {
    ESP_LOGE(TAG, "打开 NVS 句柄失败");
  }
}

void fr_user_load()
{
  nvs_handle_t nvs_handle;
  esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
  if (err == ESP_OK) {
    size_t required_size = sizeof(user_list);
    err = nvs_get_blob(nvs_handle, "user_list", user_list, &required_size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "从 NVS 加载用户数据失败");
    }
    nvs_close(nvs_handle);
  } else {
    ESP_LOGE(TAG, "打开 NVS 句柄失败");
  }
}

void fr_user_save()
{
  nvs_handle_t nvs_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if (err == ESP_OK) {
    err = nvs_set_blob(nvs_handle, "user_list", user_list, sizeof(user_list));
    if (err == ESP_OK) {
      err = nvs_commit(nvs_handle);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "提交用户数据到 NVS 失败");
      }
    } else {
      ESP_LOGE(TAG, "保存用户数据到 NVS 失败");
    }
    nvs_close(nvs_handle);
  } else {
    ESP_LOGE(TAG, "打开 NVS 句柄失败");
  }
}

void fr_user_print()
{
  printf("用户数据-------------------------------------------\n");
  for(int i = 0; i < FR_USER_MAX; i++) {
    printf("Index:%d\tName:%s\tGender:%s\tAge:%d\tEmployee ID:%s\tPosition:%s\tFace ID:%d\n", 
      i, user_list[i].name, user_list[i].gender, user_list[i].age, 
      user_list[i].employee_id, user_list[i].position, user_list[i].face_id);
  }
}

void fr_user_set(int index, const fr_user_t* data)
{
  if (index >= 0 && index < FR_USER_MAX) {
    snprintf(user_list[index].name, sizeof(user_list[index].name), "%s", data->name);
    snprintf(user_list[index].gender, sizeof(user_list[index].gender), "%s", data->gender);
    user_list[index].age = data->age;
    snprintf(user_list[index].employee_id, sizeof(user_list[index].employee_id), "%s", data->employee_id);
    snprintf(user_list[index].position, sizeof(user_list[index].position), "%s", data->position);
    user_list[index].face_id = data->face_id;
  } else {
    ESP_LOGE(TAG, "索引超出范围");
  }
}

int fr_user_register(const fr_user_t* data)
{
  for (int index = 0; index < FR_USER_MAX; index++) {
    if (user_list[index].face_id == 0) {
      snprintf(user_list[index].name, sizeof(user_list[index].name), "%s", data->name);
      snprintf(user_list[index].gender, sizeof(user_list[index].gender), "%s", data->gender);
      user_list[index].age = data->age;
      snprintf(user_list[index].employee_id, sizeof(user_list[index].employee_id), "%s", data->employee_id);
      snprintf(user_list[index].position, sizeof(user_list[index].position), "%s", data->position);
      user_list[index].face_id = data->face_id;
      return index;
    }
  }
  ESP_LOGE(TAG, "没有可用的位置");
  return -1;
}

void fr_user_modify(int index, const fr_user_t* data)
{
  if (index >= 0 && index < FR_USER_MAX) {
    snprintf(user_list[index].name, sizeof(user_list[index].name), "%s", data->name);
    snprintf(user_list[index].gender, sizeof(user_list[index].gender), "%s", data->gender);
    user_list[index].age = data->age;
    snprintf(user_list[index].employee_id, sizeof(user_list[index].employee_id), "%s", data->employee_id);
    snprintf(user_list[index].position, sizeof(user_list[index].position), "%s", data->position);
    // 不修改 user_list[index].face_id
  } else {
    ESP_LOGE(TAG, "索引超出范围");
  }
}

void fr_user_delete(int index)
{
  if (index >= 0 && index < FR_USER_MAX) {
    memset(&user_list[index], 0, sizeof(fr_user_t));
  } else {
    ESP_LOGE(TAG, "索引超出范围");
  }
}

int fr_user_get_cnt()
{
  int cnt = 0;
  for (int i = 0; i < FR_USER_MAX; i++) {
    if (user_list[i].face_id != 0) {
      cnt++;
    }
  }
  return cnt;
}

const char* fr_user_get_name(int index)
{
  if (index >= 0 && index < FR_USER_MAX) {
    return user_list[index].name;
  } else {
    ESP_LOGE(TAG, "索引超出范围");
    return NULL;
  }
}

const char* fr_user_get_gender(int index)
{
  if (index >= 0 && index < FR_USER_MAX) {
    return user_list[index].gender;
  } else {
    ESP_LOGE(TAG, "索引超出范围");
    return NULL;
  }
}

int fr_user_get_age(int index)
{
  if (index >= 0 && index < FR_USER_MAX) {
    return user_list[index].age;
  } else {
    ESP_LOGE(TAG, "索引超出范围");
    return -1;
  }
}

const char* fr_user_get_employee_id(int index)
{
  if (index >= 0 && index < FR_USER_MAX) {
    return user_list[index].employee_id;
  } else {
    ESP_LOGE(TAG, "索引超出范围");
    return NULL;
  }
}

const char* fr_user_get_position(int index)
{
  if (index >= 0 && index < FR_USER_MAX) {
    return user_list[index].position;
  } else {
    ESP_LOGE(TAG, "索引超出范围");
    return NULL;
  }
}

int fr_user_get_face_id(int index)
{
  if (index >= 0 && index < FR_USER_MAX) {
    return user_list[index].face_id;
  } else {
    ESP_LOGE(TAG, "索引超出范围");
    return -1;
  }
}

fr_user_t* fr_user_get_ptr(int index)
{
  if (index >= 0 && index < FR_USER_MAX) {
    return &user_list[index];
  } else {
    ESP_LOGE(TAG, "索引超出范围");
    return NULL;
  }
}