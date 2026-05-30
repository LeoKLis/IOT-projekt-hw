idf_component_register(SRCS         "main.c" "mqtt_wifi_bridge.c"
                       REQUIRES     esp_wifi nvs_flash mqtt esp_netif esp_driver_i2c
                       INCLUDE_DIRS "."
                    )
