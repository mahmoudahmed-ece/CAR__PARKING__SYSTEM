/******************************************************************************
 * File: gpio.c
 *
 * Purpose:
 *     Implementation of the GPIO driver for the ATmega32.
 *
 * ATmega32 registers used (x = A, B, C or D):
 *     DDRx  -> Data Direction Register: bit = 1 output, bit = 0 input.
 *     PORTx -> Output value for output pins, pull-up enable for input pins.
 *     PINx  -> Read-only register holding the actual level on each pin.
 ******************************************************************************/

#include "gpio.h"            /* Port/pin IDs and the function prototypes. */
#include "common_macros.h"   /* SET_BIT and CLEAR_BIT macros. */
#include <avr/io.h>          /* Register names: DDRx, PORTx, PINx. */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Configure the direction of one pin.
 * port_num: PORTA_ID, PORTB_ID, PORTC_ID, or PORTD_ID
 * pin_num : PIN0_ID to PIN7_ID
 */
void GPIO_setupPinDirection(uint8 port_num, uint8 pin_num,
                            GPIO_PinDirectionType direction)
{
    /* Step 1: Ignore the request if the port or pin number is out of range. */
    if (port_num >= NUM_OF_PORTS || pin_num >= NUM_OF_PINS_PER_PORT)
    {
        return;
    }

    /* Step 2: Select the DDR register of the requested port. */
    switch (port_num)
    {
        case PORTA_ID:
            /* Step 3: bit = 1 makes the pin an output, bit = 0 makes it an input. */
            if (direction == PIN_OUTPUT)
                SET_BIT(DDRA, pin_num);
            else
                CLEAR_BIT(DDRA, pin_num);
            break;

        case PORTB_ID:
            /* Same logic for PORTB. */
            if (direction == PIN_OUTPUT)
                SET_BIT(DDRB, pin_num);
            else
                CLEAR_BIT(DDRB, pin_num);
            break;

        case PORTC_ID:
            /* Same logic for PORTC. */
            if (direction == PIN_OUTPUT)
                SET_BIT(DDRC, pin_num);
            else
                CLEAR_BIT(DDRC, pin_num);
            break;

        case PORTD_ID:
            /* Same logic for PORTD. */
            if (direction == PIN_OUTPUT)
                SET_BIT(DDRD, pin_num);
            else
                CLEAR_BIT(DDRD, pin_num);
            break;
    }
}

/*
 * Write a logic value to one pin.
 * If the pin is configured as input, this controls its pull-up resistor.
 */
void GPIO_writePin(uint8 port_num, uint8 pin_num, uint8 value)
{
    /* Step 1: Ignore the request if the port or pin number is out of range. */
    if (port_num >= NUM_OF_PORTS || pin_num >= NUM_OF_PINS_PER_PORT)
    {
        return;
    }

    /* Step 2: Select the PORT register of the requested port. */
    switch (port_num)
    {
        case PORTA_ID:
            /* Step 3: Set the bit for LOGIC_HIGH, clear it for anything else. */
            if (value == LOGIC_HIGH)
                SET_BIT(PORTA, pin_num);
            else
                CLEAR_BIT(PORTA, pin_num);
            break;

        case PORTB_ID:
            /* Same logic for PORTB. */
            if (value == LOGIC_HIGH)
                SET_BIT(PORTB, pin_num);
            else
                CLEAR_BIT(PORTB, pin_num);
            break;

        case PORTC_ID:
            /* Same logic for PORTC. */
            if (value == LOGIC_HIGH)
                SET_BIT(PORTC, pin_num);
            else
                CLEAR_BIT(PORTC, pin_num);
            break;

        case PORTD_ID:
            /* Same logic for PORTD. */
            if (value == LOGIC_HIGH)
                SET_BIT(PORTD, pin_num);
            else
                CLEAR_BIT(PORTD, pin_num);
            break;
    }
}

/* Read the logic level of one pin. */
uint8 GPIO_readPin(uint8 port_num, uint8 pin_num)
{
    /* Step 1: Return LOGIC_LOW if the port or pin number is out of range. */
    if (port_num >= NUM_OF_PORTS || pin_num >= NUM_OF_PINS_PER_PORT)
    {
        return LOGIC_LOW;
    }

    /*
     * Step 2: Read the PIN register of the requested port, shift the wanted
     * bit down to position 0, and mask it so the result is 0 or 1.
     */
    switch (port_num)
    {
        case PORTA_ID:
            return (uint8)((PINA >> pin_num) & 0x01);

        case PORTB_ID:
            return (uint8)((PINB >> pin_num) & 0x01);

        case PORTC_ID:
            return (uint8)((PINC >> pin_num) & 0x01);

        case PORTD_ID:
            return (uint8)((PIND >> pin_num) & 0x01);

        default:
            return LOGIC_LOW;
    }
}

/* Configure all pins of one port as input or output. */
void GPIO_setupPortDirection(uint8 port_num, GPIO_PortDirectionType direction)
{
    /* Step 1: Ignore the request if the port number is out of range. */
    if (port_num >= NUM_OF_PORTS)
    {
        return;
    }

    /*
     * Step 2: Write the direction value to the whole DDR register:
     * 0x00 = all pins input, 0xFF = all pins output.
     */
    switch (port_num)
    {
        case PORTA_ID:
            DDRA = (uint8)direction;
            break;

        case PORTB_ID:
            DDRB = (uint8)direction;
            break;

        case PORTC_ID:
            DDRC = (uint8)direction;
            break;

        case PORTD_ID:
            DDRD = (uint8)direction;
            break;
    }
}

/*
 * Write a byte to a port.
 * Output pins receive the output value; input pins have their pull-ups
 * enabled or disabled according to the corresponding value bits.
 */
void GPIO_writePort(uint8 port_num, uint8 value)
{
    /* Write the byte to the PORT register of the requested port. */
    switch (port_num)
    {
        case PORTA_ID:
            PORTA = value;
            break;

        case PORTB_ID:
            PORTB = value;
            break;

        case PORTC_ID:
            PORTC = value;
            break;

        case PORTD_ID:
            PORTD = value;
            break;

        default:
            /* Invalid port number: do nothing. */
            break;
    }
}

/* Read the logic levels of all pins in one port. */
uint8 GPIO_readPort(uint8 port_num)
{
    /* Return the full PIN register (all 8 pin levels) of the requested port. */
    switch (port_num)
    {
        case PORTA_ID:
            return PINA;

        case PORTB_ID:
            return PINB;

        case PORTC_ID:
            return PINC;

        case PORTD_ID:
            return PIND;

        default:
            /* Invalid port number: return 0. */
            return 0x00;
    }
}
