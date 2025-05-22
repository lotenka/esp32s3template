#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>      // Добавлено для vTaskDelay
#include "webguiapp.h"
#include "AppConfiguration.h"



void UserMQTTEventHndlr(int idx, void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void SaveUserConf();


void vTaskBlink(void *pvParameters){
    //vTaskDelay(pdMS_TO_TICKS(5000));
    ledc_channel_t *channel = pvParameters;
    while (1){
        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, *channel, 254, 10000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, *channel, LEDC_FADE_NO_WAIT);

        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, *channel, 1, 10000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, *channel, LEDC_FADE_NO_WAIT);
    }
}







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
    LEDC_config_init();
    
/*
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
        .duty = 2,
        .hpoint = 255, // максимальное разрешение duty(не точно)

    };
    ledc_channel_config_t my_channel_config_17 = {
        .gpio_num = 17,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_FADE_END,
        .timer_sel = LEDC_TIMER_0,
        .duty = 2,
        .hpoint = 255,

    };


    
    ledc_timer_config(&my_timer_config);
    ledc_channel_config(&my_channel_config_18);
    ledc_channel_config(&my_channel_config_17);

    //ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 254);


    ledc_fade_func_install(0);

    ledc_channel_t channels[] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1};
    xTaskCreate(
        vTaskBlink,
        "LEDC_blink_18",
        1024,
        &channels[0],
        25,
        NULL
    );


    xTaskCreate(
        vTaskBlink,
        "LEDC_blink_17",
        1024,
        &channels[1],
        25,
        NULL
    );
*/
    while (1) {

    }
}