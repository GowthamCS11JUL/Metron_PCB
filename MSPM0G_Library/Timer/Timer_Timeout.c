

#include "ti/driverlib/m0p/dl_core.h"
#include <ti/driverlib/driverlib.h>
#include <Timer_Timeout.h>

void defaultTimerSingleTriggerCB(uint8_t dummy)
{
    __NOP();
}

Time_Timeout_Error Timer_Timeout_Inialize(Timer_Timeout *thisTimer, GPTIMER_Regs *thisTimerRegs, uint8_t thisInstance, uint32_t clockFrequency) {

  
    thisTimer->timer        = thisTimerRegs;
    thisTimer->instanceNum  = thisInstance;
    thisTimer->callBack     = defaultTimerSingleTriggerCB;
    thisTimer->error        = TIMER_TIMEOUT_ERROR_NONE;
    thisTimer->state        = TIMER_TIMEOUT_IDLE;
    // timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
    // Set to 1MHz frequency or 1us period
    thisTimer->clkConfig.clockSel       = DL_TIMER_CLOCK_BUSCLK;
    thisTimer->clkConfig.divideRatio    = DL_TIMER_CLOCK_DIVIDE_1;
    thisTimer->clkConfig.prescale       = (uint8_t) (clockFrequency/1000000 - 1);
    DL_TimerG_setClockConfig(thisTimer->timer,  &(thisTimer->clkConfig));

    // Set the timer mode
    thisTimer->config.startTimer    = DL_TIMER_START;
    thisTimer->config.timerMode     = DL_TIMER_TIMER_MODE_ONE_SHOT_UP;
    thisTimer->config.period        = 50000 - 1; // to get 50ms period
    DL_TimerG_initTimerMode(thisTimer->timer, &(thisTimer->config));

    DL_Timer_setCounterValueAfterEnable(thisTimer->timer, DL_TIMER_COUNT_AFTER_EN_ZERO );

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
    else {
        thisTimer->error = TIMER_TIMEOUT_ERROR_INSTACE; 
    }
    return thisTimer->error;
}


Time_Timeout_Error Timer_Timeout_Time_Set(Timer_Timeout *thisTimer, uint32_t durationInMicroSeconds)
{
     if (thisTimer->state == TIMER_TIMEOUT_IDLE) {
        // Stop the counter
        DL_Timer_stopCounter(thisTimer->timer);
        // Clear all interrupts
        DL_Timer_clearInterruptStatus(thisTimer->timer, 0xFFFFFFFF);
        // Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
        // TIMER_0_INST_LOAD_VALUE = (1 us * 32000000 Hz) - 1
        DL_Timer_setLoadValue(thisTimer->timer, durationInMicroSeconds - 1);
        // Enable load interrupt
        DL_Timer_disableInterrupt(thisTimer->timer, 0xFFFFFFFF);
        DL_Timer_enableInterrupt(thisTimer->timer, DL_TIMER_INTERRUPT_LOAD_EVENT);
     }
    else
        thisTimer->error = TIMER_TIMEOUT_ERROR_RUNNING;
    return thisTimer->error;     
}

Time_Timeout_Error Timer_Timeout_Start(Timer_Timeout *thisTimer) {
    if (thisTimer->state == TIMER_TIMEOUT_IDLE) {
        DL_TimerG_startCounter(thisTimer->timer);
        thisTimer->state = TIMER_TIMEOUT_RUNNING;
        thisTimer->error = TIMER_TIMEOUT_ERROR_NONE;
    }
    else
        thisTimer->error = TIMER_TIMEOUT_ERROR_RUNNING;
    return thisTimer->error;
}

Time_Timeout_Error Timer_Timeout_reStart(Timer_Timeout *thisTimer) {
    DL_TimerG_stopCounter(thisTimer->timer);
    DL_TimerG_startCounter(thisTimer->timer);
    thisTimer->state = TIMER_TIMEOUT_RUNNING;
    thisTimer->error = TIMER_TIMEOUT_ERROR_NONE;
    return thisTimer->error;
}


void Timer_Timeout_IRQ(Timer_Timeout *thisTimer) {
    switch (DL_TimerG_getPendingInterrupt(thisTimer->timer)) {
        case DL_TIMER_IIDX_LOAD:
            thisTimer->state = TIMER_TIMEOUT_IDLE;
            thisTimer->callBack(thisTimer->instanceNum);
            break;
        default:
            break;
    }
}

void Timer_Timeout_Wait(Timer_Timeout *thisTimer) {
    while (thisTimer->state == TIMER_TIMEOUT_RUNNING);
    return;
}

void Timer_Timeout_RemoveCallBack(Timer_Timeout *thisTimer)
{
    thisTimer->callBack = defaultTimerSingleTriggerCB;
}