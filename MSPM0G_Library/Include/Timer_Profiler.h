#ifndef TIMER_PROFILER_H_
#define TIMER_PROFILER_H_

#define NUM_PROFILERS 16

typedef struct {
    uint16_t us;
    uint16_t msec50;
} Profiler;


typedef struct {
    GPTIMER_Regs *timer;
    uint8_t instanceNum;
    DL_Timer_ClockConfig clkConfig;
    DL_TimerG_TimerConfig config;
    uint16_t msec50;
    Profiler profilers[NUM_PROFILERS];
} Timer_Profiler;

void Timer_Profiler_Inialize(Timer_Profiler *thisTimer, GPTIMER_Regs *thisTimerRegs, uint8_t thisInstance, uint32_t clockFrequency) ;
void Timer_Profiler_Start(Timer_Profiler *thisTimer, uint8_t profilerIndex);
uint32_t Timer_Profiler_Get(Timer_Profiler *thisTimer, uint8_t profilerIndex);
void Timer_Profiler_IRQ(Timer_Profiler *thisTimer);
#endif