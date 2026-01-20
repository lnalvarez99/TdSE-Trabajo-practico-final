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
 *
 * @file   : task_system.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"
#include "task_display_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_SYS_CNT_INI			0ul
#define G_TASK_SYS_TICK_CNT_INI		0ul

// Definiciones de tiempo base (ticks de 1ms)
#define TIMEOUT_MIN   10000UL // 10 Segundos
#define TIMEOUT_MED   20000UL // 20 Segundos
#define TIMEOUT_MAX   30000UL // 30 Segundos
#define MAX_PERSONS   5
#define MIN_PERSONS   1

/********************** internal data declaration ****************************/

#define SYSTEM_DTA_QTY	(sizeof(task_system_dta)/sizeof(task_system_dta_t))

/********************** internal functions declaration ***********************/
task_system_dta_t task_system_dta;
/********************** internal data definition *****************************/
const char *p_task_system 		= "Task System (System Statechart)";
const char *p_task_system_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_system_cnt;
volatile uint32_t g_task_system_tick_cnt;

/********************** external functions definition ************************/
void task_system_init(void *parameters)
{
	task_system_dta_t 	*p_task_system_dta;
	task_system_st_t	state;
	task_system_ev_t	event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_system_init), p_task_system);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_system), p_task_system_);

	g_task_system_cnt = G_TASK_SYS_CNT_INI;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_system_cnt), g_task_system_cnt);

	init_queue_event_task_system();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_system_dta = &task_system_dta;

	/* Print out: Task execution FSM */
	state = p_task_system_dta->state;
	LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

	event = p_task_system_dta->event;
	LOGGER_LOG("   %s = %lu", GET_NAME(event), (uint32_t)event);

	b_event = p_task_system_dta->flag;
	LOGGER_LOG("   %s = %s\r\n", GET_NAME(b_event), (b_event ? "true" : "false"));

	// Inicialización de Variables

	    task_system_dta.state = ST_SYS_IDLE;
	    task_system_dta.event = EV_SYS_IDLE;
	    task_system_dta.flag = false;
	    task_system_dta.people_counter = 0;
	    task_system_dta.timeout_stability = TIMEOUT_MAX;
	    task_system_dta.cfg_timeout_max = TIMEOUT_MAX; // 30 segundos
	    task_system_dta.cfg_people_limit = MIN_PERSONS;          // Con 1 persona ya acelera

	    /* --------VECTOR DE ESTADOS DE MI SISTEMA X=[x1,x2,x3,x4,x5------ */
	    /*
	     *          x1 = Motor al minimo
	     *          x2 = Motor al maximo
	     *          x3 = Sistema activo
	     *          x4 = Alerta
	     *          x5 = Sirena
	     *
	     *
	     -------------------------------------------------------------------*/

	  // ESTADO INICIAL DE ACTUADORES
	    put_event_task_actuator(EV_ACTUATOR_ON, ID_ACT_MOTOR_MIN); //  1
	    put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MAX); // 0
	    put_event_task_actuator(EV_ACTUATOR_ON, ID_ACT_SYSTEM_OK); //  1
	    put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_ALERT); //     0
	    put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_BUZZER); //    0

	  // ESTADO INICIAL DEL DISPLAY
	    Display_SetState(ST_DSP_MAIN_STATUS);
	    Display_UpdateData("IDLE", 0);

	    g_task_system_tick_cnt = G_TASK_SYS_TICK_CNT_INI;
}

void task_system_update(void *parameters)
{
    task_system_dta_t *p_task_system_dta;
    bool b_time_update_required = false;
    g_task_system_cnt++;

    __asm("CPSID i");
    if (G_TASK_SYS_TICK_CNT_INI < g_task_system_tick_cnt) {
        g_task_system_tick_cnt--;
        b_time_update_required = true;
    }
    __asm("CPSIE i");

    while (b_time_update_required)
    {
        /* Protección de recurso compartido */
        __asm("CPSID i");
        if (G_TASK_SYS_TICK_CNT_INI < g_task_system_tick_cnt) {
            g_task_system_tick_cnt--;
            b_time_update_required = true;
        } else {
            b_time_update_required = false;
        }
        __asm("CPSIE i");

        /* Puntero a la estructura de datos en RAM */
        p_task_system_dta = &task_system_dta;

        /* 1. VERIFICAR SI HAY EVENTOS NUEVOS EN LA COLA */
        if (true == any_event_task_system()) {
            p_task_system_dta->flag = true;
            p_task_system_dta->event = get_event_task_system();
        }

        /* 2. MAQUINA DE ESTADOS PRINCIPAL */
        switch (p_task_system_dta->state)
        {

        // ================================================================
        // MODO NORMAL: IDLE (Reposo)
        // ================================================================
        case ST_SYS_IDLE:
            if (true == p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                // A. ENTRADA A MODO SETUP (Botón MODE)
                if (EV_SISTEMA_TOGGLE == p_task_system_dta->event)
                {
                    // Feedback Visual: Apagamos motores para indicar "Modo Config"
                    put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MIN);
                    put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MAX);
                    // Parpadeamos System OK para indicar que estamos en menú
                    put_event_task_actuator(EV_ACTUATOR_BLINK, ID_ACT_SYSTEM_OK);

                    Display_SetState(ST_DSP_SETUP_TIMEOUT);
                    Display_UpdateConfig(p_task_system_dta->timeout_stability, 0);

                    LOGGER_LOG("[SYS] Entrando a SETUP: Config Timeout\r\n");
                    p_task_system_dta->state = ST_SYS_SETUP_TIMEOUT;
                }
                // B. FUNCIONAMIENTO NORMAL (Ingresa Persona)
                else if (EV_PERSONA_INGRESA == p_task_system_dta->event)
                {
                    p_task_system_dta->people_counter = 1;
                    p_task_system_dta->timeout_stability = p_task_system_dta->cfg_timeout_max; // Usamos valor configurado

                    // Lógica de Velocidad según Configuración
                    if (p_task_system_dta->people_counter >= p_task_system_dta->cfg_people_limit) {
                        put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MIN);
                        put_event_task_actuator(EV_ACTUATOR_ON,  ID_ACT_MOTOR_MAX);
                    } else {
                        // Si el límite es alto (ej: 3 personas) y entra 1, seguimos en Min pero reseteamos timer
                        put_event_task_actuator(EV_ACTUATOR_ON,  ID_ACT_MOTOR_MIN);
                        put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MAX);
                    }

                    Display_UpdateData("RUN ", p_task_system_dta->people_counter);
                    p_task_system_dta->state = ST_SYS_RUNNING;
                }
                // C. SEGURIDAD (Emergencias)
                else if ((EV_PARADA_EMERGENCIA == p_task_system_dta->event) ||
                         (EV_BARRERA_INTERRUMPIDA == p_task_system_dta->event))
                {
                    put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MIN);
                    put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MAX);
                    put_event_task_actuator(EV_ACTUATOR_BLINK, ID_ACT_ALERT);
                    put_event_task_actuator(EV_ACTUATOR_ON,    ID_ACT_BUZZER);
                    Display_SetState(ST_DSP_ALERT);
                    p_task_system_dta->state = ST_SYS_EMERGENCY;
                }
            }
            break;

        // ================================================================
        // MODO SETUP: CONFIGURAR TIMEOUT (Variable 2)
        // ================================================================
        case ST_SYS_SETUP_TIMEOUT:
            if (true == p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                // OPCIÓN 1: CAMBIAR VALOR (Botón MODE)
                if (EV_SISTEMA_TOGGLE == p_task_system_dta->event)
                {
                    // Ciclo: 10s -> 20s -> 30s -> 10s...
                    if (p_task_system_dta->cfg_timeout_max == TIMEOUT_MAX) {
                        p_task_system_dta->cfg_timeout_max = TIMEOUT_MIN;
                    } else {
                        p_task_system_dta->cfg_timeout_max += 10000;
                    }
                    LOGGER_LOG("[SETUP] Nuevo Timeout: %lu ms\r\n", p_task_system_dta->cfg_timeout_max);

                    Display_SetState(ST_DSP_SETUP_TIMEOUT);
                    Display_UpdateConfig(p_task_system_dta->cfg_timeout_max, 0);
                }
                // OPCIÓN 2: CONFIRMAR Y SIGUIENTE (Botón ENTER)
                else if (EV_MENU_ENTER == p_task_system_dta->event)
                {
                    LOGGER_LOG("[SYS] Guardado. Pasando a Config Personas...\r\n");
                    p_task_system_dta->state = ST_SYS_SETUP_THRESHOLD;
                }
            }
            break;

        // ================================================================
        // MODO SETUP: CONFIGURAR CANTIDAD PERSONAS (Variable 1)
        // ================================================================
        case ST_SYS_SETUP_THRESHOLD:
            if (true == p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                // OPCIÓN A: MODIFICAR VALOR (Botón MODE)
                if (EV_SISTEMA_TOGGLE == p_task_system_dta->event)
                {
                    // Lógica para cambiar 1 -> 2 -> 3 ...
                    p_task_system_dta->cfg_people_limit++;
                    if (p_task_system_dta->cfg_people_limit > MAX_PERSONS) {
                        p_task_system_dta->cfg_people_limit = MIN_PERSONS;
                    }
                    Display_SetState(ST_DSP_SETUP_THRESHOLD);
                    Display_UpdateConfig(p_task_system_dta->cfg_people_limit, 0);
                }

                // OPCIÓN B: SALIR Y GUARDAR (Botón ENTER)
                else if (EV_MENU_ENTER == p_task_system_dta->event)
                {
                    LOGGER_LOG("[SYS] Guardando cambios y volviendo a NORMAL...\r\n");


                    // 1. Encendemos Motor Mínima (La escalera siempre se mueve lento en reposo)
                    put_event_task_actuator(EV_ACTUATOR_ON, ID_ACT_MOTOR_MIN);

                    // 2. Apagamos Motor Máxima (Por seguridad)
                    put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MAX);

                    // 3. Dejamos el Led de Sistema OK fijo (ya no parpadea)
                    put_event_task_actuator(EV_ACTUATOR_ON, ID_ACT_SYSTEM_OK);

                    // 4. Apagamos Alerta por si quedó prendida
                    put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_ALERT);

                    p_task_system_dta->state = ST_SYS_IDLE;
                }
            }
            break;



            // ----------------------------------------------------------------
            // ESTADO: RUNNING
            // ----------------------------------------------------------------
            case ST_SYS_RUNNING:

                // RAMA 1: SI OCURRE UN EVENTO (Sensores Activos)
                if (true == p_task_system_dta->flag)
                {
                    p_task_system_dta->flag = false;

                    // "Kick the Dog": Si los sensores funcionan, reiniciamos el timeout
                    p_task_system_dta->timeout_stability = p_task_system_dta->cfg_timeout_max; // "Kick the dog"
                    if (EV_PERSONA_INGRESA == p_task_system_dta->event)
                    {
                        p_task_system_dta->people_counter++;
                        Display_UpdateData("RUN ", p_task_system_dta->people_counter);
                        if (p_task_system_dta->people_counter >= p_task_system_dta->cfg_people_limit)
                        {
                            put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MIN);
                            put_event_task_actuator(EV_ACTUATOR_ON,  ID_ACT_MOTOR_MAX);
                        }

                    }
                    else if (EV_PERSONA_EGRESA == p_task_system_dta->event)
                    {
                        if (p_task_system_dta->people_counter > 0) {
                            p_task_system_dta->people_counter--;
                            Display_UpdateData("RUN ", p_task_system_dta->people_counter);
                        }

                        // Condición Normal de Salida
                        if (p_task_system_dta->people_counter == 0)
                        {
                            put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MAX);
                            put_event_task_actuator(EV_ACTUATOR_ON,  ID_ACT_MOTOR_MIN);
                            Display_UpdateData("IDLE", 0);
                            p_task_system_dta->state = ST_SYS_IDLE;
                        }
                    }
                    // Prioridad de Seguridad
                    else if ((EV_PARADA_EMERGENCIA == p_task_system_dta->event) ||
                             (EV_BARRERA_INTERRUMPIDA == p_task_system_dta->event))
                    {
                        put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MAX);
                        put_event_task_actuator(EV_ACTUATOR_BLINK, ID_ACT_ALERT);
                        put_event_task_actuator(EV_ACTUATOR_ON,    ID_ACT_BUZZER);
                        p_task_system_dta->state = ST_SYS_EMERGENCY;
                    }
                }

                // RAMA 2: SI NO HAY EVENTOS (Control por Tiempo)
                else
                {
                    if (p_task_system_dta->timeout_stability > 0)
                    {
                        p_task_system_dta->timeout_stability--; // Decremento del Watchdog
                    }
                    else
                    {
                        // El sistema perdió observabilidad (sensores no reportan salida).
                        // Forzamos el retorno al estado de mínima energía.

                        p_task_system_dta->people_counter = 0; // Reset forzado
                        Display_UpdateData("IDLE", 0);
                        put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_MOTOR_MAX);
                        put_event_task_actuator(EV_ACTUATOR_ON,  ID_ACT_MOTOR_MIN);

                        p_task_system_dta->state = ST_SYS_IDLE;
                    }
                }
                break;

            // ----------------------------------------------------------------
            // ESTADO: EMERGENCY (Bloqueo Total)
            // ----------------------------------------------------------------
            case ST_SYS_EMERGENCY:
                if (true == p_task_system_dta->flag)
                {
                    p_task_system_dta->flag = false;

                    // Salida solo si se restaura la condición segura
                    if ((EV_PARADA_RESTAURADA == p_task_system_dta->event) ||
                        (EV_BARRERA_RESTAURADA == p_task_system_dta->event))
                    {
                        put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_BUZZER);
                        put_event_task_actuator(EV_ACTUATOR_OFF, ID_ACT_ALERT);

                        p_task_system_dta->people_counter = 0; // Limpieza por seguridad

                        put_event_task_actuator(EV_ACTUATOR_ON, ID_ACT_MOTOR_MIN);
                        p_task_system_dta->state = ST_SYS_IDLE;
                    }
                }
                break;

            default:
                // Recuperación de fallas
                p_task_system_dta->state = ST_SYS_IDLE;
                break;
        }
    }
}


