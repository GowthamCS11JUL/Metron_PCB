#include "delay_timer.h"

/* ------------------------------------------------ */
/* INITIALIZE TIMER                                 */
/* ------------------------------------------------ */

void Delay_Timer_Initialize(Delay_Timer *thisTimer, GPTIMER_Regs *thisTimerRegs,
                            uint8_t thisInstance, uint32_t clockFrequency) {
  thisTimer->timer = thisTimerRegs;

  thisTimer->instanceNum = thisInstance;

  thisTimer->overflowCount = 0;

  /* -------------------------------------------- */
  /* Enable Power                                 */
  /* -------------------------------------------- */

  DL_TimerG_enablePower(thisTimer->timer);

  delay_cycles(1000);

  /* -------------------------------------------- */
  /* Enable Clock                                 */
  /* -------------------------------------------- */

  DL_TimerG_enableClock(thisTimer->timer);

  /* -------------------------------------------- */
  /* Configure 1 MHz timer                        */
  /* 1 tick = 1 us                                */
  /* -------------------------------------------- */

  thisTimer->clkConfig.clockSel = DL_TIMER_CLOCK_BUSCLK;

  thisTimer->clkConfig.divideRatio = DL_TIMER_CLOCK_DIVIDE_1;

  thisTimer->clkConfig.prescale = (clockFrequency / 1000000) - 1;

  DL_TimerG_setClockConfig(thisTimer->timer, &thisTimer->clkConfig);

  /* -------------------------------------------- */
  /* Periodic Up Mode                             */
  /* -------------------------------------------- */

  thisTimer->config.timerMode = DL_TIMER_TIMER_MODE_PERIODIC_UP;

  thisTimer->config.period = 0xFFFF;

  thisTimer->config.startTimer = DL_TIMER_STOP;

  DL_TimerG_initTimerMode(thisTimer->timer, &thisTimer->config);

  /* -------------------------------------------- */
  /* Enable Overflow Interrupt                    */
  /* -------------------------------------------- */

  DL_TimerG_enableInterrupt(thisTimer->timer, DL_TIMERG_INTERRUPT_ZERO_EVENT);

  /* -------------------------------------------- */
  /* NVIC                                         */
  /* -------------------------------------------- */

  if (thisInstance == 7) {
    NVIC_ClearPendingIRQ(TIMG7_INT_IRQn);

    NVIC_EnableIRQ(TIMG7_INT_IRQn);
  }
}

/* ------------------------------------------------ */
/* START TIMER                                      */
/* ------------------------------------------------ */

void Delay_Timer_Start(Delay_Timer *thisTimer) {
  /*
   * Free running timer
   */

  DL_Timer_startCounter(thisTimer->timer);
}

/* ------------------------------------------------ */
/* GET CURRENT TIME                                 */
/* ------------------------------------------------ */

uint32_t Delay_Timer_Get(Delay_Timer *thisTimer) {
  uint32_t overflow1;

  uint32_t overflow2;

  uint16_t current;

  /*
   * Prevent race condition during overflow
   */

  do {
    overflow1 = thisTimer->overflowCount;

    current = DL_Timer_getTimerCount(thisTimer->timer);

    overflow2 = thisTimer->overflowCount;

  } while (overflow1 != overflow2);

  return ((overflow1 << 16) | current);
}

/* ------------------------------------------------ */
/* TIMER IRQ                                        */
/* ------------------------------------------------ */

void Delay_Timer_IRQ(Delay_Timer *thisTimer) {
  switch (DL_TimerG_getPendingInterrupt(thisTimer->timer)) {
  case DL_TIMER_IIDX_ZERO: {
    thisTimer->overflowCount++;

    break;
  }

  default:
    break;
  }
}