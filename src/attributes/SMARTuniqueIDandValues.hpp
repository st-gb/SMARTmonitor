/*
 * SMARTuniqueIDandValues.hpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#ifndef ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_
#define ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_

#include "SMARTuniqueID.hpp" //struct SMARTuniqueID
#include <stdint.h> //uint64_t
//#include "SMARTaccessBase.hpp"
#include <hardware/CPU/fastest_data_type.h>

//class SMARTaccessBase;

#define NUM_DIFFERENT_SMART_ENTRIES 256

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
  long int m_rawValue [sizeof(long int) < 8 ? 8 / sizeof(long int) : 8];
  long int m_rawValueCheckSum;
  long int m_timeStampOfRetrieval;
  long int m_successfullyReadSMARTrawValue;
  
  ///Declared here to have no dependancy to SMARTaccessBase or SMARTmonitorBase
  static fastestUnsignedDataType s_sizeOfLongIntInBytes;
  void SetRawValue(/*long int * ,*/ const uint64_t & rawSMARTattrValue);
  bool IsConsistent(uint64_t &) const;
};

class SMARTuniqueIDandValues
{
private:
  SMARTuniqueID m_SMARTuniqueID;
public:
//  /*uint64_t*/ long int m_SMARTrawValues[NUM_DIFFERENT_SMART_ENTRIES];
//  long int m_successfullyReadSMARTrawValue[NUM_DIFFERENT_SMART_ENTRIES];
//  long int m_timeStampOfRetrieval[NUM_DIFFERENT_SMART_ENTRIES];
  SMARTvalue m_SMARTvalues[NUM_DIFFERENT_SMART_ENTRIES];
  SMARTuniqueIDandValues (const SMARTuniqueID & _SMARTuniqueID);
  SMARTuniqueIDandValues () {}
  SMARTuniqueIDandValues( const SMARTuniqueIDandValues &);
  virtual ~SMARTuniqueIDandValues ();

  const SMARTuniqueID & getSMARTuniqueID() const { return m_SMARTuniqueID; }
  void SetDataCarrierID(const SMARTuniqueID & sMARTuniqueID) {
    m_SMARTuniqueID = sMARTuniqueID;
  }
  int GetSMARTentry(fastestUnsignedDataType SMART_ID, SMARTvalue & sMARTvalue);
};

bool operator < (const SMARTuniqueIDandValues & left,
                 const SMARTuniqueIDandValues & right);

#endif /* ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_ */
