/******************************************************************************
 * File: std_types.h
 *
 * Purpose:
 *     Defines fixed-size data types and common constants used by every
 *     driver in the project, so all files use the same type names
 *     (uint8, uint16, ...) instead of raw C types.
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef STD_TYPES_H_
#define STD_TYPES_H_

/* Boolean type: an 8-bit unsigned value holding FALSE (0) or TRUE (1). */
typedef unsigned char boolean;

/*
 * Boolean values.
 * #ifndef protects against a redefinition if another header already
 * defined FALSE or TRUE.
 */
#ifndef FALSE
#define FALSE       (0u)
#endif
#ifndef TRUE
#define TRUE        (1u)
#endif

/* Logic levels used when writing to or reading from a GPIO pin. */
#define LOGIC_HIGH        (1u)
#define LOGIC_LOW         (0u)

/* Null pointer constant used to check pointers before using them. */
#define NULL_PTR    ((void*)0)

/*
 * Fixed-size integer and floating-point types for AVR (avr-gcc).
 * The comment next to each type shows the range of values it can hold.
 */
typedef unsigned char         uint8;          /*           0 .. 255              */
typedef signed char           sint8;          /*        -128 .. +127             */
typedef unsigned short        uint16;         /*           0 .. 65535            */
typedef signed short          sint16;         /*      -32768 .. +32767           */
typedef unsigned long         uint32;         /*           0 .. 4294967295       */
typedef signed long           sint32;         /* -2147483648 .. +2147483647      */
typedef unsigned long long    uint64;         /*       0 .. 18446744073709551615  */
typedef signed long long      sint64;         /* -9223372036854775808 .. 9223372036854775807 */
typedef float                 float32;
typedef double                float64;

#endif /* STD_TYPE_H_ */
