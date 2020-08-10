/** SMARTuniqueID.hpp
 *  Created on: 05.08.2016
 * Author:Stefan Gebauer, M.Sc.Comp.Sc. TU Berlin */

#ifndef ATTRIBUTES_SMARTUNIQUEID_HPP_
#define ATTRIBUTES_SMARTUNIQUEID_HPP_

///standard C/C++ lib
#include <iostream>///class std::ostream
#include <limits>///std::numeric_limits<>::max()
#include <map> //class std::map
#include <set> //class std::set
#include <stdint.h>///uint64_t
#include <string.h>///memset(...)

///common_sourcecode repo of Stefan Gebauer
#include <hardware/CPU/fastest_data_type.h>///fastestUnsignedDataType
#include <hardware/CPU/atomic/AtomicExchange.h>///AtomicExchange(...)
///numSMART_SNbytes, numSMART_FWbytes, numSMARTmodelBytes, numDifferentSMART_IDs
#include <hardware/dataCarrier/ATA3Std.h>
#include <hardware/dataCarrier/SMARTattributeNames.h>///enum SMARTattributeNames

#include <hardware/dataCarrier/SMARTattributeNames.h>///enum SMARTattributeNames
#include "SMARTattributeNameAndID.hpp"///class SMARTattributeNameAndID

///So the last SMART attrubute ID (254) can be used to index (255 items array)
///Does not need so many items? # supported SMART attributes = 30?
#define numItems (numDifferentSMART_IDs+1)
///Could also make it inline functions for type safety?
#define mostSignificantBit(value) (1L << (sizeof(value)*8-1) )
#define removeBits(value, bits) (value & ~bits)
#define setGreaterBit(value) (value |= mostSignificantBit(value) );/// ">" in UI

/** Same structure as SkIdentifyParsedData in Linux' "atasmart.h" */
struct SMARTuniqueID {
  ///Member vars are set by SMARTaccessBase-derived class, fill(...) method.
  char m_serialNumber[numSMART_SNbytes+1];
  char m_firmWareName[numSMART_FWbytes+1];
  char m_modelName[numSMARTmodelBytes+1];

  //TODO # supported SMART IDs may only be 30->less space needed
  fastestUnsignedDataType supportedSMART_IDs[numDifferentSMART_IDs] = {0};
  
  ///Source code for unit detection follows.
  typedef fastestUnsignedDataType SMART_IDsToReadType;
  /** SMART IDs are fetched in an interval->make access fast. Must be sorted
  * ascending for the algorithms to work.*/
  fastestUnsignedDataType m_SMART_IDsToRd[numDifferentSMART_IDs];
  bool SMART_IDsToReadNotEnd(const fastestUnsignedDataType SMART_IDsToReadIdx)
    const{
    return m_SMART_IDsToRd[SMART_IDsToReadIdx] !=0
      && SMART_IDsToReadIdx < numDifferentSMART_IDs;
  }
  bool noSMARTattrsToRead() const{ return ! SMART_IDsToReadNotEnd(0); }
  
  typedef long int unitDataType;
  //TODO not needed anoymore because calculated from upper - lower bound?
  unitDataType units[numItems];
  enum states {/**real unit >= this value*/getMinUnit,getUnitTillValInc,
    /**get unit more accurately*/getUnit};
  uint64_t m_prevSMARTrawVals[numItems];
  uint64_t m_otherMetricVal[numItems];
  uint64_t m_SMARTrawValDiffs[numItems];
  fastestUnsignedDataType state[numItems];
//  fastestUnsignedDataType numSamples[numItems];//Not needed?
  unitDataType otherMtrcValAtLastSMARTrawValInc[numItems];//TODO Show in UI?
  unitDataType lowerUnitBound[numItems];
  unitDataType upperUnitBound[numItems];
  SMARTuniqueID & operator = (const SMARTuniqueID & l);
  SMARTuniqueID(){
    memset(units, 0, sizeof(units[0])*numItems);
    memset(m_prevSMARTrawVals, 0xFF,sizeof(uint64_t)*numItems);
    memset(m_otherMetricVal, 0xFF,sizeof(uint64_t)*numItems);
    memset(m_SMARTrawValDiffs, 0, sizeof(uint64_t)*numItems);
    memset(state, getMinUnit, sizeof(state[0])*numItems);
//    memset(numSamples, 0, sizeof(numSamples[0])*numItems);
    memset(otherMtrcValAtLastSMARTrawValInc, 0, sizeof(
      otherMtrcValAtLastSMARTrawValInc[0])*numItems);
    memset(lowerUnitBound, 0, sizeof(lowerUnitBound[0])*numItems);
    memset(upperUnitBound, 0, sizeof(upperUnitBound[0])*numItems);
  }
#ifdef __linux__
//  SMARTuniqueID(const SkIdentifyParsedData & l);
#endif
  ~SMARTuniqueID();
  
  const fastestUnsignedDataType * getSupportedSMART_IDs() const{
    return supportedSMART_IDs;
  }
  void copyArr(const fastestUnsignedDataType orig [],
    fastestUnsignedDataType cpy []);
  void setSupportedSMART_IDs(suppSMART_IDsType & suppSMARTattrNamesAndIDs)
  {
    //TODO replace by copyArr();
    int idx = 0;
    for(suppSMART_IDsType::const_iterator iter=suppSMARTattrNamesAndIDs.begin();
      iter != suppSMARTattrNamesAndIDs.end() && idx < numDifferentSMART_IDs;
      iter++)
      supportedSMART_IDs[idx++] = iter->GetID();
    if(idx < numDifferentSMART_IDs)
      supportedSMART_IDs[idx] = 0;
  }
  
  /** \brief Intersection of both SMART IDs supported by data carrier and SMART
   * IDs to read. */
  //TODO also intersect where is a SMART attribute definition?
  void SetSMART_IDsToRead(
    const suppSMART_IDsType & suppSMARTattrNamesAndIDs,
    /**Sorted ascending, last entry has value 0.*/
    const fastestUnsignedDataType SMART_IDsToRead[])
  {
    if(SMART_IDsToRead[0] == 0){///Empty->read all supported S.M.A.R.T. IDs
      ///Prevent stack overflow.
      if(suppSMARTattrNamesAndIDs.size() <= numDifferentSMART_IDs){
      fastestUnsignedDataType SMART_IDsToReadIdx = 0;
      for(suppSMART_IDsType::const_iterator iter = 
        suppSMARTattrNamesAndIDs.begin(); iter !=suppSMARTattrNamesAndIDs.end();
        iter++)
      {
        m_SMART_IDsToRd[SMART_IDsToReadIdx++] = iter->GetID();
      }
      ///Prevent stack overflow.
      if(suppSMARTattrNamesAndIDs.size() < numDifferentSMART_IDs)
        m_SMART_IDsToRd[SMART_IDsToReadIdx] = 0;///Mark last entry
      }
//      else///return error code
    }
    else{
      fastestUnsignedDataType sMART_IDtoRead, suppSMART_ID;
      fastestUnsignedDataType SMART_IDsToReadIdx = 0;
      ///Intersection of supported SMART IDs and SMART IDs to read array index.
      fastestUnsignedDataType SMART_IDsToObsIdx = 0;
      for(suppSMART_IDsType::const_iterator iter =
        suppSMARTattrNamesAndIDs.begin();
        iter !=suppSMARTattrNamesAndIDs.end(); iter++
//        SMART_IDsToObs[SMART_IDsToReadIdx] != 0; SMART_IDsToReadIdx++
        )
      {
//        if(iter !=suppSMARTattrNamesAndIDs.end() )
        suppSMART_ID = iter->GetID();
        do{
        sMART_IDtoRead = SMART_IDsToRead[SMART_IDsToReadIdx];
        if( sMART_IDtoRead == 0){///Last entry reached
          iter = suppSMARTattrNamesAndIDs.end();
          iter--;
          break;
        }
        if(sMART_IDtoRead == suppSMART_ID)
        {
          SMART_IDsToReadIdx++;
          m_SMART_IDsToRd[SMART_IDsToObsIdx++] = sMART_IDtoRead;
          break;///Get next supported SMART ID
        }
        else if(sMART_IDtoRead < suppSMART_ID)
          SMART_IDsToReadIdx++;
        }while(SMART_IDsToReadIdx < numDifferentSMART_IDs &&
           sMART_IDtoRead < suppSMART_ID);
      }
      if(SMART_IDsToReadIdx < numDifferentSMART_IDs)
        m_SMART_IDsToRd[SMART_IDsToReadIdx] = 0;
    }
  }
  
  /**\param otherVal : Can be called with value directly from device or
  * delivered to client/SMARTvalueProcessorBase(-derived).*/
  void guessUnit(
    const fastestUnsignedDataType SMARTattrID,
    const uint64_t & SMARTrawVal,
    const uint64_t otherVal///E.g. # B written to data carrier since OS start
    )
  {
    /** Should make a difference of m_otherMetricVal minus otherVal from the 2nd
     * time the S.M.A.R.T. attribute value
     * increases. Else if doing it the 1st time it increases then the difference
     * may be wrong:
     *       x <- 1st time getting # data written since OS start
     * x     | x <-S.M.A.R.T.'s Total Data/LBAs Written value increases
     * t1   t2 t3 -> time (t1,t2,t3: point in time) */
    
    switch(state[SMARTattrID])//TODO take value overflow into account
    {
      case getMinUnit:
      if(otherVal < m_otherMetricVal[SMARTattrID])
      {
        m_otherMetricVal[SMARTattrID] = otherVal;
        m_prevSMARTrawVals[SMARTattrID] = SMARTrawVal;
      }
      else if(SMARTrawVal > m_prevSMARTrawVals[SMARTattrID]){
        state[SMARTattrID] = getUnitTillValInc;
        long int unit = otherVal - m_otherMetricVal[SMARTattrID];
        /**Show in column "~unit range" as "> lowerUnitBound[SMARTattrID]" if
         *  no upper unit bound.*/
        lowerUnitBound[SMARTattrID] = unit;
        AtomicExchange(&units[SMARTattrID], unit);
        ///Increased for the 1st time->now memorize the values
        m_otherMetricVal[SMARTattrID] = otherVal;
        m_prevSMARTrawVals[SMARTattrID] = SMARTrawVal;
      }
      else{
        long int unit = otherVal - m_otherMetricVal[SMARTattrID];
        /**Value is likely not the ~unit because we didn't regard the S.M.A.R.T.
        * value increase ("t1" from "diagram" above). So set the greater bit.*/
        setGreaterBit(unit);
        AtomicExchange(&units[SMARTattrID], unit);
      }
      break;
      case getUnitTillValInc:
      ///SMART raw value increased for the 2nd time.
      if(SMARTrawVal > m_prevSMARTrawVals[SMARTattrID])
      {
        state[SMARTattrID] = getUnit;
//        numSamples[SMARTattrID]++;
        uint64_t SMARTrawValDiff = SMARTrawVal -m_prevSMARTrawVals[SMARTattrID];
        m_SMARTrawValDiffs[SMARTattrID] = SMARTrawValDiff;
        uint64_t diff = otherVal - m_otherMetricVal[SMARTattrID];
        //TODO long int may not be sufficient
        long int unit = diff / SMARTrawValDiff;
        ///For the Power-On Time a calculation like this could be more accurate:
//        if(SMARTattrID == SMARTattributeNames::PowerOnTime){
//          lowerBound[SMARTattrID] = unit-/**measurement interval in ms*/10000;
//          upperBound[SMARTattrID] = unit+/**measurement interval in ms*/10000;
//        }
//        else{
        //TODO can use lowerUnitBound[SMARTattrID] from state "getMinUnit" as
        // value for lowerUnitBound[SMARTattrID] if it is > unit?
        lowerUnitBound[SMARTattrID] = unit;
        upperUnitBound[SMARTattrID] = unit;
//        }
        AtomicExchange(&units[SMARTattrID], unit);
      }
      else{
        long int unit = otherVal - m_otherMetricVal[SMARTattrID];
        ///Only if value > value from state "getMinUnit"
        if(unit > lowerUnitBound[SMARTattrID])
          lowerUnitBound[SMARTattrID] = unit;
        /** As long as we did't regard a 2nd S.M.A.R.T. value increase we only
         * know the unit is > ("t1" from "diagram" above). So set the greater
         * bit.*/
        setGreaterBit(unit) //if(unit > units[SMARTattrID])
        AtomicExchange(&units[SMARTattrID], unit);
      }
      break;
      /**SMART raw value increased for at least the 3rd time*/
      case getUnit:{///The higher the difference  the more accurate?
        uint64_t SMARTrawValDiff = SMARTrawVal -m_prevSMARTrawVals[SMARTattrID];
        if( SMARTrawValDiff > m_SMARTrawValDiffs[SMARTattrID]){
          m_SMARTrawValDiffs[SMARTattrID] = SMARTrawValDiff;
//          numSamples[SMARTattrID]++;
          uint64_t otherMetricDiff = otherVal - m_otherMetricVal[SMARTattrID];
          //TODO long int may not be sufficient
          long int unit = otherMetricDiff / SMARTrawValDiff;
          const long int unitDiff = units[SMARTattrID] - unit;
        ///For the Power-On Time a calculation like this could be more accurate:
          /*if(SMARTattrID == SMARTattributeNames::PowerOnTime){
            if(unitDiff < 0)
              lowerBound[SMARTattrID] += unitDiff;
            else
              upperBound[SMARTattrID] -= unitDiff;
          }else*/
          if(unitDiff > 0)///New value lower than stored value.
            lowerUnitBound[SMARTattrID] = unit;
          else
            upperUnitBound[SMARTattrID] = unit;
          /** Can be shown in user interface to give an info when the SMART raw
           *  value increments next time. For this calculate:
           * otherVal - otherMtrcValAtLastSMARTrawValInc[SMARTattrID] */
          otherMtrcValAtLastSMARTrawValInc[SMARTattrID] = unit;
          /** This prevents a value overflow in contrast to "(lowerBound[
           *  SMARTattrID] + (upperBound[SMARTattrID])/2 but is slower.*/
          unit = lowerUnitBound[SMARTattrID] + (upperUnitBound[SMARTattrID] - 
            lowerUnitBound[SMARTattrID]) / 2;
          AtomicExchange(&units[SMARTattrID], unit);
        }
      }
    }
  }

  /** For necessary function signature see
   *  https://msdn.microsoft.com/en-us/library/1z2f6c2k.aspx */
  friend std::ostream & operator << ( std::ostream & ostr,
    const SMARTuniqueID & obj)
  {
    ostr << "model:" << obj.m_modelName << " firmware:" << obj.m_firmWareName 
      << " serial #:" << obj.m_serialNumber;
    return ostr;
  }
  std::string str() const;
  void SetModelName(const char *);
  void SetSerialNumber(const char *);
  void SetFirmwareName(const char *);
};
  
  typedef std::set<fastestSignedDataType> supportedSMARTattrIDsType;
  typedef std::map<SMARTuniqueID,supportedSMARTattrIDsType >
    dataCarrierID2supportedSMARTattrMap_type;
typedef std::map<SMARTuniqueID,std::string> dataCarrierID2devicePath_type;

  bool operator < (const SMARTuniqueID & left,
                   const SMARTuniqueID & right);

#endif /* ATTRIBUTES_SMARTUNIQUEID_HPP_ */
