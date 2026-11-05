/******************************************************************************
 * File: gpio.h
 *
 * Purpose:
 *     Public interface of the GPIO driver for the ATmega32.
 *     Lets the rest of the project configure and use the I/O pins of
 *     PORTA..PORTD by port/pin ID, without touching DDRx/PORTx/PINx directly.
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef GPIO_H_
#define GPIO_H_

#include "std_types.h"   /* uint8 and the LOGIC_HIGH / LOGIC_LOW values. */

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* The ATmega32 has 4 I/O ports (A, B, C, D), each with 8 pins. */
#define NUM_OF_PORTS           4
#define NUM_OF_PINS_PER_PORT   8

/* Port IDs passed to the GPIO functions. */
#define PORTA_ID               0
#define PORTB_ID               1
#define PORTC_ID               2
#define PORTD_ID               3

/* Pin IDs inside a port (bit number in the port registers). */
#define PIN0_ID                0
#define PIN1_ID                1
#define PIN2_ID                2
#define PIN3_ID                3
#define PIN4_ID                4
#define PIN5_ID                5
#define PIN6_ID                6
#define PIN7_ID                7

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

/* Direction of a single pin: PIN_INPUT = 0, PIN_OUTPUT = 1. */
typedef enum
{
	PIN_INPUT,PIN_OUTPUT
}GPIO_PinDirectionType;

/*
 * Direction of a whole port: PORT_INPUT = 0x00 (all 8 pins input),
 * PORT_OUTPUT = 0xFF (all 8 pins output). The value is written directly
 * to the DDRx register.
 */
typedef enum
{
	PORT_INPUT,PORT_OUTPUT=0xFF
}GPIO_PortDirectionType;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Set one pin as input or output.
 * Invalid port or pin numbers are ignored.
 */
void GPIO_setupPinDirection(uint8 port_num, uint8 pin_num, GPIO_PinDirectionType direction);

/*
 * Write LOGIC_HIGH or LOGIC_LOW to one pin.
 * Output pin -> drives the pin high or low.
 * Input pin  -> enables (HIGH) or disables (LOW) the internal pull-up resistor.
 * Invalid port or pin numbers are ignored.
 */
void GPIO_writePin(uint8 port_num, uint8 pin_num, uint8 value);

/*
 * Read one pin and return LOGIC_HIGH or LOGIC_LOW.
 * Invalid port or pin numbers return LOGIC_LOW.
 */
uint8 GPIO_readPin(uint8 port_num, uint8 pin_num);

/*
 * Set all 8 pins of a port as input (PORT_INPUT) or output (PORT_OUTPUT).
 * Invalid port numbers are ignored.
 */
void GPIO_setupPortDirection(uint8 port_num, GPIO_PortDirectionType direction);

/*
 * Write a full byte to a port.
 * Output pins take the matching bit value; input pins get their pull-up
 * resistor enabled or disabled by the matching bit.
 * Invalid port numbers are ignored.
 */
void GPIO_writePort(uint8 port_num, uint8 value);

/*
 * Read all 8 pins of a port and return them as one byte.
 * Invalid port numbers return 0.
 */
uint8 GPIO_readPort(uint8 port_num);

#endif /* GPIO_H_ */
