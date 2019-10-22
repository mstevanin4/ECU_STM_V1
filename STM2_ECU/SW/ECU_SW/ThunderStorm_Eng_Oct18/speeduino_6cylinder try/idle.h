#ifndef IDLE_H
#define IDLE_H

#include "globals.h"
#include "table.h"

#define IAC_ALGORITHM_NONE    0
#define IAC_ALGORITHM_ONOFF   1
#define IAC_ALGORITHM_PWM_OL  2
#define IAC_ALGORITHM_PWM_CL  3
#define IAC_ALGORITHM_STEP_OL 4
#define IAC_ALGORITHM_STEP_CL 5

#define STEPPER_FORWARD 0
#define STEPPER_BACKWARD 1
#define IDLE_TABLE_SIZE 10

enum StepperStatus {SOFF, STEPPING, COOLING}; //The 2 statuses that a stepper can have. STEPPING means that a high pulse is currently being sent and will need to be turned off at some point.

struct StepperIdle
{
  int curIdleStep; //Tracks the current location of the stepper
  int targetIdleStep; //What the targetted step is
  volatile StepperStatus stepperStatus;
  volatile unsigned long stepStartTime; //The time the curren
};

#if defined(CORE_AVR)
  #define IDLE_COUNTER TCNT4
  #define IDLE_COMPARE OCR4C

  #define IDLE_TIMER_ENABLE() TIMSK4 |= (1 << OCIE4C)
  #define IDLE_TIMER_DISABLE() TIMSK4 &= ~(1 << OCIE4C)

#elif defined(CORE_TEENSY)
  #define IDLE_COUNTER FTM2_CNT
  #define IDLE_COMPARE FTM2_C0V

  #define IDLE_TIMER_ENABLE() FTM2_C0SC |= FTM_CSC_CHIE
  #define IDLE_TIMER_DISABLE() FTM2_C0SC &= ~FTM_CSC_CHIE

#elif defined(CORE_STM32)
  #if defined(ARDUINO_ARCH_STM32) // STM32GENERIC core
    #define IDLE_COUNTER   (TIM1)->CNT
    #define IDLE_COMPARE   (TIM1)->CCR4

    #define IDLE_TIMER_ENABLE()  (TIM1)->CCER |= TIM_CCER_CC4E
    #define IDLE_TIMER_DISABLE() (TIM1)->CCER &= ~TIM_CCER_CC4E
  #else //libmaple core aka STM32DUINO
    #define IDLE_COUNTER   (TIMER1->regs).gen->CNT
    #define IDLE_COMPARE   (TIMER1->regs).gen->CCR4

    #define IDLE_TIMER_ENABLE()  (TIMER1->regs).gen->CCER |= TIMER_CCER_CC4E
    #define IDLE_TIMER_DISABLE() (TIMER1->regs).gen->CCER &= ~TIMER_CCER_CC4E
  #endif
#endif

struct table2D iacClosedLoopTable;
struct table2D iacPWMTable;
struct table2D iacStepTable;
//Open loop tables specifically for cranking
struct table2D iacCrankStepsTable;
struct table2D iacCrankDutyTable;

struct StepperIdle idleStepper;
bool idleOn; //Simply tracks whether idle was on last time around
byte idleInitComplete = 99; //TRacks which idle method was initialised. 99 is a method that will never exist
unsigned int iacStepTime;
unsigned int completedHomeSteps;

volatile byte *idle_pin_port;
volatile byte idle_pin_mask;
volatile byte *idle2_pin_port;
volatile byte idle2_pin_mask;
volatile bool idle_pwm_state;
unsigned int idle_pwm_max_count; //Used for variable PWM frequency
volatile unsigned int idle_pwm_cur_value;
long idle_pid_target_value;
long idle_pwm_target_value;
long idle_cl_target_rpm;
byte idleCounter; //Used for tracking the number of calls to the idle control function

void initialiseIdle();
static inline void disableIdle();
static inline void enableIdle();
static inline byte isStepperHomed();
static inline byte checkForStepping();
static inline void doStep();
#if defined (CORE_TEENSY) || defined(CORE_STM32)
  static inline void idleInterrupt();
#endif

#endif
