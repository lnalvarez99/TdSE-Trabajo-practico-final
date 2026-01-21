/*
 * display.c
 * Driver para Display LCD 16x2 (Modo 4 Bits)
 * Adaptado para usar definiciones de board.h
 */

#include "main.h"
#include "board.h"
#include "display.h"
#include <stdint.h>

// Definiciones de tiempos (en milisegundos)
#define LCD_DELAY_BOOT      50
#define LCD_DELAY_INST      2   // Tiempo genérico para instrucciones
#define LCD_DELAY_CLEAR     3   // Tiempo para Clear Display

/* --- FUNCIONES PRIVADAS (Helpers) --- */

/**
 * @brief  Escribe un valor lógico (0 o 1) en un pin del LCD
 * mapeando los nombres genéricos a los pines de board.h
 */

static void displayPinWrite(uint8_t pinName, int value)
{
    switch(pinName) {
        case DISPLAY_PIN_RS:
            HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, value);
            break;
        case DISPLAY_PIN_EN:
            HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, value);
            break;
        case DISPLAY_PIN_D4:
            HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, value);
            break;
        case DISPLAY_PIN_D5:
            HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, value);
            break;
        case DISPLAY_PIN_D6:
            HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, value);
            break;
        case DISPLAY_PIN_D7:
            HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, value);
            break;
        // RW está a GND físico, ignoramos DISPLAY_PIN_RW
        default:
            break;
    }
}

/**
 * @brief  Envía medio byte (nibble) a los pines de datos D4-D7
 */
static void displayDataBusWrite(uint8_t dataBus)
{
    displayPinWrite(DISPLAY_PIN_D4, (dataBus & 0x01));
    displayPinWrite(DISPLAY_PIN_D5, (dataBus & 0x02) >> 1);
    displayPinWrite(DISPLAY_PIN_D6, (dataBus & 0x04) >> 2);
    displayPinWrite(DISPLAY_PIN_D7, (dataBus & 0x08) >> 3);
}

/**
 * @brief  Genera el pulso de Enable para que el LCD lea los datos
 */
static void displayEnablePulse(void)
{
    displayPinWrite(DISPLAY_PIN_EN, 1);
    displayPinWrite(DISPLAY_PIN_EN, 0);
}

/**
 * @brief  Envía un byte completo en dos partes (Nibble Alto -> Nibble Bajo)
 * @param  value: El byte a enviar
 * @param  mode:  DISPLAY_PIN_RS (1 para Dato) o 0 (para Instrucción)
 */
static void displayByteWrite(uint8_t value, uint8_t mode)
{
    // 1. Configurar RS (Comando o Caracter)
    displayPinWrite(DISPLAY_PIN_RS, mode);

    // 2. Enviar Nibble Alto (Bits 7-4)
    displayDataBusWrite((value >> 4) & 0x0F);
    displayEnablePulse();

    // 3. Enviar Nibble Bajo (Bits 3-0)
    displayDataBusWrite(value & 0x0F);
    displayEnablePulse();

    // Retardo para que el LCD procese
    HAL_Delay(LCD_DELAY_INST);
}

/* --- FUNCIONES PÚBLICAS (API) --- */

void displayInit(displayConnection_t connection)
{
    // Inicialización física de pines (asegurar nivel bajo)
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, GPIO_PIN_RESET);

    HAL_Delay(LCD_DELAY_BOOT); // Esperar a que el voltaje se estabilice

    // --- Secuencia de Inicialización Mágica (Datasheet HD44780) ---
    // 1. Enviar 0x03 tres veces para resetear
    displayPinWrite(DISPLAY_PIN_RS, 0);

    displayDataBusWrite(0x03);
    displayEnablePulse();
    HAL_Delay(5);

    displayDataBusWrite(0x03);
    displayEnablePulse();
    HAL_Delay(1);

    displayDataBusWrite(0x03);
    displayEnablePulse();
    HAL_Delay(1);

    // 2. Cambiar a modo 4-bits (Enviar 0x02)
    displayDataBusWrite(0x02);
    displayEnablePulse();
    HAL_Delay(1);

    // A partir de aquí usamos displayByteWrite que envía los dos nibbles
    // 3. Configurar Función: 4 bits, 2 líneas, 5x8 puntos (0x28)
    displayByteWrite(0x28, 0);

    // 4. Apagar Display (0x08)
    displayByteWrite(0x08, 0);

    // 5. Limpiar Display (0x01)
    displayByteWrite(0x01, 0);
    HAL_Delay(LCD_DELAY_CLEAR);

    // 6. Configurar Modo Entrada: Incrementar cursor, sin shift (0x06)
    displayByteWrite(0x06, 0);

    // 7. Encender Display, Cursor Off, Blink Off (0x0C)
    displayByteWrite(0x0C, 0);
}

void displayCharPositionWrite(uint8_t x, uint8_t y)
{
    uint8_t addr = 0;

    // Calcular dirección de memoria DDRAM
    // Línea 0 empieza en 0x00, Línea 1 empieza en 0x40
    if (y > 0) addr = 0x40;
    addr += x;

    // Comando Set DDRAM Address (Bit 7 en 1)
    displayByteWrite(0x80 | addr, 0);
}

void displayStringWrite(const char * str)
{
    while (*str)
    {
        displayByteWrite((uint8_t)*str, 1); // Modo 1 = Dato
        str++;
    }
}
