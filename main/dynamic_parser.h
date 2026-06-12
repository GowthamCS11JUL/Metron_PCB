#ifndef DYNAMIC_PARSER_H_
#define DYNAMIC_PARSER_H_

#include <stdbool.h>
#include <stdint.h>


/* ------------------------------------------------ */
/* CONFIG                                           */
/* ------------------------------------------------ */

#define MAX_TOKENS 16

#define MAX_TOKEN_LENGTH 32

/* ------------------------------------------------ */
/* PARSED COMMAND STRUCTURE                         */
/* ------------------------------------------------ */

typedef struct {
  char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];

  uint8_t token_count;

} Parsed_Command_t;

/* ------------------------------------------------ */
/* PARSE COMMAND                                    */
/* ------------------------------------------------ */

/*
 * Example 1:
 *
 * INPUT:
 *     TEST:USB:CH0
 *
 * OUTPUT:
 *     tokens[0] = TEST
 *     tokens[1] = USB
 *     tokens[2] = CH0
 *
 *
 * Example 2:
 *
 * INPUT:
 *     DEMO:LED:GREEN:ON
 *
 * OUTPUT:
 *     tokens[0] = DEMO
 *     tokens[1] = LED
 *     tokens[2] = GREEN
 *     tokens[3] = ON
 *
 *
 * Example 3:
 *
 * INPUT:
 *     MES:RES:BET:CON1_MUX1:PIN_1:CON2_MUX2:PIN_5
 *
 * OUTPUT:
 *     tokens[0] = MES
 *     tokens[1] = RES
 *     tokens[2] = BET
 *     tokens[3] = CON1_MUX1
 *     tokens[4] = PIN_1
 *     tokens[5] = CON2_MUX2
 *     tokens[6] = PIN_5
 */

bool dynamic_parse(char *input, Parsed_Command_t *cmd);

#endif /* DYNAMIC_PARSER_H_ */