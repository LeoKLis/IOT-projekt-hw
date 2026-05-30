#pragma once

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
