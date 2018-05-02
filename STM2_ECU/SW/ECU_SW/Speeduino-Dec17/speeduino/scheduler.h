/*
This scheduler is designed to maintain 2 schedules for use by the fuel and ignition systems.
It functions by waiting for the overflow vectors from each of the timers in use to overflow, which triggers an interrupt

//Technical
Currently I am prescaling the 16-bit timers to 256 for injection and 64 for ignition. This means that the counter increments every 16us (injection) / 4uS (ignition) and will overflow every 1048576uS
Max Period = (Prescale)*(1/Frequency)*(2^17)
(See http://playground.arduino.cc/code/timer1)
This means that the precision of the scheduler is 16uS (+/- 8uS of target) for fuel and 4uS (+/- 2uS) for ignition

/Features
This differs from most other schedulers in that its calls are non-recurring (IE You schedule an event at a certain time and once it has occurred, it will not reoccur unless you explicitely ask for it)
Each timer can have only 1 callback associated with it at any given time. If you call the setCallback function a 2nd time, the original schedule will be overwritten and not occur

Timer identification
The Arduino timer3 is used for schedule 1
The Arduino timer4 is used for schedule 2
Both of these are 16-bit timers (ie count to 65536)
See page 136 of the processors datasheet: http://www.atmel.com/Images/doc2549.pdf

256 prescale gives tick every 16uS
256 prescale gives overflow every 1048576uS (This means maximum wait time is 1.0485 seconds)

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

#define IGNITION_REFRESH_THRESHOLD  30 //Time in uS that the refresh functions will check to ensure there is enough time before changing the end compare
#if defined(CORE_AVR)
  #include <avr/interrupt.h>
  #include <avr/io.h>

  //Refer to http://svn.savannah.nongnu.org/viewvc/trunk/avr-libc/include/avr/iomxx0_1.h?root=avr-libc&view=markup
  #define FUEL1_COUNTER TCNT3
  #define FUEL2_COUNTER TCNT3
  #define FUEL3_COUNTER TCNT3
  #define FUEL4_COUNTER TCNT4
  #define FUEL5_COUNTER TCNT3

  #define IGN1_COUNTER  TCNT5
  #define IGN2_COUNTER  TCNT5
  #define IGN3_COUNTER  TCNT5
  #define IGN4_COUNTER  TCNT4
  #define IGN5_COUNTER  TCNT1

  #define FUEL1_COMPARE OCR3A
  #define FUEL2_COMPARE OCR3B
  #define FUEL3_COMPARE OCR3C
  #define FUEL4_COMPARE OCR4B
  #define FUEL5_COMPARE OCR3A //Shared with FUEL1

  #define IGN1_COMPARE  OCR5A
  #define IGN2_COMPARE  OCR5B
  #define IGN3_COMPARE  OCR5C
  #define IGN4_COMPARE  OCR4A
  #define IGN5_COMPARE  OCR1C

  #define FUEL1_TIMER_ENABLE() TIMSK3 |= (1 << OCIE3A) //Turn on the A compare unit (ie turn on the interrupt)
  #define FUEL2_TIMER_ENABLE() TIMSK3 |= (1 << OCIE3B) //Turn on the B compare unit (ie turn on the interrupt)
  #define FUEL3_TIMER_ENABLE() TIMSK3 |= (1 << OCIE3C) //Turn on the C compare unit (ie turn on the interrupt)
  #define FUEL4_TIMER_ENABLE() TIMSK4 |= (1 << OCIE4B) //Turn on the B compare unit (ie turn on the interrupt)

  #define FUEL1_TIMER_DISABLE() TIMSK3 &= ~(1 << OCIE3A); //Turn off this output compare unit
  #define FUEL2_TIMER_DISABLE() TIMSK3 &= ~(1 << OCIE3B); //Turn off this output compare unit
  #define FUEL3_TIMER_DISABLE() TIMSK3 &= ~(1 << OCIE3C); //Turn off this output compare unit
  #define FUEL4_TIMER_DISABLE() TIMSK4 &= ~(1 << OCIE4B); //Turn off this output compare unit

  #define IGN1_TIMER_ENABLE() TIMSK5 |= (1 << OCIE5A) //Turn on the A compare unit (ie turn on the interrupt)
  #define IGN2_TIMER_ENABLE() TIMSK5 |= (1 << OCIE5B) //Turn on the B compare unit (ie turn on the interrupt)
  #define IGN3_TIMER_ENABLE() TIMSK5 |= (1 << OCIE5C) //Turn on the C compare unit (ie turn on the interrupt)
  #define IGN4_TIMER_ENABLE() TIMSK4 |= (1 << OCIE4A) //Turn on the A compare unit (ie turn on the interrupt)
  #define IGN5_TIMER_ENABLE() TIMSK1 |= (1 << OCIE1C) //Turn on the A compare unit (ie turn on the interrupt)

  #define IGN1_TIMER_DISABLE() TIMSK5 &= ~(1 << OCIE5A) //Turn off this output compare unit
  #define IGN2_TIMER_DISABLE() TIMSK5 &= ~(1 << OCIE5B) //Turn off this output compare unit
  #define IGN3_TIMER_DISABLE() TIMSK5 &= ~(1 << OCIE5C) //Turn off this output compare unit
  #define IGN4_TIMER_DISABLE() TIMSK4 &= ~(1 << OCIE4A) //Turn off this output compare unit
  #define IGN5_TIMER_DISABLE() TIMSK1 &= ~(1 << OCIE1C) //Turn off this output compare unit

  #define MAX_TIMER_PERIOD 262140UL //The longest period of time (in uS) that the timer can permit (IN this case it is 65535 * 4, as each timer tick is 4uS)
  #define MAX_TIMER_PERIOD_SLOW 1048560UL //The longest period of time (in uS) that the timer can permit (IN this case it is 65535 * 16, as each timer tick is 16uS)
  #define uS_TO_TIMER_COMPARE(uS1) (uS1 >> 2) //Converts a given number of uS into the required number of timer ticks until that time has passed
  //This is a hack until I make all the AVR timers run at the same speed
  #define uS_TO_TIMER_COMPARE_SLOW(uS1) (uS1 >> 4)

#elif defined(CORE_TEENSY)
  //http://shawnhymel.com/661/learning-the-teensy-lc-interrupt-service-routines/
  #define FUEL1_COUNTER FTM0_CNT
  #define FUEL2_COUNTER FTM0_CNT
  #define FUEL3_COUNTER FTM0_CNT
  #define FUEL4_COUNTER FTM0_CNT
  #define FUEL5_COUNTER FTM3_CNT
  #define FUEL6_COUNTER FTM3_CNT
  #define FUEL7_COUNTER FTM3_CNT
  #define FUEL8_COUNTER FTM3_CNT

  #define IGN1_COUNTER  FTM0_CNT
  #define IGN2_COUNTER  FTM0_CNT
  #define IGN3_COUNTER  FTM0_CNT
  #define IGN4_COUNTER  FTM0_CNT
  #define IGN5_COUNTER  FTM3_CNT
  #define IGN6_COUNTER  FTM3_CNT
  #define IGN7_COUNTER  FTM3_CNT
  #define IGN8_COUNTER  FTM3_CNT

  #define FUEL1_COMPARE FTM0_C0V
  #define FUEL2_COMPARE FTM0_C1V
  #define FUEL3_COMPARE FTM0_C2V
  #define FUEL4_COMPARE FTM0_C3V
  #define FUEL5_COMPARE FTM3_C0V
  #define FUEL6_COMPARE FTM3_C1V
  #define FUEL7_COMPARE FTM3_C2V
  #define FUEL8_COMPARE FTM3_C3V

  #define IGN1_COMPARE  FTM0_C4V
  #define IGN2_COMPARE  FTM0_C5V
  #define IGN3_COMPARE  FTM0_C6V
  #define IGN4_COMPARE  FTM0_C7V
  #define IGN5_COMPARE  FTM3_C4V
  #define IGN6_COMPARE  FTM3_C5V
  #define IGN7_COMPARE  FTM3_C6V
  #define IGN8_COMPARE  FTM3_C7V

  #define FUEL1_TIMER_ENABLE() FTM0_C0SC |= FTM_CSC_CHIE //Write 1 to the CHIE (Channel Interrupt Enable) bit of channel 0 Status/Control
  #define FUEL2_TIMER_ENABLE() FTM0_C1SC |= FTM_CSC_CHIE
  #define FUEL3_TIMER_ENABLE() FTM0_C2SC |= FTM_CSC_CHIE
  #define FUEL4_TIMER_ENABLE() FTM0_C3SC |= FTM_CSC_CHIE
  #define FUEL5_TIMER_ENABLE() FTM3_C0SC |= FTM_CSC_CHIE
  #define FUEL6_TIMER_ENABLE() FTM3_C1SC |= FTM_CSC_CHIE
  #define FUEL7_TIMER_ENABLE() FTM3_C2SC |= FTM_CSC_CHIE
  #define FUEL8_TIMER_ENABLE() FTM3_C3SC |= FTM_CSC_CHIE

  #define FUEL1_TIMER_DISABLE() FTM0_C0SC &= ~FTM_CSC_CHIE //Write 0 to the CHIE (Channel Interrupt Enable) bit of channel 0 Status/Control
  #define FUEL2_TIMER_DISABLE() FTM0_C1SC &= ~FTM_CSC_CHIE
  #define FUEL3_TIMER_DISABLE() FTM0_C2SC &= ~FTM_CSC_CHIE
  #define FUEL4_TIMER_DISABLE() FTM0_C3SC &= ~FTM_CSC_CHIE
  #define FUEL5_TIMER_DISABLE() FTM3_C0SC &= ~FTM_CSC_CHIE //Write 0 to the CHIE (Channel Interrupt Enable) bit of channel 0 Status/Control
  #define FUEL6_TIMER_DISABLE() FTM3_C1SC &= ~FTM_CSC_CHIE
  #define FUEL7_TIMER_DISABLE() FTM3_C2SC &= ~FTM_CSC_CHIE
  #define FUEL8_TIMER_DISABLE() FTM3_C3SC &= ~FTM_CSC_CHIE

  #define IGN1_TIMER_ENABLE() FTM0_C4SC |= FTM_CSC_CHIE
  #define IGN2_TIMER_ENABLE() FTM0_C5SC |= FTM_CSC_CHIE
  #define IGN3_TIMER_ENABLE() FTM0_C6SC |= FTM_CSC_CHIE
  #define IGN4_TIMER_ENABLE() FTM0_C7SC |= FTM_CSC_CHIE
  #define IGN5_TIMER_ENABLE() FTM3_C4SC |= FTM_CSC_CHIE
  #define IGN6_TIMER_ENABLE() FTM3_C5SC |= FTM_CSC_CHIE
  #define IGN7_TIMER_ENABLE() FTM3_C6SC |= FTM_CSC_CHIE
  #define IGN8_TIMER_ENABLE() FTM3_C7SC |= FTM_CSC_CHIE

  #define IGN1_TIMER_DISABLE() FTM0_C4SC &= ~FTM_CSC_CHIE
  #define IGN2_TIMER_DISABLE() FTM0_C5SC &= ~FTM_CSC_CHIE
  #define IGN3_TIMER_DISABLE() FTM0_C6SC &= ~FTM_CSC_CHIE
  #define IGN4_TIMER_DISABLE() FTM0_C7SC &= ~FTM_CSC_CHIE
  #define IGN5_TIMER_DISABLE() FTM3_C4SC &= ~FTM_CSC_CHIE
  #define IGN6_TIMER_DISABLE() FTM3_C5SC &= ~FTM_CSC_CHIE
  #define IGN7_TIMER_DISABLE() FTM3_C6SC &= ~FTM_CSC_CHIE
  #define IGN8_TIMER_DISABLE() FTM3_C7SC &= ~FTM_CSC_CHIE

  #define MAX_TIMER_PERIOD 139808 // 2.13333333uS * 65535
  #define MAX_TIMER_PERIOD_SLOW 139808
  #define uS_TO_TIMER_COMPARE(uS) ((uS * 15) >> 5) //Converts a given number of uS into the required number of timer ticks until that time has passed.
  //Hack compatibility with AVR timers that run at different speeds
  #define uS_TO_TIMER_COMPARE_SLOW(uS) ((uS * 15) >> 5)

#elif defined(CORE_STM32)
  #define MAX_TIMER_PERIOD 131070 //The longest period of time (in uS) that the timer can permit (IN this case it is 65535 * 2, as each timer tick is 2uS)
  #define MAX_TIMER_PERIOD_SLOW 131070 //The longest period of time (in uS) that the timer can permit (IN this case it is 65535 * 2, as each timer tick is 2uS)
  #define uS_TO_TIMER_COMPARE(uS) (uS >> 1) //Converts a given number of uS into the required number of timer ticks until that time has passed.
  #define uS_TO_TIMER_COMPARE_SLOW(uS) (uS >> 1) //Converts a given number of uS into the required number of timer ticks until that time has passed.
  #if defined(ARDUINO_ARCH_STM32) // STM32GENERIC core
    #include "HardwareTimer.h"
    #define FUEL1_COUNTER (TIM2)->CNT
    #define FUEL2_COUNTER (TIM2)->CNT
    #define FUEL3_COUNTER (TIM2)->CNT
    #define FUEL4_COUNTER (TIM2)->CNT
    #define FUEL5_COUNTER (TIM2)->CNT

    #define IGN1_COUNTER  (TIM3)->CNT
    #define IGN2_COUNTER  (TIM3)->CNT
    #define IGN3_COUNTER  (TIM3)->CNT
    #define IGN4_COUNTER  (TIM3)->CNT
    #define IGN5_COUNTER  (TIM3)->CNT

    #define FUEL1_COMPARE (TIM2)->CCR1
    #define FUEL2_COMPARE (TIM2)->CCR2
    #define FUEL3_COMPARE (TIM2)->CCR3
    #define FUEL4_COMPARE (TIM2)->CCR4

    #define IGN1_COMPARE (TIM3)->CCR1
    #define IGN2_COMPARE (TIM3)->CCR2
    #define IGN3_COMPARE (TIM3)->CCR3
    #define IGN4_COMPARE (TIM3)->CCR4
    #define IGN5_COMPARE (TIM3)->CCR1

    //https://github.com/rogerclarkmelbourne/Arduino_STM32/blob/754bc2969921f1ef262bd69e7faca80b19db7524/STM32F1/system/libmaple/include/libmaple/timer.h#L444
    #define FUEL1_TIMER_ENABLE() (TIM2)->CCER |= TIM_CCER_CC1E
    #define FUEL2_TIMER_ENABLE() (TIM2)->CCER |= TIM_CCER_CC2E
    #define FUEL3_TIMER_ENABLE() (TIM2)->CCER |= TIM_CCER_CC3E
    #define FUEL4_TIMER_ENABLE() (TIM2)->CCER |= TIM_CCER_CC4E

    #define IGN1_TIMER_ENABLE() (TIM3)->CCER |= TIM_CCER_CC1E
    #define IGN2_TIMER_ENABLE() (TIM3)->CCER |= TIM_CCER_CC2E
    #define IGN3_TIMER_ENABLE() (TIM3)->CCER |= TIM_CCER_CC3E
    #define IGN4_TIMER_ENABLE() (TIM3)->CCER |= TIM_CCER_CC4E
    #define IGN5_TIMER_ENABLE() (TIM1)->CCER |= TIM_CCER_CC1E

    #define FUEL1_TIMER_DISABLE() (TIM2)->CCER &= ~TIM_CCER_CC1E
    #define FUEL2_TIMER_DISABLE() (TIM2)->CCER &= ~TIM_CCER_CC2E
    #define FUEL3_TIMER_DISABLE() (TIM2)->CCER &= ~TIM_CCER_CC3E
    #define FUEL4_TIMER_DISABLE() (TIM2)->CCER &= ~TIM_CCER_CC4E

    #define IGN1_TIMER_DISABLE() (TIM3)->CCER &= ~TIM_CCER_CC1E
    #define IGN2_TIMER_DISABLE() (TIM3)->CCER &= ~TIM_CCER_CC2E
    #define IGN3_TIMER_DISABLE() (TIM3)->CCER &= ~TIM_CCER_CC3E
    #define IGN4_TIMER_DISABLE() (TIM3)->CCER &= ~TIM_CCER_CC4E
    #define IGN5_TIMER_DISABLE() (TIM1)->CCER &= ~TIM_CCER_CC1E
  #else //libmaple core aka STM32DUINO
    #define FUEL1_COUNTER (TIMER2->regs).gen->CNT
    #define FUEL2_COUNTER (TIMER2->regs).gen->CNT
    #define FUEL3_COUNTER (TIMER2->regs).gen->CNT
    #define FUEL4_COUNTER (TIMER2->regs).gen->CNT
    #define FUEL5_COUNTER (TIMER1->regs).gen->CNT

    #define IGN1_COUNTER  (TIMER3->regs).gen->CNT
    #define IGN2_COUNTER  (TIMER3->regs).gen->CNT
    #define IGN3_COUNTER  (TIMER3->regs).gen->CNT
    #define IGN4_COUNTER  (TIMER3->regs).gen->CNT
    #define IGN5_COUNTER  (TIMER1->regs).gen->CNT

    #define FUEL1_COMPARE (TIMER2->regs).gen->CCR1
    #define FUEL2_COMPARE (TIMER2->regs).gen->CCR2
    #define FUEL3_COMPARE (TIMER2->regs).gen->CCR3
    #define FUEL4_COMPARE (TIMER2->regs).gen->CCR4

    #define IGN1_COMPARE (TIMER3->regs).gen->CCR1
    #define IGN2_COMPARE (TIMER3->regs).gen->CCR2
    #define IGN3_COMPARE (TIMER3->regs).gen->CCR3
    #define IGN4_COMPARE (TIMER3->regs).gen->CCR4
    #define IGN5_COMPARE (TIMER1->regs).gen->CCR1

    //https://github.com/rogerclarkmelbourne/Arduino_STM32/blob/754bc2969921f1ef262bd69e7faca80b19db7524/STM32F1/system/libmaple/include/libmaple/timer.h#L444
    #define FUEL1_TIMER_ENABLE() (TIMER2->regs).gen->CCER |= TIMER_CCER_CC1E
    #define FUEL2_TIMER_ENABLE() (TIMER2->regs).gen->CCER |= TIMER_CCER_CC2E
    #define FUEL3_TIMER_ENABLE() (TIMER2->regs).gen->CCER |= TIMER_CCER_CC3E
    #define FUEL4_TIMER_ENABLE() (TIMER2->regs).gen->CCER |= TIMER_CCER_CC4E

    #define IGN1_TIMER_ENABLE() (TIMER3->regs).gen->CCER |= TIMER_CCER_CC1E
    #define IGN2_TIMER_ENABLE() (TIMER3->regs).gen->CCER |= TIMER_CCER_CC2E
    #define IGN3_TIMER_ENABLE() (TIMER3->regs).gen->CCER |= TIMER_CCER_CC3E
    #define IGN4_TIMER_ENABLE() (TIMER3->regs).gen->CCER |= TIMER_CCER_CC4E
    #define IGN5_TIMER_ENABLE() (TIMER1->regs).gen->CCER |= TIMER_CCER_CC1E

    #define FUEL1_TIMER_DISABLE() (TIMER2->regs).gen->CCER &= ~TIMER_CCER_CC1E
    #define FUEL2_TIMER_DISABLE() (TIMER2->regs).gen->CCER &= ~TIMER_CCER_CC2E
    #define FUEL3_TIMER_DISABLE() (TIMER2->regs).gen->CCER &= ~TIMER_CCER_CC3E
    #define FUEL4_TIMER_DISABLE() (TIMER2->regs).gen->CCER &= ~TIMER_CCER_CC4E

    #define IGN1_TIMER_DISABLE() (TIMER3->regs).gen->CCER &= ~TIMER_CCER_CC1E
    #define IGN2_TIMER_DISABLE() (TIMER3->regs).gen->CCER &= ~TIMER_CCER_CC2E
    #define IGN3_TIMER_DISABLE() (TIMER3->regs).gen->CCER &= ~TIMER_CCER_CC3E
    #define IGN4_TIMER_DISABLE() (TIMER3->regs).gen->CCER &= ~TIMER_CCER_CC4E
    #define IGN5_TIMER_DISABLE() (TIMER1->regs).gen->CCER &= ~TIMER_CCER_CC1E
  #endif
#endif

void initialiseSchedulers();
void setFuelSchedule1(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setFuelSchedule2(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setFuelSchedule3(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setFuelSchedule4(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setFuelSchedule5(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setFuelSchedule6(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setFuelSchedule7(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setFuelSchedule8(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setIgnitionSchedule1(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setIgnitionSchedule2(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setIgnitionSchedule3(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setIgnitionSchedule4(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setIgnitionSchedule5(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setIgnitionSchedule6(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setIgnitionSchedule7(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());
void setIgnitionSchedule8(void (*startCallback)(), unsigned long timeout, unsigned long duration, void(*endCallback)());

static inline void refreshIgnitionSchedule1(unsigned long timeToEnd) __attribute__((always_inline));

//Needed for STM32 interrupt handlers
#if defined(CORE_STM32)
  static inline void fuelSchedule1Interrupt();
  static inline void fuelSchedule2Interrupt();
  static inline void fuelSchedule3Interrupt();
  static inline void fuelSchedule4Interrupt();
  static inline void fuelSchedule5Interrupt();
  static inline void ignitionSchedule1Interrupt();
  static inline void ignitionSchedule2Interrupt();
  static inline void ignitionSchedule3Interrupt();
  static inline void ignitionSchedule4Interrupt();
  static inline void ignitionSchedule5Interrupt();
#endif

enum ScheduleStatus {OFF, PENDING, STAGED, RUNNING}; //The 3 statuses that a schedule can have

struct Schedule {
  volatile unsigned long duration;
  volatile ScheduleStatus Status;
  volatile byte schedulesSet; //A counter of how many times the schedule has been set
  void (*StartCallback)(); //Start Callback function for schedule
  void (*EndCallback)(); //Start Callback function for schedule
  volatile unsigned long startTime; //The system time (in uS) that the schedule started
  volatile unsigned int startCompare; //The counter value of the timer when this will start
  volatile unsigned int endCompare;

  unsigned int nextStartCompare;
  unsigned int nextEndCompare;
  volatile bool hasNextSchedule = false;
};

volatile Schedule *timer3Aqueue[4];
Schedule *timer3Bqueue[4];
Schedule *timer3Cqueue[4];

Schedule fuelSchedule1;
Schedule fuelSchedule2;
Schedule fuelSchedule3;
Schedule fuelSchedule4;
Schedule fuelSchedule5;
Schedule fuelSchedule6;
Schedule fuelSchedule7;
Schedule fuelSchedule8;
Schedule ignitionSchedule1;
Schedule ignitionSchedule2;
Schedule ignitionSchedule3;
Schedule ignitionSchedule4;
Schedule ignitionSchedule5;
Schedule ignitionSchedule6;
Schedule ignitionSchedule7;
Schedule ignitionSchedule8;

Schedule nullSchedule; //This is placed at the end of the queue. It's status will always be set to OFF and hence will never perform any action within an ISR

static inline unsigned int setQueue(volatile Schedule *queue[], Schedule *schedule1, Schedule *schedule2, unsigned int CNT)
{
  //Create an array of all the upcoming targets, relative to the current count on the timer
  unsigned int tmpQueue[4];

  //Set the initial queue state. This order matches the tmpQueue order
  if(schedule1->Status == OFF)
  {
    queue[0] = schedule2;
    queue[1] = schedule2;
    tmpQueue[0] = schedule2->startCompare - CNT;
    tmpQueue[1] = schedule2->endCompare - CNT;
  }
  else
  {
    queue[0] = schedule1;
    queue[1] = schedule1;
    tmpQueue[0] = schedule1->startCompare - CNT;
    tmpQueue[1] = schedule1->endCompare - CNT;
  }

  if(schedule2->Status == OFF)
  {
    queue[2] = schedule1;
    queue[3] = schedule1;
    tmpQueue[2] = schedule1->startCompare - CNT;
    tmpQueue[3] = schedule1->endCompare - CNT;
  }
  else
  {
    queue[2] = schedule2;
    queue[3] = schedule2;
    tmpQueue[2] = schedule2->startCompare - CNT;
    tmpQueue[3] = schedule2->endCompare - CNT;
  }


  //Sort the queues. Both queues are kept in sync.
  //This implementes a sorting networking based on the Bose-Nelson sorting network
  //See: http://pages.ripco.net/~jgamble/nw.html
  #define SWAP(x,y) if(tmpQueue[y] < tmpQueue[x]) { unsigned int tmp = tmpQueue[x]; tmpQueue[x] = tmpQueue[y]; tmpQueue[y] = tmp; volatile Schedule *tmpS = queue[x]; queue[x] = queue[y]; queue[y] = tmpS; }
  //SWAP(0, 1); //Likely not needed
  //SWAP(2, 3); //Likely not needed
  SWAP(0, 2);
  SWAP(1, 3);
  SWAP(1, 2);

  //Return the next compare time in the queue
  return tmpQueue[0] + CNT; //Return the
}

/*
 * Moves all the Schedules in a queue forward one position.
 * The current item (0) is discarded
 * The final queue slot is set to nullSchedule to indicate that no action should be taken
 */
static inline unsigned int popQueue(volatile Schedule *queue[])
{
  queue[0] = queue[1];
  queue[1] = queue[2];
  queue[2] = queue[3];
  queue[3] = &nullSchedule;

  unsigned int returnCompare;
  if( queue[0]->Status == PENDING ) { returnCompare = queue[0]->startCompare; }
  else { returnCompare = queue[0]->endCompare; }

  return returnCompare;
}


#endif // SCHEDULER_H
