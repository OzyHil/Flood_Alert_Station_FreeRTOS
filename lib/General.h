#ifndef GENERAL_H
#define GENERAL_H

// Inclusão das bibliotecas padrão e específicas do hardware
#include <stdio.h> // Biblioteca padrão para entrada/saída
#include <stdlib.h> // Biblioteca padrão para alocação de memória e conversões
#include <stdint.h> // Biblioteca padrão para tipos inteiros

#include "hardware/gpio.h" // Controle de GPIO (General Purpose Input/Output)
#include "hardware/pwm.h"    // Controle de PWM (Pulse Width Modulation)
#include "hardware/pio.h"    // Programação de E/S PIO (Programmable I/O)
#include "hardware/adc.h"
#include "hardware/i2c.h"    // Comunicação I2C
#include "hardware/clocks.h" // Controle de clocks

#include "pico/stdlib.h"     // Biblioteca principal para o Raspberry Pi Pico
#include "pio_matrix.pio.h"  // Programa específico para controle da matriz de LEDs
#include "pico/bootrom.h"    // Trecho para modo BOOTSEL com botão B

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"          // Biblioteca para tarefas do FreeRTOS
#include "queue.h"

// Função para inicializar a configuração do sistema (clocks, I/O, etc.)
void init_system_config();

// Função para inicializar o PWM em um pino específico com um valor de wrap
void init_pwm(uint gpio, uint wrap);

#endif