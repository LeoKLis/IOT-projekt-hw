#include "driver/i2c_types.h"
#include "freertos/idf_additions.h"
#include "hal/gpio_types.h"
#include "hal/i2c_types.h"
#include "portmacro.h"
#include "soc/gpio_num.h"
#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/i2c_master.h>

#include "mqtt_wifi_bridge.h"

#include <dht.h>
#include <stdint.h>

float humidity = 0.0;
float temperature = 0.0;
uint16_t concentration = 0;

static i2c_master_dev_handle_t dev_handle;
static const gpio_num_t BUSY_PIN = GPIO_NUM_10;

void dhtTask(void* pvParams) {
    esp_err_t err = 0;

    while(1) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        err = dht_read_float_data(DHT_TYPE_DHT11, GPIO_NUM_4, &humidity, &temperature);
        vTaskDelay(100 / portTICK_PERIOD_MS);

        if (err != ESP_OK) {
            ESP_LOGE("main", "Error reading DHT %d", err);
        } else {
            ESP_LOGI("main", "Temp: %f Hum: %f", temperature, humidity);
            mqtt_publish_sensors();
        }
    }
}

void init_i2c(i2c_master_dev_handle_t* dev_handle) {
    i2c_master_bus_handle_t master_bus_handle;
    i2c_master_bus_config_t master_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 0,
        .sda_io_num = GPIO_NUM_8,
        .scl_io_num = GPIO_NUM_9,
        .flags.enable_internal_pullup = 1
    };
    i2c_new_master_bus(&master_config, &master_bus_handle);

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0b1101000,
        .scl_speed_hz = 100000
    };
    i2c_master_bus_add_device(master_bus_handle, &dev_config, dev_handle);
}

void cdmTask(void* pvParams) {
    uint8_t conc_addr = 0b11;
    uint8_t conc_data[2];

    while(1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        while (gpio_get_level(BUSY_PIN) == 1);

        i2c_master_transmit_receive(dev_handle, &conc_addr, 1, conc_data, sizeof(conc_data), -1);
        concentration = conc_data[1] << 8 | conc_data[0];

        vTaskDelay(100 / portTICK_PERIOD_MS);
        ESP_LOGI("main", "CO2 concentration: %d", concentration);
        mqtt_publish_sensors();
    }
}

void app_main(void) {
    gpio_set_direction(BUSY_PIN, GPIO_MODE_INPUT);
    init_i2c(&dev_handle);

    wifi_mqtt_init();

    xTaskCreate(dhtTask, "DHT Task", 2048, NULL, 1, NULL);
    xTaskCreate(cdmTask, "CDM Task", 2048, NULL, 1, NULL);

    while(1) { vTaskDelay(1000 / portTICK_PERIOD_MS); }
}