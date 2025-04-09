#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "webguiapp.h"
#include "AppConfiguration.h"
#include "driver/gpio.h"

void UserMQTTEventHndlr(int idx, void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void SaveUserConf();





void vTaskLed18(void *pvParameters){
    while (1){
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(18,1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(18,0);
    }
}

void vTaskLed17(void *pvParameters){
    while (1){
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(17,0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(17,1);
    }
}



void app_main(void)
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

    //init
    gpio_set_direction(18, GPIO_MODE_DEF_OUTPUT);
    gpio_set_direction(17, GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(18, 0);
    gpio_set_level(17, 0);


// tasks
    xTaskCreate(
        vTaskLed18,
        "VIU18",
        2048,
        NULL,
        25,
        NULL
    );
    xTaskCreate(
        vTaskLed17,
        "VIU17",
        2048,
        NULL,
        25,
        NULL
    );


    while (true)
    {


/*
        if(gpio_get_level(GPIO_NUM_41) == 0 && !BtnPressed)
        {
            PublicTestMQTT(MQTT1);
            PublicTestMQTT(MQTT2);
            BtnPressed = true;
        }
        if(gpio_get_level(GPIO_NUM_41) == 1 && BtnPressed)
            BtnPressed = false;
*/
    }
}
