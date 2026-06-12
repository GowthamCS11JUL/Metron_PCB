#ifndef DELAY_TIMER_H_
#define DELAY_TIMER_H_

#include <stdint.h>
#include <ti/driverlib/driverlib.h>

/* ------------------------------------------------ */
/* DELAY TIMER STRUCT                               */
/* ------------------------------------------------ */

typedef struct {
  GPTIMER_Regs *timer;

  uint8_t instanceNum;

  volatile uint32_t overflowCount;

  DL_TimerG_ClockConfig clkConfig;

  DL_TimerG_TimerConfig config;

} Delay_Timer;

/* ------------------------------------------------ */
/* FUNCTIONS                                        */
/* ------------------------------------------------ */

void Delay_Timer_Initialize(Delay_Timer *thisTimer, GPTIMER_Regs *thisTimerRegs,
                            uint8_t thisInstance, uint32_t clockFrequency);

void Delay_Timer_Start(Delay_Timer *thisTimer);

uint32_t Delay_Timer_Get(Delay_Timer *thisTimer);

void Delay_Timer_IRQ(Delay_Timer *thisTimer);

#endif