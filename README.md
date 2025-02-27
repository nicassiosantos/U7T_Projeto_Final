# Monitoramento de Nível de Tanque com Raspberry Pi Pico

## Descrição
Este projeto utiliza um Raspberry Pi Pico para monitorar o nível de um tanque de líquido. O sistema inclui sensores para medição do nível, um buzzer para alarmes, uma matriz de LEDs controlada via PIO para indicação visual e um display OLED via I2C para exibir mensagens de status.

## 📹 video do Projeto
- **[Ver Video](https://drive.google.com/file/d/1G7pz9QxcnxozLGu3qG6_1T0-QDrFy6T7/view?usp=sharing)**

## Funcionalidades
- Leitura do nível do tanque utilizando sensores.
- Indicação visual do nível através de uma matriz de LEDs.
- Alerta sonoro via buzzer em caso de nível crítico.
- Exibição de mensagens no display OLED.
- Comunicação eficiente utilizando PIO para controle de LEDs.

## Requisitos
### Hardware:
- Raspberry Pi Pico
- Sensores de nível de líquido
- Buzzer
- Matriz de LEDs
- Display OLED (I2C)
- Resistores e fiação conforme necessário

### Software:
- C/C++
- SDK do Raspberry Pi Pico
- Bibliotecas para comunicação I2C e PIO

## Instalação e Uso
1. **Configuração do ambiente:**
   - Instale o SDK do Raspberry Pi Pico.
   - Configure o ambiente de desenvolvimento (ex: VS Code com o plugin do Pico ou CMake com Makefile).

2. **Compilação do código:**
   - Clone o repositório e navegue até o diretório do projeto.
   - Compile o código utilizando `cmake` e `make`:
     ```sh
     mkdir build
     cd build
     cmake ..
     make
     ```

3. **Upload do firmware:**
   - Conecte o Raspberry Pi Pico ao computador segurando o botão BOOTSEL.
   - Copie o arquivo `.uf2` gerado para o dispositivo montado.

4. **Operação:**
   - O sistema começará a monitorar o nível do tanque automaticamente.
   - A matriz de LEDs indicará o nível.
   - O display OLED mostrará mensagens de status.
   - O buzzer emitirá alertas caso o nível atinja um valor crítico.

## Personalização
- Ajuste os limites de nível no código conforme necessário.
- Modifique as mensagens exibidas no OLED para atender às suas necessidades.
- Integre outros sensores ou módulos para expandir a funcionalidade do sistema.



# 💡 Desenvolvido com dedicação por:
- **[Nicassio Santos.](https://github.com/nicassiosantos)**


