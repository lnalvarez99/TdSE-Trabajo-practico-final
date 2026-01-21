/*
 * task_display.h
 *
 * Created on: Jan 17, 2026
 * Author: Lautaro Alvarez
 */

#ifndef TASK_INC_TASK_DISPLAY_H_
#define TASK_INC_TASK_DISPLAY_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

/**
 * @brief  Inicializa el hardware del display (4-bit mode) y las estructuras de datos.
 * @param  parameters: Puntero a parámetros (no utilizado).
 */
extern void task_display_init(void *parameters);

/**
 * @brief  Máquina de estados del display.
 * Revisa la bandera de evento (flag) y actualiza la pantalla si es necesario.
 * @param  parameters: Puntero a parámetros (no utilizado).
 */
extern void task_display_update(void *parameters);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_DISPLAY_H_ */
