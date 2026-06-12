#ifndef MAINI2C_H_
#define MAINI2C_H_



/* =========================================================
 * IO EXPANDER ADDRESSES
 * ========================================================= */
#define U6 0x38
#define U7 0x39
#define U9 0x3A
#define U10 0x3B
#define U11 0x3C

/* =========================================================
 * VRMUX ENUM
 * ========================================================= */
typedef enum {
  CON1_MUX1 = 0,
  CON1_MUX2,
  CON1_MUX3,
  CON2_MUX1,
  CON2_MUX2,
  CON2_MUX3,
  CON2_MUX4,
  GPIO_MUX,
  CON3_MUX1,
  CON3_MUX2,
  CON3_MUX3,
  CON3_MUX4

} enable_mux_number;

/* =========================================================
 * PIN ENUM
 * ========================================================= */
typedef enum {
  PIN_0 = 0,
  PIN_1,
  PIN_2,
  PIN_3,
  PIN_4,
  PIN_5,
  PIN_6,
  PIN_7,
  PIN_8,
  PIN_9,
  PIN_10,
  PIN_11,
  PIN_12,
  PIN_13,
  PIN_14,
  PIN_15

} pin_number;

/* =========================================================
 * VRCOIL ENUM
 * ========================================================= */
typedef enum {
  CON1_MUX1_SEL = 0,
  CON1_MUX2_SEL,
  CON1_MUX3_SEL,
  CON2_MUX1_SEL,
  CON2_MUX2_SEL,
  CON2_MUX3_SEL,
  CON2_MUX4_SEL,

  VR_USB_SEL_DP,
  VR_USB_SEL_DN,
  VR_UART_SEL_TX,
  VR_UART_SEL_RX

} vrcoil_number;

/* =========================================================
 * RCOIL ENUM
 * ========================================================= */
typedef enum {
  CON3_MUX1_SEL = 0,
  CON3_MUX2_SEL,
  CON3_MUX3_SEL,
  CON3_MUX4_SEL

} rcoil_number;

/* =========================================================
 * FUNCTION PROTOTYPES
 * ========================================================= */

void io_expander_init(void);

void reset_all(void); 

uint8_t enable_mux(enable_mux_number mux) ;

uint8_t vrpin_select(pin_number pin);

uint8_t rpin_select(pin_number pin);

uint8_t vrcoil_select(vrcoil_number pin);

uint8_t rcoil_select(rcoil_number pin);
uint32_t single_voltage_measurement(enable_mux_number mux,pin_number pin);
uint32_t differential_voltage_measurement(enable_mux_number mux1, enable_mux_number mux2, pin_number pin1, pin_number pin2);
void multiple_voltage_measurement(pin_number pin,uint32_t value[16]);
void resistance_measurement(vrcoil_number num1, rcoil_number num2,pin_number pin1, pin_number pin2,usb_channel ch1, uart_channel ch2);

#endif /* MAINI2C_H_ */