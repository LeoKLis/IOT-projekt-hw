#pragma once

#define DEBUG

#ifndef ROOMID
#define ROOMID "room1"
#endif

#ifndef DEBUG

// TODO: Zamijeniti template sa pravim vrijednostima
#define WIFI_SSID      "TEMPLATE"
#define WIFI_PASS      "TEMPLATE"
#define MQTT_BROKER    "TEMPLATE" // nešto u stilu mqtt://192.168.x.x"

#else

#define WIFI_SSID      "Burbon"
#define WIFI_PASS      "daj2eura"
#define MQTT_BROKER    "mqtt://192.168.1.113"

#endif

#define MQTT_PORT      1883

// Topics ESP->HA
#define MQTT_TOPIC_ROOM   "roomsense/" ROOMID "/telemetry"

// Topics HA->ESP
#define MQTT_TOPIC_AIRCOND  ROOMID "/aircond" // klimatizacija
#define MQTT_TOPIC_AIRFLOW  ROOMID "/airflow" // vantilacija
#define MQTT_TOPIC_DEHUMID  ROOMID "/dehumid" // odvlaživanje

// Actuator pins
#define ACTUATOR_ALERT_PIN GPIO_NUM_11

// Constants
#define BLINK_TIME_US 500000LL

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
