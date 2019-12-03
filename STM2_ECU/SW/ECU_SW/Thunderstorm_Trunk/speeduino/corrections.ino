/*
Speeduino - Simple engine management for the Arduino Mega 2560 platform
Copyright (C) Josh Stewart
A full copy of the license may be found in the projects root directory
*/

/*
The corrections functions in this file affect the fuel pulsewidth (Either increasing or decreasing)
based on factors other than the VE lookup.

These factors include temperature (Warmup Enrichment and After Start Enrichment), Acceleration/Decelleration,
Flood clear mode etc.
*/
//************************************************************************************************************

#include "globals.h"
#include "corrections.h"
#include "timers.h"
#include "maths.h"
#include "sensors.h"
#include "src/PID_v1/PID_v1.h"

long PID_O2, PID_output, PID_AFRTarget;
PID egoPID(&PID_O2, &PID_output, &PID_AFRTarget, configPage6.egoKP, configPage6.egoKI, configPage6.egoKD, REVERSE); //This is the PID object if that algorithm is used. Needs to be global as it maintains state outside of each function call

void initialiseCorrections()
{
  egoPID.SetMode(AUTOMATIC); //Turn O2 PID on
  currentStatus.flexIgnCorrection = 0;
  currentStatus.egoCorrection = 100; //Default value of no adjustment must be set to avoid randomness on first correction cycle after startup
  AFRnextCycle = 0;
  currentStatus.knockActive = false;
}

/*
correctionsTotal() calls all the other corrections functions and combines their results.
This is the only function that should be called from anywhere outside the file
*/
static inline byte correctionsFuel()
{
  #define MAX_CORRECTIONS 3 //The maximum number of corrections allowed before the sum is reprocessed
  uint32_t sumCorrections = 100;
  byte activeCorrections = 0;
  byte result; //temporary variable to store the result of each corrections function

  //The values returned by each of the correction functions are multipled together and then divided back to give a single 0-255 value.
  currentStatus.wueCorrection = correctionWUE();
  if (currentStatus.wueCorrection != 100) { sumCorrections = (sumCorrections * currentStatus.wueCorrection); activeCorrections++; }

  result = correctionASE();
  if (result != 100) { sumCorrections = (sumCorrections * result); activeCorrections++; }

  result = correctionCranking();
  if (result != 100) { sumCorrections = (sumCorrections * result); activeCorrections++; }
  if (activeCorrections == MAX_CORRECTIONS) { sumCorrections = sumCorrections / powint(100,activeCorrections); activeCorrections = 0; } // Need to check this to ensure that sumCorrections doesn't overflow. Can occur when the number of corrections is greater than 3 (Which is 100^4) as 100^5 can overflow

  currentStatus.AEamount = correctionAccel();
  if (currentStatus.AEamount != 100) { sumCorrections = (sumCorrections * currentStatus.AEamount); activeCorrections++; }
  if (activeCorrections == MAX_CORRECTIONS) { sumCorrections = sumCorrections / powint(100,activeCorrections); activeCorrections = 0; }

  result = correctionFloodClear();
  if (result != 100) { sumCorrections = (sumCorrections * result); activeCorrections++; }
  if (activeCorrections == MAX_CORRECTIONS) { sumCorrections = sumCorrections / powint(100,activeCorrections); activeCorrections = 0; }

  currentStatus.egoCorrection = correctionAFRClosedLoop();
  if (currentStatus.egoCorrection != 100) { sumCorrections = (sumCorrections * currentStatus.egoCorrection); activeCorrections++; }
  if (activeCorrections == MAX_CORRECTIONS) { sumCorrections = sumCorrections / powint(100,activeCorrections); activeCorrections = 0; }

  currentStatus.batCorrection = correctionBatVoltage();
  if (currentStatus.batCorrection != 100) { sumCorrections = (sumCorrections * currentStatus.batCorrection); activeCorrections++; }
  if (activeCorrections == MAX_CORRECTIONS) { sumCorrections = sumCorrections / powint(100,activeCorrections); activeCorrections = 0; }

  currentStatus.iatCorrection = correctionIATDensity();
  if (currentStatus.iatCorrection != 100) { sumCorrections = (sumCorrections * currentStatus.iatCorrection); activeCorrections++; }
  if (activeCorrections == MAX_CORRECTIONS) { sumCorrections = sumCorrections / powint(100,activeCorrections); activeCorrections = 0; }

  currentStatus.baroCorrection = correctionBaro();
  if (currentStatus.baroCorrection != 100) { sumCorrections = (sumCorrections * currentStatus.baroCorrection); activeCorrections++; }
  if (activeCorrections == MAX_CORRECTIONS) { sumCorrections = sumCorrections / powint(100,activeCorrections); activeCorrections = 0; }

  currentStatus.flexCorrection = correctionFlex();
  if (currentStatus.flexCorrection != 100) { sumCorrections = (sumCorrections * currentStatus.flexCorrection); activeCorrections++; }
  if (activeCorrections == MAX_CORRECTIONS) { sumCorrections = sumCorrections / powint(100,activeCorrections); activeCorrections = 0; }

  currentStatus.launchCorrection = correctionLaunch();
  if (currentStatus.launchCorrection != 100) { sumCorrections = (sumCorrections * currentStatus.launchCorrection); activeCorrections++; }

  bitWrite(currentStatus.status1, BIT_STATUS1_DFCO, correctionDFCO());
  if ( bitRead(currentStatus.status1, BIT_STATUS1_DFCO) == 1 ) { sumCorrections = 0; }

  sumCorrections = sumCorrections / powint(100,activeCorrections);

  if(sumCorrections > 255) { sumCorrections = 255; } //This is the maximum allowable increase
  return (byte)sumCorrections;
}

/*
Warm Up Enrichment (WUE)
Uses a 2D enrichment table (WUETable) where the X axis is engine temp and the Y axis is the amount of extra fuel to add
*/
static inline byte correctionWUE()
{
  byte WUEValue;
  //Possibly reduce the frequency this runs at (Costs about 50 loops per second)
  //if (currentStatus.coolant > (WUETable.axisX[9] - CALIBRATION_TEMPERATURE_OFFSET))
  if (currentStatus.coolant > (table2D_getAxisValue(&WUETable, 9) - CALIBRATION_TEMPERATURE_OFFSET))
  {
    //This prevents us doing the 2D lookup if we're already up to temp
    BIT_CLEAR(currentStatus.engine, BIT_ENGINE_WARMUP);
    //WUEValue = WUETable.values[9]; //Set the current value to be whatever the final value on the curve is.
    WUEValue = table2D_getRawValue(&WUETable, 9);
  }
  else
  {
    BIT_SET(currentStatus.engine, BIT_ENGINE_WARMUP);
    WUEValue = table2D_getValue(&WUETable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET);
  }

  return WUEValue;
}

/*
Cranking Enrichment
Additional fuel % to be added when the engine is cranking
*/
static inline byte correctionCranking()
{
  byte crankingValue = 100;
  //if ( BIT_CHECK(currentStatus.engine, BIT_ENGINE_CRANK) ) { crankingValue = 100 + configPage2.crankingPct; }
  if ( BIT_CHECK(currentStatus.engine, BIT_ENGINE_CRANK) )
  {
    crankingValue = table2D_getValue(&crankingEnrichTable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET);
  }
  return crankingValue;
}

/*
After Start Enrichment
This is a short period (Usually <20 seconds) immediately after the engine first fires (But not when cranking)
where an additional amount of fuel is added (Over and above the WUE amount)
*/
static inline byte correctionASE()
{
  int16_t ASEValue;
  //Two checks are requiredL:
  //1) Is the engine run time less than the configured ase time
  //2) Make sure we're not still cranking
  if ( (currentStatus.runSecs < (table2D_getValue(&ASECountTable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET))) && !(BIT_CHECK(currentStatus.engine, BIT_ENGINE_CRANK)) )
  {
    BIT_SET(currentStatus.engine, BIT_ENGINE_ASE); //Mark ASE as active.
    ASEValue = 100 + table2D_getValue(&ASETable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET);
  }
  else
  {
    BIT_CLEAR(currentStatus.engine, BIT_ENGINE_ASE); //Mark ASE as inactive.
    ASEValue = 100;
  }

  //Safety checks
  if(ASEValue > 255) { ASEValue = 255; }
  if(ASEValue < 0) { ASEValue = 0; }

  return (byte)ASEValue;
}

/**
 * @brief Acceleration enrichment correction calculation
 * 
 * Calculates the % change of the throttle over time (%/second) and performs a lookup based on this
 * When the enrichment is turned on, it runs at that amount for a fixed period of time (taeTime)
 * 
 * @return int16_t The Acceleration enrichment modifier as a %. 100% = No modification. 
 * As the maximum enrichment amount is +255%, the overall return value from this function can be 100+255=355. Hence this function returns a int16_t rather than byte
 */
static inline int16_t correctionAccel()
{
  int16_t accelValue = 100;
  //First, check whether the accel. enrichment is already running
  if( BIT_CHECK(currentStatus.engine, BIT_ENGINE_ACC) )
  {
    //If it is currently running, check whether it should still be running or whether it's reached it's end time
    if( micros_safe() >= currentStatus.AEEndTime )
    {
      //Time to turn enrichment off
      BIT_CLEAR(currentStatus.engine, BIT_ENGINE_ACC);
      currentStatus.AEamount = 0;
      accelValue = 100;

      //Reset the relevant DOT value to 0
      if(configPage2.aeMode == AE_MODE_MAP) { currentStatus.mapDOT = 0; }
      else if(configPage2.aeMode == AE_MODE_TPS) { currentStatus.tpsDOT = 0; }
    }
    else
    {
      //Enrichment still needs to keep running. Simply return the total TAE amount
      accelValue = currentStatus.AEamount;
    }
  }
  else
  {
    if(configPage2.aeMode == AE_MODE_MAP)
    {
      int16_t MAP_change = (currentStatus.MAP - MAPlast);

      if (MAP_change <= 2)
      {
        accelValue = 100;
        currentStatus.mapDOT = 0;
      }
      else
      {
        //If MAE isn't currently turned on, need to check whether it needs to be turned on
        int rateOfChange = ldiv(1000000, (MAP_time - MAPlast_time)).quot * MAP_change; //This is the % per second that the TPS has moved
        currentStatus.mapDOT = rateOfChange / 10; //The MAE bins are divided by 10 in order to allow them to be stored in a byte. Faster as this than divu10

        if (rateOfChange > configPage2.maeThresh)
        {
          BIT_SET(currentStatus.engine, BIT_ENGINE_ACC); //Mark accleration enrichment as active.
          currentStatus.AEEndTime = micros_safe() + ((unsigned long)configPage2.aeTime * 10000); //Set the time in the future where the enrichment will be turned off. taeTime is stored as mS / 10, so multiply it by 100 to get it in uS
          accelValue = table2D_getValue(&maeTable, currentStatus.mapDOT);

          //Apply the taper to the above
          //The RPM settings are stored divided by 100:
          uint16_t trueTaperMin = configPage2.aeTaperMin * 100;
          uint16_t trueTaperMax = configPage2.aeTaperMax * 100;
          if (currentStatus.RPM > trueTaperMin)
          {
            if(currentStatus.RPM > trueTaperMax) { accelValue = 0; } //RPM is beyond taper max limit, so accel enrich is turned off
            else 
            {
              int16_t taperRange = trueTaperMax - trueTaperMin;
              int16_t taperPercent = ((currentStatus.RPM - trueTaperMin) * 100) / taperRange; //The percentage of the way through the RPM taper range
              accelValue = percentage((100-taperPercent), accelValue); //Calculate the above percentage of the calculated accel amount. 
            }
          }
          accelValue = 100 + accelValue; //Add the 100 normalisation to the calculated amount
        } //MAE Threshold
      }
    }
    else if(configPage2.aeMode == AE_MODE_TPS)
    {
    
      int8_t TPS_change = (currentStatus.TPS - TPSlast);
      //Check for deceleration (Deceleration adjustment not yet supported)
      //Also check for only very small movement (Movement less than or equal to 2% is ignored). This not only means we can skip the lookup, but helps reduce false triggering around 0-2% throttle openings
      if (TPS_change <= 2)
      {
        accelValue = 100;
        currentStatus.tpsDOT = 0;
      }
      else
      {
        //If TAE isn't currently turned on, need to check whether it needs to be turned on
        int rateOfChange = ldiv(1000000, (TPS_time - TPSlast_time)).quot * TPS_change; //This is the % per second that the TPS has moved
        currentStatus.tpsDOT = rateOfChange / 10; //The TAE bins are divided by 10 in order to allow them to be stored in a byte. Faster as this than divu10

        if (rateOfChange > configPage2.taeThresh)
        {
          BIT_SET(currentStatus.engine, BIT_ENGINE_ACC); //Mark accleration enrichment as active.
          currentStatus.AEEndTime = micros_safe() + ((unsigned long)configPage2.aeTime * 10000); //Set the time in the future where the enrichment will be turned off. taeTime is stored as mS / 10, so multiply it by 100 to get it in uS
          accelValue = table2D_getValue(&taeTable, currentStatus.tpsDOT);

          //Apply the taper to the above
          //The RPM settings are stored divided by 100:
          uint16_t trueTaperMin = configPage2.aeTaperMin * 100;
          uint16_t trueTaperMax = configPage2.aeTaperMax * 100;
          if (currentStatus.RPM > trueTaperMin)
          {
            if(currentStatus.RPM > trueTaperMax) { accelValue = 0; } //RPM is beyond taper max limit, so accel enrich is turned off
            else 
            {
              int16_t taperRange = trueTaperMax - trueTaperMin;
              int16_t taperPercent = ((currentStatus.RPM - trueTaperMin) * 100) / taperRange; //The percentage of the way through the RPM taper range
              accelValue = percentage((100-taperPercent), accelValue); //Calculate the above percentage of the calculated accel amount. 
            }
          }
          accelValue = 100 + accelValue; //Add the 100 normalisation to the calculated amount
        } //TAE Threshold
      } //TPS change > 2
    } //AE Mode
  } //AE active

  return accelValue;
}

/*
Simple check to see whether we are cranking with the TPS above the flood clear threshold
This function always returns either 100 or 0
*/

static inline byte correctionFloodClear()
{
  byte floodValue = 100;
  if( BIT_CHECK(currentStatus.engine, BIT_ENGINE_CRANK) )
  {
    //Engine is currently cranking, check what the TPS is
    if(currentStatus.TPS >= configPage4.floodClear)
    {
      //Engine is cranking and TPS is above threshold. Cut all fuel
      floodValue = 0;
    }
  }
  return floodValue;
}

/*
Battery Voltage correction
Uses a 2D enrichment table (WUETable) where the X axis is engine temp and the Y axis is the amount of extra fuel to add
*/
static inline byte correctionBatVoltage()
{
  byte batValue = 100;
  if (configPage2.battVCorMode == BATTV_COR_MODE_WHOLE)
  {
    batValue = table2D_getValue(&injectorVCorrectionTable, currentStatus.battery10);
  }

  return batValue;
}

/*
Simple temperature based corrections lookup based on the inlet air temperature.
This corrects for changes in air density from movement of the temperature
*/
static inline byte correctionIATDensity()
{
  byte IATValue = 100;
  IATValue = table2D_getValue(&IATDensityCorrectionTable, currentStatus.IAT + CALIBRATION_TEMPERATURE_OFFSET); //currentStatus.IAT is the actual temperature, values in IATDensityCorrectionTable.axisX are temp+offset

  return IATValue;
}

/**
 * @brief 
 * @returns A percentage value indicating the amount the fueling should be changed based on the barometric reading. 100 = No change. 110 = 10% increase. 90 = 10% decrease
 */
static inline byte correctionBaro()
{
  byte baroValue = 100;
  baroValue = table2D_getValue(&baroFuelTable, currentStatus.baro);

  return baroValue;
}

/*
Launch control has a setting to increase the fuel load to assist in bringing up boost
This simple check applies the extra fuel if we're currently launching
*/
static inline byte correctionLaunch()
{
  byte launchValue = 100;
  if(currentStatus.launchingHard || currentStatus.launchingSoft) { launchValue = (100 + configPage6.lnchFuelAdd); }

  return launchValue;
}

/*
 * Returns true if decelleration fuel cutoff should be on, false if its off
 */
static inline bool correctionDFCO()
{
  bool DFCOValue = false;
  if ( configPage2.dfcoEnabled == 1 )
  {
    if ( bitRead(currentStatus.status1, BIT_STATUS1_DFCO) == 1 ) { DFCOValue = ( currentStatus.RPM > ( configPage4.dfcoRPM * 10) ) && ( currentStatus.TPS < configPage4.dfcoTPSThresh ); }
    else { DFCOValue = ( currentStatus.RPM > (unsigned int)( (configPage4.dfcoRPM * 10) + configPage4.dfcoHyster) ) && ( currentStatus.TPS < configPage4.dfcoTPSThresh ); }
  }
  return DFCOValue;
}

/*
 * Flex fuel adjustment to vary fuel based on ethanol content
 * The amount of extra fuel required is a linear relationship based on the % of ethanol.
*/
static inline byte correctionFlex()
{
  byte flexValue = 100;

  if (configPage2.flexEnabled == 1)
  {
    flexValue = table2D_getValue(&flexFuelTable, currentStatus.ethanolPct);
  }
  return flexValue;
}

/*
Lookup the AFR target table and perform either a simple or PID adjustment based on this

Simple (Best suited to narrowband sensors):
If the O2 sensor reports that the mixture is lean/rich compared to the desired AFR target, it will make a 1% adjustment
It then waits <egoDelta> number of ignition events and compares O2 against the target table again. If it is still lean/rich then the adjustment is increased to 2%
This continues until either:
  a) the O2 reading flips from lean to rich, at which point the adjustment cycle starts again at 1% or
  b) the adjustment amount increases to <egoLimit> at which point it stays at this level until the O2 state (rich/lean) changes

PID (Best suited to wideband sensors):

*/

static inline byte correctionAFRClosedLoop()
{
  byte AFRValue = 100;
  if( configPage6.egoType > 0 ) //egoType of 0 means no O2 sensor
  {
    currentStatus.afrTarget = currentStatus.O2; //Catch all incase the below doesn't run. This prevents the Include AFR option from doing crazy things if the AFR target conditions aren't met. This value is changed again below if all conditions are met.

    //Determine whether the Y axis of the AFR target table tshould be MAP (Speed-Density) or TPS (Alpha-N)
    //Note that this should only run after the sensor warmup delay necause it is used within the Include AFR option
    if(currentStatus.runSecs > configPage6.ego_sdelay) { currentStatus.afrTarget = get3DTableValue(&afrTable, currentStatus.fuelLoad, currentStatus.RPM); } //Perform the target lookup

    //Check all other requirements for closed loop adjustments
    if( (currentStatus.coolant > (int)(configPage6.egoTemp - CALIBRATION_TEMPERATURE_OFFSET)) && (currentStatus.RPM > (unsigned int)(configPage6.egoRPM * 100)) && (currentStatus.TPS < configPage6.egoTPSMax) && (currentStatus.O2 < configPage6.ego_max) && (currentStatus.O2 > configPage6.ego_min) && (currentStatus.runSecs > configPage6.ego_sdelay) )
    {
      AFRValue = currentStatus.egoCorrection; //Need to record this here, just to make sure the correction stays 'on' even if the nextCycle count isn't ready

      if(ignitionCount >= AFRnextCycle)
      {
        AFRnextCycle = ignitionCount + configPage6.egoCount; //Set the target ignition event for the next calculation

        //Check which algorithm is used, simple or PID
        if (configPage6.egoAlgorithm == EGO_ALGORITHM_SIMPLE)
        {
          //*************************************************************************************************************************************
          //Simple algorithm
          if(currentStatus.O2 > currentStatus.afrTarget)
          {
            //Running lean
            if(currentStatus.egoCorrection < (100 + configPage6.egoLimit) ) //Fueling adjustment must be at most the egoLimit amount (up or down)
            {
              AFRValue = (currentStatus.egoCorrection + 1); //Increase the fueling by 1%
            }
            else { AFRValue = currentStatus.egoCorrection; } //Means we're at the maximum adjustment amount, so simply return that again
          }
          else if(currentStatus.O2 < currentStatus.afrTarget)
          {
            //Running Rich
            if(currentStatus.egoCorrection > (100 - configPage6.egoLimit) ) //Fueling adjustment must be at most the egoLimit amount (up or down)
            {
              AFRValue = (currentStatus.egoCorrection - 1); //Decrease the fueling by 1%
            }
            else { AFRValue = currentStatus.egoCorrection; } //Means we're at the maximum adjustment amount, so simply return that again
          }
          else { AFRValue = currentStatus.egoCorrection; } //Means we're already right on target

        }
        else if(configPage6.egoAlgorithm == EGO_ALGORITHM_PID)
        {
          //*************************************************************************************************************************************
          //PID algorithm
          egoPID.SetOutputLimits((long)(-configPage6.egoLimit), (long)(configPage6.egoLimit)); //Set the limits again, just incase the user has changed them since the last loop. Note that these are sent to the PID library as (Eg:) -15 and +15
          egoPID.SetTunings(configPage6.egoKP, configPage6.egoKI, configPage6.egoKD); //Set the PID values again, just incase the user has changed them since the last loop
          PID_O2 = (long)(currentStatus.O2);
          PID_AFRTarget = (long)(currentStatus.afrTarget);

          egoPID.Compute();
          //currentStatus.egoCorrection = 100 + PID_output;
          AFRValue = 100 + PID_output;
        }
        else { AFRValue = 100; } // Occurs if the egoAlgorithm is set to 0 (No Correction)
      } //Ignition count check
    } //Multi variable check
  } //egoType

  return AFRValue; //Catch all (Includes when AFR target = current AFR
}

//******************************** IGNITION ADVANCE CORRECTIONS ********************************

int8_t correctionsIgn(int8_t base_advance)
{
  int8_t advance;
  advance = correctionFlexTiming(base_advance);
  advance = correctionIATretard(advance);
  advance = correctionCLTadvance(advance);
  advance = correctionIdleAdvance(advance);
  advance = correctionSoftRevLimit(advance);
  advance = correctionNitrous(advance);
  advance = correctionSoftLaunch(advance);
  advance = correctionSoftFlatShift(advance);
  advance = correctionKnock(advance);

  //Fixed timing check must go last
  advance = correctionFixedTiming(advance);
  advance = correctionCrankingFixedTiming(advance); //This overrides the regular fixed timing, must come last

  return advance;
}

static inline int8_t correctionFixedTiming(int8_t advance)
{
  int8_t ignFixValue = advance;
  if (configPage2.fixAngEnable == 1) { ignFixValue = configPage4.FixAng; } //Check whether the user has set a fixed timing angle
  return ignFixValue;
}

static inline int8_t correctionCrankingFixedTiming(int8_t advance)
{
  byte ignCrankFixValue = advance;
  if ( BIT_CHECK(currentStatus.engine, BIT_ENGINE_CRANK) ) { ignCrankFixValue = configPage4.CrankAng; } //Use the fixed cranking ignition angle
  return ignCrankFixValue;
}

static inline int8_t correctionFlexTiming(int8_t advance)
{
  byte ignFlexValue = advance;
  if( configPage2.flexEnabled == 1 ) //Check for flex being enabled
  {
    currentStatus.flexIgnCorrection = (int8_t)table2D_getValue(&flexAdvTable, currentStatus.ethanolPct); //This gets cast to a signed 8 bit value to allows for negative advance (ie retard) values here. 
    ignFlexValue = advance + currentStatus.flexIgnCorrection;
  }
  return ignFlexValue;
}

static inline int8_t correctionIATretard(int8_t advance)
{
  byte ignIATValue = advance;
  //Adjust the advance based on IAT. If the adjustment amount is greater than the current advance, just set advance to 0
  int8_t advanceIATadjust = table2D_getValue(&IATRetardTable, currentStatus.IAT);
  int tempAdvance = (advance - advanceIATadjust);
  if (tempAdvance >= -OFFSET_IGNITION) { ignIATValue = tempAdvance; }
  else { ignIATValue = -OFFSET_IGNITION; }

  return ignIATValue;
}

static inline int8_t correctionCLTadvance(int8_t advance)
{
  int8_t ignCLTValue = advance;
  //Adjust the advance based on CLT.
  int8_t advanceCLTadjust = (int16_t)(table2D_getValue(&CLTAdvanceTable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET)) - 15;
  ignCLTValue = (advance + advanceCLTadjust);
  
  return ignCLTValue;
}

static inline int8_t correctionIdleAdvance(int8_t advance)
{

  int8_t ignIdleValue = advance;
  //Adjust the advance based on idle target rpm.
  if( (configPage2.idleAdvEnabled) >= 1 && (currentStatus.runSecs >= configPage2.IdleAdvDelay))
  {
    currentStatus.CLIdleTarget = (byte)table2D_getValue(&idleTargetTable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET); //All temps are offset by 40 degrees
    int idleRPMdelta = (currentStatus.CLIdleTarget - currentStatus.RPM / 10) + 50;
    // Limit idle rpm delta between -500rpm - 500rpm
    if(idleRPMdelta > 100) { idleRPMdelta = 100; }
    if(idleRPMdelta < 0) { idleRPMdelta = 0; }
    if(configPage2.idleAdvAlgorithm == 0 && ((currentStatus.RPM < (unsigned int)(configPage2.idleAdvRPM * 100)) && (currentStatus.TPS < configPage2.idleAdvTPS))) // TPS based idle state
    {
      int8_t advanceIdleAdjust = (int16_t)(table2D_getValue(&idleAdvanceTable, idleRPMdelta)) - 15;
      if(configPage2.idleAdvEnabled == 1) { ignIdleValue = (advance + advanceIdleAdjust); }
      else if(configPage2.idleAdvEnabled == 2) { ignIdleValue = advanceIdleAdjust; }
    }
    else if(configPage2.idleAdvAlgorithm == 1 && (currentStatus.RPM < (unsigned int)(configPage2.idleAdvRPM * 100) && currentStatus.CTPSActive == 1)) // closed throttle position sensor (CTPS) based idle state
    {
      int8_t advanceIdleAdjust = (int16_t)(table2D_getValue(&idleAdvanceTable, idleRPMdelta)) - 15;
      if(configPage2.idleAdvEnabled == 1) { ignIdleValue = (advance + advanceIdleAdjust); }
      else if(configPage2.idleAdvEnabled == 2) { ignIdleValue = advanceIdleAdjust; }
    }
  }
  return ignIdleValue;
}

static inline int8_t correctionSoftRevLimit(int8_t advance)
{
  byte ignSoftRevValue = advance;
  BIT_CLEAR(currentStatus.spark, BIT_SPARK_SFTLIM);
  if (currentStatus.RPM > ((unsigned int)(configPage4.SoftRevLim) * 100) ) { BIT_SET(currentStatus.spark, BIT_SPARK_SFTLIM); ignSoftRevValue = configPage4.SoftLimRetard;  } //Softcut RPM limit (If we're above softcut limit, delay timing by configured number of degrees)

  return ignSoftRevValue;
}

static inline int8_t correctionNitrous(int8_t advance)
{
  byte ignNitrous = advance;
  //Check if nitrous is currently active
  if(configPage10.n2o_enable > 0)
  {
    //Check which stage is running (if any)
    if( currentStatus.nitrous_status == NITROUS_STAGE1 )
    {
      ignNitrous -= configPage10.n2o_stage1_retard;
    }
    if( currentStatus.nitrous_status == NITROUS_STAGE2 )
    {
      ignNitrous -= configPage10.n2o_stage2_retard;
    }
  }

  return ignNitrous;
}

static inline int8_t correctionSoftLaunch(int8_t advance)
{
  byte ignSoftLaunchValue = advance;
  //SoftCut rev limit for 2-step launch control.
  if (configPage6.launchEnabled && clutchTrigger && (currentStatus.clutchEngagedRPM < ((unsigned int)(configPage6.flatSArm) * 100)) && (currentStatus.RPM > ((unsigned int)(configPage6.lnchSoftLim) * 100)) && (currentStatus.TPS >= configPage10.lnchCtrlTPS) )
  {
    currentStatus.launchingSoft = true;
    BIT_SET(currentStatus.spark, BIT_SPARK_SLAUNCH);
    ignSoftLaunchValue = configPage6.lnchRetard;
  }
  else
  {
    currentStatus.launchingSoft = false;
    BIT_CLEAR(currentStatus.spark, BIT_SPARK_SLAUNCH);
  }

  return ignSoftLaunchValue;
}

static inline int8_t correctionSoftFlatShift(int8_t advance)
{
  byte ignSoftFlatValue = advance;

  if(configPage6.flatSEnable && clutchTrigger && (currentStatus.clutchEngagedRPM > ((unsigned int)(configPage6.flatSArm) * 100)) && (currentStatus.RPM > (currentStatus.clutchEngagedRPM-configPage6.flatSSoftWin) ) )
  {
    BIT_SET(currentStatus.spark2, BIT_SPARK2_FLATSS);
    ignSoftFlatValue = configPage6.flatSRetard;
  }
  else { BIT_CLEAR(currentStatus.spark2, BIT_SPARK2_FLATSS); }

  return ignSoftFlatValue;
}

static inline int8_t correctionKnock(int8_t advance)
{
  byte knockRetard = 0;

  //First check is to do the window calculations (ASsuming knock is enabled)
  if( configPage10.knock_mode != KNOCK_MODE_OFF )
  {
    knockWindowMin = table2D_getValue(&knockWindowStartTable, currentStatus.RPM);
    knockWindowMax = knockWindowMin + table2D_getValue(&knockWindowDurationTable, currentStatus.RPM);
  }


  if( (configPage10.knock_mode == KNOCK_MODE_DIGITAL)  )
  {
    //
    if(knockCounter > configPage10.knock_count)
    {
      if(currentStatus.knockActive == true)
      {
        //Knock retard is currently 
      }
      else
      {
        //Knock needs to be activated
        lastKnockCount = knockCounter;
        knockStartTime = micros();
        knockRetard = configPage10.knock_firstStep;
      }
    }

  }

  return advance - knockRetard;
}

//******************************** DWELL CORRECTIONS ********************************
uint16_t correctionsDwell(uint16_t dwell)
{
  uint16_t tempDwell = dwell;
  //Pull battery voltage based dwell correction and apply if needed
  currentStatus.dwellCorrection = table2D_getValue(&dwellVCorrectionTable, currentStatus.battery10);
  if (currentStatus.dwellCorrection != 100) { tempDwell = divs100(dwell) * currentStatus.dwellCorrection; }

  //Dwell limiter
  uint16_t dwellPerRevolution = tempDwell + (uint16_t)(configPage4.sparkDur * 100); //Spark duration is in mS*10. Multiple it by 100 to get spark duration in uS
  int8_t pulsesPerRevolution = 1;
  //Single channel spark mode is the only time there will be more than 1 pulse per revolution on any given output
  if( ( (configPage4.sparkMode == IGN_MODE_SINGLE) || (configPage4.sparkMode == IGN_MODE_ROTARY) ) && (configPage2.nCylinders > 1) ) //No point in running this for 1 cylinder engines
  {
    pulsesPerRevolution = (configPage2.nCylinders >> 1);
    dwellPerRevolution = dwellPerRevolution * pulsesPerRevolution;
  }

  if(dwellPerRevolution > revolutionTime)
  {
    //Possibly need some method of reducing spark duration here as well, but this is a start
    tempDwell = (revolutionTime / pulsesPerRevolution) - (configPage4.sparkDur * 100);
  }
  return tempDwell;
}
