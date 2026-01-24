/*
 * display.c
 * Driver Optimizado para LCD 16x2 (Modo 4 Bits)
 * Características:
 * - Uso de DWT para delays de microsegundos (Alta Velocidad).
 * - Fallback a HAL_Delay si DWT no está listo (Seguridad en Init).
 * - Mapeo estricto a board.h
 */

#include "main.h"
#include "board.h"
#include "display.h"
#include "dwt.h"        // Necesario para los retardos de microsegundos
#include <stdint.h>
#include <stdbool.h>

// --- DEFINICIONES DE COMANDOS LCD (HD44780) ---
#define DISPLAY_IR_CLEAR_DISPLAY   0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL 0b00001000
#define DISPLAY_IR_FUNCTION_SET    0b00100000
#define DISPLAY_IR_SET_DDRAM_ADDR  0b10000000

#define DISPLAY_IR_ENTRY_MODE_SET_INCREMENT 0b00000010
#define DISPLAY_IR_ENTRY_MODE_SET_DECREMENT 0b00000000
#define DISPLAY_IR_ENTRY_MODE_SET_SHIFT     0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT  0b00000000

#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF 0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_ON   0b00000010
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF  0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_ON    0b00000001
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF   0b00000000

#define DISPLAY_IR_FUNCTION_SET_8BITS    0b00010000
#define DISPLAY_IR_FUNCTION_SET_4BITS    0b00000000
#define DISPLAY_IR_FUNCTION_SET_2LINES   0b00001000
#define DISPLAY_IR_FUNCTION_SET_1LINE    0b00000000
#define DISPLAY_IR_FUNCTION_SET_5x10DOTS 0b00000100
#define DISPLAY_IR_FUNCTION_SET_5x8DOTS  0b00000000

// Direcciones de memoria para 16x2
#define DISPLAY_LINE1_ADDR 0x00
#define DISPLAY_LINE2_ADDR 0x40

// Tipos de Mensaje
#define DISPLAY_RS_INSTRUCTION 0
#define DISPLAY_RS_DATA        1

// --- TIEMPOS OPTIMIZADOS (Microsegundos) ---
#define US_PULSE_WIDTH      2   // Ancho del pulso EN (>450ns)
#define US_INTER_NIBBLE     2   // Tiempo entre nibbles
#define US_EXEC_STD         50  // Tiempo ejecución comando estándar (>37us)
#define MS_EXEC_CLEAR       2   // Tiempo ejecución Clear/Home (>1.52ms)

/* --- VARIABLES PRIVADAS --- */
static bool initial8BitCommunicationIsCompleted = false;

/* --- PROTOTIPOS --- */
static void displayPinWrite(uint8_t pinName, int value);
static void displayDataBusWrite(uint8_t dataByte);
static void displayCodeWrite(bool type, uint8_t dataBus);
static void lcdFastDelay(uint32_t us);

/* --- FUNCIONES PÚBLICAS --- */

void displayInit(displayConnection_t connection)
{
    // 1. Inicialización física de pines (Reset)
    displayPinWrite(DISPLAY_PIN_RS, 0);
    displayPinWrite(DISPLAY_PIN_EN, 0);
    displayPinWrite(DISPLAY_PIN_D4, 0);
    displayPinWrite(DISPLAY_PIN_D5, 0);
    displayPinWrite(DISPLAY_PIN_D6, 0);
    displayPinWrite(DISPLAY_PIN_D7, 0);

    // Espera inicial de encendido (Lenta y segura)
    HAL_Delay(50);

    initial8BitCommunicationIsCompleted = false;

    // --- Secuencia de Reset "Mágica" (8 bits mode) ---
    displayCodeWrite(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_8BITS);
    HAL_Delay(5); // Usamos HAL_Delay porque DWT podría no estar listo aún en init

    displayCodeWrite(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_8BITS);
    HAL_Delay(1);

    displayCodeWrite(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_8BITS);
    HAL_Delay(1);

    // --- Pasar a 4 bits ---
    displayCodeWrite(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_4BITS);
    HAL_Delay(1);

    // --- AHORA YA ESTAMOS EN MODO 4 BITS ---
    initial8BitCommunicationIsCompleted = true;

    // Configurar: 4 bits, 2 líneas, fuente 5x8
    displayCodeWrite(DISPLAY_RS_INSTRUCTION,
                     DISPLAY_IR_FUNCTION_SET |
                     DISPLAY_IR_FUNCTION_SET_4BITS |
                     DISPLAY_IR_FUNCTION_SET_2LINES |
                     DISPLAY_IR_FUNCTION_SET_5x8DOTS);

    // Apagar display
    displayCodeWrite(DISPLAY_RS_INSTRUCTION,
                     DISPLAY_IR_DISPLAY_CONTROL |
                     DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF);

    // Limpiar pantalla (Comando lento)
    displayCodeWrite(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_CLEAR_DISPLAY);
    HAL_Delay(MS_EXEC_CLEAR);

    // Modo de entrada
    displayCodeWrite(DISPLAY_RS_INSTRUCTION,
                     DISPLAY_IR_ENTRY_MODE_SET |
                     DISPLAY_IR_ENTRY_MODE_SET_INCREMENT |
                     DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT);

    // Encender display
    displayCodeWrite(DISPLAY_RS_INSTRUCTION,
                     DISPLAY_IR_DISPLAY_CONTROL |
                     DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON |
                     DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF |
                     DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF);
}

void displayCharPositionWrite(uint8_t x, uint8_t y)
{
    uint8_t addr = DISPLAY_LINE1_ADDR;
    if (y > 0) addr = DISPLAY_LINE2_ADDR;
    addr += x;
    displayCodeWrite(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_SET_DDRAM_ADDR | addr);
}

void displayStringWrite(const char * str)
{
    while (*str) {
        displayCodeWrite(DISPLAY_RS_DATA, *str++);
    }
}

/* --- FUNCIONES PRIVADAS OPTIMIZADAS --- */

/**
 * @brief  Retardo híbrido: Usa DWT (us) si está activo, o HAL_Delay (ms) si no.
 * Esto asegura que funcione en displayInit() y vuele en displayStringWrite().
 */
static void lcdFastDelay(uint32_t us)
{
    // Verificamos si el contador de ciclos DWT está habilitado
    if (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)
    {
        uint32_t start = DWT->CYCCNT;
        // Calculamos ciclos necesarios (SystemCoreClock debe estar definido en main.h/stm32f1xx_hal.h)
        uint32_t cycles = us * (SystemCoreClock / 1000000);
        while ((DWT->CYCCNT - start) < cycles);
    }
    else
    {
        // Fallback seguro para inicialización (mínimo 1ms)
        uint32_t ms = us / 1000;
        if (ms == 0 && us > 0) ms = 1;
        HAL_Delay(ms);
    }
}

/**
 * @brief  Envío inteligente de datos con tiempos mínimos usando lcdFastDelay
 */
static void displayDataBusWrite(uint8_t dataBus)
{
    // 1. Enviar Nibble Alto (Bits 7-4)
    displayPinWrite(DISPLAY_PIN_EN, 0);
    displayPinWrite(DISPLAY_PIN_D7, (dataBus & 0x80) >> 7);
    displayPinWrite(DISPLAY_PIN_D6, (dataBus & 0x40) >> 6);
    displayPinWrite(DISPLAY_PIN_D5, (dataBus & 0x20) >> 5);
    displayPinWrite(DISPLAY_PIN_D4, (dataBus & 0x10) >> 4);

    // 2. Si estamos en modo 4 bits, enviamos Nibble Alto + Nibble Bajo
    if (initial8BitCommunicationIsCompleted)
    {
        // Pulso Nibble Alto
        displayPinWrite(DISPLAY_PIN_EN, 1);
        lcdFastDelay(US_PULSE_WIDTH);
        displayPinWrite(DISPLAY_PIN_EN, 0);
        lcdFastDelay(US_INTER_NIBBLE);

        // Enviar Nibble Bajo (Bits 3-0)
        displayPinWrite(DISPLAY_PIN_D7, (dataBus & 0x08) >> 3);
        displayPinWrite(DISPLAY_PIN_D6, (dataBus & 0x04) >> 2);
        displayPinWrite(DISPLAY_PIN_D5, (dataBus & 0x02) >> 1);
        displayPinWrite(DISPLAY_PIN_D4, (dataBus & 0x01));
    }

    // 3. Pulso Final (Nibble Bajo o Único Nibble en Init)
    displayPinWrite(DISPLAY_PIN_EN, 1);
    lcdFastDelay(US_PULSE_WIDTH);
    displayPinWrite(DISPLAY_PIN_EN, 0);

    // Tiempo de ejecución del comando (Dead time)
    lcdFastDelay(US_EXEC_STD);
}

static void displayCodeWrite(bool type, uint8_t dataBus)
{
    if (type == DISPLAY_RS_INSTRUCTION)
        displayPinWrite(DISPLAY_PIN_RS, DISPLAY_RS_INSTRUCTION);
    else
        displayPinWrite(DISPLAY_PIN_RS, DISPLAY_RS_DATA);

    displayDataBusWrite(dataBus);
}

/**
 * @brief  Capa física mapeada a board.h
 */
static void displayPinWrite(uint8_t pinName, int value)
{
    GPIO_PinState state = (value == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET;

    switch(pinName) {
        case DISPLAY_PIN_D4: HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, state); break;
        case DISPLAY_PIN_D5: HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, state); break;
        // Nota: D6 y D7 están en GPIOC según tu board.h
        case DISPLAY_PIN_D6: HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, state); break;
        case DISPLAY_PIN_D7: HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, state); break;

        case DISPLAY_PIN_RS: HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, state); break;
        case DISPLAY_PIN_EN: HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, state); break;
        default: break;
    }
}
