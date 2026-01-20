/*
 * task_temperature.c
 *
 *  Created on: Jan 20, 2026
 *      Author: lauta
 */


#include "main.h"
#include "task_temperature.h"
#include "task_temperature_attribute.h"
#include "task_display_interface.h" // Para enviar datos al display

// Nota: Ajustar los multiplicadores según voltaje (3.3V) y resolución (12 bits)
// LM35: 10mV/°C. ADC = (Volts * 4095) / 3.3.
// Factor aprox: (3300 mV / 4095 steps) / 10 mV/°C = 0.0805 °C/step

/********************** macros and definitions *******************************/
#define G_TASK_TEMP_CNT_INI			0ul
#define G_TASK_TEMP_TICK_CNT_INI	0ul

/********************** external data declaration ****************************/
extern ADC_HandleTypeDef hadc1; // Referencia al ADC configurado en main.c

/********************** internal data declaration ****************************/


const task_temperature_cfg_t task_temp_cfg_list[] = {
    // 1. LM35 (Externo)
    {
        ID_TEMP_LM35,
        &hadc1,            // Asumiendo que usas ADC1
        ADC_LM35_CHANNEL,
        0.0805f,           // Multiplicador calculado
        0.0f               // Sin offset
    },
    // 2. Sensor Interno STM32
    {
        ID_TEMP_INTERNAL,
        &hadc1,
        ADC_INTERNAL_CHANNEL,
        0.0f,              // El interno usa una fórmula especial, no lineal simple
        0.0f
    }
};

#define TEMP_SENSOR_QTY (sizeof(task_temp_cfg_list)/sizeof(task_temperature_cfg_t))

// Datos en RAM
task_temperature_dta_t task_temp_dta_list[TEMP_SENSOR_QTY];

/********************** external data definition *****************************/
uint32_t g_task_temp_cnt;
volatile uint32_t g_task_temp_tick_cnt;

/********************** external functions definition ************************/

void task_temperature_init(void *parameters)
{
	// Inicialización de contadores globales
    g_task_temp_cnt = G_TASK_TEMP_CNT_INI;
    g_task_temp_tick_cnt = G_TASK_TEMP_TICK_CNT_INI;

    // Inicialización similar a task_sensor_init
    for (int i = 0; i < TEMP_SENSOR_QTY; i++) {
        task_temp_dta_list[i].state = ST_ADC_IDLE;
        task_temp_dta_list[i].tick = 1000; // Primer muestreo al 1s
        task_temp_dta_list[i].last_temp = 0;
    }
}

void task_temperature_update(void *parameters)
{

		bool b_time_update_required = false;

		/* Update Task Sensor Counter */
		g_task_temp_cnt++;

		/* Protect shared resource (g_task_sensor_tick_cnt) */
		__asm("CPSID i");	/* disable interrupts*/
	    if (G_TASK_TEMP_TICK_CNT_INI < g_task_temp_tick_cnt)
	    {
	    	g_task_temp_tick_cnt--;
	    	b_time_update_required = true;
	    }
	    __asm("CPSIE i");	/* enable interrupts*/

	    while (b_time_update_required)
	    {
			/* Protect shared resource (g_task_sensor_tick_cnt) */
			__asm("CPSID i");	/* disable interrupts*/
			if (G_TASK_SEN_TICK_CNT_INI < g_task_sensor_tick_cnt)
			{
				g_task_temp_tick_cnt--;
				b_time_update_required = true;
			}
			else
			{
				b_time_update_required = false;
			}
			__asm("CPSIE i");	/* enable interrupts*/

    // Iteramos por cada sensor
			for (int i = 0; i < TEMP_SENSOR_QTY; i++)
			{
				task_temperature_dta_t *p_dta = &task_temp_dta_list[i];
				const task_temperature_cfg_t *p_cfg = &task_temp_cfg_list[i];

				switch (p_dta->state)
				{
					case ST_ADC_IDLE:
						if (p_dta->tick > 0) {
							p_dta->tick--;
						} else {
							p_dta->state = ST_ADC_SELECT_CH;
						}
						break;

					case ST_ADC_SELECT_CH:
						// Configurar Canal ADC (requiere función HAL)
						ADC_ChannelConfTypeDef sConfig = {0};
						sConfig.Channel = p_cfg->channel;
						sConfig.Rank = ADC_REGULAR_RANK_1;
						sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5; // Tiempo estable
						HAL_ADC_ConfigChannel(p_cfg->hadc, &sConfig);

						p_dta->state = ST_ADC_START;
						break;

					case ST_ADC_START:
						HAL_ADC_Start(p_cfg->hadc);
						p_dta->state = ST_ADC_WAITING;
						break;

					case ST_ADC_WAITING:
						if (HAL_ADC_PollForConversion(p_cfg->hadc, 1) == HAL_OK) {
							p_dta->raw_value = HAL_ADC_GetValue(p_cfg->hadc);
							p_dta->state = ST_ADC_READ;
						}
						break;

					case ST_ADC_READ:
						// Conversión Matemática
						if (p_cfg->id == ID_TEMP_INTERNAL) {
							// Fórmula específica del datasheet STM32F1 para sensor interno
							// Temp = (V25 - Vsense) / Avg_Slope + 25
							// (Simplificado aquí, requiere valores de calibración)
							float vsense = (p_dta->raw_value * 3.3f) / 4095.0f;
							p_dta->last_temp = (int32_t)((1.43f - vsense) / 0.0043f + 25.0f);
						} else {
							// LM35 Lineal
							p_dta->last_temp = (int32_t)(p_dta->raw_value * p_cfg->multiplier + p_cfg->offset);
						}

						// Enviar al Display (Solo cuando tenemos ambos listos o individualmente)
						// Aquí asumimos que actualizamos las variables globales del display
						if (p_cfg->id == ID_TEMP_LM35)
							Display_UpdateTemps(task_temp_dta_list[1].last_temp, p_dta->last_temp);

						// Reiniciar ciclo
						p_dta->tick = 500; // Muestrear cada 500ms
						p_dta->state = ST_ADC_IDLE;
						break;
				}
			}
    }
}
