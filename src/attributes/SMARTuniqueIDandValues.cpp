/** SMARTuniqueIDandValues.cpp
 *  Created on: 05.08.2016
 *  Author: Stefan Gebauer, M.Sc.Comp.Sc.(TU Berlin) */

///_This_ project's (repository) (header) files:
 #include <attributes/SMARTuniqueIDandValues.hpp>
///Stefan Gebauer's(TU Berlin matriculation number 361095) ~"cmnSrc" files:
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint
 ///TU_Bln361095::CPU::atomicXchg(...)
 #include <hardware/CPU/atomic/AtomicExchange.h>
 ///LOGN(...),LOGN_DEBUG(...),LOGN_WARNING(...)
 #include <preprocessor_macros/logging_preprocessor_macros.h>

TU_Bln361095SMARTmonNmSpcBgn

///Static (class) variables definitions:
/** E.g. 32 bit Linux: size of long int is 4 bytes*/
fastestUnsignedDataType SMARTvalue::s_sizeOfLongIntInBytes = sizeof(long int);
fastestUnsignedDataType SMARTvalue::s_numTimesLongIntFitsInto8Bytes = 8/
  s_sizeOfLongIntInBytes;
ATA3Std_NrmlzdValTyp SMARTvalue::maxNrmlzdVals[];

/**\brief does all copy operations necessary for assignment operator or copy
 * constructor */
void SMARTvalue::copyAttrs(const SMARTvalue & copyFrom){
  m_timeStampOfRetrieval = copyFrom.m_timeStampOfRetrieval;
  m_timeCheckSum = copyFrom.m_timeCheckSum;
  m_successfullyReadSMARTrawValue = copyFrom.m_successfullyReadSMARTrawValue;
  SetRawValue( * (uint64_t *) copyFrom.m_rawValue);
}

///https://en.wikibooks.org/wiki/C%2B%2B_Programming/Operators/Operator_Overloading#Assignment_operator
SMARTvalue & SMARTvalue::operator = (const SMARTvalue & copyFrom)
{
  LOGN_DEBUG("begin")
  copyAttrs(copyFrom);
  return *this;
}

/** Copy constructor.
 *  http://web.stanford.edu/class/archive/cs/cs106b/cs106b.1084/cs106l/handouts/170_Copy_Constructor_Assignment_Operator.pdf :
 *  "Copy constructors are invoked whenever:
 *   1.A newly-created object is initialized to the value of an existing object.
 *   2.An object is passed to a function as a non-reference parameter.
 *   3.An object is returned from a function." */
SMARTvalue::SMARTvalue(const SMARTvalue & copyFrom)
{
  LOGN_DEBUG("begin")
  copyAttrs(copyFrom);
}

//TODO is this sufficient? parts of the raw value may have been changed
/**@param rawSMARTattrValue : An ATA S.M.A.R.T. attribute raw value has 6 bytes:
 http://en.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology
  #Known_NVMe_S.M.A.R.T._attributes : 
 * "[...]the raw value since in reality it has 48 bits". ,
 * #Known_NVMe_S.M.A.R.T._attributes  
 *  an NVMe 16 bytes.
 * So use at least an 8 byte type here.*/
/*template<typename rawValTyp>*/void SMARTattr::setRawVal(const /*uint64_t*/
  rawValTyp & rawSMARTattrValue)
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
    fastestUnsignedDataType numBitsToShift;
    /** Save the highmost bytes because they are more comprehensive:
      For e.g. the Power-on hours a value was on a 32 bit Linux:
     * 43441200000_dec ms = A1D4C1B80_hex ms ^= 12067 h as uint64_t and
     *   491527040_dec ms =  1D4C1B80_hex ms ~= 136 h as long int 
     * So if only using the highmost bits it would be: */
//    liRawSMARTattrValue = m_rawValue >> ( (8 - 
//      s_sizeOfLongIntInBytes) * 8 );
    for(TU_Bln361095::CPU::faststUint i = 0; i <
      s_numTimesLongIntFitsInto8Bytes; ++i)
    {
      numBitsToShift = //(8 - s_sizeOfLongIntInBytes) * 8;
        s_sizeOfLongIntInBytes * i * 8;
      liRawSMARTattrValue = (long int) (rawSMARTattrValue >> numBitsToShift);
      TU_Bln361095::CPU::atomicXchg( (//long int
        TU_Bln361095::hardware::CPU::atomicXchgTyp *) (m_rawValue) + i
        //(s_sizeOfLongIntInBytes * idx)//long * Target
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
    TU_Bln361095::CPU::atomicXchg( (long int *) m_rawValue, rawSMARTattrValue);
    m_rawValueCheckSum = rawSMARTattrValue;
  }
}

///TODO AtomicExchange(...) necessary?
bool SMARTvalue::GetRetrievalTime(uint64_t & uptimeInMs) const{
  long int liTimePart, timeCheckSum = m_timeStampOfRetrieval;
  for(fastestUnsignedDataType idx = 1; idx < 
    SMARTvalue::s_numTimesLongIntFitsInto8Bytes; ++idx)
  TU_Bln361095::CPU::atomicXchg( (long int *) & uptimeInMs,
    m_timeStampOfRetrieval);
  {
    liTimePart = * ( ((long int *) & m_timeStampOfRetrieval) + idx);
    TU_Bln361095::CPU::atomicXchg( ((long int *) & uptimeInMs) + idx,
      liTimePart);
    /** ServiceBasse::BeforeWait() may read this value while it is written in
    .*  the SMARTaccesBase-derived class. Therefore the checksum.*/
    timeCheckSum ^= liTimePart;
  }
#ifdef _DEBUG
  const bool identical = (timeCheckSum == m_timeCheckSum);
  if(! identical)
  	LOGN_WARNING("checksums not identical")
  return identical;
#else
  return timeCheckSum == m_timeCheckSum;
#endif
}

void SMARTvalue::SetRetrievalTime(const long double & uptimeInSeconds){
  uint64_t uptimeInMs = (uint64_t) (uptimeInSeconds * 1000.0);
  long int liTimePart;
  //TODO make as generic algorithm also for SetRawValue(...)
  liTimePart = uptimeInMs;
  TU_Bln361095::CPU::atomicXchg( (long int *) & m_timeStampOfRetrieval, liTimePart);
  //TODO Really needs atomic Compare-And-Swap?
  TU_Bln361095::CPU::atomicXchg( & m_timeCheckSum, liTimePart);
  for(fastestUnsignedDataType idx = 1; idx < 
    SMARTvalue::s_numTimesLongIntFitsInto8Bytes; ++idx)
  {
    liTimePart = * ( ((long int *) & uptimeInMs) + idx);
    TU_Bln361095::CPU::atomicXchg( ((long int *) & m_timeStampOfRetrieval) +idx, liTimePart);
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
    /** Save the highmost bytes because they are more comprehensive:
      For e.g. the Power-on hours a value was on a 32 bit Linux:
     * 43441200000_dec ms = A1D4C1B80_hex ms ^= 12067 h as uint64_t and
     *   491527040_dec ms =  1D4C1B80_hex ms ~= 136 h as long int 
     * So if only using the highmost bits it would be: */
//    liRawSMARTattrValue = liRawSMARTattrValue >> (8 - 
//      s_sizeOfLongIntInBytes);
    fastestUnsignedDataType numBitsToShift;
    for(TU_Bln361095::CPU::faststUint rawValPartIdx =
      s_numTimesLongIntFitsInto8Bytes - 1; rawValPartIdx > -1; --rawValPartIdx)
    {
      numBitsToShift = s_sizeOfLongIntInBytes * rawValPartIdx * 8;
      liRawSMARTattrValue = m_rawValue[rawValPartIdx]; //>> numBitsToShift;
      rawValueCheckSum ^= liRawSMARTattrValue;
      rawValue |=
        /** Must be unsigned before assigning/copying to "rawValue"/uint64_t.
         *  Else this happened: two's complement/value wrap happened when value
         *  > ~ 2^"size of data type in bit"/2 -> very large number when
         * interpreted as unsigned (and shifted left afterwards). */
        (unsigned long int) liRawSMARTattrValue;
      rawValue <<= numBitsToShift;
      LOGN_DEBUG("SMART raw value part (" 
        << s_sizeOfLongIntInBytes << "B) #" << 
        rawValPartIdx << ":" << liRawSMARTattrValue)
      LOGN_DEBUG("SMART raw value after iteration #" << 
        (s_numTimesLongIntFitsInto8Bytes - rawValPartIdx) << ":" << rawValue)
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
  for(TU_Bln361095::CPU::faststUint arrayIndex = 0; arrayIndex <
    TU_Bln361095numMaxATA_SMART_IDsPlus1; arrayIndex ++)
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

TU_Bln361095SMARTmonNmSpcEnd
