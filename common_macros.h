/******************************************************************************
 * File: common_macros.h
 *
 * Purpose:
 *     Bit-manipulation macros shared by all drivers.
 *     REG is a register or variable, BIT is the bit number (0..7).
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef COMMON_MACROS_H
#define COMMON_MACROS_H

/*
 * Set, clear and toggle a specific bit.
 * (1U << BIT) builds a mask with only that bit equal to 1:
 *   SET_BIT    -> OR with the mask forces the bit to 1.
 *   CLEAR_BIT  -> AND with the inverted mask forces the bit to 0.
 *   TOGGLE_BIT -> XOR with the mask flips the bit.
 */
#define SET_BIT(REG, BIT)       ((REG) |= (1U << (BIT)))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(1U << (BIT)))
#define TOGGLE_BIT(REG, BIT)    ((REG) ^= (1U << (BIT)))

/*
 * Read a specific bit: returns 0 or 1.
 * Shift the wanted bit down to position 0, then keep only that bit.
 */
#define GET_BIT(REG, BIT)       (((REG) >> (BIT)) & 0x01U)

/* Bit status tests: evaluate to true (1) or false (0). */
#define BIT_IS_SET(REG, BIT)    (GET_BIT((REG), (BIT)) != 0U)
#define BIT_IS_CLEAR(REG, BIT)  (GET_BIT((REG), (BIT)) == 0U)

/*
 * Rotate an 8-bit register left (ROL) or right (ROR) by NUM positions.
 * The bits that leave one end are brought back in at the other end,
 * and the result is masked with 0xFF to stay 8 bits wide.
 */
#define ROL(REG, NUM)           ((REG) = (uint8)((((REG) << (NUM)) | ((REG) >> (8U - (NUM)))) & 0xFFU))
#define ROR(REG, NUM)           ((REG) = (uint8)((((REG) >> (NUM)) | ((REG) << (8U - (NUM)))) & 0xFFU))

#endif /* COMMON_MACROS_H */
