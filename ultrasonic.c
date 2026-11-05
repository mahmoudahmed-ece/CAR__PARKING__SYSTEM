/******************************************************************************
 * File: ultrasonic.c
 *
 * Purpose:
 *     Implementation of the HC-SR04 ultrasonic sensor driver.
 *
 * Distance calculation:
 *     Timer1 runs at F_CPU / 8 (2 MHz at 16 MHz, so 1 tick = 0.5 us).
 *     Echo time (s)  = ticks / (F_CPU / 8)
 *     Distance (cm)  = Echo time * 34300 / 2 = ticks * 17150 / (F_CPU / 8)
 *     At 16 MHz this is about ticks / 117.
 ******************************************************************************/

#include "ultrasonic.h"      /* Driver configuration and prototypes. */
#include "gpio.h"            /* GPIO functions for the trigger pin. */
#include "icu.h"             /* ICU driver used to time the echo pulse. */
#include <util/delay.h>      /* _delay_us() (uses F_CPU). */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/*
 * Variables shared between the ICU callback (interrupt context) and normal
 * code are volatile, so the compiler always reads their latest value.
 */
static volatile uint8  g_edgesCounter = 0;   /* Edges captured in the current measurement (0, 1 or 2). */
static volatile uint16 g_highTime = 0;       /* Echo pulse width in Timer1 ticks. */
static uint16 g_distance = 0;                /* Last valid distance in cm. */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Ultrasonic_init(void)
{
	/*
	 * Step 1: Initialize the ICU:
	 *   clock = F_CPU/8 -> fine resolution (0.5 us at 16 MHz) and a range
	 *           of 65535 ticks = 32.7 ms, enough for the HC-SR04 echo.
	 *   edge  = RAISING -> the first event is the start of the echo pulse.
	 */
	ICU_ConfigType icu_config = {F_CPU_8, RAISING};
	ICU_init(&icu_config);

	/* Step 2: Register the edge-processing function as the ICU callback. */
	ICU_setCallBack(Ultrasonic_edgeProcessing);

	/* Step 3: Configure the trigger pin as an output and keep it LOW. */
	GPIO_setupPinDirection(ULTRASONIC_TRIGGER_PORT_ID, ULTRASONIC_TRIGGER_PIN_ID, PIN_OUTPUT);
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT_ID, ULTRASONIC_TRIGGER_PIN_ID, LOGIC_LOW);
}

void Ultrasonic_Trigger(void)
{
	/* Step 1: Raise TRIG to start a measurement. */
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT_ID, ULTRASONIC_TRIGGER_PIN_ID, LOGIC_HIGH);

	/* Step 2: Keep TRIG HIGH for 10 us (the HC-SR04 minimum trigger width). */
	_delay_us(10);

	/* Step 3: Bring TRIG back LOW; the sensor now sends the ultrasonic burst. */
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT_ID, ULTRASONIC_TRIGGER_PIN_ID, LOGIC_LOW);
}

uint16 Ultrasonic_readDistance(void)
{
	uint16 timeout = ULTRASONIC_TIMEOUT_STEPS;   /* Remaining wait steps (10 us each). */

	/* Step 1: Prepare a new measurement: no edges yet, wait for a rising edge. */
	g_edgesCounter = 0;
	ICU_setEdgeDetectionType(RAISING);

	/* Step 2: Send the trigger pulse. */
	Ultrasonic_Trigger();

	/* Step 3: Wait until both edges of the echo are captured, or until the timeout. */
	while ((g_edgesCounter < 2) && (timeout > 0))
	{
		_delay_us(10);
		timeout--;
	}

	/*
	 * Step 4: If the full pulse was measured, convert ticks to centimeters.
	 * The multiplication is done in 32 bits to avoid overflow
	 * (65535 * 17150 fits in uint32).
	 */
	if (g_edgesCounter >= 2)
	{
		g_distance = (uint16)(((uint32)g_highTime * 17150UL) / (F_CPU / 8UL))
		             + ULTRASONIC_DISTANCE_OFFSET_CM;
	}

	/* Step 5: Return the new distance (or the last valid one after a timeout). */
	return g_distance;
}

void Ultrasonic_edgeProcessing(void)
{
	/* Count the edge that has just been captured. */
	g_edgesCounter++;

	if (g_edgesCounter == 1)
	{
		/*
		 * Rising edge = start of the echo pulse:
		 * reset Timer1 so it counts the high time from zero,
		 * then wait for the falling edge.
		 */
		ICU_clearTimerValue();
		ICU_setEdgeDetectionType(FALLING);
	}
	else if (g_edgesCounter == 2)
	{
		/*
		 * Falling edge = end of the echo pulse:
		 * ICR1 now holds the number of ticks the echo stayed HIGH.
		 */
		g_highTime = ICU_getInputCaptureValue();

		/* Get ready for the next measurement. */
		ICU_clearTimerValue();
		ICU_setEdgeDetectionType(RAISING);
	}
}
