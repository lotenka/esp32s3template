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

extern ledc_channel_t ledc_channels[];
extern int length;
extern int result;

static void funct_time(char *argres, int rw)
{
    time_t now;
    time(&now);
    snprintf(argres, VAR_MAX_VALUE_LENGTH, "%d", (int) now);
}

#define GAMMA_R  (1.70)
#define GAMMA_G  (1.70)
#define GAMMA_B  (1.70)

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
    for (int i = 0; i < length; i++)
    {
        ledc_set_fade_with_step(LEDC_LOW_SPEED_MODE, ledc_channels[i], fade_level, 5, 50);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, ledc_channels[i], LEDC_FADE_NO_WAIT);
        /*    
        ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channels[i], fade_level);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_channels[i]);  
        */
    }

    }

const rest_var_t ApplicationVariables[] =
        {
                /*FUNCTIONS*/

                { 0, "mytime", &funct_time, VAR_FUNCT, R, 0, 0 },
                { 0, "myvar", &AppConfig.test, VAR_INT, RW, 0, 1000000 },
                { 0, "color", &funct_color, VAR_FUNCT, R, 0, 0 },
                { 0, "fade", &funct_fade, VAR_FUNCT, RW, 0, 255 }

        };

void RegAppVariables(void)
{
    SetAppVars((rest_var_t*) ApplicationVariables, sizeof(ApplicationVariables) / sizeof(rest_var_t));
}
