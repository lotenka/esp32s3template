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

/*
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
*/


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
    ledc_fade_func_install(0);
    /*
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
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}