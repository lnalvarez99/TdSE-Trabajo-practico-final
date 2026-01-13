#include "test_hardware.h"
#include "board.h"
#include "main.h" // Para HAL_Delay

void Test_Hardware_Loop(void)
{
    // Bucle infinito de prueba (Bloqueante)
    while (1)
    {
        // ---------------------------------------------------------
        // 1. CHEQUEO DE PARADA DE EMERGENCIA (Prioridad Máxima)
        // ---------------------------------------------------------
        if (HAL_GPIO_ReadPin(SW_DESACTIVAR_PORT, SW_DESACTIVAR_PIN) == SW_DESACTIVAR_ON)
        {
            // Apagar TODOS los LEDs
            HAL_GPIO_WritePin(LED_MOTOR_MAX_PORT, LED_MOTOR_MAX_PIN, LED_MOTOR_MAX_OFF);
            HAL_GPIO_WritePin(LED_MOTOR_MIN_PORT, LED_MOTOR_MIN_PIN, LED_MOTOR_MIN_OFF);
            HAL_GPIO_WritePin(LED_SYSTEM_PORT,    LED_SYSTEM_PIN,    LED_SYSTEM_OFF);
            HAL_GPIO_WritePin(LED_ALERT_PORT,     LED_ALERT_PIN,     LED_ALERT_OFF);

            HAL_Delay(50);
            continue; // Reinicia el ciclo, ignorando los botones
        }

        // ---------------------------------------------------------
        // 2. CHEQUEO DE BARRERA INFRARROJA
        // ---------------------------------------------------------
        if (HAL_GPIO_ReadPin(SW_BARRERA_PORT, SW_BARRERA_PIN) == SW_BARRERA_ON)
        {
            // Si la barrera se corta, parpadeo rápido en LED ALERTA
            HAL_GPIO_TogglePin(LED_ALERT_PORT, LED_ALERT_PIN);
            HAL_Delay(100);
        }
        else
        {
            // Si la barrera está libre, el LED responde al botón ENTER (ver abajo)
            // No hacemos nada aquí para no bloquear el botón
        }

        // ---------------------------------------------------------
        // 3. MAPEO DE BOTONES A LEDS
        // ---------------------------------------------------------

        // --- Botón INGRESO -> LED MOTOR MAX ---
        if (HAL_GPIO_ReadPin(BTN_INGRESO_PORT, BTN_INGRESO_PIN) == BTN_INGRESO_PRESSED) {
            HAL_GPIO_WritePin(LED_MOTOR_MAX_PORT, LED_MOTOR_MAX_PIN, LED_MOTOR_MAX_ON);
        } else {
            HAL_GPIO_WritePin(LED_MOTOR_MAX_PORT, LED_MOTOR_MAX_PIN, LED_MOTOR_MAX_OFF);
        }

        // --- Botón EGRESO -> LED MOTOR MIN ---
        if (HAL_GPIO_ReadPin(BTN_EGRESO_PORT, BTN_EGRESO_PIN) == BTN_EGRESO_PRESSED) {
            HAL_GPIO_WritePin(LED_MOTOR_MIN_PORT, LED_MOTOR_MIN_PIN, LED_MOTOR_MIN_ON);
        } else {
            HAL_GPIO_WritePin(LED_MOTOR_MIN_PORT, LED_MOTOR_MIN_PIN, LED_MOTOR_MIN_OFF);
        }

        // --- Botón MODE -> LED SYSTEM ---
        if (HAL_GPIO_ReadPin(BTN_MODE_PORT, BTN_MODE_PIN) == BTN_MODE_PRESSED) {
            HAL_GPIO_WritePin(LED_SYSTEM_PORT, LED_SYSTEM_PIN, LED_SYSTEM_ON);
        } else {
            HAL_GPIO_WritePin(LED_SYSTEM_PORT, LED_SYSTEM_PIN, LED_SYSTEM_OFF);
        }

        // --- Botón ENTER -> LED ALERT (Solo si barrera no está activa) ---
        if (HAL_GPIO_ReadPin(SW_BARRERA_PORT, SW_BARRERA_PIN) == SW_BARRERA_OFF)
        {
            if (HAL_GPIO_ReadPin(BTN_ENTER_PORT, BTN_ENTER_PIN) == BTN_ENTER_PRESSED) {
                HAL_GPIO_WritePin(LED_ALERT_PORT, LED_ALERT_PIN, LED_ALERT_ON);
            } else {
                HAL_GPIO_WritePin(LED_ALERT_PORT, LED_ALERT_PIN, LED_ALERT_OFF);
            }
        }

        HAL_Delay(10); // Pequeño delay para estabilidad
    }
}



