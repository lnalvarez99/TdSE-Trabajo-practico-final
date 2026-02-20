#ifndef DISPLAY_H_
#define DISPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* --- TIPOS DE DATOS --- */

// Tipos de conexión soportados
typedef enum {
    DISPLAY_CONNECTION_GPIO_4BITS,
    DISPLAY_CONNECTION_GPIO_8BITS,
    DISPLAY_CONNECTION_I2C
} displayConnection_t;

// Identificadores internos de pines del LCD
// (Usados por el driver para el switch-case)
typedef enum {
    DISPLAY_PIN_RS,
    DISPLAY_PIN_RW,
    DISPLAY_PIN_EN,
    DISPLAY_PIN_D0,
    DISPLAY_PIN_D1,
    DISPLAY_PIN_D2,
    DISPLAY_PIN_D3,
    DISPLAY_PIN_D4,
    DISPLAY_PIN_D5,
    DISPLAY_PIN_D6,
    DISPLAY_PIN_D7
} displayPin_t;

/* --- PROTOTIPOS DE FUNCIONES PÚBLICAS --- */

/**
 * @brief  Inicializa el display LCD 16x2.
 * Configura los pines definidos en board.h y ejecuta la secuencia de arranque.
 * @param  connection: Tipo de conexión (Usar DISPLAY_CONNECTION_GPIO_4BITS)
 */
void displayInit(displayConnection_t connection);

/**
 * @brief  Mueve el cursor a una posición específica.
 * @param  x: Columna (0 a 15)
 * @param  y: Fila (0 o 1)
 */
void displayCharPositionWrite(uint8_t x, uint8_t y);

/**
 * @brief  Escribe una cadena de texto en la posición actual del cursor.
 * @param  str: Puntero a la cadena (terminada en \0)
 */
void displayStringWrite(const char * str);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H_ */
