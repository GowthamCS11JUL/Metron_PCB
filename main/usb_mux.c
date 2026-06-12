#include "headers.h"

static inline void usb_clear_selection(void) {
  DL_GPIO_clearPins(USB__SEL_A1_PORT, USB__SEL_A1_PIN);
  DL_GPIO_clearPins(USB__SEL_A0_PORT, USB__SEL_A0_PIN);
  DL_GPIO_clearPins(USB__SEL_B1_PORT, USB__SEL_B1_PIN);
  DL_GPIO_clearPins(USB__SEL_B0_PORT, USB__SEL_B0_PIN);
}

void usb_pins_init() {
  DL_GPIO_initDigitalOutput(USB__ENA_IOMUX);
  DL_GPIO_enableOutput(USB__ENA_PORT, USB__ENA_PIN);

  DL_GPIO_initDigitalOutput(USB__ENB_IOMUX);
  DL_GPIO_enableOutput(USB__ENB_PORT, USB__ENB_PIN);

  DL_GPIO_initDigitalOutput(USB__SEL_A0_IOMUX);
  DL_GPIO_enableOutput(USB__SEL_A0_PORT, USB__SEL_A0_PIN);

  DL_GPIO_initDigitalOutput(USB__SEL_A1_IOMUX);
  DL_GPIO_enableOutput(USB__SEL_A1_PORT, USB__SEL_A1_PIN);

  DL_GPIO_initDigitalOutput(USB__SEL_B0_IOMUX);
  DL_GPIO_enableOutput(USB__SEL_B0_PORT, USB__SEL_B0_PIN);

  DL_GPIO_initDigitalOutput(USB__SEL_B1_IOMUX);
  DL_GPIO_enableOutput(USB__SEL_B1_PORT, USB__SEL_B1_PIN);

  DL_GPIO_initDigitalOutput(USB__VBUS_ON_IOMUX);
  DL_GPIO_enableOutput(USB__VBUS_ON_PORT, USB__VBUS_ON_PIN);

  usb_clear_selection();
  DL_GPIO_clearPins(USB__ENA_PORT, USB__ENA_PIN);
  DL_GPIO_clearPins(USB__ENB_PORT, USB__ENB_PIN);
  DL_GPIO_clearPins(USB__VBUS_ON_PORT, USB__VBUS_ON_PIN);
}
// INB  IS FLOATING AND INB IS CONNECTED
void usb_function_test(usb_channel ch) {

  usb_clear_selection();

  // Always ensure VBUS power line is active for the pendrive
  DL_GPIO_setPins(USB__VBUS_ON_PORT, USB__VBUS_ON_PIN);
  delay_cycles(800000);
  DL_GPIO_setPins(USB__ENA_PORT, USB__ENA_PIN); // Enable Channel A
  DL_GPIO_setPins(USB__ENB_PORT, USB__ENB_PIN); // Enable Channel B

  switch (ch) {
  case USB_CH0:                                         // Route INA to OUTA0
    DL_GPIO_setPins(USB__SEL_A1_PORT, USB__SEL_A1_PIN); // SAI = 1
    DL_GPIO_setPins(USB__SEL_B1_PORT, USB__SEL_B1_PIN); // SBI = 1
    DL_GPIO_clearPins(USB__SEL_A0_PORT,
                      USB__SEL_A0_PIN); // SAO = 0 (Selects OUTA0)
    // SBO is a "Don't Care" here, but keeping it cleared is safe practice
    DL_GPIO_clearPins(USB__SEL_B0_PORT, USB__SEL_B0_PIN);

    break;

  case USB_CH1:                                         // Route INA to OUTA1
    DL_GPIO_setPins(USB__SEL_A1_PORT, USB__SEL_A1_PIN); // SAI = 1
    DL_GPIO_setPins(USB__SEL_B1_PORT, USB__SEL_B1_PIN); // SBI = 1
    DL_GPIO_setPins(USB__SEL_A0_PORT,
                    USB__SEL_A0_PIN); // SAO = 1 (Selects OUTA1)
    // SBO is a "Don't Care"
    DL_GPIO_clearPins(USB__SEL_B0_PORT, USB__SEL_B0_PIN);

    break;

  case USB_CH2:                                           // Route INA to OUTB0
    DL_GPIO_clearPins(USB__SEL_A1_PORT, USB__SEL_A1_PIN); // SAI = 0
    DL_GPIO_clearPins(USB__SEL_B1_PORT, USB__SEL_B1_PIN); // SBI = 0
    // SAO is a "Don't Care"
    DL_GPIO_clearPins(USB__SEL_A0_PORT, USB__SEL_A0_PIN);
    DL_GPIO_clearPins(USB__SEL_B0_PORT,
                      USB__SEL_B0_PIN); // SBO = 0 (Selects OUTB0)

    break;

  case USB_CH3:                                           // Route INA to OUTB1
    DL_GPIO_clearPins(USB__SEL_A1_PORT, USB__SEL_A1_PIN); // SAI = 0
    DL_GPIO_clearPins(USB__SEL_B1_PORT, USB__SEL_B1_PIN); // SBI = 0
    // SAO is a "Don't Care"
    DL_GPIO_clearPins(USB__SEL_A0_PORT, USB__SEL_A0_PIN);
    DL_GPIO_setPins(USB__SEL_B0_PORT,
                    USB__SEL_B0_PIN); // SBO = 1 (Selects OUTB1)

    break;

  default:
    // Safe state: Isolate all ports if an invalid channel is requested
    DL_GPIO_clearPins(USB__ENA_PORT, USB__ENA_PIN);
    DL_GPIO_clearPins(USB__ENB_PORT, USB__ENB_PIN);
    DL_GPIO_clearPins(USB__VBUS_ON_PORT, USB__VBUS_ON_PIN);
    break;
  }
}
// INA  IS FLOATING AND INB IS CONNECTED
void usb_resistance_test(usb_channel ch) {
  DL_GPIO_clearPins(USB__VBUS_ON_PORT, USB__VBUS_ON_PIN);
  delay_cycles(8000000); // ~50 ms @ 80 MHz
  usb_clear_selection();

  DL_GPIO_setPins(USB__ENA_PORT, USB__ENA_PIN);
  DL_GPIO_setPins(USB__ENB_PORT, USB__ENB_PIN);

  switch (ch) {
  case USB_CH0:                                           // INB -> OUTA0
    DL_GPIO_clearPins(USB__SEL_A1_PORT, USB__SEL_A1_PIN); // SAI=0
    DL_GPIO_setPins(USB__SEL_B1_PORT, USB__SEL_B1_PIN);   // SBI=1

    DL_GPIO_clearPins(USB__SEL_A0_PORT, USB__SEL_A0_PIN); // SAO=0
    break;

  case USB_CH1:                                           // INB -> OUTA1
    DL_GPIO_clearPins(USB__SEL_A1_PORT, USB__SEL_A1_PIN); // SAI=0
    DL_GPIO_setPins(USB__SEL_B1_PORT, USB__SEL_B1_PIN);   // SBI=1

    DL_GPIO_setPins(USB__SEL_A0_PORT, USB__SEL_A0_PIN); // SAO=1
    break;

  case USB_CH2:                                           // INB -> OUTB0
    DL_GPIO_clearPins(USB__SEL_A1_PORT, USB__SEL_A1_PIN); // SAI=0
    DL_GPIO_setPins(USB__SEL_B1_PORT, USB__SEL_B1_PIN);   // SBI=1

    DL_GPIO_clearPins(USB__SEL_B0_PORT, USB__SEL_B0_PIN); // SBO=0
    break;

  case USB_CH3:                                           // INB -> OUTB1
    DL_GPIO_clearPins(USB__SEL_A1_PORT, USB__SEL_A1_PIN); // SAI=0
    DL_GPIO_setPins(USB__SEL_B1_PORT, USB__SEL_B1_PIN);   // SBI=1

    DL_GPIO_setPins(USB__SEL_B0_PORT, USB__SEL_B0_PIN); // SBO=1
    break;

  default:
    DL_GPIO_clearPins(USB__ENA_PORT, USB__ENA_PIN);
    DL_GPIO_clearPins(USB__ENB_PORT, USB__ENB_PIN);
    break;
  }
}