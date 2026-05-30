#pragma once

#include "esp_err.h"

// TODO: Zamijeniti template sa pravim vrijednostima

#define WIFI_SSID      "TEMPLATE"
#define WIFI_PASS      "TEMPLATE"
#define MQTT_BROKER    "TEMPLATE" // nešto u stilu mqtt://192.168.x.x"
#define MQTT_TOPIC_T   "TEMPLATE" // temperatura
#define MQTT_TOPIC_H   "TEMPLATE" // vlaga
#define MQTT_TOPIC_C   "TEMPLATE" // CO2
#define MQTT_PORT      1883

esp_err_t wifi_mqtt_init(void);
void mqtt_publish_sensors(void);