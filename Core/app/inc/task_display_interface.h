/*
 * task_display_interface.h
 *
 *  Created on: Jan 17, 2026
 *      Author: Lautaro Alvarez
 */

#ifndef TASK_INC_TASK_DISPLAY_INTERFACE_H_
#define TASK_INC_TASK_DISPLAY_INTERFACE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "task_display_attribute.h"

/********************** external functions declaration ***********************/

/**
 * @brief  Cambia el estado visual del display (Navegación de menús).
 * @param  new_state: Nuevo estado de la máquina de estados del display.
 */
extern void Display_SetState(task_display_st_t new_state);

/**
 * @brief  Actualiza los datos de estado del sistema y contador de personas.
 * @param  state_str: Cadena corta (ej: "RUN", "IDLE").
 * @param  people: Cantidad actual de personas.
 */
extern void Display_UpdateData(char *state_str, uint32_t people);

/**
 * @brief  Actualiza los valores de configuración mostrados en el menú.
 * @param  timeout: Tiempo de espera actual (en ms).
 * @param  limit: Límite de personas configurado.
 */
extern void Display_UpdateConfig(uint32_t timeout, uint32_t limit);

/**
 * @brief  Actualiza las lecturas de temperatura.
 * @param  internal: Temperatura interna del microcontrolador.
 * @param  ambient: Temperatura ambiente (Sensor LM35).
 */
extern void Display_UpdateTemps(int32_t internal, int32_t ambient);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_DISPLAY_INTERFACE_H_ */
