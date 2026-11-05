/******************************************************************************
 * File: CarParking.c
 *
 * Purpose:
 *     Main application of the ATmega32 Car Parking Sensor.
 *     Measures the distance to the nearest obstacle with an HC-SR04
 *     ultrasonic sensor, shows it on the LCD, and warns the driver with
 *     three LEDs and a buzzer as the obstacle gets closer.
 *
 * Distance -> warning mapping:
 *     <= 5 cm    : all LEDs flashing, buzzer ON, LCD shows "Stop"
 *     6 .. 10 cm : Red + Green + Blue ON, buzzer OFF
 *     11 .. 15 cm: Red + Green ON, Blue OFF
 *     16 .. 20 cm: Red ON only
 *     > 20 cm    : all LEDs OFF, buzzer OFF
 *
 * Configuration assumptions:
 *     - CPU clock F_CPU = 16 MHz.
 *     - LCD in 4-bit mode: RS = PA1, E = PA2, D4..D7 = PA3..PA6.
 *     - LEDs: Red = PC0, Green = PC1, Blue = PC2.
 *     - Buzzer transistor: PC5.
 *     - Ultrasonic: ECHO = PD6 (ICP1), TRIG = PD7.
 ******************************************************************************/

#include <avr/io.h>          /* SREG and MCUCSR registers. */
#include <util/delay.h>      /* _delay_ms() (uses F_CPU). */

#include "std_types.h"       /* Project data types. */
#include "lcd.h"             /* LCD driver. */
#include "LED.h"             /* RGB LED driver. */
#include "buzzer.h"          /* Buzzer driver. */
#include "ultrasonic.h"      /* HC-SR04 ultrasonic driver. */

/*******************************************************************************
 *                              Application Settings                           *
 *******************************************************************************/

/* Distance limits (cm) for each warning level. */
#define DISTANCE_STOP_CM        5U
#define DISTANCE_LEVEL1_CM      10U
#define DISTANCE_LEVEL2_CM      15U
#define DISTANCE_LEVEL3_CM      20U

/*
 * Delay between two measurements (ms). It also sets the LED flashing speed
 * in the Stop zone, and gives the HC-SR04 time between measurements
 * (the datasheet recommends at least 60 ms).
 */
#define MEASUREMENT_PERIOD_MS   100

/*******************************************************************************
 *                           Private Function Prototypes                       *
 *******************************************************************************/

static void Disable_JTAG(void);
static void Display_Distance(uint16 distance);
static void Update_Warnings(uint16 distance);
static void Set_LEDs(uint8 red, uint8 green, uint8 blue);

/*******************************************************************************
 *                              Main Function                                  *
 *******************************************************************************/

int main(void)
{
	uint16 distance = 0;   /* Latest measured distance in cm. */

	/*
	 * Step 1: Free PC2..PC5 for normal I/O. On the ATmega32 these pins are
	 * shared with the JTAG interface, which is enabled by default and would
	 * block the Blue LED (PC2) and the buzzer (PC5).
	 */
	Disable_JTAG();

	/*
	 * Step 2: Enable global interrupts (I-bit in SREG).
	 * The ultrasonic driver depends on the ICU interrupt.
	 */
	SREG |= (1 << 7);

	/* Step 3: Initialize all drivers. */
	LCD_init();
	LEDS_init();
	Buzzer_init();
	Ultrasonic_init();

	/* Step 4: Show a start-up message for 1 second. */
	LCD_displayStringRowColumn(0, 0, "Parking Sensor");
	LCD_displayStringRowColumn(1, 0, "System Ready");
	_delay_ms(1000);
	LCD_clearScreen();

	/* Step 5: Fixed label on row 0; only the number is updated in the loop. */
	LCD_displayStringRowColumn(0, 0, "Distance=");

	/* Step 6: Super loop: measure -> display -> warn, forever. */
	while (1)
	{
		/* Step 6.1: Measure the distance to the obstacle. */
		distance = Ultrasonic_readDistance();

		/* Step 6.2: Show the distance on the LCD. */
		Display_Distance(distance);

		/* Step 6.3: Update the LEDs and the buzzer for this distance. */
		Update_Warnings(distance);

		/* Step 6.4: Wait before the next measurement. */
		_delay_ms(MEASUREMENT_PERIOD_MS);
	}

	return 0; /* The super loop never exits. */
}

/*******************************************************************************
 *                           Private Function Definitions                      *
 *******************************************************************************/

/*
 * Disable the JTAG interface so PC2..PC5 work as normal GPIO pins.
 * The JTD bit must be written twice within 4 clock cycles; two direct
 * assignments compile to two consecutive "out" instructions.
 */
static void Disable_JTAG(void)
{
	MCUCSR = (1 << JTD);
	MCUCSR = (1 << JTD);
}

/*
 * Show the distance on row 0 and the "Stop" warning on row 1.
 * Row 0 layout: "Distance=" (columns 0..8), then the number and " cm".
 */
static void Display_Distance(uint16 distance)
{
	/* Step 1: Print the number right after "Distance=". */
	LCD_moveCursor(0, 9);
	LCD_intgerToString(distance);

	/*
	 * Step 2: Print the unit, then pad with spaces so old digits from a
	 * longer previous number are overwritten (e.g. 100 -> 99).
	 */
	LCD_displayString(" cm");
	if (distance < 100)
	{
		LCD_displayCharacter(' ');
	}
	if (distance < 10)
	{
		LCD_displayCharacter(' ');
	}

	/* Step 3: Row 1 shows "Stop" only when the obstacle is too close. */
	if (distance <= DISTANCE_STOP_CM)
	{
		LCD_displayStringRowColumn(1, 6, "Stop");
	}
	else
	{
		LCD_displayStringRowColumn(1, 6, "    ");
	}
}

/*
 * Turn each LED on (LOGIC_HIGH) or off (LOGIC_LOW) in one call.
 */
static void Set_LEDs(uint8 red, uint8 green, uint8 blue)
{
	if (red == LOGIC_HIGH)   { LED_on(RED); }   else { LED_off(RED); }
	if (green == LOGIC_HIGH) { LED_on(GREEN); } else { LED_off(GREEN); }
	if (blue == LOGIC_HIGH)  { LED_on(BLUE); }  else { LED_off(BLUE); }
}

/*
 * Apply the LED and buzzer pattern that matches the distance.
 */
static void Update_Warnings(uint16 distance)
{
	/*
	 * Flashing state for the Stop zone. "static" keeps the value between
	 * calls, so the LEDs toggle every loop (about every 100 ms).
	 */
	static uint8 flash_state = LOGIC_LOW;

	if (distance <= DISTANCE_STOP_CM)
	{
		/* <= 5 cm: danger. Toggle all LEDs to make them flash and sound the buzzer. */
		flash_state = (flash_state == LOGIC_HIGH) ? LOGIC_LOW : LOGIC_HIGH;
		Set_LEDs(flash_state, flash_state, flash_state);
		Buzzer_on();
	}
	else
	{
		/* Outside the Stop zone: buzzer off and the flashing restarts from OFF next time. */
		Buzzer_off();
		flash_state = LOGIC_LOW;

		if (distance <= DISTANCE_LEVEL1_CM)
		{
			/* 6..10 cm: all LEDs ON. */
			Set_LEDs(LOGIC_HIGH, LOGIC_HIGH, LOGIC_HIGH);
		}
		else if (distance <= DISTANCE_LEVEL2_CM)
		{
			/* 11..15 cm: Red and Green ON, Blue OFF. */
			Set_LEDs(LOGIC_HIGH, LOGIC_HIGH, LOGIC_LOW);
		}
		else if (distance <= DISTANCE_LEVEL3_CM)
		{
			/* 16..20 cm: Red ON only. */
			Set_LEDs(LOGIC_HIGH, LOGIC_LOW, LOGIC_LOW);
		}
		else
		{
			/* > 20 cm: safe. All LEDs OFF. */
			Set_LEDs(LOGIC_LOW, LOGIC_LOW, LOGIC_LOW);
		}
	}
}
