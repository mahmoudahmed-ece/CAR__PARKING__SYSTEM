/******************************************************************************
 * File: ultrasonic.h
 *
 * Purpose:
 *     Public interface and configuration of the HC-SR04 ultrasonic sensor
 *     driver. The driver measures the echo pulse width with the ICU driver
 *     and converts it to a distance in centimeters.
 *
 * Hardware connection:
 *     HC-SR04 TRIG -> PD7 (any GPIO pin, configurable below)
 *     HC-SR04 ECHO -> PD6 (ICP1, fixed: this is the Timer1 input-capture pin)
 *
 * How the HC-SR04 works:
 *     1. A HIGH pulse of at least 10 us on TRIG starts a measurement.
 *     2. The sensor sends an ultrasonic burst and raises ECHO.
 *     3. ECHO stays HIGH until the reflected sound returns.
 *     4. Distance = (echo high time * speed of sound) / 2.
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include "std_types.h"   /* uint16 type. */
#include "gpio.h"        /* Port/pin IDs used in the configuration below. */

/*******************************************************************************
 *                          Ultrasonic Configuration                           *
 *******************************************************************************/

/* Trigger pin: can be connected to any port/pin. */
#define ULTRASONIC_TRIGGER_PORT_ID      PORTD_ID
#define ULTRASONIC_TRIGGER_PIN_ID       PIN7_ID

/*
 * Small correction added to the calculated distance (in cm).
 * The integer division always rounds down, so 1 cm is added to match the
 * real distance (and the Proteus HC-SR04 model). Set to 0 if not needed.
 */
#define ULTRASONIC_DISTANCE_OFFSET_CM   1U

/*
 * Maximum time to wait for the echo pulse, in steps of 10 us.
 * 4000 * 10 us = 40 ms, longer than the HC-SR04 maximum echo (about 38 ms).
 */
#define ULTRASONIC_TIMEOUT_STEPS        4000U

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * 1. Initialize the ICU driver (Timer1 clock = F_CPU/8, first edge = rising).
 * 2. Register Ultrasonic_edgeProcessing() as the ICU callback.
 * 3. Configure the trigger pin as an output through the GPIO driver.
 */
void Ultrasonic_init(void);

/* Send the 10 us trigger pulse to the sensor. */
void Ultrasonic_Trigger(void);

/*
 * 1. Send the trigger pulse with Ultrasonic_Trigger().
 * 2. Wait for the ICU to measure the echo pulse (rising and falling edges).
 * 3. Return the measured distance in centimeters.
 *    If no echo is received in time, the last valid distance is returned.
 */
uint16 Ultrasonic_readDistance(void);

/*
 * ICU callback, called on every captured edge:
 *   rising edge  -> start timing the echo pulse,
 *   falling edge -> save the pulse width (high time).
 */
void Ultrasonic_edgeProcessing(void);

#endif /* ULTRASONIC_H_ */
