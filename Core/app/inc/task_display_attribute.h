/*
 * task_display_attribute.h
 *
 *  Created on: Jan 17, 2026
 *      Author: Lautaro Alvarez
 */

#ifndef TASK_INC_TASK_DISPLAY_ATTRIBUTE_H_
#define TASK_INC_TASK_DISPLAY_ATTRIBUTE_H_

#include "main.h"
#include <stdbool.h>

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    ST_DSP_INIT,            // Pantalla de Bienvenida / Boot
    ST_DSP_MAIN_STATUS,     // Vista Normal: Estado y Contadores
    ST_DSP_MAIN_TEMPS,      // Vista Normal: Temperaturas (Podría alternar con STATUS)
    ST_DSP_SETUP_TIMEOUT,   // Menú: Configuración de Tiempo
    ST_DSP_SETUP_THRESHOLD, // Menú: Configuración de Límite Personas
    ST_DSP_ALERT            // Pantalla de Emergencia
} task_display_st_t;


typedef enum {
    EV_DSP_NO_EVENT,
    EV_DSP_REFRESH_REQ      // Solicitud de refresco
} task_display_ev_t;


typedef struct {

    task_display_st_t state;
    task_display_ev_t event;
    bool              flag;

    // Buffer de Datos
    char              system_state_str[10]; // Ej: "RUN", "IDLE", "EMER"
    uint32_t          people_count;         // Cantidad actual de personas

    // Parámetros de Operación
    uint32_t          cfg_timeout;          // Valor actual del timeout (ms)
    uint32_t          cfg_limit;            // Valor actual del límite de personas

    // Temperaturas
    int32_t           temp_internal;        // Temperatura interna del uC
    int32_t           temp_ambient;         // Temperatura del LM35

} task_display_dta_t;


extern task_display_dta_t task_display_dta;

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_DISPLAY_ATTRIBUTE_H_ */



