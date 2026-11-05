/******************************************************************************
 * File: icu.c
 *
 * Purpose:
 *     Implementation of the ICU (Input Capture Unit) driver using Timer1.
 *
 * Registers used:
 *     TCCR1A / TCCR1B -> Timer1 mode, clock and capture-edge selection.
 *     TCNT1           -> Timer1 counter.
 *     ICR1            -> Value of TCNT1 copied by hardware at the captured edge.
 *     TIMSK (TICIE1)  -> Enables the Input Capture interrupt.
 ******************************************************************************/

#include "icu.h"             /* ICU types and prototypes. */
#include "common_macros.h"   /* Bit-manipulation macros. */
#include <avr/io.h>          /* Timer1 / ICU register names. */
#include <avr/interrupt.h>   /* ISR() macro. */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/*
 * Address of the application callback function.
 * The pointer itself is volatile because it is used inside the ISR.
 */
static void (*volatile g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

/*
 * Timer1 Input Capture ISR: runs every time the selected edge is detected
 * on ICP1 (PD6). The hardware has already copied TCNT1 into ICR1.
 */
ISR(TIMER1_CAPT_vect)
{
	/* Call the application callback only if one was registered. */
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void ICU_init(const ICU_ConfigType * Config_Ptr)
{
	/* Step 1: Configure ICP1/PD6 as an input pin. */
	DDRD &= ~(1<<PD6);

	/*
	 * Step 2: Timer1 in Normal mode (WGM11:10 = 00).
	 * FOC1A/FOC1B are set because this is a non-PWM mode.
	 */
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);

	/* Step 3: Write the clock selection into CS12:CS10 (the lowest 3 bits of TCCR1B). */
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->clock);

	/* Step 4: Write the edge selection into ICES1 (bit 6 of TCCR1B). */
	TCCR1B = (TCCR1B & 0xBF) | ((Config_Ptr->edge)<<6);

	/* Step 5: Start the counter and the capture register from 0. */
	TCNT1 = 0;
	ICR1 = 0;

	/* Step 6: Enable the Input Capture interrupt (TICIE1). */
	TIMSK |= (1<<TICIE1);
}

void ICU_setCallBack(void(*a_ptr)(void))
{
	/* Save the callback address; the ISR calls it on every captured edge. */
	g_callBackPtr = a_ptr;
}

void ICU_setEdgeDetectionType(const ICU_EdgeType a_edgeType)
{
	/* Clear ICES1 (bit 6) and write the new edge type into it. */
	TCCR1B = (TCCR1B & 0xBF) | (a_edgeType<<6);
}

uint16 ICU_getInputCaptureValue(void)
{
	/* ICR1 holds the Timer1 value saved by hardware at the last captured edge. */
	return ICR1;
}

void ICU_clearTimerValue(void)
{
	/* Restart Timer1 counting from 0. */
	TCNT1 = 0;
}

void ICU_deInit(void)
{
	/* Step 1: Clear all Timer1/ICU registers (this also stops the timer clock). */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	ICR1 = 0;

	/* Step 2: Disable the Input Capture interrupt. */
	TIMSK &= ~(1<<TICIE1);

	/* Step 3: Remove the callback. */
	g_callBackPtr = NULL_PTR;
}
