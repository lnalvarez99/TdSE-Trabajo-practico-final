/*
 * task_temperature.h
 *
 *  Created on: Jan 20, 2026
 *      Author: lauta
 */

#ifndef TASK_INC_TASK_TEMPERATURE_H_
#define TASK_INC_TASK_TEMPERATURE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
/* Contadores globales para el planificador */
extern uint32_t g_task_temp_cnt;
extern volatile uint32_t g_task_temp_tick_cnt;

/********************** external functions declaration ***********************/

/**
 * @brief  Inicializa la tarea de temperatura, configurando los estados
 * iniciales de los sensores y temporizadores.
 * @param  parameters: Puntero a parámetros opcionales (no usado).
 */
extern void task_temperature_init(void *parameters);

/**
 * @brief  Función principal de actualización de la tarea.
 * Gestiona la máquina de estados del ADC, lecturas y conversión.
 * @param  parameters: Puntero a parámetros opcionales (no usado).
 */
extern void task_temperature_update(void *parameters);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_TEMPERATURE_H_ */
