// #ifndef ADC_SPI_H_
// #define ADC_SPI_H_

// // Configuration
// #define AD4115_NUM_CHANNELS 16

// // // Public Function Declarations
// // void AD4115BCPZ_RL7_init(void);
// // uint32_t AD4115_ReadSingleChannel(uint8_t channel);
// // void AD4115_SPI_Transmit_Receive(uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);

// // // Extern the global results array so main.c can read it
// // extern uint32_t adc_results[AD4115_NUM_CHANNELS];

// #define AD4115_NUM_CHANNELS    16

// bool AD4115_Init(void);

// uint16_t AD4115_ReadID(void);

// uint32_t AD4115_ReadSingleChannel(uint8_t channel);



// #endif


#ifndef ADC_SPI_H_
#define ADC_SPI_H_



/* -----------------------------------------------------------------------------
 * Constant Definitions
 * ---------------------------------------------------------------------------*/
#define AD4115_NUM_CHANNELS      16

/* -----------------------------------------------------------------------------
 * External Global Variables
 * ---------------------------------------------------------------------------*/
/**
 * @brief Global array storing the latest 24-bit raw conversion results 
 *        for each channel.
 */
extern uint32_t adc_results[AD4115_NUM_CHANNELS];

/* -----------------------------------------------------------------------------
 * Public Function Prototypes
 * ---------------------------------------------------------------------------*/

/**
 * @brief Initializes the AD4115 ADC and applies foundational configurations
 *        using a 16-bit word aligned SPI transaction model.
 * 
 * This executes a hardware reset sequence, disables Channel 0, configures
 * status byte appending (DATA_STAT = 1), selects the internal reference, 
 * sets single conversion mode, and establishes a true unipolar setup.
 * 
 * @param spi Pointer to the active SPI peripheral handler instance.
 */
void AD4115BCPZ_RL7_init(SPI_Interface *spi);

/**
 * @brief Sequentially enables a specific channel, triggers a single conversion,
 *        polls until the data is ready, and safely unpacks the 24-bit value.
 * 
 * @param spi Pointer to the active SPI peripheral handler instance.
 * @param channel The target hardware channel index to sample (0 to 15).
 * 
 * @return uint32_t The 24-bit raw voltage conversion result, or 0 if a 
 *                  timeout or channel tracking mismatch error occurs.
 */
uint32_t AD4115_ReadSingleChannel( uint8_t channel);

#endif /* AD4115_DRIVER_H_ */