// #ifndef DB_BOARD_H_
// #define DB_BOARD_H_

// #include "DB_board.h"
// #define Andc 0x40
// #define U4 0x4A
// #define U2 0X3E
// #define U16 0x3D
// #define Rpot 0x2

// typedef enum {
//   one_microamp = 0,
//   ten_microamp,
//   hundred_microamp,
//   one_milli_amp,
//   ten_milli_amp

// } current_value;

// typedef enum
// {
//     RES_IDLE = 0,
//     RES_SET_CURRENT,
//     RES_MEASURE,
//     RES_CALCULATE,
//     RES_DONE,
//     RES_SHORT_CIRCUIT

// } RES_FSM_STATE;

// typedef struct
// {
//     RES_FSM_STATE state;
//     current_value currentRange;
//     float voltage;
//     float resistance;
// } resistance_fsm_t;

// void resistance_fsm_init(resistance_fsm_t *fsm);

// void resistance_fsm_task(resistance_fsm_t *fsm);

// #endif
