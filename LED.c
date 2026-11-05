/******************************************************************************
 * File: LED.c
 *
 * Purpose:
 *     Implementation of the RGB LED driver.
 *     The #if blocks select positive-logic or negative-logic code at compile
 *     time according to LED_CONNECTION in LED.h, so only one version is
 *     actually compiled.
 ******************************************************************************/

#include "LED.h"    /* LED pins, LED_ID type and LED_CONNECTION setting. */
#include "gpio.h"   /* GPIO functions used to drive the LED pins. */

void LEDS_init(void){
	//Initializes all Leds (red, green, blue) pins direction.
	/* Step 1: Configure the three LED pins as outputs (PIN_OUTPUT = 1). */
	GPIO_setupPinDirection(GREEN_LED_PORT_ID,GREEN_LED_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(RED_LED_PORT_ID,RED_LED_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(BLUE_LED_PORT_ID,BLUE_LED_PIN_ID,PIN_OUTPUT);
	//Turn off all the Leds
	/* Step 2: Start with every LED off. */
	LED_off(RED);
	LED_off(GREEN);
	LED_off(BLUE);

}

void LED_on(LED_ID id){//Turns on the specified LED.
#if(LED_CONNECTION == POS_LOGIC)
	/* Positive logic: writing HIGH to the pin turns the LED on. */

	/* Pick the pin of the requested LED and drive it HIGH. */
	switch(id){
	case RED:
		 GPIO_writePin(RED_LED_PORT_ID,RED_LED_PIN_ID,LOGIC_HIGH);
		 break;
	case GREEN:
		 GPIO_writePin(GREEN_LED_PORT_ID,GREEN_LED_PIN_ID,LOGIC_HIGH);
		 break;
	case BLUE:
		 GPIO_writePin(BLUE_LED_PORT_ID,BLUE_LED_PIN_ID,LOGIC_HIGH);
		 break;
	}

#elif(LED_CONNECTION == NEG_LOGIC)
	/* Negative logic: writing LOW to the pin turns the LED on. */
	switch(id){
	case RED:
	 	 GPIO_writePin(RED_LED_PORT_ID,RED_LED_PIN_ID,LOGIC_LOW);
	 	 break;
	case GREEN:
	 	 GPIO_writePin(GREEN_LED_PORT_ID,GREEN_LED_PIN_ID,LOGIC_LOW);
	 	 break;
	case BLUE:
	 	 GPIO_writePin(BLUE_LED_PORT_ID,BLUE_LED_PIN_ID,LOGIC_LOW);
	 	 break;
	}
#endif

}
void LED_off(LED_ID id){//Turns off the specified LED.
#if(LED_CONNECTION == POS_LOGIC)
	/* Positive logic: writing LOW to the pin turns the LED off. */

	/* Pick the pin of the requested LED and drive it LOW. */
	switch(id){
	case RED:
	 	 GPIO_writePin(RED_LED_PORT_ID,RED_LED_PIN_ID,LOGIC_LOW);
	 	 break;
	case GREEN:
	 	 GPIO_writePin(GREEN_LED_PORT_ID,GREEN_LED_PIN_ID,LOGIC_LOW);
	 	 break;
	case BLUE:
	 	 GPIO_writePin(BLUE_LED_PORT_ID,BLUE_LED_PIN_ID,LOGIC_LOW);
	 	 break;
	}

#elif(LED_CONNECTION == NEG_LOGIC)
	/* Negative logic: writing HIGH to the pin turns the LED off. */
	 	switch(id){
	 	case RED:
	 			 GPIO_writePin(RED_LED_PORT_ID,RED_LED_PIN_ID,LOGIC_HIGH);
	 			 break;
	 	case GREEN:
	 			 GPIO_writePin(GREEN_LED_PORT_ID,GREEN_LED_PIN_ID,LOGIC_HIGH);
	 			 break;
	 	case BLUE:
	 			 GPIO_writePin(BLUE_LED_PORT_ID,BLUE_LED_PIN_ID,LOGIC_HIGH);
	 			 break;
	 		}
#endif

}
