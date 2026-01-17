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
 * @file   : task_actuator.c
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
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_ACT_CNT_INIT			0ul
#define G_TASK_ACT_TICK_CNT_INI		0ul

#define DEL_LED_PUL				250ul
#define DEL_LED_BLI				500ul
#define DEL_LED_MIN				0ul

/********************** internal data declaration ****************************/
const task_actuator_cfg_t task_actuator_cfg_list[] = {
		// 1. MOTOR VELOCIDAD MÁXIMA (LED 1 - PA9)
		    {
		        ID_ACT_MOTOR_MAX,
		        LED_MOTOR_MAX_PORT,
		        LED_MOTOR_MAX_PIN,
		        LED_MOTOR_MAX_ON,       // Active High (board.h)
		        LED_MOTOR_MAX_OFF,
				DEL_LED_MIN,                      // No usa Blink
				DEL_LED_MIN	                       // No usa Pulse
		    },

		    // 2. MOTOR VELOCIDAD MÍNIMA (LED 2 - PC7)
		    {
		        ID_ACT_MOTOR_MIN,
		        LED_MOTOR_MIN_PORT,
		        LED_MOTOR_MIN_PIN,
		        LED_MOTOR_MIN_ON,
		        LED_MOTOR_MIN_OFF,
				DEL_LED_MIN,
				DEL_LED_MIN
		    },

		    // 3. INDICADOR SISTEMA OK (LED 3 - PB6)
		    {
		        ID_ACT_SYSTEM_OK,
		        LED_SYSTEM_PORT,
		        LED_SYSTEM_PIN,
		        LED_SYSTEM_ON,
		        LED_SYSTEM_OFF,
				DEL_LED_BLI,
		        DEL_LED_PUL
		    },

		    // 4. ALERTA / BARRERA (LED 4 - PA7) -> PARPADEANTE
		    {
		        ID_ACT_ALERT,
		        LED_ALERT_PORT,
		        LED_ALERT_PIN,
		        LED_ALERT_ON,
		        LED_ALERT_OFF,
				DEL_LED_BLI,                    // Blink rápido (250ms ON / 250ms OFF)
				DEL_LED_PUL
		    },

			// 5. BUZZER
		    {
		        ID_ACT_BUZZER,
		        BUZZER_PORT,
		        BUZZER_PIN,
		        BUZZER_ON,
		        BUZZER_OFF,
				DEL_LED_BLI,              // 0 = Sonido Continuo (Sin Blink), 250 = ululando
				DEL_LED_PUL
		    }
};

#define ACTUATOR_CFG_QTY	(sizeof(task_actuator_cfg_list)/sizeof(task_actuator_cfg_t))

task_actuator_dta_t task_actuator_dta_list[ACTUATOR_CFG_QTY];

#define ACTUATOR_DTA_QTY	(sizeof(task_actuator_dta_list)/sizeof(task_actuator_dta_t))

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
const char *p_task_actuator 		= "Task Actuator (Actuator Statechart)";
const char *p_task_actuator_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_actuator_cnt;
volatile uint32_t g_task_actuator_tick_cnt;

/********************** external functions definition ************************/
void task_actuator_init(void *parameters)
{
	uint32_t index;
	const task_actuator_cfg_t *p_task_actuator_cfg;
	task_actuator_dta_t *p_task_actuator_dta;
	task_actuator_st_t state;
	task_actuator_ev_t event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_actuator_init), p_task_actuator);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_actuator), p_task_actuator_);

	g_task_actuator_cnt = G_TASK_ACT_CNT_INIT;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_actuator_cnt), g_task_actuator_cnt);

	for (index = 0; ACTUATOR_DTA_QTY > index; index++)
	{
		/* Update Task Actuator Configuration & Data Pointer */
		p_task_actuator_cfg = &task_actuator_cfg_list[index];
		p_task_actuator_dta = &task_actuator_dta_list[index];

		// --- INICIALIZACIÓN SEGURA ---
		p_task_actuator_dta->state = ST_ACTUATOR_OFF;
		p_task_actuator_dta->event = EV_ACTUATOR_OFF;
		p_task_actuator_dta->flag = false;
		p_task_actuator_dta->tick = 0;

		/* Apagamos físicamente el actuador al inicio por seguridad */
		HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->off_state);


		/* Print out: Index & Task execution FSM */
		LOGGER_LOG("   %s = %lu", GET_NAME(index), index);

		state = p_task_actuator_dta->state;
		LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

		event = p_task_actuator_dta->event;
		LOGGER_LOG("   %s = %lu", GET_NAME(event), (uint32_t)event);

		b_event = p_task_actuator_dta->flag;
		LOGGER_LOG("   %s = %s\r\n", GET_NAME(b_event), (b_event ? "true" : "false"));

	}

	g_task_actuator_tick_cnt = G_TASK_ACT_TICK_CNT_INI;
}

void task_actuator_update(void *parameters)
{
	uint32_t index;
	const task_actuator_cfg_t *p_task_actuator_cfg;
	task_actuator_dta_t *p_task_actuator_dta;
	bool b_time_update_required = false;

	/* Update Task Actuator Counter */
	g_task_actuator_cnt++;

	/* Protect shared resource (g_task_actuator_tick_cnt) */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_ACT_TICK_CNT_INI < g_task_actuator_tick_cnt)
    {
    	g_task_actuator_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_actuator_tick_cnt) */
		__asm("CPSID i");	/* disable interrupts*/
		if (G_TASK_ACT_TICK_CNT_INI < g_task_actuator_tick_cnt)
		{
			g_task_actuator_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	for (index = 0; ACTUATOR_DTA_QTY > index; index++)
		{
    		/* Update Task Actuator Configuration & Data Pointer */
			p_task_actuator_cfg = &task_actuator_cfg_list[index];
			p_task_actuator_dta = &task_actuator_dta_list[index];

			switch (p_task_actuator_dta->state)
						{
							// --- ESTADO: APAGADO ---
							case ST_ACTUATOR_OFF:
								if (true == p_task_actuator_dta->flag)
								{
									p_task_actuator_dta->flag = false; // Consumimos evento

									if (EV_ACTUATOR_ON == p_task_actuator_dta->event) {
										HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->on_state);
										p_task_actuator_dta->state = ST_ACTUATOR_ON;
									}
									else if (EV_ACTUATOR_BLINK == p_task_actuator_dta->event) {
										// Iniciamos parpadeo: Encendemos y cargamos timer
										HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->on_state);
										p_task_actuator_dta->tick = p_task_actuator_cfg->tick_blink;
										p_task_actuator_dta->state = ST_ACTUATOR_BLINK_ON;
									}
								}
								break;

							// --- ESTADO: ENCENDIDO ---
							case ST_ACTUATOR_ON:
								if (true == p_task_actuator_dta->flag)
								{
									p_task_actuator_dta->flag = false;

									if (EV_ACTUATOR_OFF == p_task_actuator_dta->event) {
										HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->off_state);
										p_task_actuator_dta->state = ST_ACTUATOR_OFF;
									}
									// Si estamos ON y nos piden BLINK, pasamos directo
									else if (EV_ACTUATOR_BLINK == p_task_actuator_dta->event) {
										p_task_actuator_dta->tick = p_task_actuator_cfg->tick_blink;
										p_task_actuator_dta->state = ST_ACTUATOR_BLINK_ON;
									}
								}
								break;

							// --- ESTADO: PARPADEO (Fase ENCENDIDO) ---
							case ST_ACTUATOR_BLINK_ON:
								// 1. Chequeo de Eventos (Prioridad)
							    if (true == p_task_actuator_dta->flag)
							    {
							        p_task_actuator_dta->flag = false;

							        // Caso A: Nos mandan APAGAR
							        if (EV_ACTUATOR_OFF == p_task_actuator_dta->event) {
							            HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->off_state);
							            p_task_actuator_dta->state = ST_ACTUATOR_OFF;
							            break;
							        }
							        // Caso B: Nos mandan ENCENDER FIJO (Detener parpadeo y quedar ON)
							        else if (EV_ACTUATOR_ON == p_task_actuator_dta->event) {
							            // Ya estamos encendidos físicamente en esta fase, solo cambiamos estado
							            // (Opcional: forzar escritura por seguridad)
							            HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->on_state);
							            p_task_actuator_dta->state = ST_ACTUATOR_ON;
							            break;
							        }
							    }

							    // 2. Lógica de Tiempo
							    if (p_task_actuator_dta->tick > 0) {
							        p_task_actuator_dta->tick--;
							    } else {
							        // Tiempo cumplido -> Apagar y cambiar fase
							        HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->off_state);
							        p_task_actuator_dta->tick = p_task_actuator_cfg->tick_blink;
							        p_task_actuator_dta->state = ST_ACTUATOR_BLINK_OFF;
							    }
							    break;

							// --- ESTADO: PARPADEO (Fase APAGADO) ---
							case ST_ACTUATOR_BLINK_OFF:
							    // 1. Chequeo de Eventos
							    if (true == p_task_actuator_dta->flag)
							    {
							        p_task_actuator_dta->flag = false;

							        // Caso A: Nos mandan APAGAR (Confirmar apagado y salir)
							        if (EV_ACTUATOR_OFF == p_task_actuator_dta->event) {
							            p_task_actuator_dta->state = ST_ACTUATOR_OFF;
							            break;
							        }
							        // Caso B: Nos mandan ENCENDER FIJO
							        else if (EV_ACTUATOR_ON == p_task_actuator_dta->event) {
							            HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->on_state);
							            p_task_actuator_dta->state = ST_ACTUATOR_ON;
							            break;
							        }
							    }

							    // 2. Lógica de Tiempo
							    if (p_task_actuator_dta->tick > 0) {
							        p_task_actuator_dta->tick--;
							    } else {
							        // Tiempo cumplido -> Encender y cambiar fase
							        HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->on_state);
							        p_task_actuator_dta->tick = p_task_actuator_cfg->tick_blink;
							        p_task_actuator_dta->state = ST_ACTUATOR_BLINK_ON;
							    }
							    break;


							// --- ESTADO: PULSO ---
							case ST_ACTUATOR_PULSE:
								// (Lógica similar para un solo disparo si la necesitas)
								break;

							default:
								break;
						}
					}
			    }
			}

/********************** end of file ******************************************/
