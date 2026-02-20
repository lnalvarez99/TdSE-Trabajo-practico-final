#ifndef APP_INC_BOARD_H_
#define APP_INC_BOARD_H_

#include "main.h"

/* ==============================================================================
 * Mapeo de Hardware - TPF Escalera Mecánica
 * ==============================================================================*/

/* --- PULSADORES (Eventos Momentáneos - Active Low) --- */

// BOTON INGRESO DE PERSONA = BOTON S1 DE HC35S
#define BTN_INGRESO_PORT     BTN_INGRESO_GPIO_Port
#define BTN_INGRESO_PIN      BTN_INGRESO_Pin
#define BTN_INGRESO_PRESSED  GPIO_PIN_RESET
#define BTN_INGRESO_REALESED GPIO_PIN_SET

// BOTON ENGRESO DE PERSONA = BOTON S2 DE HC35S
#define BTN_EGRESO_PORT     BTN_EGRESO_GPIO_Port
#define BTN_EGRESO_PIN      BTN_EGRESO_Pin
#define BTN_EGRESO_PRESSED  GPIO_PIN_RESET
#define BTN_EGRESO_REALESED GPIO_PIN_SET

// BOTON MODO = BOTON S3 DE HC35S
#define BTN_MODE_PORT       BTN_MODE_GPIO_Port
#define BTN_MODE_PIN        BTN_MODE_Pin
#define BTN_MODE_PRESSED    GPIO_PIN_RESET
#define BTN_MODE_REALESED   GPIO_PIN_SET

// BOTON ENTER = BOTON S4 DE HC35S
#define BTN_ENTER_PORT     BTN_ENTER_GPIO_Port
#define BTN_ENTER_PIN      BTN_ENTER_Pin
#define BTN_ENTER_PRESSED  GPIO_PIN_RESET
#define BTN_ENTER_REALESED GPIO_PIN_SET

// BOTON ACTIVAR SISTEMA = BOTON K14 DE HC35S
#define BTN_ACTIVE_PORT     BTN_ACTIVE_GPIO_Port
#define BTN_ACTIVE_PIN      BTN_ACTIVE_Pin
#define BTN_ACTIVE_PRESSED  GPIO_PIN_RESET
#define BTN_ACTIVE_REALESED GPIO_PIN_SET

/* --- DIP SWITCHES (Estados Fijos - Active Low) --- */
// Usan Pull-Up: 0 = Switch ON (Cerrado a GND), 1 = Switch OFF (Abierto)

#define SW_BARRERA_PORT     SW_BARRERA_GPIO_Port
#define SW_BARRERA_PIN      SW_BARRERA_Pin
#define SW_BARRERA_ON       GPIO_PIN_RESET
#define SW_BARRERA_OFF      GPIO_PIN_SET


#define SW_DESACTIVAR_PORT   SW_DESACTIVAR_GPIO_Port
#define SW_DESACTIVAR_PIN    SW_DESACTIVAR_Pin
#define SW_DESACTIVAR_ON     GPIO_PIN_RESET
#define SW_DESACTIVAR_OFF    GPIO_PIN_SET

/* --- SENSORES DE TEMPERATURA --- */
#define ADC_LM35_CHANNEL       ADC_CHANNEL_6
#define ADC_INTERNAL_CHANNEL   ADC_CHANNEL_TEMPSENSOR

/* --- DISPLAY LCD 16x2  --- */

#define LCD_RS_PORT     GPIOB
#define LCD_RS_PIN      GPIO_PIN_12

#define LCD_EN_PORT     GPIOB
#define LCD_EN_PIN      GPIO_PIN_13

#define LCD_D4_PORT     GPIOB
#define LCD_D4_PIN      GPIO_PIN_14

#define LCD_D5_PORT     GPIOB
#define LCD_D5_PIN      GPIO_PIN_15

#define LCD_D6_PORT     GPIOC
#define LCD_D6_PIN      GPIO_PIN_6

#define LCD_D7_PORT     GPIOC
#define LCD_D7_PIN      GPIO_PIN_8

/* --- SALIDAS (Actuadores / LEDs - Active High) --- */

#define LED_MOTOR_MAX_PORT   LED_MOTOR_MAX_GPIO_Port
#define LED_MOTOR_MAX_PIN    LED_MOTOR_MAX_Pin
#define LED_MOTOR_MAX_ON     GPIO_PIN_RESET
#define LED_MOTOR_MAX_OFF    GPIO_PIN_SET

#define LED_MOTOR_MIN_PORT   LED_MOTOR_MIN_GPIO_Port
#define LED_MOTOR_MIN_PIN    LED_MOTOR_MIN_Pin
#define LED_MOTOR_MIN_ON     GPIO_PIN_RESET
#define LED_MOTOR_MIN_OFF    GPIO_PIN_SET

#define LED_SYSTEM_PORT      LED_SYSTEM_GPIO_Port
#define LED_SYSTEM_PIN       LED_SYSTEM_Pin
#define LED_SYSTEM_ON        GPIO_PIN_RESET
#define LED_SYSTEM_OFF       GPIO_PIN_SET

#define LED_ALERT_PORT       LED_ALERT_GPIO_Port
#define LED_ALERT_PIN        LED_ALERT_Pin
#define LED_ALERT_ON         GPIO_PIN_RESET
#define LED_ALERT_OFF        GPIO_PIN_SET

#define BUZZER_PORT 		BUZZER_GPIO_Port
#define BUZZER_PIN          BUZZER_Pin
#define BUZZER_ON			GPIO_PIN_RESET
#define BUZZER_OFF			GPIO_PIN_SET

#endif /* APP_INC_BOARD_H_ */
