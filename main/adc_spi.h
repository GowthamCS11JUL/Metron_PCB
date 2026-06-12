#ifndef ADC_SPI_H_
#define ADC_SPI_H_

// Configuration
#define AD4115_NUM_CHANNELS 16

// Public Function Declarations
void AD4115BCPZ_RL7_init(void);
uint32_t AD4115_ReadSingleChannel(uint8_t channel);

// Extern the global results array so main.c can read it
extern uint32_t adc_results[AD4115_NUM_CHANNELS];


#endif

