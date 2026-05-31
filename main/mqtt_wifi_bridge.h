#pragma once

#include "esp_err.h"

esp_err_t wifi_mqtt_init(void);
void mqtt_publish_sensors(void);