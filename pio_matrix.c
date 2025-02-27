#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/pwm.h"

// Definições de portas e endereços I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Inclusão do arquivo .pio para controle da matriz LED
#include "pio_matrix.pio.h"

#define NUM_PIXELS 25  // Número de LEDs na matriz

// Definição dos pinos utilizados
#define OUT_PIN 7   // Pino de saída para a matriz de LEDs 5x5
#define SENSOR_1 5  // Pino do Sensor 1
#define SENSOR_2 6  // Pino do Sensor 2
#define SENSOR_3 22 // Pino do Sensor 3
#define BUZZER 21   // Pino do Buzzer

// Variáveis globais
static volatile uint32_t last_time = 0;  // Variável para controle de debounce do botão
volatile bool SENSOR_1_state = false;  // Estado atual do botão A
volatile bool SENSOR_2_state = false;  // Estado atual do botão B
volatile bool SENSOR_3_state = false;  // Estado atual do botão joystick
PIO pio = pio0;  // Instância da PIO utilizada
uint volatile sm_global = 0;  // Variável global para armazenar o state machine
bool volatile green_on = false;  // Estado do LED verde
bool volatile blue_on = false;   // Estado do LED azul
static volatile bool buzzer_active = false;  // Estado do buzzer
static volatile uint64_t buzzer_end_time = 0;  // Tempo de término do buzzer

ssd1306_t ssd;  // Estrutura para manipulação do display OLED

// Funções externas para controle da matriz LED
extern void numeros(PIO pio, uint sm, uint cont);
extern void desligar_matriz(PIO pio, uint sm);
extern void tanque_cheio(PIO pio, uint sm);
extern void tanque_vazio(PIO pio, uint sm);
extern void tanque_estavel(PIO pio, uint sm);
extern void tanque_nivel_baixo(PIO pio, uint sm);

// Protótipo da função de interrupção dos botões
static void gpio_irq_handler(uint gpio, uint32_t events);

/**
 * Função para definir a intensidade das cores RGB do LED.
 * 
 * @param b Intensidade do azul (0 a 1)
 * @param r Intensidade do vermelho (0 a 1)
 * @param g Intensidade do verde (0 a 1)
 * @return Valor RGB formatado para o LED
 */
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

/**
 * Função para emitir um som no buzzer por um determinado tempo.
 * 
 * @param buzzer_pin Pino do buzzer.
 * @param frequency Frequência do som em Hz.
 * @param duration_ms Duração do som em milissegundos.
 */
void buzzer_sound(uint buzzer_pin, uint frequency, uint duration_ms) {
    // Configura o pino do buzzer como saída PWM
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM);

    // Obtém o slice e o canal do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(buzzer_pin);
    uint channel = pwm_gpio_to_channel(buzzer_pin);

    // Configura a frequência do PWM
    pwm_set_clkdiv(slice_num, 1.0f);  // Divisor de clock = 1 (sem divisão)
    pwm_set_wrap(slice_num, 125000000 / frequency);  // Define o valor de wrap para a frequência desejada
    pwm_set_chan_level(slice_num, channel, 125000000 / (2 * frequency));  // Define o ciclo de trabalho como 50%

    // Habilita o PWM
    pwm_set_enabled(slice_num, true);

    // Ativa o buzzer e define o tempo de término
    buzzer_active = true;
    buzzer_end_time = to_ms_since_boot(get_absolute_time()) + duration_ms;
}

/**
 * Função para atualizar o estado do buzzer.
 * Deve ser chamada no loop principal.
 */
void buzzer_update(uint buzzer_pin) {
    if (buzzer_active) {
        // Verifica se o tempo de duração do buzzer expirou
        if (to_ms_since_boot(get_absolute_time()) >= buzzer_end_time) {
            // Desativa o buzzer
            uint slice_num = pwm_gpio_to_slice_num(buzzer_pin);
            pwm_set_enabled(slice_num, false);  // Desabilita o PWM
            buzzer_active = false;
        }
    }
}

/**
 * Função para alertar que o tanque está vazio.
 * 
 * @param sm State machine da PIO
 */
void alerta_tanque_vazio(uint sm){
    tanque_vazio(pio, sm);
    ssd1306_fill(&ssd, false);  // Limpa o display
    ssd1306_send_data(&ssd);
    ssd1306_draw_string(&ssd, "  TANQUE  ", 20, 0);
    ssd1306_draw_string(&ssd, "  VAZIO  ", 20, 9);
    ssd1306_draw_empty_tank(&ssd, 39, 20);
    ssd1306_send_data(&ssd);
    buzzer_sound(BUZZER, 440, 1000);
}

/**
 * Função para alertar que o tanque está cheio.
 * 
 * @param sm State machine da PIO
 */
void alerta_tanque_cheio(uint sm){
    tanque_cheio(pio, sm);
    ssd1306_fill(&ssd, false);  // Limpa o display
    ssd1306_draw_string(&ssd, "  TANQUE  ", 20, 0);
    ssd1306_draw_string(&ssd, "  CHEIO  ", 20, 9);
    ssd1306_draw_full_tank(&ssd, 39, 20);
    ssd1306_send_data(&ssd);
    buzzer_sound(BUZZER, 440, 1000);
}

/**
 * Função para alertar que o tanque está estável.
 * 
 * @param sm State machine da PIO
 */
void alerta_tanque_estavel(uint sm){
    tanque_estavel(pio, sm);
    ssd1306_fill(&ssd, false);  // Limpa o display
    ssd1306_draw_string(&ssd, "  TANQUE  ", 20, 0);
    ssd1306_draw_string(&ssd, "  ESTAVEL  ", 20, 9);
    ssd1306_draw_stable_tank(&ssd, 39, 20);
    ssd1306_send_data(&ssd);
    buzzer_sound(BUZZER, 440, 1000);
}

/**
 * Função para alertar que o tanque está com nível baixo.
 * 
 * @param sm State machine da PIO
 */
void alerta_tanque_baixo(uint sm){
    tanque_nivel_baixo(pio, sm);
    ssd1306_fill(&ssd, false);  // Limpa o display
    ssd1306_draw_string(&ssd, "  TANQUE  ", 20, 0);
    ssd1306_draw_string(&ssd, "NIVEL BAIXO", 20, 9);
    ssd1306_draw_low_tank(&ssd, 39, 20);
    ssd1306_send_data(&ssd);
    buzzer_sound(BUZZER, 440, 1000);
}

/**
 * Função de interrupção para lidar com o pressionamento dos botões.
 * Liga ou desliga os LEDs e atualiza o display OLED com o estado dos LEDs.
 * 
 * @param gpio Pino do botão pressionado
 * @param events Evento de interrupção
 */
void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time());  // Obtém o tempo atual em microssegundos

    // Verifica se o botão A foi pressionado e aplica um debounce de 2000ms
    if (gpio == SENSOR_1 && (current_time - last_time > 200000)) {
        last_time = current_time;
        
        // Verifica o estado atual do botão
        bool new_state = !gpio_get(SENSOR_1);  // Inverte a leitura porque o botão está em pull-up
      
        // Verifica se o estado mudou
        if (new_state != SENSOR_1_state) {
            SENSOR_1_state = new_state;

            if (SENSOR_1_state) {
                printf("Sensor 1 ativado\n");
                alerta_tanque_cheio(sm_global);
            } else {
                printf("Sensor 1 desativado\n");
                alerta_tanque_estavel(sm_global);
            }
        }
    }
    
    // Verifica se o botão B foi pressionado e aplica um debounce de 200ms
    else if (gpio == SENSOR_2 && (current_time - last_time > 200000)) {
        // Verifica o estado atual do botão
        bool new_state = !gpio_get(SENSOR_2);  // Inverte a leitura porque o botão está em pull-up
      
        // Verifica se o estado mudou
        if (new_state != SENSOR_2_state) {
            SENSOR_2_state = new_state;
            if(!SENSOR_1_state){
                if (SENSOR_2_state) {
                    printf("Sensor 2 ativado\n");
                    alerta_tanque_estavel(sm_global);
                } else {
                    printf("Sensor 2 desativado\n");
                    alerta_tanque_baixo(sm_global);
                }
            }
        } 
    }

    else if (gpio == SENSOR_3 && (current_time - last_time > 200000)) {
        // Verifica o estado atual do botão
        bool new_state = !gpio_get(SENSOR_3);  // Inverte a leitura porque o botão está em pull-up
      
        // Verifica se o estado mudou
        if (new_state != SENSOR_3_state) {
            SENSOR_3_state = new_state;

            if(!SENSOR_1_state && !SENSOR_2_state){
                if (SENSOR_3_state) {
                    printf("Sensor 3 ativado\n");
                    alerta_tanque_baixo(sm_global);
                } else {
                    printf("Sensor 3 desativado\n");
                    alerta_tanque_vazio(sm_global);
                }
            }
        } 
    }
}

/**
 * Função de inicialização dos pinos e periféricos.
 */
void init(){
    gpio_init(SENSOR_1);
    gpio_set_dir(SENSOR_1, GPIO_IN);
    gpio_pull_up(SENSOR_1);

    gpio_init(SENSOR_2);
    gpio_set_dir(SENSOR_2, GPIO_IN);
    gpio_pull_up(SENSOR_2);

    gpio_init(SENSOR_3);
    gpio_set_dir(SENSOR_3, GPIO_IN);
    gpio_pull_up(SENSOR_3);

    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);

    // Inicializa a comunicação I2C para o display OLED
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Limpa o display OLED
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    stdio_init_all();  // Inicializa o console USB
}

/**
 * Função principal do programa.
 * Inicializa os pinos, configura os periféricos e entra em loop aguardando comandos do usuário.
 */
int main()
{
    // Inicialização dos pinos
    init();

    // Configuração da PIO para a matriz de LEDs
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    sm_global = sm;
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    // Configuração das interrupções dos botões
    gpio_set_irq_enabled_with_callback(SENSOR_1, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(SENSOR_2, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(SENSOR_3, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_irq_handler);

    desligar_matriz(pio, sm);

    while (true)
    {
        // Atualiza o estado do buzzer
        buzzer_update(BUZZER);
        sleep_ms(100);  // Pequeno atraso para evitar sobrecarga do loop
    }
}