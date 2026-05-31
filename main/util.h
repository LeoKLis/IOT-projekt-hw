#pragma once

// #define DEBUG

#ifndef DEBUG

// TODO: Zamijeniti template sa pravim vrijednostima
#define WIFI_SSID      "TEMPLATE"
#define WIFI_PASS      "TEMPLATE"
#define MQTT_BROKER    "TEMPLATE" // nešto u stilu mqtt://192.168.x.x"
#define MQTT_TOPIC_T   "TEMPLATE" // temperatura
#define MQTT_TOPIC_H   "TEMPLATE" // vlaga
#define MQTT_TOPIC_C   "TEMPLATE" // CO2
#define MQTT_PORT      1883

#else

#define WIFI_SSID      "rokomob"
#define WIFI_PASS      "mqtt_test1234"
#define MQTT_BROKER    "mqtt://10.243.211.121"
#define MQTT_TOPIC_T   "dht11/temp"
#define MQTT_TOPIC_H   "dht11/humidity"
#define MQTT_TOPIC_C   "ndir/co2"
#define MQTT_PORT      1883

#endif

#define ERR_CHECK(err_flag, pot_err_src)                                      \
    do {                                                                      \
        auto err_loc_flg = (err_flag);                                        \
        if(err_loc_flg != ESP_OK)                                             \
        {                                                                     \
            ESP_LOGE("EXCEPTION",                                             \
                     #pot_err_src " failed with esp_err_t = %s, loc = %s:%d", \
                     esp_err_to_name(err_loc_flg), __FILE__, __LINE__         \
            );                                                                \
            return err_loc_flg;                                               \
        }                                                                     \
    } while(0)
