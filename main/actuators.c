#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include <string.h>

#include "util.h"

#include "actuators.h"

struct _actuator_t_ {
    char* tag;
    size_t tag_len;
    esp_err_t (*handler)(void*);
};

esp_err_t air_conditioning(void* param)
{
    uint32_t val = (uint32_t)param;
    if(!val)
        ERR_CHECK(gpio_set_level(ACTUATOR_AIRCOND_PIN, 0), air_conditioning);
    else
        ERR_CHECK(gpio_set_level(ACTUATOR_AIRCOND_PIN, 1), air_conditioning);
    
    return ESP_OK;
}

esp_err_t air_ventilation(void* param)
{
    uint32_t val = (uint32_t)param;
    if(!val)
        ERR_CHECK(gpio_set_level(ACTUATOR_AIRFLOW_PIN, 0), air_ventilation);
    else
        ERR_CHECK(gpio_set_level(ACTUATOR_AIRFLOW_PIN, 1), air_ventilation);
    
    return ESP_OK;
}

esp_err_t dehumidification(void* param)
{
    uint32_t val = (uint32_t)param;
    if(!val)
        ERR_CHECK(gpio_set_level(ACTUATOR_DEHUMID_PIN, 0), dehumidification);
    else
        ERR_CHECK(gpio_set_level(ACTUATOR_DEHUMID_PIN, 1), dehumidification);
    
    return ESP_OK;
}

static struct _actuator_t_ actuators[] = {
    {MQTT_TOPIC_AIRCOND, sizeof(MQTT_TOPIC_AIRCOND) - 1, air_conditioning},
    {MQTT_TOPIC_AIRFLOW, sizeof(MQTT_TOPIC_AIRFLOW) - 1, air_ventilation},
    {MQTT_TOPIC_DEHUMID, sizeof(MQTT_TOPIC_DEHUMID) - 1, dehumidification},
    {"", -1, NULL}
};

esp_err_t init_actuators(void)
{
    ERR_CHECK(gpio_set_direction(ACTUATOR_AIRCOND_PIN, GPIO_MODE_OUTPUT), init_actuators);
    ERR_CHECK(gpio_set_direction(ACTUATOR_AIRFLOW_PIN, GPIO_MODE_OUTPUT), init_actuators);
    ERR_CHECK(gpio_set_direction(ACTUATOR_DEHUMID_PIN, GPIO_MODE_OUTPUT), init_actuators);

    ERR_CHECK(gpio_set_level(ACTUATOR_AIRCOND_PIN, 0), init_actuators);
    ERR_CHECK(gpio_set_level(ACTUATOR_AIRFLOW_PIN, 0), init_actuators);
    ERR_CHECK(gpio_set_level(ACTUATOR_DEHUMID_PIN, 0), init_actuators);

    return ESP_OK;
}

esp_err_t get_actuator(char* tag, size_t tag_len, struct _actuator_t_** act)
{
    struct _actuator_t_* iter = &actuators[0];
    while(iter->handler != NULL)
    {
        if(iter->tag_len == tag_len && !strncmp(tag, iter->tag, iter->tag_len))
        {
            *act = iter;
            return ESP_OK;
        }

        iter++;
    }

    return ESP_ERR_INVALID_ARG;
}

esp_err_t call_act_handler(struct _actuator_t_* act, void* param)
{
    return act->handler(param);
}