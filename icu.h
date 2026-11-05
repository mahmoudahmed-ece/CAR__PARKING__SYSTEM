/******************************************************************************
 * File: icu.h
 *
 * Purpose:
 *     Public interface of the ICU (Input Capture Unit) driver for the ATmega32.
 *     The ICU uses Timer1: when the selected edge appears on the ICP1 pin
 *     (PD6), the hardware copies the Timer1 value into ICR1 and raises an
 *     interrupt. This lets the application measure pulse widths precisely.
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef ICU_H_
#define ICU_H_

#include "std_types.h"   /* uint16 type and NULL_PTR. */

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

/*
 * Timer1 clock source. The enum values match the CS12:CS10 bits in TCCR1B:
 *   NO_CLOCK = 0 (timer stopped), F_CPU_CLOCK = 1 (no prescaler),
 *   F_CPU_8 = 2, F_CPU_64 = 3, F_CPU_256 = 4, F_CPU_1024 = 5.
 */
typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}ICU_ClockType;

/*
 * Edge that triggers a capture. The value is written to the ICES1 bit:
 *   FALLING = 0 (high-to-low edge), RAISING = 1 (low-to-high edge).
 */
typedef enum
{
	FALLING,RAISING
}ICU_EdgeType;

/* ICU settings passed to ICU_init(). */
typedef struct
{
	ICU_ClockType clock;   /* Timer1 clock (prescaler). */
	ICU_EdgeType edge;     /* First edge to capture. */
}ICU_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Initialize the ICU:
 *   1. Set the required clock.
 *   2. Set the required edge detection.
 *   3. Enable the Input Capture interrupt.
 *   4. Initialize the Timer1 registers.
 * Global interrupts must be enabled by the application (sei / SREG I-bit).
 */
void ICU_init(const ICU_ConfigType * Config_Ptr);

/* Save the address of the application function called on every captured edge. */
void ICU_setCallBack(void(*a_ptr)(void));

/* Change the edge (RAISING or FALLING) that triggers the next capture. */
void ICU_setEdgeDetectionType(const ICU_EdgeType edgeType);

/* Return the Timer1 value captured in ICR1 at the last detected edge. */
uint16 ICU_getInputCaptureValue(void);

/* Reset Timer1 to 0 so the next time measurement starts from zero. */
void ICU_clearTimerValue(void);

/* Stop Timer1, disable the ICU interrupt and remove the callback. */
void ICU_deInit(void);

#endif /* ICU_H_ */
