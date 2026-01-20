/*
 * task_temperature_attribute.h
 *
 *  Created on: Jan 20, 2026
 *      Author: lauta
 */

#ifndef TASK_INC_TASK_TEMPERATURE_ATTRIBUTE_H_
#define TASK_INC_TASK_TEMPERATURE_ATTRIBUTE_H_

#include "main.h"

// Estados de la Máquina de Estados del ADC
typedef enum {
    ST_ADC_IDLE,        // Esperando tiempo de muestreo
    ST_ADC_SELECT_CH,   // Seleccionando canal
    ST_ADC_START,       // Iniciando conversión
    ST_ADC_WAITING,     // Esperando fin de conversión
    ST_ADC_READ         // Leyendo y calculando
} task_temperature_st_t;

// Identificadores de sensores
typedef enum {
    ID_TEMP_LM35,       // Sensor Externo
    ID_TEMP_INTERNAL    // Sensor del Micro
} task_temperature_id_t;

// --- CONFIGURACIÓN (Estática - FLASH) ---
// Similar a task_sensor_cfg_t
typedef struct {
    task_temperature_id_t id;
    ADC_HandleTypeDef* hadc;       // Handle del ADC (ej: &hadc1)
    uint32_t              channel;    // Canal ADC (ej: ADC_CHANNEL_1)
    float                 multiplier; // Factor para convertir ADC a Grados
    float                 offset;     // Offset si fuera necesario
} task_temperature_cfg_t;

// --- DATOS (Dinámica - RAM) ---
// Similar a task_sensor_dta_t
typedef struct {
    task_temperature_st_t state;
    uint32_t              tick;       // Contador para frecuencia de muestreo
    uint32_t              raw_value;  // Valor crudo del ADC (0-4095)
    int32_t               last_temp;  // Última temperatura calculada
} task_temperature_dta_t;

#endif /* TASK_INC_TASK_TEMPERATURE_ATTRIBUTE_H_ */
