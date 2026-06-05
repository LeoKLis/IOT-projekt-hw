#pragma once

#include "esp_err.h"

typedef struct _actuator_t_ actuator_t;

esp_err_t init_actuators(void);
esp_err_t get_actuator(char* tag, size_t tag_len, actuator_t** act);
esp_err_t call_act_handler(struct _actuator_t_* act, void* param);