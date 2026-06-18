#ifndef DB_BOARD_H_
#define DB_BOARD_H_



// Hardware Current Tier Value Enum
typedef enum
{
    current_1ua = 0,
    current_10ua,
    current_100ua,
    current_1ma,
    current_10ma
} current_value;

// Auto-Ranging FSM State Enum
typedef enum {
    STATE_1UA = 0,
    STATE_10UA,
    STATE_100UA,
    STATE_1MA,
    STATE_10MA
} differential_fsm_state_t;

// --- Initialization and Core Hardware Configuration ---
void db_board_init(void);
void io_expander_daughter_config(void);
void dac_set(double i_value, current_value value);
void current_source_2_select(current_value value);

// --- Current Source Setters (Corrected to return float) ---
double current_10ma_set(void);
double current_1ma_set(void);
double current_100ua_set(void);
double current_10ua_set(void);
double current_1ua_set(void);

// --- ADS122C04 Driver Functions ---
void ads122c04_init(void);
void start_sync_cmd(void);
void ads122c04_read(uint8_t *out_data);
double ads122c04_raw_to_voltage(uint8_t *raw_bytes, uint8_t gain);
double ads122c04_calibration(current_value value);

// --- New Differential Mode & FSM Functions ---
double read_adc_differential(uint8_t gain);
double run_differential_fsm_tick(void);
double get_single_shot_resistance(void);

//
void io_expander_daughter_reset();

#endif /* DB_BOARD_H_ */