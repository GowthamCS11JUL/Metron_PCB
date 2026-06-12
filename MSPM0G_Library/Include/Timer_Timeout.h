#ifndef TIMER_SINGLE_TRIGGER_H_
#define TIMER_SINGLE_TRIGGER_H_

typedef enum {
    TIMER_TIMEOUT_ERROR_NONE = 0,
    TIMER_TIMEOUT_ERROR_INSTACE,
    TIMER_TIMEOUT_ERROR_RUNNING,
}Time_Timeout_Error;


typedef enum {
    TIMER_TIMEOUT_IDLE=0,
    TIMER_TIMEOUT_RUNNING,
} Timer_Timeout_State;

typedef struct {
    GPTIMER_Regs *timer;
    uint8_t instanceNum;
    DL_Timer_ClockConfig clkConfig;
    DL_TimerG_TimerConfig config;
    Timer_Timeout_State state;
    Time_Timeout_Error error;
    void (*callBack)(uint8_t);
} Timer_Timeout;

Time_Timeout_Error Timer_Timeout_Inialize(Timer_Timeout *thisTimer, GPTIMER_Regs *thisTimerRegs, uint8_t thisInstance, uint32_t clockFrequency);
Time_Timeout_Error Timer_Timeout_Time_Set(Timer_Timeout *thisTimer, uint32_t durationInMicroSeconds);
Time_Timeout_Error Timer_Timeout_Start(Timer_Timeout *thisTimer);
Time_Timeout_Error Timer_Timeout_reStart(Timer_Timeout *thisTimer) ;
void Timer_Timeout_IRQ(Timer_Timeout *thisTimer) ;
void Timer_Timeout_Wait(Timer_Timeout *thisTimer) ;
void Timer_Timeout_RemoveCallBack(Timer_Timeout *thisTimer);

#endif