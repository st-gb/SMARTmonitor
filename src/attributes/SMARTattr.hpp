///(c) Stefan Gebauer(Comuter Science Master from TU Berlin)
///@author Stefan Gebauer(TU Berlin matricululation number 361095)

///Include guard,see http://en.wikipedia.org/wiki/Include_guard :

/**Bln=BerLiN:http://www.acronymfinder.com/Berlin-(Bln).html
 * Prgm=PRaGMa
 * Incl=INCLude:http://www.abbreviations.com/abbreviation/include
 * Grd=GuaRD:http://www.abbreviations.com/abbreviation/guard */
#ifdef TU_Bln361095usePrgmInclGrd
/**Non-standard include guard:supported by many, but not all industry compilers:
 * see http://en.wikipedia.org/wiki/Pragma_once#Portability */
  #pragma once
#endif
#if defined TU_Bln361095usePrgmInclGrd ||\
/**Bln=Berlin:https://www.acronymfinder.com/Berlin-(Bln).html
 * SMART=Self-Monitoring, Analysis and Reporting Technology:
http://en.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology
 * attr=ATTRibute: http://www.abbreviations.com/abbreviation/Attribute */\
 ! defined TU_Bln361095SMARTmon_SMARTattr_hpp
   #define TU_Bln361095SMARTmon_SMARTattr_hpp

///Stefan Gebauer's(TU Berlin matricul.number 361095) ~"cmnSrc"repository files:
 ///TU_Bln361095::dataStructures::ByteArray
 #include <data_structures/ByteArray.hpp>
 ///TU_Bln361095::CPU::atomicXchgTyp
 #include <hardware/CPU/atomic/AtomicExchange.h>
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint
 #include <hardware/dataCarrier/ATA3Std.h>///ATA3Std_NrmlzdValTyp
 ///TU_Bln361095::dataCarrier::SMART::Attr::HW_ECC_Recovered
 #include <hardware/dataCarrier/SMARTattributeNames.h>
 //TU_Bln361095OpSysNmSpc::critSectionType
 #include <OperatingSystem/multithread/nativeCriticalSectionType.hpp>
 #include <OperatingSystem/OpSys_ID_prefix.h>///TU_Bln361095OpSysNmSpc

///_This_ repository's files:
 #include <SMARTmon_ID_prefix.h>///TU_Bln361095SMARTmonNmSpcBgn
 #include <attributes/SMARTuniqueID.hpp>

#ifdef TU_Bln361095useBoostMultiprecisionCppInt
  #include <boost/multiprecision/cpp_int.hpp>///boost::multiprecision::cpp_int
#endif

#ifdef __cplusplus
  ///val=VALue: 
  #define TU_Bln361095SMARTmonSMARTattrValNmSpc \
    TU_Bln361095SMARTmonNmSpc :: SMARTattrVal
  #define TU_Bln361095SMARTmonSMARTattrValNmSpcBgn \
    TU_Bln361095SMARTmonNmSpcBgn namespace SMARTattrVal{
  #define TU_Bln361095SMARTmonSMARTattrValNmSpcEnd \
    TU_Bln361095SMARTmonNmSpcEnd }
  #define TU_Bln361095SMARTmonSMARTattrValDef(suffix) suffix
  #define TU_Bln361095SMARTmonSMARTattrValUse(suffix) \
    TU_Bln361095SMARTmonSMARTattrValNmSpc :: suffix
#endif

#define TU_Bln361095numMaxATA_SMART_IDsPlus1\
  (TU_Bln361095dataCarrierNumSATA_SMARTattrIDs\
    /** +1 to enable a 1-based array index*/ +1)
#define TU_Bln361095dataCarrierNumNVMeSMART_IDsPlus1\
  (TU_Bln361095dataCarrierNumNVMeSMARTattrIDs\
    /** +1 to enable a 1-based array index*/ +1)

TU_Bln361095SMARTmonNmSpcBgn

/**This class represents a single S.M.A.R.T. attribute.
 * this means it stores at least the S.M.A.R.T. attribute's raw value.
 * for (S)ATA S.M.A.R.T. also the normalized and threshold values.*/
class SMARTattr
{
public:
  SMARTattr(/*TU_Bln361095::CPU::faststUint numBytes*/)
    /** Initialization of this attribute is important for 
     * SetSMARTattributesToObserve(...) */
    : m_successfullyReadSMARTrawValue(0)
#ifdef TU_Bln362095useLockFreeSMARTvalAccss
  #ifndef TU_Bln362095useSMARTrawValChckSum
    , m_partIdx(-1), m_numChanges(0)
  #endif
#endif
  {}
  SMARTattr(const SMARTattr &);///copy constructor
  SMARTattr & operator = (const SMARTattr & rightOperand);///assignment operator
  
#ifdef TU_Bln361095copyRawValInParts
  ///Data types must be TU_Bln361095::CPU::atomicXchgTyp for atomic operations
  TU_Bln361095::CPU::atomicXchgTyp m_rawValue[
  /**Calculation of array size adapted from 
http://stackoverflow.com/questions/4079243/how-can-i-use-sizeof-in-a-preprocessor-macro
   */
    /*sizeof(TU_Bln361095::CPU::atomicXchgTyp) < 8 ?
    8 / sizeof(TU_Bln361095::CPU::atomicXchgTyp) : 1*/
    ///NVMe S.M.A.R.T. attribute raw values have max. 16 bytes
    TU_Bln361095dataCarrierNVMeSMARTattrNumRawValB %
      sizeof(TU_Bln361095::CPU::atomicXchgTyp) == 0 ?
      TU_Bln361095dataCarrierNVMeSMARTattrNumRawValB /
      sizeof(TU_Bln361095::CPU::atomicXchgTyp) :
      TU_Bln361095dataCarrierNVMeSMARTattrNumRawValB /
      sizeof(TU_Bln361095::CPU::atomicXchgTyp)
      ///1 element more if there's a rest after division.
      + 1];
#else
  #ifdef TU_Bln361095useBoostMultiprecisionCppInt
    /*public*/ typedef boost::multiprecision::cpp_int rawValTyp;
  #else
    public typedef uint64_t rawValTyp;
  #endif
  rawValTyp m_rawValue;
#endif
  //unsigned m_numSMARTattrRawValBytes;
  ///https://en.wikipedia.org/wiki/Non-blocking_algorithm
#ifdef TU_Bln362095useLockFreeSMARTvalAccss
 #ifdef TU_Bln362095useSMARTrawValChckSum
  ///Use this data type because it must be changable atomically. 
  TU_Bln361095::CPU::atomicXchgTyp m_rawValueCheckSum;
 #else
  ///Use this data type because it must be changable atomically. 
  TU_Bln361095::CPU::atomicXchgTyp m_partIdx;
  TU_Bln361095::CPU::atomicXchgTyp m_numChanges;
 #endif
#else
  /**1 critical section for all instances/objects of this class.
   * -advantages:
   *  -else there could be too many (15 for NVMe, 254 for ATA) critical sections
   *   if it is a member variable for every object
   *  -because S.M.A.R.T. values are changed in series, not parallel
   * -disadvantage:
   *  -other values than the one that is read may be changed and so no locking
   *   (and waiting) for this value is necessary */
  static TU_Bln361095OpSysNmSpc::critSectionType s_critSect;
#endif
  TU_Bln361095::CPU::atomicXchgTyp m_timeCheckSum;
  /**If
   * -a S.M.A.R.T. raw value can't be read atomically(for example is larger
   * than the data type for atomic operations)
   * -and the raw value should be read lock-free(without a critical section)
   * then a flag whether the copied value is consistent is needed.*/
  TU_Bln361095::CPU::atomicXchgTyp m_successfullyReadSMARTrawValue;
  /// http://en.wikipedia.org/wiki/S.M.A.R.T.#ATA_S.M.A.R.T._attributes :
  ///Normalized current value(0-255)
  TU_Bln361095::CPU::atomicXchgTyp m_normedCurrVal;
  ///Normalized threshold value(0-255)
  TU_Bln361095::CPU::atomicXchgTyp m_normedThresh;
//  long int m_normedWorstVal;///Normalized worst value(0-255)
  TU_Bln361095::dataStructures::ByteArray m_vals;
  /**Should be set from outside of this class because it depends on bus type on
   * which the SMART data carrier is attached to and on the S.M.A.R.T. attribute
   * ID. */
  TU_Bln361095::CPU::faststUint m_numSMARTattrRawValBytes;
  
  /**Declared here to have no dependancy to classes SMARTaccessBase or
   * SMARTmonitorBase */
   /**http://en.wikipedia.org/wiki/Integer_(computer_science)#Long_integer :
    * "long" int has minimum 4 bytes.*/
  static TU_Bln361095::CPU::faststUint s_sizeOfLongIntInBytes;
  static TU_Bln361095::CPU::faststUint s_numTimesLongIntFitsInto8Bytes;
  static bool highBytesSet(
    const uint8_t arr[],
    const TU_Bln361095::CPU::faststUint numBytes)
  {
    for(TU_Bln361095::CPU::faststUint arrIdx = 0;
      arrIdx < numBytes - s_sizeOfLongIntInBytes; arrIdx++)
      if(arr[arrIdx] != 0)
        return true;
    return false;
  }
  const TU_Bln361095::CPU::atomicXchgTyp GetNrmlzdCurrVal() const{
    return m_normedCurrVal;}
  ///For use in atomic compare and swap operations for example.
  TU_Bln361095::CPU::atomicXchgTyp * const GetNrmlzdCurrVal() {
    return & m_normedCurrVal;}
  const TU_Bln361095::CPU::atomicXchgTyp GetNrmlzdThresh() const{
    return m_normedThresh;}
  ///For use in atomic compare and swap operations for example.
  TU_Bln361095::CPU::atomicXchgTyp * const GetNrmlzdThresh() {
    return & m_normedThresh;}
  template<typename rawValDataTyp> bool getRawVal(
    rawValDataTyp & rawVal,
    const TU_Bln361095::CPU::faststUint SMARTattrID,
    const SMARTuniqueID & sMARTuniqueID) const
  {
#ifdef copyRawValInParts
    const TU_Bln361095::CPU::faststUint numSMARTattrRawValB = sMARTuniqueID.
      getNumSMARTattrRawValB(SMARTattrID);
    if(sizeof(rawVal) >= /*m_numRawValB*/ numSMARTattrRawValB )
    {
      rawVal = 0;
      memcpy(&rawVal, m_rawValue, /*m_numRawValB*/numSMARTattrRawValB);
      return true;
    }
    return false;
#else
    rawVal = (rawValDataTyp) m_rawValue;
    return true;
#endif
  }
  ///Gets all (raw, current, threshold) values 
  TU_Bln361095::dataStructures::ByteArray & getAllVals(){
    return m_vals;
  }
  //template<typename rawValTyp>
    void
    setRawVal(
    /*long int * ,*/ const /*uint64_t*/SMARTattr::rawValTyp & rawSMARTattrValue);
  /*template <typename SMARTattrRawValTyp>*/ void setRawVal(
    const uint8_t SMARTattrRawVal[],
    TU_Bln361095::CPU::faststUint numBytes,
    //SMARTattrRawValTyp,
    const long double & uptimeInSeconds);
  ///@param OpSysUptimeInMs OPerating SYStem uptime in MilliSeconds
  bool GetRetrievalTime(uint64_t & OpSysUptimeInMs) const;
  void SetRetrievalTime(const long double &);
  /*template <typename rawValTyp>*/ void getRawVal(
    /*const uint8_t SMARTattrRawVal[],
    const TU_Bln361095::CPU::faststUint numSMARTattrRawValBytes*/
    /*TU_Bln361095::dataStructures::ByteArray &*/
    /*rawValTyp*/SMARTattr::rawValTyp & rawVal) const;
#ifdef TU_Bln362095useLockFreeSMARTvalAccss
  bool IsConsistent(uint64_t &) const;
#endif
  static ATA3Std_NrmlzdValTyp maxNrmlzdVals[
    TU_Bln361095numMaxATA_SMART_IDsPlus1];
  ///@brief Sets the maximum normalized value for a certain SMART attribute.
  static inline void setMaxNrmlzdVal(
    const TU_Bln361095::CPU::faststUint SMARTattrID,
    const int maxNrmlzdVal)
  {
    maxNrmlzdVals[SMARTattrID]=maxNrmlzdVal;
  }
  /**@brief GETs the NeXT GreaTeR MAXimum NoRMaLiZeD VALue.
   *
   * To solve situations like: normalized current value was 200, normalized
   * threshold value 140, (pre-defined/default) maximal normalized value 100.
   * Set maximal normalized value to the next bigger default value. For example
   * to 200 when normalized threshold value=140.
   * Affects (S)ATA S.M.A.R.T. (only), not NVMe S.M.A.R.T..*/
  static inline TU_Bln361095::CPU::faststUint getNxtGtrMaxNrmlzdVal(
    const TU_Bln361095::CPU::faststUint maxNrmlzdVal,
    const TU_Bln361095::CPU::faststUint nrmlzdCurrVal,
    const TU_Bln361095::CPU::faststUint nrmlzdThresh)
  {
    //TODO?!: for some data carriers the normalized worst value > normalized
    // current value:
    // For 40 GB model "WDC WD400EB-00CPF0", firmware "06.04G06", serial number
    // "WD-WCAAT4328734" at Power-On Time "5666" for parameter ID 199 (UDMA CRC
    // error count) normalized current value=200, normalized worst value=253
    // (and normalized threshold=0).
    if(maxNrmlzdVal < ATA3Std_GtrDfltNrmlzdVal && (
      maxNrmlzdVal < nrmlzdThresh || maxNrmlzdVal < nrmlzdCurrVal) )
      return ATA3Std_GtrDfltNrmlzdVal;
    return maxNrmlzdVal;
  }
  /**@brief SETs MAXimum NoRMaLiZeD VALueS for all S.M.A.R.T. attributes.
   * Affects (S)ATA S.M.A.R.T. (only), not NVMe S.M.A.R.T..*/
  static void setMaxNrmlzdVals()
  {
    /**http://pubs.opengroup.org/onlinepubs/000095399/functions/memset.html :
     * "(void *s, int c, size_t n);"*/
    memset(///For memset(...), the array must have 1 byte data type
      maxNrmlzdVals,ATA3Std_DfltNrmlzdVal,sizeof(maxNrmlzdVals));
    /**Seems to apply to model "WDC WD400EB-00CPFO", firmware "06.04G06":
     * current normalized value=200, model "ST500LT012-1DG142", firmware
     * "0002LVM1":current normalized value=111 for serial number "W3P78D3W"*/
    setMaxNrmlzdVal(TU_Bln361095::dataCarrier::SMART::Attr::ReadErrorRate,
      ATA3Std_GtrDfltNrmlzdVal);
    /**Max Normalized value from: model:Samsung SSD 860 EVO M.2 500GB
     * firmware:RVT24B6Q serial:S5GCNJ0N506884T, current normalized value.*/
    setMaxNrmlzdVal(TU_Bln361095::dataCarrier::SMART::Attr::HW_ECC_Recovered,
      ATA3Std_GtrDfltNrmlzdVal);
  }
  static const uint64_t* getDataPtr(
    const TU_Bln361095SMARTmonNmSpc::SMARTattr::rawValTyp& SMARTattrVal
    ) //const
  {
#if __cplusplus >= 201103L /**std::is_same(...) needs at least C++ 11.*/ \
    && TU_Bln361095useBoostMultiprecisionCppInt
    if (//decltype(SMARTattrVal) == 
      /*!*/std::is_same(SMARTattrVal, boost::multiprecision::cpp_int
        //uint64_t
      )
      )
    {
      //#define TU_Bln361095useBoostMultiprecisionCppInt
#endif
#ifdef TU_Bln361095useBoostMultiprecisionCppInt
      bool valGtrUint64 = false;
      const int stopIdx = -1 + (8 / sizeof(boost::multiprecision::/*cpp_int::*/
        ///see boost/multiprecision/cpp_int/cpp_int_config.hpp
        limb_type));
      //SMARTattrVal.backend().
        ///boost::multiprecision::backends::struct cpp_int_base
        /// <MinBits, MaxBits, signed_magnitude, Checked, Allocator, false>
        //boost::multiprecision::backends::cpp_int_base::
        /// ::const_limb_pointer
        //const_limb_pointer 
       //limb_pointer limbPtr
      const boost::multiprecision::limb_type* limbPtr =
        SMARTattrVal./*m_rawValue.*/backend().//::cpp_int_base
        limbs();
      for (int i = SMARTattrVal./*m_rawValue.*/backend().size() - 1; i > stopIdx; i--)
      {
        if (limbPtr[i] != 0)
          valGtrUint64 = true;
      }
      return (uint64_t*)limbPtr/*[0]*/; //->limbs();
      //if (valGtrUint64)
      //  return "";
      //else
      //{
      //  return timeFmt;
      //}
#endif
#if __cplusplus >= 201103L /**std::is_same(...) needs at least C++ 11.*/ \
      && TU_Bln361095useBoostMultiprecisionCppInt
    }
#endif
    return NULL;
  }
private:
  inline void copyAttrs(const SMARTattr &);
  /**Has to be uint64_t in order to also work if built as 32 bit program and a
   * long uptime in ms. 1 day has 24*60*60*1000=86M 400k 000 ms.
   * 2^32=4294967296 4G 294M 967k 296/86M 400k 000~=49,7. After ca. 49,7 days
   * the value would overflow.
   *  100d each 24 h each 60 min each 60 s each 1000 ms
   *  100*24*60*60*1000=8640000000=8640M */
  uint64_t m_timeStampOfRetrieval;///Only access via Set|Get(TimeOfRetrieval);
};///end class

TU_Bln361095SMARTmonNmSpcEnd

#endif///include guard