#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>
#include "table.h"
#include <assert.h>

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
  #define BOARD_DIGITAL_GPIO_PINS 54
  #define BOARD_NR_GPIO_PINS 62
  #define LED_BUILTIN 46 //ECU_STM2
  #define CORE_AVR
  #define BOARD_H "board_avr2560.h"
  #define INJ_CHANNELS 4
  #define IGN_CHANNELS 5

  //#define TIMER5_MICROS

#elif defined(CORE_TEENSY)
  #if defined(__MK64FX512__) || defined(__MK66FX1M0__)
    #define CORE_TEENSY35
    #define BOARD_H "board_teensy35.h"
  #elif defined(__IMXRT1062__)
    #define CORE_TEENSY40
    #define BOARD_H "board_teensy40.h"
  #endif
  #define INJ_CHANNELS 8
  #define IGN_CHANNELS 8

#elif defined(STM32_MCU_SERIES) || defined(ARDUINO_ARCH_STM32) || defined(__STM32F1__) || defined(STM32F4) || defined(STM32)
  //These should be updated to 8 later, but there's bits missing currently
  #define INJ_CHANNELS 4
  #define IGN_CHANNELS 5

  #ifndef word
    #define word(h, l) ((h << 8) | l) //word() function not defined for this platform in the main library
  #endif
  #if defined (STM32F1) || defined(__STM32F1__)
    #define BOARD_DIGITAL_GPIO_PINS 34
    #define BOARD_NR_GPIO_PINS 34
    #ifndef LED_BUILTIN
      #define LED_BUILTIN PB1 //Maple Mini
    #endif
  #elif defined(ARDUINO_BLACK_F407VE) || defined(STM32F4)
    #define BOARD_DIGITAL_GPIO_PINS 80
    #define BOARD_NR_GPIO_PINS 80
  #endif

  #if defined(CORE_STM32_OFFICIAL)
    //Need to identify the official core better
    //#define CORE_STM32_OFFICIAL
    #define BOARD_H "board_stm32_official.h"
  #else
    #define CORE_STM32_GENERIC
    #define BOARD_H "board_stm32_generic.h"
  #endif

  //Specific mode for Bluepill due to its small flash size. This disables a number of strings from being compiled into the flash
  #if defined(MCU_STM32F103C8) || defined(MCU_STM32F103CB)
    #define SMALL_FLASH_MODE
  #endif

  #if __GNUC__ < 7 //Already included on GCC 7
  extern "C" char* sbrk(int incr); //Used to freeRam
  #endif
  #if !defined(_VARIANT_ARDUINO_STM32_) // STM32GENERIC core
    inline unsigned char  digitalPinToInterrupt(unsigned char Interrupt_pin) { return Interrupt_pin; } //This isn't included in the stm32duino libs (yet)
  #else //libmaple core aka STM32DUINO
    //These are defined in STM32F1/variants/generic_stm32f103c/variant.h but return a non byte* value
    #ifndef portOutputRegister
      #define portOutputRegister(port) ((volatile byte *)( &((port)->regs->ODR) ))
      #define portInputRegister(port) ((volatile byte *)( &((port)->regs->IDR) ))
    #endif
  #endif
#elif defined(__SAMD21G18A__)
  #define BOARD_H "board_samd21.h"
  #define CORE_SAMD21
#else
  #error Incorrect board selected. Please select the correct board (Usually Mega 2560) and upload again
#endif

//This can only be included after the above section
#include BOARD_H //Note that this is not a real file, it is defined in globals.h. 

//Handy bitsetting macros
#define BIT_SET(a,b) ((a) |= (1U<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1U<<(b)))
#define BIT_CHECK(var,pos) !!((var) & (1U<<(pos)))

#define interruptSafe(c) (noInterrupts(); {c} interrupts();) //Wraps any code between nointerrupt and interrupt calls

#define MS_IN_MINUTE 60000
#define US_IN_MINUTE 60000000

//Define the load algorithm
#define LOAD_SOURCE_MAP         0
#define LOAD_SOURCE_TPS         1
#define LOAD_SOURCE_IMAPEMAP    2

//Define bit positions within engine virable
#define BIT_ENGINE_RUN      0   // Engine running
#define BIT_ENGINE_CRANK    1   // Engine cranking
#define BIT_ENGINE_ASE      2   // after start enrichment (ASE)
#define BIT_ENGINE_WARMUP   3   // Engine in warmup
#define BIT_ENGINE_ACC      4   // in acceleration mode (TPS accel)
#define BIT_ENGINE_DCC      5   // in deceleration mode
#define BIT_ENGINE_MAPACC   6   // MAP acceleration mode
#define BIT_ENGINE_MAPDCC   7   // MAP decelleration mode

//Define masks for Status1
#define BIT_STATUS1_INJ1           0  //inj1
#define BIT_STATUS1_INJ2           1  //inj2
#define BIT_STATUS1_INJ3           2  //inj3
#define BIT_STATUS1_INJ4           3  //inj4
#define BIT_STATUS1_DFCO           4  //Decelleration fuel cutoff
#define BIT_STATUS1_BOOSTCUT       5  //Fuel component of MAP based boost cut out
#define BIT_STATUS1_TOOTHLOG1READY 6  //Used to flag if tooth log 1 is ready
#define BIT_STATUS1_TOOTHLOG2READY 7  //Used to flag if tooth log 2 is ready (Log is not currently used)

//Define masks for spark variable
#define BIT_SPARK_HLAUNCH         0  //Hard Launch indicator
#define BIT_SPARK_SLAUNCH         1  //Soft Launch indicator
#define BIT_SPARK_HRDLIM          2  //Hard limiter indicator
#define BIT_SPARK_SFTLIM          3  //Soft limiter indicator
#define BIT_SPARK_BOOSTCUT        4  //Spark component of MAP based boost cut out
#define BIT_SPARK_ERROR           5  // Error is detected
#define BIT_SPARK_IDLE            6  // idle on
#define BIT_SPARK_SYNC            7  // Whether engine has sync or not

#define BIT_SPARK2_FLATSH         0  //Flat shift hard cut
#define BIT_SPARK2_FLATSS         1  //Flat shift soft cut
#define BIT_SPARK2_UNUSED3        2
#define BIT_SPARK2_UNUSED4        3
#define BIT_SPARK2_UNUSED5        4
#define BIT_SPARK2_UNUSED6        5
#define BIT_SPARK2_UNUSED7        6
#define BIT_SPARK2_UNUSED8        7

#define BIT_TIMER_1HZ             0
#define BIT_TIMER_4HZ             1
#define BIT_TIMER_10HZ            2
#define BIT_TIMER_15HZ            3
#define BIT_TIMER_30HZ            4

#define BIT_STATUS3_RESET_PREVENT 0 //Indicates whether reset prevention is enabled
#define BIT_STATUS3_NITROUS       1
#define BIT_STATUS3_FUEL2_ACTIVE  2
#define BIT_STATUS3_UNUSED3       3
#define BIT_STATUS3_UNUSED4       4
#define BIT_STATUS3_NSQUIRTS1     5
#define BIT_STATUS3_NSQUIRTS2     6
#define BIT_STATUS3_NSQUIRTS3     7

#define VALID_MAP_MAX 1022 //The largest ADC value that is valid for the MAP sensor
#define VALID_MAP_MIN 2 //The smallest ADC value that is valid for the MAP sensor

#define TOOTH_LOG_SIZE      64
#define TOOTH_LOG_BUFFER    128 //256

#define COMPOSITE_LOG_PRI   0
#define COMPOSITE_LOG_SEC   1
#define COMPOSITE_LOG_TRIG  2
#define COMPOSITE_LOG_SYNC  3

#define INJ_PAIRED          0
#define INJ_SEMISEQUENTIAL  1
#define INJ_BANKED          2
#define INJ_SEQUENTIAL      3

#define IGN_MODE_WASTED     0
#define IGN_MODE_SINGLE     1
#define IGN_MODE_WASTEDCOP  2
#define IGN_MODE_SEQUENTIAL 3
#define IGN_MODE_ROTARY     4

#define SEC_TRIGGER_SINGLE  0
#define SEC_TRIGGER_4_1     1

#define ROTARY_IGN_FC       0
#define ROTARY_IGN_FD       1
#define ROTARY_IGN_RX8      2

#define BOOST_MODE_SIMPLE   0
#define BOOST_MODE_FULL     1

#define HARD_CUT_FULL       0
#define HARD_CUT_ROLLING    1

#define SIZE_BYTE           8
#define SIZE_INT            16

#define EVEN_FIRE           0
#define ODD_FIRE            1

#define EGO_ALGORITHM_SIMPLE  0
#define EGO_ALGORITHM_PID     2

#define STAGING_MODE_TABLE  0
#define STAGING_MODE_AUTO   1

#define NITROUS_OFF         0
#define NITROUS_STAGE1      1
#define NITROUS_STAGE2      2

#define AE_MODE_TPS         0
#define AE_MODE_MAP         1

#define KNOCK_MODE_OFF      0
#define KNOCK_MODE_DIGITAL  1
#define KNOCK_MODE_ANALOG   2

#define FUEL2_MODE_OFF      0
#define FUEL2_MODE_MULTIPLY 1
#define FUEL2_MODE_ADD      2
#define FUEL2_MODE_CONDITIONAL_SWITCH   3
#define FUEL2_MODE_INPUT_SWITCH 4

#define FUEL2_CONDITION_RPM 0
#define FUEL2_CONDITION_MAP 1
#define FUEL2_CONDITION_TPS 2
#define FUEL2_CONDITION_ETH 3

#define RESET_CONTROL_DISABLED             0
#define RESET_CONTROL_PREVENT_WHEN_RUNNING 1
#define RESET_CONTROL_PREVENT_ALWAYS       2
#define RESET_CONTROL_SERIAL_COMMAND       3

#define OPEN_LOOP_BOOST     0
#define CLOSED_LOOP_BOOST   1


#define VVT_MODE_ONOFF      0
#define VVT_MODE_OPEN_LOOP  1
#define VVT_MODE_CLOSED_LOOP 2
#define VVT_LOAD_MAP      0
#define VVT_LOAD_TPS      1

#define FOUR_STROKE         0
#define TWO_STROKE          1

#define MAX_RPM 18000 //This is the maximum rpm that the ECU will attempt to run at. It is NOT related to the rev limiter, but is instead dictates how fast certain operations will be allowed to run. Lower number gives better performance

//Table sizes
#define CALIBRATION_TABLE_SIZE 512
#define CALIBRATION_TEMPERATURE_OFFSET 40 // All temperature measurements are stored offset by 40 degrees. This is so we can use an unsigned byte (0-255) to represent temperature ranges from -40 to 215
#define OFFSET_FUELTRIM 127 //The fuel trim tables are offset by 128 to allow for -128 to +128 values
#define OFFSET_IGNITION 40 //Ignition values from the main spark table are offset 40 degrees downards to allow for negative spark timing

#define SERIAL_BUFFER_THRESHOLD 32 // When the serial buffer is filled to greater than this threshold value, the serial processing operations will be performed more urgently in order to avoid it overflowing. Serial buffer is 64 bytes long, so the threshold is set at half this as a reasonable figure

#define FUEL_PUMP_ON() *pump_pin_port |= (pump_pin_mask)
#define FUEL_PUMP_OFF() *pump_pin_port &= ~(pump_pin_mask)

const char TSfirmwareVersion[] PROGMEM = "Speeduino";

const byte data_structure_version = 2; //This identifies the data structure when reading / writing.
#define NUM_PAGES     12
const uint16_t npage_size[NUM_PAGES] = {0,128,288,288,128,288,128,240,192,192,192,288}; /**< This array stores the size (in bytes) of each configuration page */
#define MAP_PAGE_SIZE 288

struct table3D fuelTable; //16x16 fuel map
struct table3D fuelTable2; //16x16 fuel map
struct table3D ignitionTable; //16x16 ignition map
struct table3D afrTable; //16x16 afr target map
struct table3D stagingTable; //8x8 fuel staging table
struct table3D boostTable; //8x8 boost map
struct table3D vvtTable; //8x8 vvt map
struct table3D trim1Table; //6x6 Fuel trim 1 map
struct table3D trim2Table; //6x6 Fuel trim 2 map
struct table3D trim3Table; //6x6 Fuel trim 3 map
struct table3D trim4Table; //6x6 Fuel trim 4 map
struct table2D taeTable; //4 bin TPS Acceleration Enrichment map (2D)
struct table2D maeTable;
struct table2D WUETable; //10 bin Warm Up Enrichment map (2D)
struct table2D ASETable; //4 bin After Start Enrichment map (2D)
struct table2D ASECountTable; //4 bin After Start duration map (2D)
struct table2D PrimingPulseTable; //4 bin Priming pulsewidth map (2D)
struct table2D crankingEnrichTable; //4 bin cranking Enrichment map (2D)
struct table2D dwellVCorrectionTable; //6 bin dwell voltage correction (2D)
struct table2D injectorVCorrectionTable; //6 bin injector voltage correction (2D)
struct table2D IATDensityCorrectionTable; //9 bin inlet air temperature density correction (2D)
struct table2D IATRetardTable; //6 bin ignition adjustment based on inlet air temperature  (2D)
struct table2D IDLEAdvanceTable; //6 bin idle advance adjustment table based on RPM difference  (2D)
struct table2D CLTAdvanceTable; //6 bin ignition adjustment based on coolant temperature  (2D)
struct table2D rotarySplitTable; //8 bin ignition split curve for rotary leading/trailing  (2D)
struct table2D flexFuelTable;  //6 bin flex fuel correction table for fuel adjustments (2D)
struct table2D flexAdvTable;   //6 bin flex fuel correction table for timing advance (2D)
struct table2D flexBoostTable; //6 bin flex fuel correction table for boost adjustments (2D)
struct table2D knockWindowStartTable;
struct table2D knockWindowDurationTable;

//These are for the direct port manipulation of the injectors, coils and aux outputs
volatile PORT_TYPE *inj1_pin_port;
volatile PINMASK_TYPE inj1_pin_mask;
volatile PORT_TYPE *inj2_pin_port;
volatile PINMASK_TYPE inj2_pin_mask;
volatile PORT_TYPE *inj3_pin_port;
volatile PINMASK_TYPE inj3_pin_mask;
volatile PORT_TYPE *inj4_pin_port;
volatile PINMASK_TYPE inj4_pin_mask;
volatile PORT_TYPE *inj5_pin_port;
volatile PINMASK_TYPE inj5_pin_mask;
volatile PORT_TYPE *inj6_pin_port;
volatile PINMASK_TYPE inj6_pin_mask;
volatile PORT_TYPE *inj7_pin_port;
volatile PINMASK_TYPE inj7_pin_mask;
volatile PORT_TYPE *inj8_pin_port;
volatile PINMASK_TYPE inj8_pin_mask;

volatile PORT_TYPE *ign1_pin_port;
volatile PINMASK_TYPE ign1_pin_mask;
volatile PORT_TYPE *ign2_pin_port;
volatile PINMASK_TYPE ign2_pin_mask;
volatile PORT_TYPE *ign3_pin_port;
volatile PINMASK_TYPE ign3_pin_mask;
volatile PORT_TYPE *ign4_pin_port;
volatile PINMASK_TYPE ign4_pin_mask;
volatile PORT_TYPE *ign5_pin_port;
volatile PINMASK_TYPE ign5_pin_mask;
volatile PORT_TYPE *ign6_pin_port;
volatile PINMASK_TYPE ign6_pin_mask;
volatile PORT_TYPE *ign7_pin_port;
volatile PINMASK_TYPE ign7_pin_mask;
volatile PORT_TYPE *ign8_pin_port;
volatile PINMASK_TYPE ign8_pin_mask;

volatile PORT_TYPE *tach_pin_port;
volatile PINMASK_TYPE tach_pin_mask;
volatile PORT_TYPE *pump_pin_port;
volatile PINMASK_TYPE pump_pin_mask;

volatile PORT_TYPE *triggerPri_pin_port;
volatile PINMASK_TYPE triggerPri_pin_mask;
volatile PORT_TYPE *triggerSec_pin_port;
volatile PINMASK_TYPE triggerSec_pin_mask;

//These need to be here as they are used in both speeduino.ino and scheduler.ino
bool channel1InjEnabled = true;
bool channel2InjEnabled = false;
bool channel3InjEnabled = false;
bool channel4InjEnabled = false;
bool channel5InjEnabled = false;
bool channel6InjEnabled = false;
bool channel7InjEnabled = false;
bool channel8InjEnabled = false;

int ignition1EndAngle = 0;
int ignition2EndAngle = 0;
int ignition3EndAngle = 0;
int ignition4EndAngle = 0;
int ignition5EndAngle = 0;

//These are variables used across multiple files
bool initialisationComplete = false; //Tracks whether the setup() function has run completely
byte fpPrimeTime = 0; //The time (in seconds, based on currentStatus.secl) that the fuel pump started priming
volatile uint16_t mainLoopCount;
unsigned long revolutionTime; //The time in uS that one revolution would take at current speed (The time tooth 1 was last seen, minus the time it was seen prior to that)
volatile unsigned long timer5_overflow_count = 0; //Increments every time counter 5 overflows. Used for the fast version of micros()
volatile unsigned long ms_counter = 0; //A counter that increments once per ms
uint16_t fixedCrankingOverride = 0;
bool clutchTrigger;
bool previousClutchTrigger;
volatile uint32_t toothHistory[TOOTH_LOG_BUFFER];
volatile uint8_t compositeLogHistory[TOOTH_LOG_BUFFER];
volatile bool fpPrimed = false; //Tracks whether or not the fuel pump priming has been completed yet
volatile unsigned int toothHistoryIndex = 0;
volatile byte toothHistorySerialIndex = 0;

  byte primaryTriggerEdge;
  byte secondaryTriggerEdge;

int CRANK_ANGLE_MAX = 720;
int CRANK_ANGLE_MAX_IGN = 360;
int CRANK_ANGLE_MAX_INJ = 360; //The number of crank degrees that the system track over. 360 for wasted / timed batch and 720 for sequential
  

//This needs to be here because using the config page directly can prevent burning the setting
byte resetControl = RESET_CONTROL_DISABLED;

volatile byte TIMER_mask;
volatile byte LOOP_TIMER;

//The status struct contains the current values for all 'live' variables
//In current version this is 64 bytes
struct statuses {
  volatile bool hasSync;
  uint16_t RPM;
  long longRPM;
  int mapADC;
  int baroADC;
  long MAP; //Has to be a long for PID calcs (Boost control)
  int16_t EMAP;
  int16_t EMAPADC;
  byte baro; //Barometric pressure is simply the inital MAP reading, taken before the engine is running. Alternatively, can be taken from an external sensor
  byte TPS; /**< The current TPS reading (0% - 100%). Is the tpsADC value after the calibration is applied */
  byte tpsADC; /**< 0-255 byte representation of the TPS. Downsampled from the original 10-bit reading, but before any calibration is applied */
  byte tpsDOT; /**< TPS delta over time. Measures the % per second that the TPS is changing. Value is divided by 10 to be stored in a byte */
  byte mapDOT; /**< MAP delta over time. Measures the kpa per second that the MAP is changing. Value is divided by 10 to be stored in a byte */
  volatile int rpmDOT;
  byte VE; /**< The current VE value being used in the fuel calculation. Can be the same as VE1 or VE2, or a calculated value of both */
  byte VE1; /**< The VE value from fuel table 1 */
  byte VE2; /**< The VE value from fuel table 2, if in use (and required conditions are met) */
  byte O2;
  byte O2_2;
  int coolant;
  int cltADC;
  int IAT;
  int iatADC;
  int batADC;
  int O2ADC;
  int O2_2ADC;
  int dwell;
  byte dwellCorrection; /**< The amount of correction being applied to the dwell time. */
  byte battery10; /**< The current BRV in volts (multiplied by 10. Eg 12.5V = 125) */
  int8_t advance; /**< Signed 8 bit as advance can now go negative (ATDC) */
  byte corrections; /**< The total current corrections % amount */
  int16_t AEamount; /**< The amount of accleration enrichment currently being applied */
  byte egoCorrection; /**< The amount of closed loop AFR enrichment currently being applied */
  byte wueCorrection; /**< The amount of warmup enrichment currently being applied */
  byte batCorrection; /**< The amount of battery voltage enrichment currently being applied */
  byte iatCorrection; /**< The amount of inlet air temperature adjustment currently being applied */
  byte launchCorrection; /**< The amount of correction being applied if launch control is active */
  byte flexCorrection; /**< Amount of correction being applied to compensate for ethanol content */
  int8_t flexIgnCorrection; /**< Amount of additional advance being applied based on flex. Note the type as this allows for negative values */
  byte afrTarget;
  byte idleDuty; /**< The current idle duty cycle amount if PWM idle is selected and active */
  byte CLIdleTarget; /**< The target idle RPM (when closed loop idle control is active) */
  bool idleUpActive; /**< Whether the externally controlled idle up is currently active */
  bool fanOn; /**< Whether or not the fan is turned on */
  volatile byte ethanolPct; /**< Ethanol reading (if enabled). 0 = No ethanol, 100 = pure ethanol. Eg E85 = 85. */
  unsigned long AEEndTime; /**< The target end time used whenever AE is turned on */
  volatile byte status1;
  volatile byte spark;
  volatile byte spark2;
  uint8_t engine;
  unsigned int PW1; //In uS
  unsigned int PW2; //In uS
  unsigned int PW3; //In uS
  unsigned int PW4; //In uS
  unsigned int PW5; //In uS
  unsigned int PW6; //In uS
  unsigned int PW7; //In uS
  unsigned int PW8; //In uS
  volatile byte runSecs; /**< Counter of seconds since cranking commenced (overflows at 255 obviously) */
  volatile byte secl; /**< Counter incrementing once per second. Will overflow after 255 and begin again. This is used by TunerStudio to maintain comms sync */
  volatile uint32_t loopsPerSecond; /**< A performance indicator showing the number of main loops that are being executed each second */ 
  bool launchingSoft; /**< Indicator showing whether soft launch control adjustments are active */
  bool launchingHard; /**< Indicator showing whether hard launch control adjustments are active */
  uint16_t freeRAM;
  unsigned int clutchEngagedRPM; /**< The RPM at which the clutch was last depressed. Used for distinguishing between launch control and flat shift */ 
  bool flatShiftingHard;
  volatile uint32_t startRevolutions; /**< A counter for how many revolutions have been completed since sync was achieved. */
  uint16_t boostTarget;
  byte testOutputs;
  bool testActive;
  uint16_t boostDuty; //Percentage value * 100 to give 2 points of precision
  byte idleLoad; /**< Either the current steps or current duty cycle for the idle control. */
  uint16_t canin[16];   //16bit raw value of selected canin data for channel 0-15
  uint8_t current_caninchannel = 0; /**< Current CAN channel, defaults to 0 */
  uint16_t crankRPM = 400; /**< The actual cranking RPM limit. This is derived from the value in the config page, but saves us multiplying it everytime it's used (Config page value is stored divided by 10) */
  volatile byte status3;
  int16_t flexBoostCorrection; /**< Amount of boost added based on flex */
  byte nitrous_status;
  byte nSquirts;
  byte nChannels; /**< Number of fuel and ignition channels.  */
  int16_t fuelLoad;
  int16_t fuelLoad2;
  int16_t ignLoad;
  bool fuelPumpOn; /**< Indicator showing the current status of the fuel pump */
  byte syncLossCounter;
  byte knockRetard;
  bool knockActive;
  bool toothLogEnabled;
  bool compositeLogEnabled;
  //int8_t vvtAngle;
  long vvtAngle;
  byte vvtTargetAngle;
  byte vvtDuty;

};
struct statuses currentStatus; //The global status object

/**
 * @brief This mostly covers off variables that are required for fuel
 * 
 * See the ini file for further reference
 * 
 */
struct config2 {

  byte unused2_0;
  byte unused2_1;
  byte unused2_2;  //Was ASE
  byte aeMode : 2; /**< Acceleration Enrichment mode. 0 = TPS, 1 = MAP. Values 2 and 3 reserved for potential future use (ie blended TPS / MAP) */
  byte unused1_3c : 6;
  byte wueValues[10]; //Warm up enrichment array (10 bytes)
  byte crankingPct; //Cranking enrichment
  byte pinMapping; // The board / ping mapping to be used
  byte tachoPin : 6; //Custom pin setting for tacho output
  byte tachoDiv : 2; //Whether to change the tacho speed
  byte tachoDuration; //The duration of the tacho pulse in mS
  byte maeThresh; /**< The MAPdot threshold that must be exceeded before AE is engaged */
  byte taeThresh; /**< The TPSdot threshold that must be exceeded before AE is engaged */
  byte aeTime;

  //Display config bits
  byte displayType : 3; //21
  byte display1 : 3;
  byte display2 : 2;

  byte display3 : 3;    //22
  byte display4 : 2;
  byte display5 : 3;

  byte displayB1 : 4;   //23
  byte displayB2 : 4;

  byte reqFuel;       //24
  byte divider;
  byte injTiming : 1;
  byte multiplyMAP : 1;
  byte includeAFR : 1;
  byte hardCutType : 1;
  byte ignAlgorithm : 3;
  byte indInjAng : 1;
  byte injOpen; //Injector opening time (ms * 10)
  uint16_t inj1Ang;
  uint16_t inj2Ang;
  uint16_t inj3Ang;
  uint16_t inj4Ang;

  //config1 in ini
  byte mapSample : 2;
  byte strokes : 1;
  byte injType : 1;
  byte nCylinders : 4; //Number of cylinders

  //config2 in ini
  byte fuelAlgorithm : 3;
  byte fixAngEnable : 1; //Whether fixed/locked timing is enabled
  byte nInjectors : 4; //Number of injectors


  //config3 in ini
  byte engineType : 1;
  byte flexEnabled : 1;
  byte legacyMAP  : 1;
  byte baroCorr : 1;
  byte injLayout : 2;
  byte perToothIgn : 1;
  byte dfcoEnabled : 1; //Whether or not DFCO is turned on

  byte unused2_39;  //Was primePulse
  byte dutyLim;
  byte flexFreqLow; //Lowest valid frequency reading from the flex sensor
  byte flexFreqHigh; //Highest valid frequency reading from the flex sensor

  byte boostMaxDuty;
  byte tpsMin;
  byte tpsMax;
  int8_t mapMin; //Must be signed
  uint16_t mapMax;
  byte fpPrime; //Time (In seconds) that the fuel pump should be primed for on power up
  byte stoich;
  uint16_t oddfire2; //The ATDC angle of channel 2 for oddfire
  uint16_t oddfire3; //The ATDC angle of channel 3 for oddfire
  uint16_t oddfire4; //The ATDC angle of channel 4 for oddfire

  byte idleUpPin : 6;
  byte idleUpPolarity : 1;
  byte idleUpEnabled : 1;

  byte idleUpAdder;
  byte aeTaperMin;
  byte aeTaperMax;

  byte iacCLminDuty;
  byte iacCLmaxDuty;
  byte boostMinDuty;

  int8_t baroMin; //Must be signed
  uint16_t baroMax;

  int8_t EMAPMin; //Must be signed
  uint16_t EMAPMax;

  byte fanWhenOff : 1;      // Only run fan when engine is running
  byte fanUnused : 7;
  byte asePct[4];  //Afterstart enrichment (%)
  byte aseCount[4]; //Afterstart enrichment cycles. This is the number of ignition cycles that the afterstart enrichment % lasts for
  byte aseBins[4]; //Afterstart enrichment temp axis
  byte primePulse[4]; //Priming pulsewidth
  byte primeBins[4]; //Priming temp axis
  byte unused2_91[37];

#if defined(CORE_AVR)
  };
#else
  } __attribute__((__packed__)); //The 32 bi systems require all structs to be fully packed
#endif

//Page 4 of the config - See the ini file for further reference
//This mostly covers off variables that are required for ignition
struct config4 {

  int16_t triggerAngle;
  int8_t FixAng; //Negative values allowed
  byte CrankAng;
  byte TrigAngMul; //Multiplier for non evenly divisible tooth counts.

  byte TrigEdge : 1;
  byte TrigSpeed : 1;
  byte IgInv : 1;
  byte TrigPattern : 5;

  byte TrigEdgeSec : 1;
  byte fuelPumpPin : 6;
  byte useResync : 1;

  byte sparkDur; //Spark duration in ms * 10
  byte trigPatternSec; //Mode for Missing tooth secondary trigger.  Either single tooth cam wheel or 4-1
  uint8_t bootloaderCaps; //Capabilities of the bootloader over stock. e.g., 0=Stock, 1=Reset protection, etc.

  byte resetControl : 2; //Which method of reset control to use (0=None, 1=Prevent When Running, 2=Prevent Always, 3=Serial Command)
  byte resetControlPin : 6;

  byte StgCycles; //The number of initial cycles before the ignition should fire when first cranking

  byte boostType : 1; //Open or closed loop boost control
  byte useDwellLim : 1; //Whether the dwell limiter is off or on
  byte sparkMode : 3; //Spark output mode (Eg Wasted spark, single channel or Wasted COP)
  byte triggerFilter : 2; //The mode of trigger filter being used (0=Off, 1=Light (Not currently used), 2=Normal, 3=Aggressive)
  byte ignCranklock : 1; //Whether or not the ignition timing during cranking is locked to a CAS pulse. Only currently valid for Basic distributor and 4G63.

  byte dwellCrank; //Dwell time whilst cranking
  byte dwellRun; //Dwell time whilst running
  byte triggerTeeth; //The full count of teeth on the trigger wheel if there were no gaps
  byte triggerMissingTeeth; //The size of the tooth gap (ie number of missing teeth)
  byte crankRPM; //RPM below which the engine is considered to be cranking
  byte floodClear; //TPS value that triggers flood clear mode (No fuel whilst cranking)
  byte SoftRevLim; //Soft rev limit (RPM/100)
  byte SoftLimRetard; //Amount soft limit retards (degrees)
  byte SoftLimMax; //Time the soft limit can run
  byte HardRevLim; //Hard rev limit (RPM/100)
  byte taeBins[4]; //TPS based acceleration enrichment bins (%/s)
  byte taeValues[4]; //TPS based acceleration enrichment rates (% to add)
  byte wueBins[10]; //Warmup Enrichment bins (Values are in configTable1)
  byte dwellLimit;
  byte dwellCorrectionValues[6]; //Correction table for dwell vs battery voltage
  byte iatRetBins[6]; // Inlet Air Temp timing retard curve bins
  byte iatRetValues[6]; // Inlet Air Temp timing retard curve values
  byte dfcoRPM; //RPM at which DFCO turns off/on at
  byte dfcoHyster; //Hysteris RPM for DFCO
  byte dfcoTPSThresh; //TPS must be below this figure for DFCO to engage

  byte ignBypassEnabled : 1; //Whether or not the ignition bypass is enabled
  byte ignBypassPin : 6; //Pin the ignition bypass is activated on
  byte ignBypassHiLo : 1; //Whether this should be active high or low.

  byte ADCFILTER_TPS;
  byte ADCFILTER_CLT;
  byte ADCFILTER_IAT;
  byte ADCFILTER_O2;
  byte ADCFILTER_BAT;
  byte ADCFILTER_MAP; //This is only used on Instantaneous MAP readings and is intentionally very weak to allow for faster response
  byte ADCFILTER_BARO;
  
  byte cltAdvBins[6]; /**< Coolant Temp timing advance curve bins */
  byte cltAdvValues[6]; /**< Coolant timing advance curve values. These are translated by 15 to allow for negative values */

  byte maeBins[4]; /**< MAP based AE MAPdot bins */
  byte maeRates[4]; /**< MAP based AE values */

  int8_t batVoltCorrect; /**< Battery voltage calibration offset */
  byte unused2_91[36];

#if defined(CORE_AVR)
  };
#else
  } __attribute__((__packed__)); //The 32 bi systems require all structs to be fully packed
#endif

//Page 6 of the config - See the ini file for further reference
//This mostly covers off variables that are required for AFR targets and closed loop
struct config6 {

  byte egoAlgorithm : 2;
  byte egoType : 2;
  byte boostEnabled : 1;
  byte vvtEnabled : 1;
  byte boostCutType : 2;

  byte egoKP;
  byte egoKI;
  byte egoKD;
  byte egoTemp; //The temperature above which closed loop functions
  byte egoCount; //The number of ignition cylces per step
  byte vvtMode : 2; //Valid VVT modes are 'on/off', 'open loop' and 'closed loop'
  byte vvtLoadSource : 2; //Load source for VVT (TPS or MAP)
  byte vvtCLDir : 1; //VVT direction (advance or retard)
  byte vvtCLUseHold : 1; //Whether or not to use a hold duty cycle (Most cases are Yes)
  byte vvtCLAlterFuelTiming : 1;
  byte unused6_6 : 1;
  byte egoLimit; //Maximum amount the closed loop will vary the fueling
  byte ego_min; //AFR must be above this for closed loop to function
  byte ego_max; //AFR must be below this for closed loop to function
  byte ego_sdelay; //Time in seconds after engine starts that closed loop becomes available
  byte egoRPM; //RPM must be above this for closed loop to function
  byte egoTPSMax; //TPS must be below this for closed loop to function
  byte vvtPin : 6;
  byte useExtBaro : 1;
  byte boostMode : 1; //Simple of full boost control
  byte boostPin : 6;
  byte VVTasOnOff : 1; //Whether or not to use the VVT table as an on/off map
  byte useEMAP : 1;
  byte voltageCorrectionBins[6]; //X axis bins for voltage correction tables
  byte injVoltageCorrectionValues[6]; //Correction table for injector PW vs battery voltage
  byte airDenBins[9];
  byte airDenRates[9];
  byte boostFreq; //Frequency of the boost PWM valve
  byte vvtFreq; //Frequency of the vvt PWM valve
  byte idleFreq;

  byte launchPin : 6;
  byte launchEnabled : 1;
  byte launchHiLo : 1;

  byte lnchSoftLim;
  int8_t lnchRetard; //Allow for negative advance value (ATDC)
  byte lnchHardLim;
  byte lnchFuelAdd;

  //PID values for idle needed to go here as out of room in the idle page
  byte idleKP;
  byte idleKI;
  byte idleKD;

  byte boostLimit; //Is divided by 2, allowing kPa values up to 511
  byte boostKP;
  byte boostKI;
  byte boostKD;

  byte lnchPullRes : 2;
  byte fuelTrimEnabled : 1;
  byte flatSEnable : 1;
  byte baroPin : 4;
  byte flatSSoftWin;
  byte flatSRetard;
  byte flatSArm;

  byte iacCLValues[10]; //Closed loop target RPM value
  byte iacOLStepVal[10]; //Open loop step values for stepper motors
  byte iacOLPWMVal[10]; //Open loop duty values for PMWM valves
  byte iacBins[10]; //Temperature Bins for the above 3 curves
  byte iacCrankSteps[4]; //Steps to use when cranking (Stepper motor)
  byte iacCrankDuty[4]; //Duty cycle to use on PWM valves when cranking
  byte iacCrankBins[4]; //Temperature Bins for the above 2 curves

  byte iacAlgorithm : 3; //Valid values are: "None", "On/Off", "PWM", "PWM Closed Loop", "Stepper", "Stepper Closed Loop"
  byte iacStepTime : 3; //How long to pulse the stepper for to ensure the step completes (ms)
  byte iacChannels : 1; //How many outputs to use in PWM mode (0 = 1 channel, 1 = 2 channels)
  byte iacPWMdir : 1; //Direction of the PWM valve. 0 = Normal = Higher RPM with more duty. 1 = Reverse = Lower RPM with more duty

  byte iacFastTemp; //Fast idle temp when using a simple on/off valve

  byte iacStepHome; //When using a stepper motor, the number of steps to be taken on startup to home the motor
  byte iacStepHyster; //Hysteresis temperature (*10). Eg 2.2C = 22

  byte fanInv : 1;        // Fan output inversion bit
  byte fanEnable : 1;     // Fan enable bit. 0=Off, 1=On/Off
  byte fanPin : 6;
  byte fanSP;             // Cooling fan start temperature
  byte fanHyster;         // Fan hysteresis
  byte fanFreq;           // Fan PWM frequency
  byte fanPWMBins[4];     //Temperature Bins for the PWM fan control

#if defined(CORE_AVR)
  };
#else
  } __attribute__((__packed__)); //The 32 bit systems require all structs to be fully packed
#endif

//Page 9 of the config mostly deals with CANBUS control
//See ini file for further info (Config Page 10 in the ini)
struct config9 {
  byte enable_secondarySerial:1;            //enable secondary serial
  byte intcan_available:1;                     //enable internal can module
  byte enable_intcan:1;
  byte caninput_sel[16];                    //bit status on/Can/analog_local/digtal_local if input is enabled
  uint16_t caninput_source_can_address[16];        //u16 [15] array holding can address of input
  uint8_t caninput_source_start_byte[16];     //u08 [15] array holds the start byte number(value of 0-7)
  uint16_t caninput_source_num_bytes;     //u16 bit status of the number of bytes length 1 or 2
  byte unused10_67;
  byte unused10_68;
  byte enable_candata_out : 1;
  byte canoutput_sel[8];
  uint16_t canoutput_param_group[8];
  uint8_t canoutput_param_start_byte[8];
  byte canoutput_param_num_bytes[8];

  byte unused10_110;
  byte unused10_111;
  byte unused10_112;
  byte unused10_113;
  byte speeduino_tsCanId:4;         //speeduino TS canid (0-14)
  uint16_t true_address;            //speeduino 11bit can address
  uint16_t realtime_base_address;   //speeduino 11 bit realtime base address
  uint16_t obd_address;             //speeduino OBD diagnostic address
  uint8_t Auxinpina[16];            //analog  pin number when internal aux in use
  uint8_t Auxinpinb[16];            // digital pin number when internal aux in use

  byte iacStepperInv : 1;  //stepper direction of travel to allow reversing. 0=normal, 1=inverted.
  byte iacCoolTime : 3; // how long to wait for the stepper to cool between steps

  byte unused10_154;
  byte unused10_155;
  byte unused10_156;
  byte unused10_157;
  byte unused10_158;
  byte unused10_159;
  byte unused10_160;
  byte unused10_161;
  byte unused10_162;
  byte unused10_163;
  byte unused10_164;
  byte unused10_165;
  byte unused10_166;
  byte unused10_167;
  byte unused10_168;
  byte unused10_169;
  byte unused10_170;
  byte unused10_171;
  byte unused10_172;
  byte unused10_173;
  byte unused10_174;
  byte unused10_175;
  byte unused10_176;
  byte unused10_177;
  byte unused10_178;
  byte unused10_179;
  byte unused10_180;
  byte unused10_181;
  byte unused10_182;
  byte unused10_183;
  byte unused10_184;
  byte unused10_185;
  byte unused10_186;
  byte unused10_187;
  byte unused10_188;
  byte unused10_189;
  byte unused10_190;
  byte unused10_191;
  
#if defined(CORE_AVR)
  };
#else
  } __attribute__((__packed__)); //The 32 bit systems require all structs to be fully packed
#endif

/*
Page 10 - No specific purpose. Created initially for the cranking enrich curve
192 bytes long
See ini file for further info (Config Page 11 in the ini)
*/
struct config10 {
  byte crankingEnrichBins[4]; //Bytes 0-4
  byte crankingEnrichValues[4]; //Bytes 4-7

  //Byte 8
  byte rotaryType : 2;
  byte stagingEnabled : 1;
  byte stagingMode : 1;
  byte EMAPPin : 4;

  byte rotarySplitValues[8]; //Bytes 9-16
  byte rotarySplitBins[8]; //Bytes 17-24

  uint16_t boostSens; //Bytes 25-26
  byte boostIntv; //Byte 27
  uint16_t stagedInjSizePri; //Bytes 28-29
  uint16_t stagedInjSizeSec; //Bytes 30-31
  byte lnchCtrlTPS; //Byte 32

  uint8_t flexBoostBins[6]; //Byets 33-38
  int16_t flexBoostAdj[6];  //kPa to be added to the boost target @ current ethanol (negative values allowed). Bytes 39-50
  uint8_t flexFuelBins[6]; //Bytes 51-56
  uint8_t flexFuelAdj[6];   //Fuel % @ current ethanol (typically 100% @ 0%, 163% @ 100%). Bytes 57-62
  uint8_t flexAdvBins[6]; //Bytes 63-68
  uint8_t flexAdvAdj[6];    //Additional advance (in degrees) @ current ethanol (typically 0 @ 0%, 10-20 @ 100%). NOTE: THIS SHOULD BE A SIGNED VALUE BUT 2d TABLE LOOKUP NOT WORKING WITH IT CURRENTLY!
                            //And another three corn rows die.
                            //Bytes 69-74

  //Byte 75
  byte n2o_enable : 2;
  byte n2o_arming_pin : 6;
  byte n2o_minCLT; //Byte 76
  byte n2o_maxMAP; //Byte 77
  byte n2o_minTPS; //Byte 78
  byte n2o_maxAFR; //Byte 79

  //Byte 80
  byte n2o_stage1_pin : 6;
  byte n2o_pin_polarity : 1;
  byte n2o_stage1_unused : 1;
  byte n2o_stage1_minRPM; //Byte 81
  byte n2o_stage1_maxRPM; //Byte 82
  byte n2o_stage1_adderMin; //Byte 83
  byte n2o_stage1_adderMax; //Byte 84
  byte n2o_stage1_retard; //Byte 85

  //Byte 86
  byte n2o_stage2_pin : 6;
  byte n2o_stage2_unused : 2;
  byte n2o_stage2_minRPM; //Byte 87
  byte n2o_stage2_maxRPM; //Byte 88
  byte n2o_stage2_adderMin; //Byte 89
  byte n2o_stage2_adderMax; //Byte 90
  byte n2o_stage2_retard; //Byte 91

  //Byte 92
  byte knock_mode : 2;
  byte knock_pin : 6;

  //Byte 93
  byte knock_trigger : 1;
  byte knock_pullup : 1;
  byte knock_limiterDisable : 1;
  byte knock_unused : 2;
  byte knock_count : 3;

  byte knock_threshold; //Byte 94
  byte knock_maxMAP; //Byte 95
  byte knock_maxRPM; //Byte 96
  byte knock_window_rpms[6]; //Bytes 97-102
  byte knock_window_angle[6]; //Bytes 103-108
  byte knock_window_dur[6]; //Bytes 109-114

  byte knock_maxRetard; //Byte 115
  byte knock_firstStep; //Byte 116
  byte knock_stepSize; //Byte 117
  byte knock_stepTime; //Byte 118
        
  byte knock_duration; //Time after knock retard starts that it should start recovering. Byte 119
  byte knock_recoveryStepTime; //Byte 120
  byte knock_recoveryStep; //Byte 121

  //Byte 122
  byte fuel2Algorithm : 3;
  byte fuel2Mode : 3;
  byte fuel2SwitchVariable : 2;
  uint16_t fuel2SwitchValue;

  //Byte 123
  byte fuel2InputPin : 6;
  byte fuel2InputPolarity : 1;
  byte fuel2InputPullup : 1;

  byte vvtCLholdDuty;
  byte vvtCLKP;
  byte vvtCLKI;
  byte vvtCLKD;
  uint16_t vvtCLMinAng;
  uint16_t vvtCLMaxAng;

  byte unused11_123_191[58];

#if defined(CORE_AVR)
  };
#else
  } __attribute__((__packed__)); //The 32 bit systems require all structs to be fully packed
#endif

byte pinInjector1; //Output pin injector 1
byte pinInjector2; //Output pin injector 2
byte pinInjector3; //Output pin injector 3 is on
byte pinInjector4; //Output pin injector 4 is on
byte pinInjector5; //Output pin injector 5 NOT USED YET
byte pinInjector6; //Placeholder only - NOT USED
byte pinInjector7; //Placeholder only - NOT USED
byte pinInjector8; //Placeholder only - NOT USED
byte pinCoil1; //Pin for coil 1
byte pinCoil2; //Pin for coil 2
byte pinCoil3; //Pin for coil 3
byte pinCoil4; //Pin for coil 4
byte pinCoil5; //Pin for coil 5
byte pinCoil6; //Pin for coil 6
byte pinCoil7; //Pin for coil 7
byte pinCoil8; //Pin for coil 8
byte pinTrigger; //The CAS pin
byte pinTrigger2; //The Cam Sensor pin
byte pinTrigger3;	//the 2nd cam sensor pin
byte pinTPS;//TPS input pin
byte pinMAP; //MAP sensor pin
byte pinEMAP; //EMAP sensor pin
byte pinMAP2; //2nd MAP sensor (Currently unused)
byte pinIAT; //IAT sensor pin
byte pinCLT; //CLS sensor pin
byte pinO2; //O2 Sensor pin
byte pinO2_2; //second O2 pin
byte pinBat; //Battery voltage pin
byte pinDisplayReset; // OLED reset pin
byte pinTachOut; //Tacho output
byte pinFuelPump; //Fuel pump on/off
byte pinIdle1; //Single wire idle control
byte pinIdle2; //2 wire idle control (Not currently used)
byte pinIdleUp; //Input for triggering Idle Up
byte pinFuel2Input; //Input for switching to the 2nd fuel table
byte pinSpareTemp1; // Future use only
byte pinSpareTemp2; // Future use only
byte pinSpareOut1; //Generic output
byte pinSpareOut2; //Generic output
byte pinSpareOut3; //Generic output
byte pinSpareOut4; //Generic output
byte pinSpareOut5; //Generic output
byte pinSpareOut6; //Generic output
byte pinSpareHOut1; //spare high current output
byte pinSpareHOut2; // spare high current output
byte pinSpareLOut1; // spare low current output
byte pinSpareLOut2; // spare low current output
byte pinSpareLOut3;
byte pinSpareLOut4;
byte pinSpareLOut5;
byte pinBoost;
byte pinVVT_1;		// vvt output 1
byte pinVVt_2;		// vvt output 2
byte pinFan;       // Cooling fan output
byte pinStepperDir; //Direction pin for the stepper motor driver
byte pinStepperStep; //Step pin for the stepper motor driver
byte pinStepperEnable; //Turning the DRV8825 driver on/off
byte pinLaunch;
byte pinIgnBypass; //The pin used for an ignition bypass (Optional)
byte pinFlex; //Pin with the flex sensor attached
byte pinBaro; //Pin that an external barometric pressure sensor is attached to (If used)
byte pinResetControl; // Output pin used control resetting the Arduino

// global variables // from speeduino.ino
extern struct statuses currentStatus; // from speeduino.ino
extern struct table3D fuelTable; //16x16 fuel map
extern struct table3D fuelTable2; //16x16 fuel map
extern struct table3D ignitionTable; //16x16 ignition map
extern struct table3D afrTable; //16x16 afr target map
extern struct table3D stagingTable; //8x8 afr target map
extern struct table2D taeTable; /**< 4 bin TPS Acceleration Enrichment curve (2D) */
extern struct table2D maeTable; /**< 4 bin MAP based Acceleration Enrichment curve (2D) */
extern struct table2D WUETable; /**< 10 bin Warm Up Enrichment curve (2D) */
extern struct table2D crankingEnrichTable; /**< 4 bin cranking Enrichment curve (2D) */
extern struct config2 configPage2;
extern struct config4 configPage4;
extern struct config6 configPage6;
extern struct config9 configPage9;
extern struct config10 configPage10;
extern unsigned long currentLoopTime; /**< The time (in uS) that the current mainloop started */
extern unsigned long previousLoopTime; /**< The time (in uS) that the previous mainloop started */
volatile uint16_t ignitionCount; /**< The count of ignition events that have taken place since the engine started */
extern byte cltCalibrationTable[CALIBRATION_TABLE_SIZE];
extern byte iatCalibrationTable[CALIBRATION_TABLE_SIZE];
extern byte o2CalibrationTable[CALIBRATION_TABLE_SIZE];

static_assert(sizeof(struct config2) == 128, "configPage2 size is not 128");
static_assert(sizeof(struct config4) == 128, "configPage4 size is not 128");
static_assert(sizeof(struct config6) == 128, "configPage6 size is not 128");
static_assert(sizeof(struct config9) == 192, "configPage9 size is not 192");
static_assert(sizeof(struct config10) == 192, "configPage10 size is not 192");

#endif // GLOBALS_H
