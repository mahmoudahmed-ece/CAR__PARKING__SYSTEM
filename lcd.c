/******************************************************************************
 * File: lcd.c
 *
 * Purpose:
 *     Implementation of the configurable 4-bit/8-bit LCD driver.
 *     Supports 2x16 and 4x16 character LCDs using a compatible
 *     HD44780-style controller.
 *
 * LCD control pins:
 *     RS = 0 -> the byte on the bus is a command (instruction).
 *     RS = 1 -> the byte on the bus is a character to display.
 *     E      -> a HIGH-then-LOW pulse tells the LCD to read the bus.
 ******************************************************************************/

#include <stdlib.h>          /* itoa() used to convert numbers to text. */
#include <util/delay.h>      /* _delay_ms() and _delay_us() (use F_CPU). */
#include "common_macros.h"   /* GET_BIT macro. */
#include "lcd.h"             /* LCD configuration, commands and prototypes. */
#include "gpio.h"            /* GPIO functions used to drive the LCD pins. */

/*******************************************************************************
 *                         PRIVATE HELPER FUNCTIONS                            *
 ******************************************************************************/

/*
 * Generate the Enable pulse. The LCD latches command/data on the falling edge
 * of E, after the data bus has been placed in a stable state.
 */
static void LCD_pulseEnable(void)
{
    /* Step 1: Raise E and hold it HIGH briefly. */
    GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_HIGH);
    _delay_us(1);
    /* Step 2: Drop E to LOW: the LCD reads the bus on this falling edge. */
    GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_LOW);
    _delay_us(1);
}

/*
 * Put one 8-bit value on the LCD data bus.
 * In 4-bit mode, send the upper nibble first, then the lower nibble.
 * In 8-bit mode, write the whole byte to the configured data port.
 */
static void LCD_writeDataBus(uint8 data)
{
#if (LCD_DATA_BITS_MODE == 4)
    /* Step 1: Put the upper nibble (bits 7..4) on DB7..DB4, then pulse E. */
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB4_PIN_ID, GET_BIT(data, 4));
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB5_PIN_ID, GET_BIT(data, 5));
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB6_PIN_ID, GET_BIT(data, 6));
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB7_PIN_ID, GET_BIT(data, 7));
    LCD_pulseEnable();

    /* Step 2: Put the lower nibble (bits 3..0) on DB7..DB4, then pulse E again. */
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB4_PIN_ID, GET_BIT(data, 0));
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB5_PIN_ID, GET_BIT(data, 1));
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB6_PIN_ID, GET_BIT(data, 2));
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB7_PIN_ID, GET_BIT(data, 3));
    LCD_pulseEnable();
#elif (LCD_DATA_BITS_MODE == 8)
    /* 8-bit mode: write the whole byte to the data port, then pulse E once. */
    GPIO_writePort(LCD_DATA_PORT_ID, data);
    LCD_pulseEnable();
#endif
}

/*******************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS                        *
 ******************************************************************************/

/*
 * Initialize the LCD:
 * 1. Configure GPIO directions.
 * 2. Wait for the LCD power-up interval.
 * 3. Select the configured 4-bit or 8-bit interface.
 * 4. Turn the cursor off and clear the display.
 */
void LCD_init(void)
{
    /* Step 1: RS and E are outputs. */
    GPIO_setupPinDirection(LCD_RS_PORT_ID, LCD_RS_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(LCD_E_PORT_ID, LCD_E_PIN_ID, PIN_OUTPUT);

#if (LCD_DATA_BITS_MODE == 4)
    /* 4-bit mode: only DB4..DB7 are used, so make those four pins outputs. */
    GPIO_setupPinDirection(LCD_DATA_PORT_ID, LCD_DB4_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(LCD_DATA_PORT_ID, LCD_DB5_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(LCD_DATA_PORT_ID, LCD_DB6_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(LCD_DATA_PORT_ID, LCD_DB7_PIN_ID, PIN_OUTPUT);
#else
    /* 8-bit mode: the whole data port is an output. */
    GPIO_setupPortDirection(LCD_DATA_PORT_ID, PORT_OUTPUT);
#endif

    /* Step 2: Keep E LOW and wait for the LCD to finish its power-up (more than 15 ms). */
    GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_LOW);
    _delay_ms(20);

#if (LCD_DATA_BITS_MODE == 4)
    /*
     * Step 3 (4-bit): Standard 4-bit startup sequence for common
     * HD44780-compatible LCDs.
     * The initialization nibbles are sent as raw nibbles because the LCD
     * may still be in its power-on 8-bit state.
     */
    GPIO_writePin(LCD_RS_PORT_ID, LCD_RS_PIN_ID, LOGIC_LOW);   /* RS = 0: command. */

    /* Send the 0x3 nibble three times with the required waits between them. */
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB4_PIN_ID, LOGIC_HIGH);
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB5_PIN_ID, LOGIC_HIGH);
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB6_PIN_ID, LOGIC_LOW);
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB7_PIN_ID, LOGIC_LOW);
    LCD_pulseEnable();
    _delay_ms(5);

    LCD_pulseEnable();
    _delay_us(150);

    LCD_pulseEnable();
    _delay_us(150);

    /* Send 0x02 nibble to switch the controller into 4-bit mode. */
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB4_PIN_ID, LOGIC_LOW);
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB5_PIN_ID, LOGIC_HIGH);
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB6_PIN_ID, LOGIC_LOW);
    GPIO_writePin(LCD_DATA_PORT_ID, LCD_DB7_PIN_ID, LOGIC_LOW);
    LCD_pulseEnable();

    /* 0x28: 4-bit bus, 2-line controller mode, 5x8 character font. */
    LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE);
#else
    /* Step 3 (8-bit) - 0x38: 8-bit bus, 2-line controller mode, 5x8 character font. */
    LCD_sendCommand(LCD_TWO_LINES_EIGHT_BITS_MODE);
#endif

    /* Step 4: Display on with the cursor hidden, then clear the screen. */
    LCD_sendCommand(LCD_CURSOR_OFF);
    LCD_sendCommand(LCD_CLEAR_COMMAND);
    _delay_ms(2);
}

/*
 * Send one instruction to the LCD.
 * RS=0 selects instruction mode. LCD_writeDataBus handles the bus width.
 */
void LCD_sendCommand(uint8 command)
{
    /* Step 1: RS = 0 (command), then put the command on the bus. */
    GPIO_writePin(LCD_RS_PORT_ID, LCD_RS_PIN_ID, LOGIC_LOW);
    LCD_writeDataBus(command);

    /* Step 2: Wait for the LCD to execute the command. */
    /* Clear and Return Home take longer than ordinary LCD instructions. */
    if ((command == LCD_CLEAR_COMMAND) || (command == LCD_GO_TO_HOME))
    {
        _delay_ms(2);
    }
    else
    {
        _delay_us(50);
    }
}

/*
 * Display one character.
 * RS=1 selects character-data mode.
 */
void LCD_displayCharacter(uint8 data)
{
    /* Step 1: RS = 1 (data), Step 2: put the character code on the bus, Step 3: wait. */
    GPIO_writePin(LCD_RS_PORT_ID, LCD_RS_PIN_ID, LOGIC_HIGH);
    LCD_writeDataBus(data);
    _delay_us(50);
}

/* Display a null-terminated string, character by character. */
void LCD_displayString(const char *Str)
{
    uint16 i = 0;   /* Index of the current character. */

    /* Step 1: Do nothing if no string was given. */
    if (Str == NULL_PTR)
    {
        return;
    }

    /* Step 2: Display characters one by one until the '\0' terminator. */
    while (Str[i] != '\0')
    {
        LCD_displayCharacter((uint8)Str[i]);
        i++;
    }
}

/*
 * Map zero-based row/column coordinates to the LCD DDRAM address.
 * Common 16-column HD44780-compatible mapping:
 * row 0 -> 0x00, row 1 -> 0x40, row 2 -> 0x10, row 3 -> 0x50.
 * A 2-row configuration accepts only rows 0 and 1; a 4-row configuration
 * accepts rows 0 through 3.
 */
void LCD_moveCursor(uint8 row, uint8 col)
{
    uint8 lcd_memory_address;   /* DDRAM address of the requested position. */

    /* Step 1: Ignore coordinates outside the configured display dimensions. */
    if ((row >= LCD_ROWS) || (col >= LCD_COLUMNS))
    {
        return;
    }

    /* Step 2: Address = start address of the row + column number. */
    switch (row)
    {
        case 0:
            lcd_memory_address = col;
            break;

        case 1:
            lcd_memory_address = (uint8)(col + 0x40);
            break;

#if (LCD_ROWS == 4)
        case 2:
            lcd_memory_address = (uint8)(col + 0x10);
            break;

        case 3:
            lcd_memory_address = (uint8)(col + 0x50);
            break;
#endif

        default:
            return;
    }

    /* Step 3: Send "set DDRAM address" (0x80 OR address) to move the cursor. */
    LCD_sendCommand((uint8)(lcd_memory_address | LCD_SET_CURSOR_LOCATION));
}

/* Position the cursor, then display the supplied string. */
void LCD_displayStringRowColumn(uint8 row, uint8 col, const char *Str)
{
    LCD_moveCursor(row, col);   /* Step 1: Go to the requested row/column. */
    LCD_displayString(Str);     /* Step 2: Print the string from there. */
}

/* Convert an integer to decimal text and display it. */
void LCD_intgerToString(int data)
{
    char buff[17];   /* Text buffer: enough for any int value plus '\0'. */

    itoa(data, buff, 10);    /* Step 1: Convert the number to base-10 text. */
    LCD_displayString(buff); /* Step 2: Display the text. */
}

/* Clear the screen; the command helper applies the required longer delay. */
void LCD_clearScreen(void)
{
    LCD_sendCommand(LCD_CLEAR_COMMAND);
}
