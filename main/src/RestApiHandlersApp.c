/*! Copyright 2023 Bogdan Pilyugin
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  	 \file RestApiHandlersApp.c
 *    \version 1.0
 * 		 \date 2023-08-17
 *     \author Bogdan Pilyugin
 * 	    \brief    
 *    \details 
 *	\copyright Apache License, Version 2.0
 */

#include <driver/ledc.h>
#include "webguiapp.h"
#include "AppConfiguration.h"
#include "led_strip.h"
#include "math.h"


extern APP_CONFIG AppConfig;

static void funct_time(char *argres, int rw)
{
    time_t now;
    time(&now);
    snprintf(argres, VAR_MAX_VALUE_LENGTH, "%d", (int) now);
}

#define GAMMA_R  (1.70)
#define GAMMA_G  (1.70)
#define GAMMA_B  (1.70)

static void LEDC_config_init()
{
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
        .intr_type = LEDC_INTR_FADE_END,
        .timer_sel = LEDC_TIMER_0,
        .duty = 2,
        .hpoint = 255, // максимальное разрешение duty(не точно)
    };
    ledc_channel_config_t my_channel_config_17 = 
    {
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
}


static int GammaCorrection(int input, float gamma)
{
    return (int) ((float) 255 * pow(((float) input / 255), gamma));
}

extern led_strip_handle_t led_strip;

static void funct_color(char *argres, int rw)
{
    struct jReadElement result;
    jRead(argres, "", &result);
    if (result.dataType == JREAD_OBJECT)
    {
        int R = jRead_int(argres, "{'r'", 0);
        int G = jRead_int(argres, "{'g'", 0);
        int B = jRead_int(argres, "{'b'", 0);

        struct jWriteControl jwc;
        jwOpen(&jwc, argres, VAR_MAX_VALUE_LENGTH, JW_OBJECT, JW_COMPACT);
        jwObj_int(&jwc, "r", R);
        jwObj_int(&jwc, "g", G);
        jwObj_int(&jwc, "b", B);
        jwEnd(&jwc);
        jwClose(&jwc);

        R = GammaCorrection(R, GAMMA_R);
        G = GammaCorrection(G, GAMMA_G);
        B = GammaCorrection(B, GAMMA_B);

        for (int i = 0; i < CONFIG_LED_STRIP_NUMBER; i++)
        {
            led_strip_set_pixel(led_strip, i, R, G, B);
        }
        led_strip_refresh(led_strip);
    }
}

static void funct_fade(char *argres, int rw)
{
    int fade_level = jRead_int(argres, "{'fade'", 0);
    
    LEDC_config_init();
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, fade_level);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, fade_level);
}

const rest_var_t ApplicationVariables[] =
        {
                /*FUNCTIONS*/

                { 0, "mytime", &funct_time, VAR_FUNCT, R, 0, 0 },
                { 0, "myvar", &AppConfig.test, VAR_INT, R, 0, 0 },
                { 0, "color", &funct_color, VAR_FUNCT, R, 0, 0 },
                { 0, "fade", &funct_fade, VAR_FUNCT, RW, 0, 300 }

        };

void RegAppVariables(void)
{
    SetAppVars((rest_var_t*) ApplicationVariables, sizeof(ApplicationVariables) / sizeof(rest_var_t));
}
