/******************************************************************************
 * File: buzzer.h
 *
 * Purpose:
 *     Public interface and pin configuration of the buzzer driver.
 *
 * Hardware connection:
 *     PC5 -> base of the NPN transistor (through a resistor) that switches
 *            the buzzer. Pin HIGH = buzzer ON, pin LOW = buzzer OFF.
 *
 * The buzzer can be moved to any port/pin by changing the macros below.
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"   /* Project data types. */
#include "gpio.h"        /* Port/pin IDs used in the configuration below. */

/*******************************************************************************
 *                              Buzzer Configuration                           *
 *******************************************************************************/

/* Port and pin that drive the buzzer transistor. */
#define BUZZER_PORT_ID      PORTC_ID
#define BUZZER_PIN_ID       PIN5_ID

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/* Configure the buzzer pin as an output and start with the buzzer off. */
void Buzzer_init(void);

/* Turn the buzzer on (pin HIGH). */
void Buzzer_on(void);

/* Turn the buzzer off (pin LOW). */
void Buzzer_off(void);

#endif /* BUZZER_H_ */
