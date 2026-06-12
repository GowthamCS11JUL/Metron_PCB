#ifndef USB_MUX_H_
#define USB_MUX_H_



/* =========================================================
 * USB CHANNEL ENUM
 * ========================================================= */
typedef enum
{
    USB_CH0 = 0,   /* OUTA0 */
    USB_CH1 = 1,   /* OUTA1 */
    USB_CH2 = 2,   /* OUTB0 */
    USB_CH3 = 3    /* OUTB1 */

} usb_channel;

/* =========================================================
 * FUNCTION PROTOTYPES
 * ========================================================= */

/**
 * @brief Initialize TS3DS10224 control pins.
 */
void usb_pins_init(void);

/**
 * @brief Route powered pendrive (INA) to selected output.
 *
 * INA = Pendrive USB D+/D-
 * INB = Floating
 *
 * @param ch Output channel
 */
void usb_function_test(usb_channel ch);

/**
 * @brief Route resistance measurement path (INB)
 *        to selected output.
 *
 * INA = Floating (VBUS OFF)
 * INB = Relay resistance path
 *
 * @param ch Output channel
 */
void usb_resistance_test(usb_channel ch);

#endif /* USB_MUX_H_ */