/*
 * task_display_interface.c
 *
 *  Created on: Jan 17, 2026
 *      Author: Lautaro Alvarez
 */


/********************** inclusions *******************************************/
#include "main.h"
#include "task_display_attribute.h"
#include "task_display_interface.h"
#include <string.h>
#include <stdio.h>

/********************** external functions definition ************************/

void Display_SetState(task_display_st_t new_state)
{
    // Cambiamos el estado (la "Pantalla" que se ve)
    task_display_dta.state = new_state;

    // Generamos un evento de refresco para que se pinte inmediatamente
    task_display_dta.event = EV_DSP_REFRESH_REQ;
    task_display_dta.flag = true;
}

void Display_UpdateData(char *state_str, uint32_t people)
{
    // Actualizamos el texto de estado (ej: "RUN", "IDLE")
    // Usamos snprintf para evitar desbordamientos de buffer
    snprintf(task_display_dta.system_state_str, sizeof(task_display_dta.system_state_str), "%s", state_str);

    // Actualizamos el contador de personas
    task_display_dta.people_count = people;

    // Solicitamos pintar la pantalla
    task_display_dta.event = EV_DSP_REFRESH_REQ;
    task_display_dta.flag = true;
}

void Display_UpdateConfig(uint32_t timeout, uint32_t limit)
{
    task_display_dta.cfg_timeout = timeout;
    task_display_dta.cfg_limit = limit;

    task_display_dta.event = EV_DSP_REFRESH_REQ;
    task_display_dta.flag = true;
}

void Display_UpdateTemps(int32_t internal, int32_t ambient)
{
    task_display_dta.temp_internal = internal;
    task_display_dta.temp_ambient = ambient;

    task_display_dta.event = EV_DSP_REFRESH_REQ;
    task_display_dta.flag = true;
}

/********************** end of file ******************************************/
