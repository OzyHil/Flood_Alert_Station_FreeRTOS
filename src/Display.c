#include "Display.h" // Inclusão do cabeçalho com definições relacionadas ao display OLED

ssd1306_t ssd;   // Estrutura que representa o display OLED SSD1306
bool cor = true; // Variável para armazenar a cor do display (true ou false)

// Função para configurar a comunicação I2C e inicializar o display OLED
void configure_i2c_display()
{
    // Inicializa o barramento I2C na porta e com frequência de 400 kHz
    i2c_init(I2C_PORT, 400 * 1000);

    // Define as funções dos pinos SDA e SCL como I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // Ativa resistores de pull-up nos pinos SDA e SCL
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display com resolução 128x64, sem rotação, endereço I2C e porta definida
    ssd1306_init(&ssd, 128, 64, false, DISPLAY_ADDRESS, I2C_PORT);

    // Configura o display com parâmetros padrão
    ssd1306_config(&ssd);

    // Envia os dados iniciais para o display
    ssd1306_send_data(&ssd);
}

// Função para limpar o display OLED
void clear_i2c_display()
{
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void draw_info(uint8_t water_level_percent, uint8_t rain_level_percent, char *status, bool emphasis)   
{
    char buffer_w[16];
    char buffer_r[16];
    char buffer_s[20];

    sprintf(buffer_w, "AGUA  %d%%", water_level_percent);
    sprintf(buffer_r, "CHUVA  %d%%", rain_level_percent);
    sprintf(buffer_s, "ESTADO %s", status);

    
    // Desenha informações no display OLED
    ssd1306_fill(&ssd, false); // Limpa o display antes de desenhar o pedestre
    
    ssd1306_draw_string(&ssd, buffer_w, 8, 15);
    ssd1306_draw_string(&ssd, buffer_r, 8, 28);
    ssd1306_draw_string(&ssd, buffer_s , 8, 50);
    
    if(emphasis)
        ssd1306_rect(&ssd, 45, 60, 55, 18, cor, !cor); // Desenha um retângulo de destaque

    // Atualiza o display
    ssd1306_send_data(&ssd);
}
