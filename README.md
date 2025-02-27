# 📖 Controle de Matriz LED e Display OLED com Pico

## 📌 Introdução
Este projeto utiliza a Raspberry Pi Pico (RP2040) para controlar uma matriz de LEDs 5x5, LEDs RGB e um display OLED SSD1306. O programa permite controlar os LEDs e exibir informações no display OLED por meio de botões e entrada via console USB.

O sistema é estruturado em diferentes arquivos auxiliares, responsáveis por animações, manipulação de fontes e controle do display OLED.

## 🎯 Objetivos

• Aplicar os conhecimentos adquiridos sobre UART e I2C na prática.
• Manipular e controlar LEDs comuns e LEDs endereçáveis WS2812.
• Fixar o estudo do uso botões de acionamento, interrupções e Debounce.
• Desenvolver um projeto funcional que combine hardware e software

## 📹 video do Projeto
- **[Ver Video](https://drive.google.com/file/d/1-UUp2QFG4MBCVXd2P7ZYwXoJd2ywZ1m8/view?usp=sharing)**
  
## 📂 Estrutura dos Arquivos

- `pio_matrix.c` → Código principal que gerencia a exibição na matriz de LED.
- `animations.c` → Implementa animações para os caracteres exibidos na matriz.
- `font.h` → Define fontes e caracteres utilizados na exibição.
- `ssd1306.h` → Declara funções para controle do display OLED SSD1306.
- `ssd1306.c` → Implementa as funções de comunicação com o display SSD1306.

## Componentes

- **Raspberry Pi Pico (RP2040)**
- **LEDs RGB** (Controle de dois LEDs)
- **Matriz de LEDs 5x5** (Controlada via PIO)
- **Display OLED SSD1306** (Conexão I2C)
- **Botões Físicos** (Dois botões para controlar LEDs)
- **Console USB** (Entrada de números para exibição na matriz de LEDs e no display)

## 🔧 Funcionamento do Código

### 🔹 `pio_matrix.c` (Código Principal)
Este arquivo contém a lógica principal para o controle da matriz de LED. Ele faz a configuração do PIO para manipular a exibição e utiliza os arquivos auxiliares para gerenciar animações e exibição de caracteres.

O sistema permite a interação com o usuário através de dois botões físicos e um console USB. O controle de LEDs e a exibição de números na matriz de LEDs 5x5 e no display OLED são feitos conforme o pressionamento de botões ou a entrada de números via console.

### 1. **Botões Físicos e LEDs RGB**

- **Botão A (GPIO 5)**: Controla o **LED verde** (GPIO 11). Cada pressionamento alterna entre o estado **ligado** e **desligado**.
- **Botão B (GPIO 6)**: Controla o **LED azul** (GPIO 12). Cada pressionamento alterna entre o estado **ligado** e **desligado**.

Quando um botão é pressionado, o estado do LED correspondente é alterado e o display OLED é atualizado para refletir a mudança de estado.

### 2. **Display OLED SSD1306**

O display OLED é utilizado para exibir informações ao usuário:

- **Exibe o estado dos LEDs**: O display mostra se o LED verde ou azul foi **ligado** ou **desligado**.
- **Exibe números de 0 a 9**: Quando um número é enviado via console USB, ele é exibido no display OLED e na matriz de LEDs 5x5.
- **Exibe letras minúsculas e maiúsculas**: Quando uma letra é enviada via console USB, ele é exibido no display OLED e na matriz de LEDs 5x5.

### 3. **Matriz de LEDs 5x5**

A matriz de LEDs 5x5 é controlada por meio de um programa **PIO** (Programmable Input/Output), que permite exibir números de 0 a 9. O número enviado via console USB é exibido tanto no display OLED quanto na matriz de LEDs 5x5.

- **Controle da Matriz**: A matriz é controlada pelo programa PIO, que manipula os LEDs para exibir os números.
- **Desligamento da Matriz**: A função `desligar_matriz()` apaga todos os LEDs da matriz de 5x5.

### 4. **Comunicação I2C**

O display OLED é conectado ao Raspberry Pi Pico via **I2C**. O código configura os pinos **SDA (GPIO 14)** e **SCL (GPIO 15)** para comunicação I2C e inicializa o display com a função `ssd1306_init()`.

### 5. **Função Principal (main)**

- **Configuração de pinos**: Configura os pinos de entrada (botões) e saída (LEDs).
- **Inicialização do PIO e Display OLED**: O programa PIO é carregado e o display OLED é configurado.
- **Loop Principal**: O sistema aguarda entradas do console USB e pressiona botões. Quando um número (0-9) é recebido, ele é exibido no display OLED e na matriz de LEDs.

### 6. **Controle via Console USB**

O console USB permite que o usuário envie números (0-9). Quando um número é enviado, ele é exibido na matriz de LEDs 5x5 e no display OLED.

### 7. **Função de Interrupção**

A função `gpio_irq_handler()` é usada para lidar com o pressionamento dos botões. Quando um botão é pressionado, o código altera o estado do LED correspondente e atualiza o display OLED.

### 🎞 `animations.c` (Animações)
Este arquivo contém funções para criar efeitos animados na matriz de LED.

### 🔠 `font.h` (Definição de Fontes)
Define os caracteres e fontes utilizados na exibição.

#### Principais conteúdos:
- **Matrizes de caracteres** → Define pixels de cada caractere.
- **Mapeamento de caracteres** → Relaciona códigos ASCII aos caracteres exibíveis.

### 📟 `ssd1306.h` e `ssd1306.c` (Controle do Display OLED)
Conjunto de funções para comunicação com o display SSD1306 via I2C.

#### Principais funções implementadas:
- **Inicialização do display** → Configuração do protocolo I2C.
- **Exibição de caracteres e gráficos** → Interface para desenhar na tela.
- **Atualização da tela** → Redesenha a tela conforme necessário.

# 💡 Desenvolvido com dedicação por:
- **[Nicassio Santos.](https://github.com/nicassiosantos)**


