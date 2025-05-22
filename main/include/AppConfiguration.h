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
 *  	 \file AppConfiguration.h
 *    \version 1.0
 * 		 \date 2023-03-08
 *     \author Bogdan Pilyugin
 * 	    \brief    
 *    \details 
 *	\copyright Apache License, Version 2.0
 */

#ifndef MAIN_INCLUDE_APPCONFIGURATION_H_
#define MAIN_INCLUDE_APPCONFIGURATION_H_

#include <stdint.h>
#include <stdbool.h>
#include <driver/ledc.h>
#include "CronTimers.h"
#include "esp_netif.h"
#include "led_strip.h"

/// Application-dependent structure used to contain address information

/**
 * @struct  APP_CONFIG
 * @brief The main configuration structure
 * @details This structure saving to EEPROM and loading from EEPROM\n
 * on device boot. On load the checksumm is calculate and compare to \n
 * saved one. If find difference (due to eeprom memory distortions),\n
 * the default values will be loaded.
 */
typedef struct appconf
{
int test;

} APP_CONFIG;

void UserInitIO(void);
void UserAppInit(void);
void InitSPI(void);
void InitI2C(void);

APP_CONFIG* GetAppConf(void);
void ResetAppConfig(APP_CONFIG *Conf);
esp_err_t WriteNVSAppConfig(APP_CONFIG *AppConf);
esp_err_t ReadNVSAppConfig(APP_CONFIG *AppConf);
esp_err_t InitAppConfig(void);
esp_err_t ResetInitAppConfig(void);

void LoadDefaultReset(void);
void DelayedRestart(void);
void RegAppVariables(void);
void RegObjects(void);

void LEDStripInit(int gpio);
led_strip_handle_t * LEDStripGetHandle();

void LEDC_config_init(void);


#endif /* MAIN_INCLUDE_APPCONFIGURATION_H_ */
