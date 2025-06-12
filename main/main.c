#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>      // Добавлено для vTaskDelay
#include "webguiapp.h"
#include "AppConfiguration.h"



void UserMQTTEventHndlr(int idx, void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void SaveUserConf();


void vTaskLedFadeSet(void *pvParameters)
{
    int result;
    int raw_adc_data;
    
    while(1){
        vTaskDelay(pdMS_TO_TICKS(50));
        adc_oneshot_unit_handle_t *conf = pvParameters;
        adc_oneshot_read(*conf, ADC_CHANNEL_5, &raw_adc_data);
        result = raw_adc_data * 995 / 4095 * 11;
        int fade_level = result / 37.6;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, fade_level);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);  
    }
}


void vTaskAdcInputShow(void *pvParameters){
    int raw_adc_data;
    int result;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        adc_oneshot_unit_handle_t *conf = pvParameters;
        adc_oneshot_read(*conf, ADC_CHANNEL_5, &raw_adc_data);
        result = raw_adc_data * 995 / 4095 * 11;
        printf("adc_result = %d mV\n", result);
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

    LEDC_config_init();
    //ledc_fade_func_install(0);

    adc_oneshot_unit_init_cfg_t my_adc_unit_config =
    {
        .unit_id = ADC_UNIT_1,
        //.ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_new_unit(&my_adc_unit_config, &adc1_handle);
    
    adc_oneshot_chan_cfg_t my_config =
    {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_0,
    };

    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_5, &my_config);     //почему каналов только 10?

    /*
    gpio_set_direction(17, GPIO_MODE_DEF_OUTPUT);
    gpio_set_direction(9, GPIO_MODE_INPUT);
    gpio_set_level(9, 1);
    gpio_set_level(17, 1);
    */
    
//    ledc_channel_t channels[] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1};

    xTaskCreate(
        vTaskAdcInputShow,
        "ADC_input",
        4096,
        &adc1_handle,
        25,
        NULL
    );

    xTaskCreate(
        vTaskLedFadeSet,
        "Ledc_fade",
        4096,
        &adc1_handle,
        25,
        NULL
    );
    
    int raw_adc_data;
    float result;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}