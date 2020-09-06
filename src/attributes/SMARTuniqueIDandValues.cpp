/** SMARTuniqueIDandValues.cpp
 *  Created on: 05.08.2016
 *  Author: Stefan Gebauer, M.Sc.Comp.Sc.(TU Berlin) */

#include <attributes/SMARTuniqueIDandValues.hpp>
#include <hardware/CPU/fastest_data_type.h>
#include "hardware/CPU/atomic/AtomicExchange.h"
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN())

/** E.g. 32 bit Linux: size of long int is 4 bytes*/
fastestUnsignedDataType SMARTvalue::s_sizeOfLongIntInBytes = sizeof(long int);
fastestUnsignedDataType SMARTvalue::s_numTimesLongIntFitsInto8Bytes = 8/
  s_sizeOfLongIntInBytes;

///https://en.wikibooks.org/wiki/C%2B%2B_Programming/Operators/Operator_Overloading#Assignment_operator
SMARTvalue & SMARTvalue::operator = ( const SMARTvalue & copyFrom )
{
  LOGN_DEBUG("begin")
  m_timeStampOfRetrieval = copyFrom.m_timeStampOfRetrieval;
  m_successfullyReadSMARTrawValue = copyFrom.m_successfullyReadSMARTrawValue;
  SetRawValue( * (uint64_t *) copyFrom.m_rawValue);
  return *this;
}

/** copy c'tor */
SMARTvalue::SMARTvalue(SMARTvalue & copyFrom)
{
  LOGN_DEBUG("begin")
  m_timeStampOfRetrieval = copyFrom.m_timeStampOfRetrieval;
  m_successfullyReadSMARTrawValue = copyFrom.m_successfullyReadSMARTrawValue;
  SetRawValue( * (uint64_t *) copyFrom.m_rawValue);
}

//TODO is this sufficient? psrts of the raw value may have been changed
void SMARTvalue::SetRawValue(const /** A SMART raw value has 6 bytes. So use
  an 8 byte type here */ uint64_t & rawSMARTattrValue)
{
  LOGN_DEBUG("for raw value:" << rawSMARTattrValue)
  /** important:-1.Use same datatype as atomic functions because of its size 
     -2. use an unsigned type else there were miscalutions. */
  unsigned long int liRawSMARTattrValue;
  m_rawValueCheckSum = 0;
  /** If shorter than a (8 byte) uint64_t */
  if(s_sizeOfLongIntInBytes < 8)
  {
    /** because a SMART raw value takes 6 bytes.
     *   But on a 32 bit OS (even when executed on a 64 bit CPU) this value can't
     *    be stored in 1 thread safe atomic operation ( e.g. 
     *    __sync_lock_test_and_set() when using GCC/g++).
     *   So a "check sum" is calcutated from the RAW value.
     *   Because of multithreading: while a part of the checksum is copied
     *    it may happen that other parts are changed and so the whole value 
     *    may become inconsistent:
     *   -for instance the SMART raw value may be
     *    0xFF FF FF FF.and then changes to 0x01 00 00 00 00 value here in 
     *     this function while another threads reads it, it finally may become:
     *    0x01 FF FF FF FF
     *    so an exclusive or operation is used in order to detect these cases:
     *     originally:         00 00 00 00 xor FF FF FF FF = FF FF FF FF
     *     reading thread does:00 00 00 01 xor FF FF FF FF = FF FF FF FE
     *    -for instance the SMART raw value may be
     *    0xFF FF FF FF.and then changes to 0x01 00 00 00 00 while copying the
     *     value here in this function, it finally may become
     *    0x01 FF FF FF FF  */
    fastestUnsignedDataType numTimesLongIntFitsInto8Bytes = 8/
      s_sizeOfLongIntInBytes;
    fastestUnsignedDataType numBitsToShift;
    /** Save the highmost bytes because they are more comprehensive:
      For e.g. the Power-on hours a value was on a 32 bit Linux:
     * 43441200000_dec ms = A1D4C1B80_hex ms ^= 12067 h as uint64_t and
     *   491527040_dec ms =  1D4C1B80_hex ms ~= 136 h as long int 
     * So if only using the highmost bits it would be: */
//    liRawSMARTattrValue = m_rawValue >> ( (8 - 
//      s_sizeOfLongIntInBytes) * 8 );
    for( fastestUnsignedDataType i = 0; i < numTimesLongIntFitsInto8Bytes; ++i)
    {
      numBitsToShift = //(8 - s_sizeOfLongIntInBytes) * 8;
        s_sizeOfLongIntInBytes * i * 8;
      liRawSMARTattrValue = (long int) (rawSMARTattrValue >> numBitsToShift);
      AtomicExchange( (long int *) (m_rawValue) + i 
        //(s_sizeOfLongIntInBytes * i)//long * Target
        , //* (long int *) //SMARTattributesToObserveIter->pretty_value /*raw*/ /*long val*/
        liRawSMARTattrValue );
      LOGN_DEBUG("SMART raw value part (" 
        << s_sizeOfLongIntInBytes << "B) #" << 
        i << ":" << liRawSMARTattrValue)
      m_rawValueCheckSum ^= liRawSMARTattrValue;
    }
  }
  else
  {
//    liRawSMARTattrValue = rawSMARTattrValue;
    //TODO atomic necessary?
    AtomicExchange( (long int *) m_rawValue, rawSMARTattrValue);
    m_rawValueCheckSum = rawSMARTattrValue;
  }
}

///TODO AtomicExchange(...) necessary?
bool SMARTvalue::GetRetrievalTime(uint64_t & uptimeInMs) const{
  long int liTimePart, timeCheckSum = m_timeStampOfRetrieval;
  AtomicExchange( (long int *) & uptimeInMs, m_timeStampOfRetrieval);
  for(fastestUnsignedDataType idx = 1; idx < 
    SMARTvalue::s_numTimesLongIntFitsInto8Bytes; ++idx)
  {
    liTimePart = * ( ((long int *) & m_timeStampOfRetrieval) + idx);
    AtomicExchange( ((long int *) & uptimeInMs) + idx, liTimePart);
    /** ServiceBasse::BeforeWait() may read this value while it is written in
    .*  the SMARTaccesBase-derived class. Therefore the checksum.*/
    timeCheckSum ^= liTimePart;
  }
  return timeCheckSum == m_timeCheckSum;
}

void SMARTvalue::SetRetrievalTime(const long double & uptimeInSeconds){
  uint64_t uptimeInMs = (uint64_t) (uptimeInSeconds * 1000.0);
  long int liTimePart;
  //TODO make as generic algorithm also for SetRawValue(...)
  liTimePart = uptimeInMs;
  AtomicExchange( (long int *) & m_timeStampOfRetrieval, liTimePart);
  m_timeCheckSum = liTimePart;
  for(fastestUnsignedDataType idx = 1; idx < 
    SMARTvalue::s_numTimesLongIntFitsInto8Bytes; ++idx)
  {
    liTimePart = * ( ((long int *) & uptimeInMs) + idx);
    AtomicExchange( ((long int *) & m_timeStampOfRetrieval) +idx, liTimePart);
    /** ServiceBasse::BeforeWait() may read this value while it is written in
     *  the SMARTaccesBase-derived class. Therefore the checksum.*/
    m_timeCheckSum ^= liTimePart;
  }
}

/** @brief purpose of this function: because a SMART raw value takes 6 bytes.
 *   But on a 32 bit OS (even when executed on a 64 bit CPU) this value can't
 *    be stored in 1 thread safe atomic operation ( e.g. 
 *    __sync_lock_test_and_set() when using GCC/g++).
 *   So a "check sum" is calcutated using the RAW value parts stored in this 
 *    class. If the raw value then matches */
bool SMARTvalue::IsConsistent(uint64_t & rawValue) const
{
  /** important:-1.Use same datatype as atomic functions because of its size 
     -2. use an unsigned type else there were miscalculations. */
  long int liRawSMARTattrValue, rawValueCheckSum = 0;
  /** If shorter than a uint64_t */
  if(s_sizeOfLongIntInBytes < 8 )
  {
    rawValue = 0;
    fastestUnsignedDataType numTimesLongIntFitsInto8Bytes = 8/
      s_sizeOfLongIntInBytes;
    /** Save the highmost bytes because they are more comprehensive:
      For e.g. the Power-on hours a value was on a 32 bit Linux:
     * 43441200000_dec ms = A1D4C1B80_hex ms ^= 12067 h as uint64_t and
     *   491527040_dec ms =  1D4C1B80_hex ms ~= 136 h as long int 
     * So if only using the highmost bits it would be: */
//    liRawSMARTattrValue = liRawSMARTattrValue >> (8 - 
//      s_sizeOfLongIntInBytes);
    fastestUnsignedDataType numBitsToShift;
    for( fastestSignedDataType i = numTimesLongIntFitsInto8Bytes - 1; i > -1; --i)
    {
      numBitsToShift = s_sizeOfLongIntInBytes * i * 8;
      liRawSMARTattrValue = m_rawValue[i]; //>> numBitsToShift;
      rawValueCheckSum ^= liRawSMARTattrValue;
      rawValue |= liRawSMARTattrValue;
      rawValue <<= numBitsToShift;
      LOGN_DEBUG("SMART raw value part (" 
        << s_sizeOfLongIntInBytes << "B) #" << 
        i << ":" << liRawSMARTattrValue)
      LOGN_DEBUG("SMART raw value after iteration #" << 
        (numTimesLongIntFitsInto8Bytes - i) << ":" << rawValue)
    }
  }
  else
  {//TODO runtime error by here.
    //TODO beim kopieren Big/Little endian beachten?
    rawValueCheckSum = /*(uint64_t *)*/ m_rawValue[0];
    rawValue = * (uint64_t *) m_rawValue;
  }
#ifdef DEBUG
  if( rawValueCheckSum != m_rawValueCheckSum )
    LOGN_WARNING("values differ: " << rawValueCheckSum << " " <<
      m_rawValueCheckSum)
#endif
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
  for(fastestUnsignedDataType arrayIndex = 0; arrayIndex <
    numDifferentSMART_IDsPlus1; arrayIndex ++)
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
