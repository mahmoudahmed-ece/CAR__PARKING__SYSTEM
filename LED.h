/******************************************************************************
 * File: LED.h
 *
 * Purpose:
 *     Public interface and pin configuration of the RGB LED driver.
 *
 * Hardware connection:
 *     RED   LED -> PC0
 *     GREEN LED -> PC1
 *     BLUE  LED -> PC2
 *
 * The LEDs can be moved to any port/pin by changing the macros below.
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef LED_H_
#define LED_H_

#include "std_types.h"   /* Project data types. */
#include "gpio.h"        /* Port/pin IDs used in the configuration below. */

/*******************************************************************************
 *                              LED Configuration                              *
 *******************************************************************************/

/* Port of each LED (all three are on PORTC). */
#define RED_LED_PORT_ID     PORTC_ID
#define GREEN_LED_PORT_ID   PORTC_ID
#define BLUE_LED_PORT_ID    PORTC_ID

/* Pin of each LED inside the port. */
#define RED_LED_PIN_ID      PIN0_ID
#define GREEN_LED_PIN_ID    PIN1_ID
#define BLUE_LED_PIN_ID     PIN2_ID

/*
 * How the LEDs are wired:
 * POS_LOGIC -> LED turns ON when the pin is HIGH (anode on the MCU pin).
 * NEG_LOGIC -> LED turns ON when the pin is LOW  (cathode on the MCU pin).
 * LED.c uses this value to pick the correct code at compile time.
 */
#define LED_CONNECTION      POS_LOGIC

/*******************************************************************************
 *                              Types Declaration                              *
 *******************************************************************************/

/* Wiring logic values used by LED_CONNECTION. */
typedef enum{NEG_LOGIC=0,POS_LOGIC=1}led_logic;

/* LED identifiers passed to LED_on() and LED_off(). */
typedef enum{RED=0,GREEN=1,BLUE=2}LED_ID;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/* Configure the three LED pins as outputs and turn all LEDs off. */
void LEDS_init(void);

/* Turn on the selected LED. */
void LED_on(LED_ID id);

/* Turn off the selected LED. */
void LED_off(LED_ID id);

#endif /* LED_H_ */
