#include "webguiapp.h"
#include "AppConfiguration.h"

ledc_channel_t ledc_channels[] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1};
int length = sizeof(ledc_channels) / sizeof(ledc_channels[0]);

void LEDC_config_init()
{
    //if (!initialized){
    if(1){
        ledc_timer_config_t my_timer_config = 
        {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = LEDC_TIMER_8_BIT,
            .timer_num = LEDC_TIMER_0,
            .freq_hz = 1000,
            .clk_cfg = LEDC_AUTO_CLK,
        };
        ledc_channel_config_t my_channel_config_18 = 
        {
            .gpio_num = 18,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = LEDC_CHANNEL_0,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0, // максимальное разрешение duty(не точно)
        };
        ledc_channel_config_t my_channel_config_17 = 
        {
            .gpio_num = 17,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = LEDC_CHANNEL_1,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0,
        };
        ledc_timer_config(&my_timer_config);
        ledc_channel_config(&my_channel_config_18);
        ledc_channel_config(&my_channel_config_17);
    }
}
