///(c) Stefan Gebauer

///_This_ project's (repository) header files:
 //#include "SMARTuniqueIDandValues.hpp"///class SMARTattr declaration
 #include "SMARTattr.hpp"///this class' declaration

#ifdef TU_Bln361095useBoostMultiprecisionCppInt
  #pragma message("__FILE__--TU_Bln361095useBoostMultiprecisionCppInt defined")
  #include <boost/multiprecision/cpp_int.hpp>///boost::multiprecision::cpp_int
#else
  #pragma message("__FILE__--TU_Bln361095useBoostMultiprecisionCppInt NOT defined")
#endif

using namespace TU_Bln361095SMARTmonNmSpc;

///static/class variable definitions:
#ifndef TU_Bln362095useLockFreeSMARTvalAccss
  TU_Bln361095OpSysNmSpc::critSectionType TU_Bln361095SMARTmonNmSpc::SMARTattr::
    s_critSect;
#endif

TU_Bln361095SMARTmonNmSpcBgn

/*template <typename rawValTyp>*/ void SMARTattr::getRawVal(
  //const uint8_t SMARTattrRawVal[],
  //const TU_Bln361095::CPU::FaststUint numSMARTattrRawValBytes
  /*TU_Bln361095::dataStructures::ByteArray & allVals*/
  /*rawValTyp*//*SMARTuniqueID::SMARTattrRawValType*/
  TU_Bln361095SMARTmonNmSpc::SMARTattr::rawValTyp &
    rawVal
  ) const
{
  /**Can not change the whole S.M.A.R.T. attribute raw value with an atomic
   * exchange operation.*/
  if(m_numSMARTattrRawValBytes > s_sizeOfLongIntInBytes)
#ifdef TU_Bln361095useLockFreeSMARTvalAccss
    /**Try to avoid so-called starvation (endless loop here so the raw value may
     * be never read) by signalling a thread that wants to modify the raw value
     * that we are about to read this value. */
    TU_Bln361095::CPU::atomicXChng(m_concurrencyState, wantToRead);
  ///See 
  do{
  /**A reading thread should check if "m_numBeforeChange" or "m_numChanges" has
   * changed: then the raw value is going to be written to.*/
    int numBeforeChangeBeforeRead = m_numBeforeChange;
    int numChangesBeforeRead = m_numChanges;
#else
  /**Concurrent threads should enter this critical section (directly/shortly)
   * before reading or writing the S.M.A.R.T. attribute (raw) value and leave
   * this critical section afterwards.*/
    s_critSect.Enter();
#endif
#ifdef TU_Bln362095copyRawValInParts
  //unsigned long int liRawSMARTattrValue;
  /**This is not known at compile time if data carriers with different
   * S.M.A.R.T. attribute raw value sizes(NVMe:16 bytes, (S)ATA:6 bytes) should
   * be possible.*/
  const TU_Bln361095::CPU::faststUint numSMARTattrRawValBmodSizeOfLongIntInB
    numSMARTattrRawValBmodSizeOfLongIntInB =
    m_numSMARTattrRawValBytes % s_sizeOfLongIntInBytes;
  const TU_Bln361095::CPU::faststUint numLongIntsForSMARTattrRawVal =
    m_numSMARTattrRawValBytes / s_sizeOfLongIntInBytes +
    numSMARTattrRawValBmodSizeOfLongIntInB == 0 ? 0 :
    /**If the size of the raw value is not a multiple of the size of long int
     * then an additional long int ist necessary.*/
    1;
  const uint8_t * const p_SMARTattrRawVal = m_vals.GetArray();
  for(TU_Bln361095::CPU::faststUint SMARTattrRawValLongIntArrIdx = 0;
    SMARTattrRawValLongIntArrIdx < numLongIntsForSMARTattrRawVal;
    ++SMARTattrRawValLongIntArrIdx)
  {
    //numBytesToShift = //(8 - s_sizeOfLongIntInBytes) * 8;
    //  s_sizeOfLongIntInBytes * i;
    //liRawSMARTattrValue = ( ( (long int*) SMARTattrRawVal) + numBytesToShift);
    *( ( (long int*)p_SMARTattrRawVal)+SMARTattrRawValLongIntArrIdx) =
      m_rawValue[SMARTattrRawValLongIntArrIdx];
  }
#else
  rawVal = m_rawValue;
#endif
#ifdef TU_Bln362095useLockFreeSMARTvalAccss
  ///Probably changed while copying the value, so repeat the copy process.
  while(numBeforeChangeBeforeRead != m_numBeforeChange ||
    numChangesBeforeRead != m_numChanges);
  TU_Bln361095::CPU::atomicXChng(m_concurrencyState, wasRead);
#else
  /**Concurrent threads should enter this critical section before reading or
   * writing the S.M.A.R.T. attribute (raw) value and leave this critical
   * section afterwards.*/
  s_critSect.Leave();
#endif
}

/**An ATA S.M.A.R.T. attribute raw value has 6 bytes:
 * ///TODO quote T13 docs here 
 * An NVMe S.M.A.R.T. attribute raw value has up to 16 bytes:
 * ///TODO quote T13 docs here 
 * So use an array and number of bytes
 * type here.*/
/*template <typename SMARTattrRawValTyp>*/ void SMARTattr::setRawVal(
  const uint8_t SMARTattrRawVal[],
  const TU_Bln361095::CPU::FaststUint numSMARTattrRawValBytes,
  //SMARTattrRawValTyp & SMARTattrRawVal,
  const long double & uptimeInSeconds)
{
  TU_Bln361095::CPU::atomicXchg(& m_successfullyReadSMARTrawValue, 1);
  SetRetrievalTime(uptimeInSeconds);
  /**Can not change the whole S.M.A.R.T. attribute raw value with an atomic
   * exchange operation.*/
  if(numSMARTattrRawValBytes > s_sizeOfLongIntInBytes)
#ifdef TU_Bln361095useLockFreeSMARTvalAccss
    if(m_concurrencyState != wantToRead)
    /**A reading thread should check if "m_numBeforeChange" has changed: then
     * the raw value is going to be written to.*/
      TU_Bln361095::CPU::atomicXchg(&m_numBeforeChange, ++m_numBeforeChange);
#else
  /**Concurrent threads should enter this critical section (directly) before
   * reading or writing the S.M.A.R.T. attribute (raw) value and leave this
   * critical section afterwards.*/
    s_critSect.Enter();
#endif

#ifdef TU_Bln361095copyRawValInParts
  ///Idea: only change the bytes that are not zero. This is only possible where
  /// values are increasing and not overflowing. Because else 
  //if(SMARTattr::highBytesSet(SMARTattrRawVal, numSMARTattrRawValBytes) )
  //  ;
  unsigned long int liRawSMARTattrValue;
  TU_Bln361095::CPU::FaststUint numBtillLongInt = numSMARTattrRawValBytes %
    s_sizeOfLongIntInBytes;
  TU_Bln361095::CPU::FaststUint numLongIntsForRawVal = numSMARTattrRawValBytes /
    s_sizeOfLongIntInBytes + numBtillLongInt == 0 ? 0 : 1;
  for(TU_Bln361095::CPU::FaststUint SMARTattrRawValLongIntArrIdx = 0;
    SMARTattrRawValLongIntArrIdx < /*s_numTimesLongIntFitsInto8Bytes*/
    numLongIntsForRawVal;
    ++SMARTattrRawValLongIntArrIdx)
  {
    liRawSMARTattrValue = *((long int*)SMARTattrRawVal + 
      SMARTattrRawValLongIntArrIdx);
    if( (SMARTattrRawValLongIntArrIdx + 1) == numLongIntsForRawVal &&
      numBtillLongInt != 0)
      memset( (uint8_t*) &liRawSMARTattrValue + numBtillLongInt, 0,
        s_sizeOfLongIntInBytes - numBtillLongInt);
#if defined TU_Bln362095useLockFreeSMARTvalAccss &&\
    /**Don't need atomic exchange of the raw value when a memory fence happens
     * afterwards (in atomic exchange of the counter/checksum) ?*/\
    defined TU_Bln362095atomicExchgForSMARTattrVals
    TU_Bln361095::hardware::CPU::atomicXchg( (long int *) (m_rawValue) +
      SMARTattrRawValLongIntArrIdx
      //(s_sizeOfLongIntInBytes * i)//long * Target
      , //* (long int *) //SMARTattributesToObserveIter->pretty_value /*raw*/ /*long val*/
      liRawSMARTattrValue );
#else
    m_rawValue[SMARTattrRawValLongIntArrIdx] = liRawSMARTattrValue;
#endif
  }
#else///TU_Bln361095copyRawValInParts
  #ifdef TU_Bln361095useBoostMultiprecisionCppInt /*TU_Bln361095BoostRootDir*/
  //m_rawValue = SMARTattrRawVal;
  ///see https://stackoverflow.com/questions/15720841/boost-multiprecision-cpp-int-convert-into-an-array-of-bytes
  ///https://www.boost.org/doc/libs/1_65_0/libs/multiprecision/doc/html/boost_multiprecision/tut/import_export.html
  boost::multiprecision::import_bits(m_rawValue, SMARTattrRawVal,
    SMARTattrRawVal + numSMARTattrRawValBytes, 0, /** MSV first*/false);

  /////https://stackoverflow.com/questions/52958235/how-do-i-convert-an-array-of-bytes-to-a-boostmultiprecisionuint128-t
  //m_rawValue = 1;
  ////boost::multiprecision::cpp_int::import_bits(boostMultiPrecNum, number, number+16);
  ////memset(&boostMultiPrecNum, 0, 16);
  /////Write number encoded as byte array into "boost multiprecision" number.
  //memcpy(&m_rawValue, SMARTattrRawVal, (size_t)numSMARTattrRawValBytes);

  //m_rawValue.backend().limbs
  #endif
#endif
  //else
  //  setRawVal(* ( (uint64_t *)SMARTattrRawVal) );
#ifdef TU_Bln362095useLockFreeSMARTvalAccss
  if(numSMARTattrRawValBytes > s_sizeOfLongIntInBytes)
  /**A reading thread should check if "m_numChanges" has changed: then the raw
   * value was written to.*/
    TU_Bln361095::hardware::CPU::atomicXchg(&m_numChanges, ++m_numChanges);
#else
    s_critSect.Leave();
#endif
}

TU_Bln361095SMARTmonNmSpcEnd
