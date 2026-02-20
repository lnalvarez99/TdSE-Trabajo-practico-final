/*
 * task_display.c
 *
 *  Created on: Jan 17, 2026
 *      Author: Lautaro Alvarez
 */


#include "main.h"
#include "task_display_attribute.h"
#include "task_display_interface.h"
#include "display.h"
#include <stdio.h>

// Instancia de los datos
task_display_dta_t task_display_dta;

// Buffer temporal para formatear texto (17 chars + null)
// Nota: Aunque el display es de 16x2, la solucion para la problematica
// de impresion de basura en el display fue aumentar el buffer
static char line_buffer[18];

void task_display_init(void *parameters)
{
    // 1. Inicializar Hardware del Display
    displayInit(DISPLAY_CONNECTION_GPIO_4BITS);

    // 2. Inicializar Datos
    task_display_dta.state = ST_DSP_INIT;
    task_display_dta.event = EV_DSP_REFRESH_REQ;
    task_display_dta.flag = true;

    // Valores por defecto seguros
    sprintf(task_display_dta.system_state_str, "INIT");
    task_display_dta.people_count = 0;
    task_display_dta.temp_internal = 0;
    task_display_dta.temp_ambient = 0;
}

void task_display_update(void *parameters)
{
    // Solo actualizamos si hay una petición de refresco
    if (task_display_dta.flag == true)
    {
    	HAL_Delay(10);
        task_display_dta.flag = false;
        switch (task_display_dta.state)
        {
            // --- PANTALLA DE INICIO ---
            case ST_DSP_INIT:
                displayCharPositionWrite(0, 0);
                displayStringWrite("TdSE Grupo 09");
                displayCharPositionWrite(0, 1);
                displayStringWrite("Iniciando...");
                HAL_Delay(2000);
                task_display_dta.state = ST_DSP_MAIN_STATUS;
                task_display_dta.flag = true;
                break;

            // --- VISTA PRINCIPAL (Estado + Personas + Temps) ---
            // Fila 0: "ST:IDLE   P:00"
            // Fila 1: "Ti:35C   Ta:24C"
            // ----------------------------------------------------

            case ST_DSP_MAIN_STATUS:
            	// Fila 0 (Estado y personas)
            	snprintf(line_buffer, sizeof(line_buffer), "ST:%-4s P:%02lu    ",
            	         task_display_dta.system_state_str,
            	         task_display_dta.people_count);
            	displayCharPositionWrite(0, 0);
            	displayStringWrite(line_buffer);

                // Fila 1 (Temperaturas)
            	snprintf(line_buffer, sizeof(line_buffer), "Ti:%02ldC  Ta:%02ldC  ",
            	         task_display_dta.temp_internal,
            	         task_display_dta.temp_ambient);
            	displayCharPositionWrite(0, 1);
            	displayStringWrite(line_buffer);
                break;

            // --- MENÚ SETUP: TIEMPO ---
            // Fila 0: "CFG TIEMPO ESP:"
            // Fila 1: ">> 30 Segundos"
            case ST_DSP_SETUP_TIMEOUT:
                displayCharPositionWrite(0, 0);
                displayStringWrite(" CFG TIEMPO ESP: ");

                snprintf(line_buffer, 18, ">>> %02lu Segundos  ", task_display_dta.cfg_timeout / 1000);
                displayCharPositionWrite(0, 1);
                displayStringWrite(line_buffer);
                break;

            // --- MENÚ SETUP: LÍMITE PERSONAS ---
            // Fila 0: "CFG UMBRAL VEL:"
            // Fila 1: ">> 02 Personas"
            case ST_DSP_SETUP_THRESHOLD:
                displayCharPositionWrite(0, 0);
                displayStringWrite(" CFG UMBRAL VEL: ");

                snprintf(line_buffer, 18, ">>> %02lu Personas  ", task_display_dta.cfg_limit);
                displayCharPositionWrite(0, 1);
                displayStringWrite(line_buffer);
                break;

            // --- ALERTA / EMERGENCIA ---
            case ST_DSP_ALERT:
                displayCharPositionWrite(0, 0);
                displayStringWrite("  ! ATENCION !  ");
                displayCharPositionWrite(0, 1);
                displayStringWrite("   SYSTEM OFF   ");
                break;

            default:
                break;
        }
    }
}
