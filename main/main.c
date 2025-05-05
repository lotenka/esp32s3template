#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <driver/gpio.h>
#include <driver/gptimer.h>
#include <driver/ledc.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>      // Добавлено для vTaskDelay
#include "webguiapp.h"
#include "AppConfiguration.h"

#define LED_GPIO    18  // GPIO для светодиода (GPIO21 по умолчанию на ESP32-S3)
#define TIMER_GROUP 0   // Номер группы таймера (0 или 1)
#define TIMER_IDX   0   // Номер таймера в группе (0 или 1)


void UserMQTTEventHndlr(int idx, void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void SaveUserConf();




void app_main()
{
    regUserEventHandler(&UserMQTTEventHndlr, NULL);
    regCustomSaveConf(&SaveUserConf);
    regCustomPayloadTypeHandler(NULL);
    RegAppVariables();
    RegObjects();

    WebGuiAppInit();
    if (GetUserAppNeedReset())
    {
        SetUserAppNeedReset(false);
        ESP_ERROR_CHECK(InitAppConfig());
    }
    ESP_ERROR_CHECK(InitAppConfig());

    LEDStripInit(CONFIG_LED_STRIP_GPIO);

    //Указал структуру конфига таймера для LEDC
    ledc_timer_config_t my_timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ledc_channel_config_t my_channel_config_18 = {
        .gpio_num = 18,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_FADE_END,
        .timer_sel = LEDC_TIMER_0,
        .duty = 16,
        .hpoint = 255, // максимальное разрешение duty(не точно)

    };
    ledc_channel_config_t my_channel_config_17 = {
        .gpio_num = 17,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_FADE_END,
        .timer_sel = LEDC_TIMER_0,
        .duty = 16,
        .hpoint = 255,

    };


    
    ledc_timer_config(&my_timer_config);
    ledc_channel_config(&my_channel_config_18);
    ledc_channel_config(&my_channel_config_17);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 254);


    ledc_fade_func_install(0);

    while (1) {
        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 254, 3000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 254, 3000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);

        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 3000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0, 3000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
    }
}