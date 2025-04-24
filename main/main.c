#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <driver/gpio.h>
#include <driver/gptimer.h>
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


static const char *TAG = "GPTimer Example";


gpio_num_t led_gpio = LED_GPIO;
bool led_state = false;

// Обработчик alarm-события (вызывается в контексте прерывания)
static void IRAM_ATTR alarm_callback(gptimer_handle_t timer, const void *ctx)
{
    led_state = !led_state;
    gpio_set_level(led_gpio, led_state ? 1 : 0);
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

    // 1. Настройка GPIO
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << led_gpio),
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // 2. Создание GPTimer
    gptimer_handle_t gptimer;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT, // APB_CLK (80MHz)
        .direction = GPTIMER_COUNT_UP,      // Счетчик растёт
        .resolution_hz = 1 * 1000 * 1000,  // 1 МГц (1 тик = 1 мкс)
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    // 3. Настройка alarm
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000000, // 1 секунда (1e6 тиков)
        .reload_count = 0,
        .flags = {
            .auto_reload_on_alarm = true // Авто-перезагрузка
        }
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    // 4. Регистрация обработчика событий
    gptimer_event_callbacks_t cbs = {
        .on_alarm = alarm_callback // Обработчик alarm-события
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    // 5. Включение таймера (это активирует прерывания)
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    // 6. Запуск таймера
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    // 7. Бесконечный цикл
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}