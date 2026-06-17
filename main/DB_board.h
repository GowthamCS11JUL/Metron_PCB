// #ifndef DB_BOARD_H_
// #define DB_BOARD_H_


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

#ifndef DB_BOARD_H_
#define DB_BOARD_H_

typedef enum
{
    current_1ua = 0,
    current_10ua,
    current_100ua,
    current_1ma,
    current_10ma
} current_value;

 void io_expander_daughter_config(void);
void dac_set(void);
void current_source_2_select(current_value value);
void db_board_init(void) ;
void ads122c04_init(void);
void current_10ma_set();
void current_1ma_set();
void current_100ua_set();
void current_10ua_set();
void current_1ua_set();
void start_sync_cmd(void);
void ads122c04_read(uint8_t *out_data);
float ads122c04_raw_to_voltage(uint8_t *raw_bytes, uint8_t gain) ;
void ads122c04_calibration(current_value value);
#endif


