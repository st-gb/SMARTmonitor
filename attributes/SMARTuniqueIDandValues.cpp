/*
 * SMARTuniqueIDandValues.cpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#include <attributes/SMARTuniqueIDandValues.hpp>
#include <fastest_data_type.h>
#include "SMARTaccessBase.hpp"
#include "hardware/CPU/atomic/AtomicExchange.h"

//class SMARTaccessBase;
//extern fastestUnsignedDataType SMARTaccessBase::s_sizeOfLongIntInBytes;

SMARTvalue & SMARTvalue::operator = ( const SMARTvalue & copyFrom )
{
  m_timeStampOfRetrieval = copyFrom.m_timeStampOfRetrieval;
  m_successfullyReadSMARTrawValue = copyFrom.m_successfullyReadSMARTrawValue;
  SetRawValue( * (uint64_t *) copyFrom.m_rawValue);
}

/** copy c'tor */
SMARTvalue::SMARTvalue(SMARTvalue & copyFrom)
{
  m_timeStampOfRetrieval = copyFrom.m_timeStampOfRetrieval;
  m_successfullyReadSMARTrawValue = copyFrom.m_successfullyReadSMARTrawValue;
  SetRawValue( * (uint64_t *) copyFrom.m_rawValue);
}

void SMARTvalue::SetRawValue(/*long int * pRawValue,*/ const uint64_t & rawSMARTattrValue)
{
  long int liRawSMARTattrValue;
  m_rawValueCheckSum = 0;
  /** If shorter than a uint64_t */
  if( SMARTaccessBase::s_sizeOfLongIntInBytes < 8 )
  {
    fastestUnsignedDataType numTimesLongIntFitsInto8Bytes = 8/SMARTaccessBase::
      s_sizeOfLongIntInBytes;
    fastestUnsignedDataType numBitsToShift;
    /** Save the highmost bytes because they are more comprehensive:
      For e.g. the Power-on hours a value was on a 32 bit Linux:
     * 43441200000_dec ms = A1D4C1B80_hex ms ^= 12067 h as uint64_t and
     *   491527040_dec ms =  1D4C1B80_hex ms ~= 136 h as long int 
     * So if only using the highmost bits it would be: */
//    liRawSMARTattrValue = m_rawValue >> ( (8 - 
//      SMARTaccessBase::s_sizeOfLongIntInBytes) * 8 );
    for( fastestUnsignedDataType i = 0; i < numTimesLongIntFitsInto8Bytes; ++i)
    {
      numBitsToShift = //(8 - SMARTaccessBase::s_sizeOfLongIntInBytes) * 8;
        SMARTaccessBase::s_sizeOfLongIntInBytes * i * 8;
      liRawSMARTattrValue = (long int) (rawSMARTattrValue >> numBitsToShift);
      AtomicExchange( (long int *) (m_rawValue) + i 
        //(SMARTaccessBase::s_sizeOfLongIntInBytes * i)//long * Target
        , //* (long int *) //SMARTattributesToObserveIter->pretty_value /*raw*/ /*long val*/
        liRawSMARTattrValue );
      m_rawValueCheckSum ^= liRawSMARTattrValue;
    }
  }
  else
  {
    liRawSMARTattrValue = rawSMARTattrValue;
    m_rawValueCheckSum = rawSMARTattrValue;
  }
}

bool SMARTvalue::IsConsistent(uint64_t & rawValue) const
{
  long int liRawSMARTattrValue, rawValueCheckSum = 0;
  /** If shorter than a uint64_t */
  if( SMARTaccessBase::s_sizeOfLongIntInBytes < 8 )
  {
    rawValue = 0;
    fastestUnsignedDataType numTimesLongIntFitsInto8Bytes = 8/SMARTaccessBase::
      s_sizeOfLongIntInBytes;
    /** Save the highmost bytes because they are more comprehensive:
      For e.g. the Power-on hours a value was on a 32 bit Linux:
     * 43441200000_dec ms = A1D4C1B80_hex ms ^= 12067 h as uint64_t and
     *   491527040_dec ms =  1D4C1B80_hex ms ~= 136 h as long int 
     * So if only using the highmost bits it would be: */
//    liRawSMARTattrValue = liRawSMARTattrValue >> (8 - 
//      SMARTaccessBase::s_sizeOfLongIntInBytes);
    fastestUnsignedDataType numBitsToShift;
    for( fastestSignedDataType i = numTimesLongIntFitsInto8Bytes - 1; i > -1; --i)
    {
      numBitsToShift = SMARTaccessBase::s_sizeOfLongIntInBytes * i * 8;
      liRawSMARTattrValue = m_rawValue[i]; //>> numBitsToShift;
      rawValueCheckSum ^= liRawSMARTattrValue;
      rawValue |= liRawSMARTattrValue;
      rawValue <<= numBitsToShift;
    }
  }
  else
  {
    rawValueCheckSum = * (uint64_t *) m_rawValue;
    rawValue = * (uint64_t *) m_rawValue;
  }  
  return rawValueCheckSum == m_rawValueCheckSum;
}

SMARTuniqueIDandValues::SMARTuniqueIDandValues (const SMARTuniqueID & _SMARTuniqueID)
{
  m_SMARTuniqueID = _SMARTuniqueID;
}

/** Copy constructor */
SMARTuniqueIDandValues::SMARTuniqueIDandValues( const SMARTuniqueIDandValues & obj)
{
  m_SMARTuniqueID = obj.getSMARTuniqueID();
  for(fastestUnsignedDataType arrayIndex = 0; arrayIndex < NUM_DIFFERENT_SMART_ENTRIES; arrayIndex ++)
  {
    m_SMARTvalues[arrayIndex] = obj.m_SMARTvalues[arrayIndex];
  }
}

SMARTuniqueIDandValues::~SMARTuniqueIDandValues ()
{
  // TODO Auto-generated destructor stub
}

int SMARTuniqueIDandValues::GetSMARTentry(
  fastestUnsignedDataType SMART_ID, SMARTvalue & sMARTvalue)
{
  sMARTvalue = m_SMARTvalues[SMART_ID];
  return 0;
}

bool operator < (const SMARTuniqueIDandValues & left,
                 const SMARTuniqueIDandValues & right)
{
  return left.getSMARTuniqueID() < right.getSMARTuniqueID();
}
