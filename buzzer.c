/******************************************************************************
 * File: buzzer.c
 *
 * Purpose:
 *     Implementation of the buzzer driver.
 *     The MCU pin drives a transistor, and the transistor switches the
 *     buzzer current, so the pin only needs to go HIGH (on) or LOW (off).
 ******************************************************************************/

#include "buzzer.h"   /* Buzzer port/pin configuration and prototypes. */
#include "gpio.h"     /* GPIO functions used to drive the buzzer pin. */

/*
 * Initialize the buzzer and leave it off.
 */
void Buzzer_init(void)
{
    /* Step 1: Set the buzzer pin as an output so the MCU can drive it. */
    GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PIN_ID, PIN_OUTPUT);

    /* Step 2: Drive the pin LOW so the transistor is off and the buzzer is silent. */
    GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}

/*
 * Turn the buzzer on.
 */
void Buzzer_on(void)
{
    /* Pin HIGH turns the transistor on, so current flows through the buzzer. */
    GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_HIGH);
}

/*
 * Turn the buzzer off.
 */
void Buzzer_off(void)
{
    /* Pin LOW turns the transistor off, so the buzzer is silent. */
    GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}
