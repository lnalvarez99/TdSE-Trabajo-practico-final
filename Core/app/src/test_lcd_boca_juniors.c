#include "main.h"
#include "board.h"
#include "display.h"

/**
 * @brief Función de test de hardware para el LCD.
 * IMPORTANTE: Esta función es bloqueante (usa while(1) y HAL_Delay).
 * Úsala solo para verificar conexiones, no junto con el resto del sistema.
 */
void test_lcd_boca_juniors(void)
{
    // 1. Inicializar el Display
    // Usa los pines definidos en board.h (GPIOB y GPIOC)
    displayInit(DISPLAY_CONNECTION_GPIO_4BITS);

    while (1)
    {
        // --- MENSAJE 1: BIENVENIDA ---

        // Limpiamos o sobrescribimos la Línea 0
        displayCharPositionWrite(0, 0);
        // Escribimos espacios extra para asegurar borrar rastros anteriores
        displayStringWrite("   BIENVENIDO   ");

        // Vamos a la Línea 1
        displayCharPositionWrite(0, 1);
        displayStringWrite(" Sistema TdSE 9 ");

        // Esperar 5 segundos
        HAL_Delay(5000);

        // --- MENSAJE 2: AGUANTE BOCA ---

        // Línea 0
        displayCharPositionWrite(0, 0);
        displayStringWrite("  TEST DISPLAY  ");

        // Línea 1
        displayCharPositionWrite(0, 1);
        // El mensaje solicitado
        displayStringWrite("  AGUANTE BOCA  ");

        // Esperar 5 segundos
        HAL_Delay(5000);
    }
}
