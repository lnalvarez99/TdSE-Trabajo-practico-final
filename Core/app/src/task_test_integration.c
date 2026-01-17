#include "main.h"
#include "logger.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "task_actuator_attribute.h"

/* --- CONFIGURACIÓN DEL TEST --- */
#define TEST_STEP_DELAY 2000 // 2 segundos entre pasos para poder ver los logs

/* Estados del Test */
typedef enum {
    TEST_INIT,
    TEST_CASE_1_ENTRY,
    TEST_CASE_1_CHECK,
    TEST_CASE_2_EXIT,
    TEST_CASE_2_CHECK,
    TEST_CASE_3_TIMEOUT,
    TEST_CASE_3_WAIT,
    TEST_CASE_3_CHECK,
    TEST_DONE
} test_state_t;

/* Variables Globales del Test */
static test_state_t test_state = TEST_INIT;
static uint32_t test_tick = 0;
static uint32_t test_wait_accumulator = 0;

/* Referencia externa a los datos del sistema para "espiarlos" */
extern task_system_dta_t task_system_dta;

void task_test_integration_init(void *parameters)
{
    LOGGER_LOG("\r\n[TEST] INICIANDO SUITE DE PRUEBAS AUTOMATICAS...\r\n");
    test_state = TEST_INIT;
    test_tick = 0;
}

void task_test_integration_update(void *parameters)
{
    // Simples delays no bloqueantes
    test_tick++;
    if (test_tick < (TEST_STEP_DELAY / 50)) { // Asumiendo tick de 50ms
        return;
    }
    test_tick = 0;

    switch (test_state)
    {
        case TEST_INIT:
            LOGGER_LOG("[TEST] Esperando estabilizacion del sistema...\r\n");
            test_state = TEST_CASE_1_ENTRY;
            break;

        // ---------------------------------------------------------------
        // CASO 1: SIMULAR ENTRADA DE PERSONA
        // ---------------------------------------------------------------
        case TEST_CASE_1_ENTRY:
            LOGGER_LOG("[TEST] Paso 1: Inyectando EV_PERSONA_INGRESA...\r\n");
            put_event_task_system(EV_PERSONA_INGRESA);
            test_state = TEST_CASE_1_CHECK;
            break;

        case TEST_CASE_1_CHECK:
            // Verificamos si el sistema pasó a RUNNING
            if (task_system_dta.state == ST_SYS_RUNNING) {
                LOGGER_LOG("[TEST] >> OK: El sistema paso a RUNNING.\r\n");
                test_state = TEST_CASE_2_EXIT;
            } else {
                LOGGER_LOG("[TEST] >> FAIL: El sistema NO paso a RUNNING (State=%d)\r\n", task_system_dta.state);
                test_state = TEST_DONE; // Abortar
            }
            break;

        // ---------------------------------------------------------------
        // CASO 2: SIMULAR SALIDA DE PERSONA
        // ---------------------------------------------------------------
        case TEST_CASE_2_EXIT:
            LOGGER_LOG("[TEST] Paso 2: Inyectando EV_PERSONA_EGRESA...\r\n");
            put_event_task_system(EV_PERSONA_EGRESA);
            test_state = TEST_CASE_2_CHECK;
            break;

        case TEST_CASE_2_CHECK:
            // Verificamos si volvió a IDLE (porque cont. personas llegó a 0)
            if (task_system_dta.state == ST_SYS_IDLE) {
                LOGGER_LOG("[TEST] >> OK: El sistema volvio a IDLE.\r\n");
                test_state = TEST_CASE_3_TIMEOUT;
            } else {
                LOGGER_LOG("[TEST] >> FAIL: El sistema NO volvio a IDLE (State=%d, Counter=%lu)\r\n",
                            task_system_dta.state, task_system_dta.people_counter);
                test_state = TEST_DONE;
            }
            break;

        // ---------------------------------------------------------------
        // CASO 3: PRUEBA DE ESTABILIDAD (WATCHDOG / TIMEOUT)
        // ---------------------------------------------------------------
        case TEST_CASE_3_TIMEOUT:
            LOGGER_LOG("[TEST] Paso 3: Prueba de Watchdog. Entrando persona y esperando...\r\n");
            put_event_task_system(EV_PERSONA_INGRESA);
            test_wait_accumulator = 0;
            test_state = TEST_CASE_3_WAIT;
            break;

        case TEST_CASE_3_WAIT:
            // Esperamos ~35 segundos (más que los 30s del timeout)
            // Cada vuelta de este switch son 2 segundos (por el TEST_STEP_DELAY)
            test_wait_accumulator += 2;

            if (test_wait_accumulator % 10 == 0) {
                 LOGGER_LOG("[TEST] ... Esperando %lu segundos ...\r\n", test_wait_accumulator);
            }

            if (test_wait_accumulator >= 35) {
                test_state = TEST_CASE_3_CHECK;
            }
            break;

        case TEST_CASE_3_CHECK:
            // Debería haber vuelto a IDLE solo, por timeout
            if (task_system_dta.state == ST_SYS_IDLE) {
                LOGGER_LOG("[TEST] >> OK: WATCHDOG FUNCIONA. Sistema reseteado por tiempo.\r\n");
                LOGGER_LOG("[TEST] *** TODAS LAS PRUEBAS PASARON ***\r\n");
            } else {
                LOGGER_LOG("[TEST] >> FAIL: Watchdog fallo. Sistema sigue en RUNNING.\r\n");
            }
            test_state = TEST_DONE;
            break;

        case TEST_DONE:
            // Nada más que hacer
            break;
    }
}
