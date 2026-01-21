/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file   : task_system_attribute.h
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

#ifndef TASK_INC_TASK_SYSTEM_ATTRIBUTE_H_
#define TASK_INC_TASK_SYSTEM_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* System Statechart - State Transition Table */
/* 	------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * 	| Current               | Event                 |                       | Next                  |                       |
 * 	| State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * 	|=======================+=======================+=======================+=======================+=======================|
 * 	| ST_SYS_XX_IDLE        | EV_SYS_XX_ACTIVE      |                       | ST_SYS_XX_ACTIVE      | put_event_t.._actuator|
 * 	|                       |                       |                       |                       | (event, identifier)   |
 * 	|-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * 	| ST_SYS_XX_ACTIVE      | EV_SYS_XX_IDLE        |                       | ST_SYS_XX_IDLE        | put_event_t.._actuator|
 * 	|                       |                       |                       |                       | (event, identifier)   |
 * 	------------------------+-----------------------+-----------------------+-----------------------+------------------------
 */

/* Events to excite Task System */
typedef enum task_system_ev {
		EV_SYS_IDLE = 0,        // Evento Nulo (No hace nada)

	    EV_PERSONA_INGRESA,     // Alguien quiere subir
	    EV_PERSONA_EGRESA,      // Alguien terminó de bajar

		EV_SISTEMA_TOGGLE,      // Activar/Desactivar sistema (Botón Mode)
	    EV_MENU_ENTER,          // Confirmar selección (Botón Enter)

	    EV_BARRERA_INTERRUMPIDA,// Se cortó el haz de luz
	    EV_BARRERA_RESTAURADA,  // Se restauró el haz

	    EV_PARADA_EMERGENCIA,   // Se activó el switch de corte
	    EV_PARADA_RESTAURADA,    // Se desactivó el switch de corte

		EV_SYS_ACTIVE

		/*EV_TIMEOUT  -> DETIENE LA ESCLARA (ST_SYS_IDLE) VEL = 0*/
} task_system_ev_t;

/* State of Task System */
typedef enum task_system_st {
	    ST_SYS_IDLE,        // Modo Ahorro (Velocidad Mínima / Espera)
	    ST_SYS_RUNNING,     // Modo Activo (Velocidad Máxima / Transportando)
	    ST_SYS_EMERGENCY,   // Parada de Emergencia (Bloqueo)
	    ST_SYS_SETUP_TIMEOUT,
		ST_SYS_SETUP_THRESHOLD
} task_system_st_t;

typedef struct
{
	task_system_st_t	state;
	task_system_ev_t	event;
	bool				flag;
	uint32_t            people_counter; // Cuenta cuántas personas hay en la escalera
	uint32_t            timeout_stability; //Variable de Estabilidad (Watchdog)
	uint32_t            cfg_timeout_max;    // Tiempo de espera (ej: 30000ms)
	uint32_t            cfg_people_limit;   // Cantidad para cambio de velocidad
} task_system_dta_t;

/********************** external data declaration ****************************/
extern task_system_dta_t task_system_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
