/*
 *  Por: Wilton Lacerda Silva
 *  Data: 10/05/2025
 *
 *  Exemplo do uso de Filas queue no FreeRTOS com Raspberry Pi Pico
 *
 *  Descrição: Leitura do valor do joystick e exibição no display OLED SSD1306
 *  com comunicação I2C. O valor do joystick é lido a cada 100ms e enviado para a fila.
 *  A task de exibição recebe os dados da fila e atualiza o display a cada 100ms.
 *  Os leds são controlados por PWM, com brilho proporcional ao desvio do joystick.
 *  O led verde controla o eixo X e o led azul controla o eixo Y.
 */

#include "General.h"    // Inclusão da biblioteca geral com definições e funções comuns
#include "Led_Matrix.h" // Inclusão da biblioteca para controlar a matriz de LEDs>
#include "Buzzer.h"     // Inclusão da biblioteca para controlar o buzzer
#include "Display.h"    // Inclusão da biblioteca para controlar o display OLED
#include "Led.h"        // Inclusão da biblioteca para controlar os LEDs

#define ADC_JOYSTICK_X 26
#define ADC_JOYSTICK_Y 27

typedef struct
{
    uint8_t water_level_percent;
    uint8_t rain_level_percent;
} level_data_t;

QueueHandle_t xQueueLevelData;

uint8_t transform_to_level(uint16_t adc_value);

void vJoystickTask(void *params)
{
    adc_gpio_init(ADC_JOYSTICK_Y);
    adc_gpio_init(ADC_JOYSTICK_X);
    adc_init();

    uint16_t pos_y = 0;
    uint16_t pos_x = 0;

    level_data_t level_data;

    while (true)
    {
        adc_select_input(0); // GPIO 26 = ADC0
        pos_y = adc_read();

        adc_select_input(1); // GPIO 27 = ADC1
        pos_x = adc_read();

        level_data.rain_level_percent = transform_to_level(pos_y);
        level_data.water_level_percent = transform_to_level(pos_x);

        xQueueSend(xQueueLevelData, &level_data, 0);
        vTaskDelay(pdMS_TO_TICKS(100)); // 10 Hz de leitura
    }
}

void vDisplayTask()
{
    level_data_t level_data;

    char *status;
    bool emphasis = false;

    while (true)
    {
        if (xQueueReceive(xQueueLevelData, &level_data, portMAX_DELAY) == pdTRUE)
        {
            if (level_data.rain_level_percent > 70 || level_data.water_level_percent > 80)
            {
                status = "ALERTA";
                emphasis = true; // Ativa o destaque
            }

            else if (level_data.water_level_percent >= 50 || level_data.rain_level_percent >= 60)
            {
                status = "ATENCAO";
                emphasis = false; // Desativa o destaque
            }

            else
            {
                status = "NORMAL";
                emphasis = false; // Desativa o destaque
            }
        }

        draw_info(level_data.water_level_percent, level_data.rain_level_percent, status, emphasis); // Exibe informações no display

        vTaskDelay(pdMS_TO_TICKS(100)); // Atualiza a cada 100ms
    }
}

void vLedRGBTask(void *params)
{
    level_data_t level_data;

    while (true)
    {
        if (xQueueReceive(xQueueLevelData, &level_data, portMAX_DELAY) == pdTRUE)
        {
            if (level_data.water_level_percent >= 70 || level_data.rain_level_percent >= 80)
                set_led_color(RED);

            else if (level_data.water_level_percent >= 50 || level_data.rain_level_percent >= 60)
                set_led_color(YELLOW);

            else
                set_led_color(GREEN);
        }
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // Atualiza a cada 50ms
}

void vLedMatrixTask(void *params)
{
    level_data_t level_data;

    while (true)
    {
        if (xQueueReceive(xQueueLevelData, &level_data, portMAX_DELAY) == pdTRUE)
        {
            if (level_data.water_level_percent >= 70 || level_data.rain_level_percent >= 80)
                draw_matrix(RED);
            else
                draw_matrix(DARK);
        }
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // Atualiza a cada 50ms
}

void vBuzzerTask(void *params)
{
    level_data_t level_data;

    while (true)
    {
        if (xQueueReceive(xQueueLevelData, &level_data, portMAX_DELAY) == pdTRUE)
        {
            if (level_data.water_level_percent >= 70 || level_data.rain_level_percent >= 80)
                buzzer_mode_alert(); // Ativa o modo de alerta do buzzer
        }
        else
            set_buzzer_level(BUZZER_A, 0); // Desliga o buzzer
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // Atualiza a cada 50ms
}

int main()
{
    init_system_config(); // Inicializa a configuração do sistema

    configure_buzzer(BUZZER_A); // Configura o buzzer
    configure_leds();           // Configura os LEDs RGB
    configure_leds_matrix();    // Configura a matriz de LEDs
    configure_i2c_display();    // Configura o display I2C

    // Cria a fila para compartilhamento de valor do joystick
    xQueueLevelData = xQueueCreate(5, sizeof(level_data_t));

    // Criação das tasks
    xTaskCreate(vJoystickTask, "Joystick Task", 256, NULL, 1, NULL);
    xTaskCreate(vDisplayTask, "Display Task", 512, NULL, 1, NULL);
    xTaskCreate(vLedRGBTask, "LED RGB Task", 256, NULL, 1, NULL);
    xTaskCreate(vLedMatrixTask, "LED Matriz Task", 256, NULL, 1, NULL);
    xTaskCreate(vBuzzerTask, "Buzzer Task", 256, NULL, 1, NULL);

    // Inicia o agendador
    vTaskStartScheduler();
    panic_unsupported();
}

uint8_t transform_to_level(uint16_t adc_value)
{
    return (adc_value * 100) / 4095;
}
