/** SMARTuniqueIDandValues.hpp
 *  Created on: 05.08.2016
 *  Author: Stefan Gebauer, M.Sc. Comp.Sc. */

#ifndef ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_
#define ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_

#include "SMARTuniqueID.hpp" //struct SMARTuniqueID
#include <stdint.h> //uint64_t
//#include "SMARTaccessBase.hpp"
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
  SMARTvalue(SMARTvalue & copyFrom); //copy constructor
  SMARTvalue & operator = ( const SMARTvalue & rhs ); //assignment operator
  
  //calculation of array size adapted from 
  // http://stackoverflow.com/questions/4079243/how-can-i-use-sizeof-in-a-preprocessor-macro
  // Data types must be long int for atomic operations
  long int m_rawValue [sizeof(long int) < 8 ? 8 / sizeof(long int) : 1];
  long int m_rawValueCheckSum, m_timeCheckSum;
  long int m_successfullyReadSMARTrawValue;
  
  ///Declared here to have no dependancy to SMARTaccessBase or SMARTmonitorBase
  static fastestUnsignedDataType s_sizeOfLongIntInBytes;
  static fastestUnsignedDataType s_numTimesLongIntFitsInto8Bytes;
  void SetRawValue(/*long int * ,*/ const uint64_t & rawSMARTattrValue);
  bool GetRetrievalTime(uint64_t & uptimeInMs) const;
  void SetRetrievalTime(const long double &);
  bool IsConsistent(uint64_t &) const;
private:  
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
