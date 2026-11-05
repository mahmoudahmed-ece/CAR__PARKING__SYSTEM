/******************************************************************************
 * File: lcd.h
 *
 * Purpose:
 *     Configuration and public interface of the character LCD driver
 *     (HD44780-compatible controller).
 *     Supports 4-bit or 8-bit data mode and 2x16 or 4x16 displays.
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef LCD_H_
#define LCD_H_

#include "std_types.h"   /* uint8 type. */
#include "gpio.h"        /* Port/pin IDs used in the pin configuration. */

/*******************************************************************************
 *                         USER CONFIGURATION                                  *
 ******************************************************************************/

/*
 * Select the LCD data-bus width:
 * 4 -> Uses DB4, DB5, DB6 and DB7.
 * 8 -> Uses DB0 through DB7 and requires a complete GPIO port for data.
 */
#define LCD_DATA_BITS_MODE  4

/*
 * Select the physical LCD dimensions.
 * Supported configurations in this driver are 2x16 and 4x16.
 * Change LCD_ROWS to 2 or 4 according to your display.
 */
#define LCD_ROWS            2
#define LCD_COLUMNS         16

/*
 * Compile-time checks: stop the build with a clear message if the
 * configuration above has an unsupported value.
 */
#if ((LCD_DATA_BITS_MODE != 4) && (LCD_DATA_BITS_MODE != 8))
#error "LCD_DATA_BITS_MODE must be 4 or 8"
#endif

#if ((LCD_ROWS != 2) && (LCD_ROWS != 4))
#error "LCD_ROWS must be 2 or 4"
#endif

#if (LCD_COLUMNS != 16)
#error "This driver configuration currently supports 16 columns"
#endif

/*******************************************************************************
 *                         LCD PIN CONFIGURATION                               *
 ******************************************************************************/

/*
 * RS: Selects command mode (0) or character-data mode (1).
 * E:  Enable strobe; the LCD reads the data when E is pulsed.
 *
 * Default wiring below is for 4-bit mode:
 * RS -> PA1, E -> PA2, DB4 -> PA3, DB5 -> PA4,
 * DB6 -> PA5, DB7 -> PA6.
 */
#define LCD_RS_PORT_ID      PORTA_ID
#define LCD_RS_PIN_ID       PIN1_ID

#define LCD_E_PORT_ID       PORTA_ID
#define LCD_E_PIN_ID        PIN2_ID

/*
 * In 4-bit mode, data pins are individually controlled and may share
 * a port with RS/E as long as the pin assignments do not overlap.
 *
 * In 8-bit mode, LCD_writeDataBus() writes a whole port at once.
 * Therefore LCD_DATA_PORT_ID MUST be different from the RS and E ports.
 * Example 8-bit wiring: data bus on PORTB, RS/E on PORTA.
 */
#define LCD_DATA_PORT_ID    PORTA_ID

#if (LCD_DATA_BITS_MODE == 4)
#define LCD_DB4_PIN_ID      PIN3_ID
#define LCD_DB5_PIN_ID      PIN4_ID
#define LCD_DB6_PIN_ID      PIN5_ID
#define LCD_DB7_PIN_ID      PIN6_ID
#endif

#if (LCD_DATA_BITS_MODE == 8) && \
    ((LCD_DATA_PORT_ID == LCD_RS_PORT_ID) || (LCD_DATA_PORT_ID == LCD_E_PORT_ID))
#error "In 8-bit mode, LCD data port must be separate from RS/E ports"
#endif

/*******************************************************************************
 *                              LCD COMMANDS                                   *
 ******************************************************************************/

/*
 * HD44780 instruction codes:
 * 0x01 clear display, 0x02 cursor home,
 * 0x38 / 0x28 function set (8-bit / 4-bit bus, 2 lines, 5x8 font),
 * 0x33 / 0x32 4-bit start-up sequence values,
 * 0x0C display on & cursor off, 0x0E display on & cursor on,
 * 0x80 "set DDRAM address" (OR-ed with the cursor address).
 */
#define LCD_CLEAR_COMMAND                  0x01
#define LCD_GO_TO_HOME                     0x02
#define LCD_TWO_LINES_EIGHT_BITS_MODE      0x38
#define LCD_TWO_LINES_FOUR_BITS_MODE       0x28
#define LCD_TWO_LINES_FOUR_BITS_MODE_INIT1 0x33
#define LCD_TWO_LINES_FOUR_BITS_MODE_INIT2 0x32
#define LCD_CURSOR_OFF                     0x0C
#define LCD_CURSOR_ON                      0x0E
#define LCD_SET_CURSOR_LOCATION            0x80

/*******************************************************************************
 *                            FUNCTION PROTOTYPES                              *
 ******************************************************************************/

/* Initialize control/data pins and configure the LCD interface. */
void LCD_init(void);

/* Send an LCD instruction; RS is set low before transmission. */
void LCD_sendCommand(uint8 command);

/* Display one character; RS is set high before transmission. */
void LCD_displayCharacter(uint8 data);

/* Display a null-terminated string. */
void LCD_displayString(const char *Str);

/*
 * Move cursor to zero-based row/column:
 * row 0 is the first row; col 0 is the first column.
 * Invalid coordinates are ignored.
 */
void LCD_moveCursor(uint8 row, uint8 col);

/* Move cursor, then display a null-terminated string at that position. */
void LCD_displayStringRowColumn(uint8 row, uint8 col, const char *Str);

/* Convert a signed integer to decimal text and display it. */
void LCD_intgerToString(int data);

/* Clear the display and return the cursor to the home position. */
void LCD_clearScreen(void);

#endif /* LCD_H_ */
