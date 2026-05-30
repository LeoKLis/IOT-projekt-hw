#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <esp_log.h>

#include "mqtt_wifi_bridge.h"

extern float humidity;
extern float temperature;
extern uint16_t concentration;

static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false;

static void mqtt_event_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data) 
{
    esp_mqtt_event_handle_t event = event_data;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            mqtt_connected = true;
            ESP_LOGI("mqtt", "Connected to broker");
            break;
        case MQTT_EVENT_DISCONNECTED:
            mqtt_connected = false;
            ESP_LOGW("mqtt", "Disconnected from broker");
            break;
        default:
            break;
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data) 
{
    if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        esp_wifi_connect();
    else if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        ESP_LOGW("wifi", "Disconnected, retrying...");
        esp_wifi_connect();
    } 
    else if (base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        ESP_LOGI("wifi", "Got IP");
}

void wifi_mqtt_init(void) 
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,    wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT,   IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid     = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    esp_wifi_start();

    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_ip_info_t ip_info;
    while (esp_netif_get_ip_info(netif, &ip_info) != ESP_OK || ip_info.ip.addr == 0) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri  = MQTT_BROKER,
        .broker.address.port = MQTT_PORT,
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID,
                                   mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void mqtt_publish_sensors(void) 
{
    if (!mqtt_connected || mqtt_client == NULL) return;

    char buf[16];

    snprintf(buf, sizeof(buf), "%.1f", temperature);
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_T, buf, 0, 1, 0);

    snprintf(buf, sizeof(buf), "%.1f", humidity);
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_H, buf, 0, 1, 0);

    snprintf(buf, sizeof(buf), "%d", concentration);
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_C, buf, 0, 1, 0);

    ESP_LOGI("MQTT", "PUB data:\n\ttemp=%.1f hum=%.1f co2=%d", temperature, humidity, concentration);
}