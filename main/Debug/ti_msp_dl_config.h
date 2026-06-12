/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0G3507
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG0)
#define TIMER_0_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                             (0U)
/* Defines for TIMER_1 */
#define TIMER_1_INST                                                     (TIMG7)
#define TIMER_1_INST_IRQHandler                                 TIMG7_IRQHandler
#define TIMER_1_INST_INT_IRQN                                   (TIMG7_INT_IRQn)
#define TIMER_1_INST_LOAD_VALUE                                         (64999U)
/* Defines for TIMER_2 */
#define TIMER_2_INST                                                     (TIMG6)
#define TIMER_2_INST_IRQHandler                                 TIMG6_IRQHandler
#define TIMER_2_INST_INT_IRQN                                   (TIMG6_INT_IRQn)
#define TIMER_2_INST_LOAD_VALUE                                             (0U)




/* Defines for Daughter_I2C */
#define Daughter_I2C_INST                                                   I2C0
#define Daughter_I2C_INST_IRQHandler                             I2C0_IRQHandler
#define Daughter_I2C_INST_INT_IRQN                                 I2C0_INT_IRQn
#define Daughter_I2C_BUS_SPEED_HZ                                         100000
#define GPIO_Daughter_I2C_SDA_PORT                                         GPIOA
#define GPIO_Daughter_I2C_SDA_PIN                                 DL_GPIO_PIN_28
#define GPIO_Daughter_I2C_IOMUX_SDA                               (IOMUX_PINCM3)
#define GPIO_Daughter_I2C_IOMUX_SDA_FUNC                IOMUX_PINCM3_PF_I2C0_SDA
#define GPIO_Daughter_I2C_SCL_PORT                                         GPIOA
#define GPIO_Daughter_I2C_SCL_PIN                                 DL_GPIO_PIN_31
#define GPIO_Daughter_I2C_IOMUX_SCL                               (IOMUX_PINCM6)
#define GPIO_Daughter_I2C_IOMUX_SCL_FUNC                IOMUX_PINCM6_PF_I2C0_SCL

/* Defines for Master_I2C */
#define Master_I2C_INST                                                     I2C1
#define Master_I2C_INST_IRQHandler                               I2C1_IRQHandler
#define Master_I2C_INST_INT_IRQN                                   I2C1_INT_IRQn
#define Master_I2C_BUS_SPEED_HZ                                           100000
#define GPIO_Master_I2C_SDA_PORT                                           GPIOB
#define GPIO_Master_I2C_SDA_PIN                                    DL_GPIO_PIN_3
#define GPIO_Master_I2C_IOMUX_SDA                                (IOMUX_PINCM16)
#define GPIO_Master_I2C_IOMUX_SDA_FUNC                 IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_Master_I2C_SCL_PORT                                           GPIOB
#define GPIO_Master_I2C_SCL_PIN                                    DL_GPIO_PIN_2
#define GPIO_Master_I2C_IOMUX_SCL                                (IOMUX_PINCM15)
#define GPIO_Master_I2C_IOMUX_SCL_FUNC                 IOMUX_PINCM15_PF_I2C1_SCL


/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_32_MHZ_115200_BAUD                                      (17)
#define UART_0_FBRD_32_MHZ_115200_BAUD                                      (23)
/* Defines for UART_3 */
#define UART_3_INST                                                        UART3
#define UART_3_INST_IRQHandler                                  UART3_IRQHandler
#define UART_3_INST_INT_IRQN                                      UART3_INT_IRQn
#define GPIO_UART_3_RX_PORT                                                GPIOA
#define GPIO_UART_3_TX_PORT                                                GPIOA
#define GPIO_UART_3_RX_PIN                                        DL_GPIO_PIN_25
#define GPIO_UART_3_TX_PIN                                        DL_GPIO_PIN_26
#define GPIO_UART_3_IOMUX_RX                                     (IOMUX_PINCM55)
#define GPIO_UART_3_IOMUX_TX                                     (IOMUX_PINCM59)
#define GPIO_UART_3_IOMUX_RX_FUNC                      IOMUX_PINCM55_PF_UART3_RX
#define GPIO_UART_3_IOMUX_TX_FUNC                      IOMUX_PINCM59_PF_UART3_TX
#define UART_3_BAUD_RATE                                                (115200)
#define UART_3_IBRD_32_MHZ_115200_BAUD                                      (17)
#define UART_3_FBRD_32_MHZ_115200_BAUD                                      (23)




/* Defines for SPI_0 */
#define SPI_0_INST                                                         SPI0
#define SPI_0_INST_IRQHandler                                   SPI0_IRQHandler
#define SPI_0_INST_INT_IRQN                                       SPI0_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOB
#define GPIO_SPI_0_PICO_PIN                                      DL_GPIO_PIN_17
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM43)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                   IOMUX_PINCM43_PF_SPI0_PICO
#define GPIO_SPI_0_POCI_PORT                                              GPIOB
#define GPIO_SPI_0_POCI_PIN                                      DL_GPIO_PIN_19
#define GPIO_SPI_0_IOMUX_POCI                                   (IOMUX_PINCM45)
#define GPIO_SPI_0_IOMUX_POCI_FUNC                   IOMUX_PINCM45_PF_SPI0_POCI
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOB
#define GPIO_SPI_0_SCLK_PIN                                      DL_GPIO_PIN_18
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM44)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                   IOMUX_PINCM44_PF_SPI0_SCLK
#define GPIO_SPI_0_CS0_PORT                                               GPIOA
#define GPIO_SPI_0_CS0_PIN                                        DL_GPIO_PIN_8
#define GPIO_SPI_0_IOMUX_CS0                                    (IOMUX_PINCM19)
#define GPIO_SPI_0_IOMUX_CS0_FUNC                     IOMUX_PINCM19_PF_SPI0_CS0



/* Port definition for Pin Group GPIO_GRP_0 */
#define GPIO_GRP_0_PORT                                                  (GPIOA)

/* Defines for PIN_0: GPIOA.0 with pinCMx 1 on package pin 33 */
#define GPIO_GRP_0_PIN_0_PIN                                     (DL_GPIO_PIN_0)
#define GPIO_GRP_0_PIN_0_IOMUX                                    (IOMUX_PINCM1)
/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOA)

/* Defines for Green: GPIOA.14 with pinCMx 36 on package pin 7 */
#define LED_Green_PIN                                           (DL_GPIO_PIN_14)
#define LED_Green_IOMUX                                          (IOMUX_PINCM36)
/* Defines for ENA: GPIOA.2 with pinCMx 7 on package pin 42 */
#define USB__ENA_PORT                                                    (GPIOA)
#define USB__ENA_PIN                                             (DL_GPIO_PIN_2)
#define USB__ENA_IOMUX                                            (IOMUX_PINCM7)
/* Defines for ENB: GPIOA.3 with pinCMx 8 on package pin 43 */
#define USB__ENB_PORT                                                    (GPIOA)
#define USB__ENB_PIN                                             (DL_GPIO_PIN_3)
#define USB__ENB_IOMUX                                            (IOMUX_PINCM8)
/* Defines for SEL_A0: GPIOA.4 with pinCMx 9 on package pin 44 */
#define USB__SEL_A0_PORT                                                 (GPIOA)
#define USB__SEL_A0_PIN                                          (DL_GPIO_PIN_4)
#define USB__SEL_A0_IOMUX                                         (IOMUX_PINCM9)
/* Defines for SEL_A1: GPIOA.5 with pinCMx 10 on package pin 45 */
#define USB__SEL_A1_PORT                                                 (GPIOA)
#define USB__SEL_A1_PIN                                          (DL_GPIO_PIN_5)
#define USB__SEL_A1_IOMUX                                        (IOMUX_PINCM10)
/* Defines for SEL_B0: GPIOA.9 with pinCMx 20 on package pin 55 */
#define USB__SEL_B0_PORT                                                 (GPIOA)
#define USB__SEL_B0_PIN                                          (DL_GPIO_PIN_9)
#define USB__SEL_B0_IOMUX                                        (IOMUX_PINCM20)
/* Defines for SEL_B1: GPIOB.6 with pinCMx 23 on package pin 58 */
#define USB__SEL_B1_PORT                                                 (GPIOB)
#define USB__SEL_B1_PIN                                          (DL_GPIO_PIN_6)
#define USB__SEL_B1_IOMUX                                        (IOMUX_PINCM23)
/* Defines for VBUS_ON: GPIOB.15 with pinCMx 32 on package pin 3 */
#define USB__VBUS_ON_PORT                                                (GPIOB)
#define USB__VBUS_ON_PIN                                        (DL_GPIO_PIN_15)
#define USB__VBUS_ON_IOMUX                                       (IOMUX_PINCM32)
/* Defines for Select_A0: GPIOB.9 with pinCMx 26 on package pin 61 */
#define UART_Select_A0_PORT                                              (GPIOB)
#define UART_Select_A0_PIN                                       (DL_GPIO_PIN_9)
#define UART_Select_A0_IOMUX                                     (IOMUX_PINCM26)
/* Defines for En: GPIOB.8 with pinCMx 25 on package pin 60 */
#define UART_En_PORT                                                     (GPIOB)
#define UART_En_PIN                                              (DL_GPIO_PIN_8)
#define UART_En_IOMUX                                            (IOMUX_PINCM25)
/* Defines for Select_A1: GPIOA.1 with pinCMx 2 on package pin 34 */
#define UART_Select_A1_PORT                                              (GPIOA)
#define UART_Select_A1_PIN                                       (DL_GPIO_PIN_1)
#define UART_Select_A1_IOMUX                                      (IOMUX_PINCM2)
/* Defines for SPI_CS: GPIOB.20 with pinCMx 48 on package pin 19 */
#define ADC_SPI_CS_PORT                                                  (GPIOB)
#define ADC_SPI_CS_PIN                                          (DL_GPIO_PIN_20)
#define ADC_SPI_CS_IOMUX                                         (IOMUX_PINCM48)
/* Defines for nERROR: GPIOA.22 with pinCMx 47 on package pin 18 */
#define ADC_nERROR_PORT                                                  (GPIOA)
#define ADC_nERROR_PIN                                          (DL_GPIO_PIN_22)
#define ADC_nERROR_IOMUX                                         (IOMUX_PINCM47)
/* Defines for nSYNC: GPIOA.21 with pinCMx 46 on package pin 17 */
#define ADC_nSYNC_PORT                                                   (GPIOA)
#define ADC_nSYNC_PIN                                           (DL_GPIO_PIN_21)
#define ADC_nSYNC_IOMUX                                          (IOMUX_PINCM46)
/* Port definition for Pin Group LED_ */
#define LED__PORT                                                        (GPIOB)

/* Defines for GREEN: GPIOB.27 with pinCMx 58 on package pin 29 */
#define LED__GREEN_PIN                                          (DL_GPIO_PIN_27)
#define LED__GREEN_IOMUX                                         (IOMUX_PINCM58)
/* Defines for RED: GPIOB.26 with pinCMx 57 on package pin 28 */
#define LED__RED_PIN                                            (DL_GPIO_PIN_26)
#define LED__RED_IOMUX                                           (IOMUX_PINCM57)
/* Defines for BLUE: GPIOB.22 with pinCMx 50 on package pin 21 */
#define LED__BLUE_PIN                                           (DL_GPIO_PIN_22)
#define LED__BLUE_IOMUX                                          (IOMUX_PINCM50)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_TIMER_1_init(void);
void SYSCFG_DL_TIMER_2_init(void);
void SYSCFG_DL_Daughter_I2C_init(void);
void SYSCFG_DL_Master_I2C_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_UART_3_init(void);
void SYSCFG_DL_SPI_0_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
