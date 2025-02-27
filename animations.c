#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"


// arquivo .pio
#include "pio_matrix.pio.h"

#define NUM_PIXELS 25

// pino do led 5x5
#define OUT_PIN 7

// pino do buzzer
#define BUZZER_PIN 28

extern uint32_t matrix_rgb(double b, double r, double g);

void desligar_matriz(PIO pio, uint sm){
    double frame[1][25] = {
        {0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,
         0, 0,  0,  0, 0,
         0, 0,  0,  0, 0,
         0, 0, 0, 0, 0}
    };

    uint32_t valor_led;
    
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        valor_led = matrix_rgb(0.0, frame[0][24 - i], 0.0); // LEDs vermelhos para o número 0
        pio_sm_put_blocking(pio, sm, valor_led);
    }

}

void tanque_estavel(PIO pio, uint sm){
    double frame[1][25] = {
        {0, 0, 0,   0, 0,
         0.2, 0.2,  0.2,  0.2, 0.2,
         0.2, 0.2,  0.2,  0.2, 0.2,
         0.2, 0.2,  0.2,  0.2, 0.2,
         0.2, 0.2, 0.2, 0.2, 0.2}
    };

    uint32_t valor_led;
    
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        valor_led = matrix_rgb(frame[0][24 - i], 0, 0); // LEDs azuis para indicar que o tanque está estavel
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void tanque_nivel_baixo(PIO pio, uint sm){
    double frame[1][25] = {
        {0, 0, 0,   0, 0,
         0, 0,  0,  0, 0,
         0, 0,  0,  0, 0,
         0.2, 0.2,  0.2,  0.2, 0.2,
         0.2, 0.2, 0.2, 0.2, 0.2}
    };

    uint32_t valor_led;
    
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        valor_led = matrix_rgb(frame[0][24 - i], 0, 0); // LEDs azuis para indicar que o tanque está com nivel baixo
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void tanque_vazio(PIO pio, uint sm){
    double frame[1][25] = {
        {0.2, 0,  0,   0, 0.2,
         0, 0.2,  0,  0.2, 0,
         0, 0,  0.2,0, 0,
         0, 0.2,  0,  0.2, 0,
         0.2, 0,  0, 0, 0.2}
    };

    uint32_t valor_led;
    
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        valor_led = matrix_rgb(0, frame[0][24 - i], 0); // LEDs vermelhos para o alerta de tanque vazui
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void tanque_cheio(PIO pio, uint sm){

    double frame[1][25] = {
        {0, 0.2,  0.2,   0.2, 0,
         0.2, 0,  0,    0, 0.2,
         0.2, 0,  0,    0, 0.2,
         0.2, 0,  0,    0, 0.2,
         0, 0.2,  0.2, 0.2, 0}
    };

    uint32_t valor_led;
    
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        valor_led = matrix_rgb(0, 0, frame[0][24 - i]); // LEDs verdes para o alerta de tanque cheio
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}