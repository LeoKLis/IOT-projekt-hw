#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "led_strip.h"

#include <string.h>

#include "util.h"

#include "actuators.h"

volatile uint8_t red = 0, green = 0, blue = 0;

struct _actuator_t_ {
    char* tag;
    size_t tag_len;
    esp_err_t (*handler)(void*);
};

led_strip_handle_t led_strip;

esp_err_t air_conditioning(void* param)
{
    uint32_t* val = (uint32_t*)param;
    ESP_LOGI("foo_ac", "Val: %u", *val);
    green = (*val) ? 255 : 0;
    led_strip_set_pixel(led_strip, 0, red, green, blue);
    led_strip_refresh(led_strip);

    return ESP_OK;
}

esp_err_t air_ventilation(void* param)
{
    uint32_t* val = (uint32_t*)param;
    red = (*val) ? 255 : 0;
    led_strip_set_pixel(led_strip, 0, red, green, blue);
    led_strip_refresh(led_strip);

    return ESP_OK;
}

esp_err_t dehumidification(void* param)
{
    uint32_t* val = (uint32_t*)param;
    blue = (*val) ? 255 : 0;
    led_strip_set_pixel(led_strip, 0, red, green, blue);
    led_strip_refresh(led_strip);

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
    ERR_CHECK(gpio_set_direction(ACTUATOR_ALERT_PIN, GPIO_MODE_OUTPUT), init_actuators);
    ERR_CHECK(gpio_set_level(ACTUATOR_ALERT_PIN, 0), init_actuators);

    led_strip_config_t strip_config = {
        .strip_gpio_num = 8,
        .max_leds = 1,
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10000000,
    };
    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);

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