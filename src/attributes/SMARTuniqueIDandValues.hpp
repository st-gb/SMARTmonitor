/** SMARTuniqueIDandValues.hpp
 *  Created on: 05.08.2016
 *  Author: Stefan Gebauer, M.Sc. Comp.Sc. */

#ifndef ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_
#define ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_

///"Standard C" header files:
#include <stdint.h> //uint64_t

///_This_ repository's files:
//#include "SMARTaccessBase.hpp"
#include "SMARTuniqueID.hpp"///struct SMARTuniqueID

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
#include <hardware/dataCarrier/ATA3Std.h>///ATA3Std_NrmlzdValTyp
#include <hardware/CPU/fastest_data_type.h>

//class SMARTaccessBase;

///+1 to enable a 1-based array index
#define numDifferentSMART_IDsPlus1 (numDifferentSMART_IDs + 1)

class SMARTvalue
{
public:
  SMARTvalue() 
    /** Initialization of this attribute is important for 
     * SetSMARTattributesToObserve(...) */
    : m_successfullyReadSMARTrawValue(0) {}
  SMARTvalue(const SMARTvalue &);///copy constructor
  SMARTvalue & operator = ( const SMARTvalue & rhs ); //assignment operator
  
  //calculation of array size adapted from 
  // http://stackoverflow.com/questions/4079243/how-can-i-use-sizeof-in-a-preprocessor-macro
  // Data types must be long int for atomic operations
  long int m_rawValue [sizeof(long int) < 8 ? 8 / sizeof(long int) : 1];
  long int m_rawValueCheckSum, m_timeCheckSum;
  long int m_successfullyReadSMARTrawValue;
  /// http://en.wikipedia.org/wiki/S.M.A.R.T.#ATA_S.M.A.R.T._attributes :
  AtomicExchType m_normedCurrVal;///Normalized current value(0-255)
  AtomicExchType m_normedThresh;///Normalized threshold value(0-255)
//  long int m_normedWorstVal;///Normalized worst value(0-255)
  
  ///Declared here to have no dependancy to SMARTaccessBase or SMARTmonitorBase
  static fastestUnsignedDataType s_sizeOfLongIntInBytes;
  static fastestUnsignedDataType s_numTimesLongIntFitsInto8Bytes;
  const AtomicExchType GetNrmlzdCurrVal() const{return m_normedCurrVal;}
  ///For use in atomic compare and swap operations for example.
  AtomicExchType * const GetNrmlzdCurrVal() {return &m_normedCurrVal;}
  const AtomicExchType GetNrmlzdThresh() const{return m_normedThresh;}
  ///For use in atomic compare and swap operations for example.
  AtomicExchType * const GetNrmlzdThresh() {return &m_normedThresh;}
  void SetRawValue(/*long int * ,*/ const uint64_t & rawSMARTattrValue);
  bool GetRetrievalTime(uint64_t & uptimeInMs) const;
  void SetRetrievalTime(const long double &);
  bool IsConsistent(uint64_t &) const;
  static ATA3Std_NrmlzdValTyp maxNrmlzdVals[numDifferentSMART_IDsPlus1];
  ///@brief Sets the maximum normalized value for a certain SMART attribute.
  static inline void setMaxNrmlzdVal(
    const TU_Bln361095CPUuse(FaststUint) SMARTattrID,
    const int maxNrmlzdVal)
  {
    maxNrmlzdVals[SMARTattrID]=maxNrmlzdVal;
  }
  ///@brief Sets maximum normalized value for all SMART attributes.
  static void setMaxNrmlzdVals(){
    /**http://pubs.opengroup.org/onlinepubs/000095399/functions/memset.html :
     * "(void *s, int c, size_t n);"*/
    memset(///For memset(...), the array must have 1 byte data type
      maxNrmlzdVals,ATA3Std_DfltNrmlzdVal,sizeof(maxNrmlzdVals));
    /**Seems to apply to model "WDC WD400EB-00CPFO", firmware "06.04G06":
     * current normalized value=200, model "ST500LT012-1DG142", firmware
     * "0002LVM1":current normalized value=111 for serial number "W3P78D3W"*/
    setMaxNrmlzdVal(SMARTattributeNames::ReadErrorRate,
      ATA3Std_GtrDfltNrmlzdVal);
    /**Max Normalized value from: model:Samsung SSD 860 EVO M.2 500GB
     * firmware:RVT24B6Q serial:S5GCNJ0N506884T, current normalized value.*/
    setMaxNrmlzdVal(SMARTattributeNames::HW_ECC_Recovered,200);
  }
private:
  inline void copyAttrs(const SMARTvalue &);
  /** Has to be uint64_t in order to also work if built as 32 bit program and a
   *  long uptime in ms. 100d each 24 h each 60 min each 60 s each 1000 ms=
   *  100*24*60*60*1000=8640000000=8640M */
  uint64_t m_timeStampOfRetrieval;///Only access via Set|Get(TimeOfRetrieval);
};

class SMARTuniqueIDandValues
{
private:
  SMARTuniqueID m_SMARTuniqueID;
public:
//  /*uint64_t*/ long int m_SMARTrawValues[NUM_DIFFERENT_SMART_ENTRIES];
//  long int m_successfullyReadSMARTrawValue[NUM_DIFFERENT_SMART_ENTRIES];
//  long int m_timeStampOfRetrieval[NUM_DIFFERENT_SMART_ENTRIES];
  SMARTvalue m_SMARTvalues[numDifferentSMART_IDsPlus1];
  SMARTuniqueIDandValues (const SMARTuniqueID & _SMARTuniqueID);
  SMARTuniqueIDandValues () {}
  SMARTuniqueIDandValues( const SMARTuniqueIDandValues &);
  virtual ~SMARTuniqueIDandValues ();

  const SMARTuniqueID & getSMARTuniqueID() const { return m_SMARTuniqueID; }
  void SetDataCarrierID(const SMARTuniqueID & sMARTuniqueID) {
    m_SMARTuniqueID = sMARTuniqueID;
  }
  /** Important if no S.M.A.R.T. attributes to read exist yet.
   * Therefore get S.M.A.R.T. values at least once in order to set the 
   * SMARTvalue::m_successfullyReadSMARTrawValue */
  void setSMART_IDsToReadFromSuccSMARTrawValUpd8(){
    fastestUnsignedDataType SMART_IDsToReadArrIdx = 0;
    for(fastestUnsignedDataType sMARTvalsIdx = 1; sMARTvalsIdx <
      numDifferentSMART_IDs; sMARTvalsIdx++)
      if(m_SMARTvalues[sMARTvalsIdx].m_successfullyReadSMARTrawValue)
        m_SMARTuniqueID.m_SMART_IDsToRd[SMART_IDsToReadArrIdx++] = sMARTvalsIdx;
    m_SMARTuniqueID.m_SMART_IDsToRd[SMART_IDsToReadArrIdx] = 0;
  }
  int GetSMARTentry(fastestUnsignedDataType SMART_ID, SMARTvalue & sMARTvalue);
};

bool operator < (const SMARTuniqueIDandValues & left,
                 const SMARTuniqueIDandValues & right);

#endif /* ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_ */
