

#include <ti/driverlib/driverlib.h>
#include <Timer_Profiler.h>


void Timer_Profiler_Inialize(Timer_Profiler *thisTimer, GPTIMER_Regs *thisTimerRegs, uint8_t thisInstance, uint32_t clockFrequency) {

  
    thisTimer->timer        = thisTimerRegs;
    thisTimer->instanceNum  = thisInstance;

    // timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
    // Set to 1MHz frequency or 1us period
    thisTimer->clkConfig.clockSel       = DL_TIMER_CLOCK_BUSCLK;
    thisTimer->clkConfig.divideRatio    = DL_TIMER_CLOCK_DIVIDE_1;
    thisTimer->clkConfig.prescale       = (uint8_t) (clockFrequency/1000000 - 1);
    DL_TimerG_setClockConfig(thisTimer->timer,  &(thisTimer->clkConfig));
    DL_Timer_setLoadValue(thisTimer->timer, 0xFFFF);
    // Set the timer mode
    thisTimer->config.startTimer    = DL_TIMER_START;
    thisTimer->config.timerMode     = DL_TIMER_TIMER_MODE_PERIODIC_UP;
       thisTimer->config.period     = 50000 ; // to get 50ms period
    DL_TimerG_initTimerMode(thisTimer->timer, &(thisTimer->config));

    DL_Timer_setCounterValueAfterEnable(thisTimer->timer, DL_TIMER_COUNT_AFTER_EN_ZERO );
    DL_TimerG_enableInterrupt(thisTimer->timer , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    if (thisTimer->instanceNum == 0){
        NVIC_ClearPendingIRQ(TIMG0_INT_IRQn);
        NVIC_EnableIRQ(TIMG0_INT_IRQn);
    }
    else if (thisInstance == 6){
        NVIC_ClearPendingIRQ(TIMG6_INT_IRQn);
        NVIC_EnableIRQ(TIMG6_INT_IRQn);
    }
    else if (thisInstance == 7){
        NVIC_ClearPendingIRQ(TIMG7_INT_IRQn);
        NVIC_EnableIRQ(TIMG7_INT_IRQn);
    }
    else if (thisInstance == 8){
        NVIC_ClearPendingIRQ(TIMG8_INT_IRQn);
        NVIC_EnableIRQ(TIMG8_INT_IRQn);
    }
   
    thisTimer->msec50 = 0;
}

void Timer_Profiler_Start(Timer_Profiler *thisTimer, uint8_t profilerIndex) {
        thisTimer->profilers[profilerIndex].us = DL_Timer_getTimerCount(thisTimer->timer);
        thisTimer->profilers[profilerIndex].msec50 = thisTimer->msec50;
}
uint32_t Timer_Profiler_Get(Timer_Profiler *thisTimer, uint8_t profilerIndex) {
    return (thisTimer->msec50 - thisTimer->profilers[profilerIndex].msec50)*50000 + DL_Timer_getTimerCount(thisTimer->timer) - thisTimer->profilers[profilerIndex].us;
}


void Timer_Profiler_IRQ(Timer_Profiler *thisTimer) {
    switch (DL_TimerG_getPendingInterrupt(thisTimer->timer)) {
        case DL_TIMER_IIDX_ZERO:
            thisTimer->msec50++;
            break;
        default:
            break;
    }
}