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

int result;
int raw_adc_data;
adc_channel_t adc_channels[4] = {ADC_CHANNEL_8, ADC_CHANNEL_7, ADC_CHANNEL_6, ADC_CHANNEL_5};
gpio_num_t control_channels[6] = {GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_35, GPIO_NUM_34, GPIO_NUM_33};
int adc_data_matrix[4][8];


void vTaskLedFadeSet(void *pvParameters)
{    
    while(1){
        vTaskDelay(pdMS_TO_TICKS(50));
        adc_oneshot_unit_handle_t *conf = pvParameters;
        for (int i = 0; i < 4; i++)
        {
            adc_channel_t channel = adc_channels[i];
            if (i > 1){
                for (int j = 0; j < 8; j++)
                {
                    gpio_set_level(control_channels[0], j & 1);
                    gpio_set_level(control_channels[1], j & 10);
                    gpio_set_level(control_channels[2], j & 100);
                    adc_oneshot_read(*conf, adc_channels[i], &raw_adc_data);
                    result = raw_adc_data * 995 / 4095 * 11;
                    adc_data_matrix[i][j] = result;

                }
            }

            //Case for ADC which are controlled by bus S3 S4 S5
            else
            {
                for (int j = 0; j < 8; j++)
                {
                    gpio_set_level(control_channels[3], j & 1);
                    gpio_set_level(control_channels[4], j & 10);
                    gpio_set_level(control_channels[5], j & 100);
                    adc_oneshot_read(*conf, adc_channels[i], &raw_adc_data);
                    result = raw_adc_data * 995 / 4095 * 11;
                    adc_data_matrix[i][j] = result;
                }
            }
        }    
    }
}


void vTaskAdcInputShow(void *pvParameters){
    while(1){
        vTaskDelay(pdMS_TO_TICKS(5000));
        for (int i = 0; i < 4; i++)
        {
            printf("\nADC_CHANNEL_%d\n", 8 - i);
            for (int j = 0; j < 8; j++)
            {
                printf("Y%d: %dmV\t\t", j, adc_data_matrix[i][j]);
            }
        }
        printf("\n\n\t\t32In info");
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

    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_5, &my_config);
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &my_config);
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_7, &my_config);
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_8, &my_config);
//    ledc_channel_t channels[] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1};
    gpio_set_direction(11, GPIO_MODE_OUTPUT);
    gpio_set_direction(12, GPIO_MODE_OUTPUT);
    gpio_set_direction(13, GPIO_MODE_OUTPUT);
    
    gpio_set_direction(33, GPIO_MODE_OUTPUT);
    gpio_set_direction(34, GPIO_MODE_OUTPUT);
    gpio_set_direction(35, GPIO_MODE_OUTPUT);
    


    xTaskCreate(
        vTaskAdcInputShow,
        "ADC_input",
        2048,
        0,
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