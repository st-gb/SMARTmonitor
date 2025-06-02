/**Created on: 05.08.2016
 * @author Stefan Gebauer(TU Berlin Computer Science Master 2019,
 *  matriculation number:361095) */

///Include guard, see http://en.wikipedia.org/wiki/Include_guard :

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
/**Include guard supported by (nearly) all industry compilers:*/\
/**Bln=BerLiN: https://www.acronymfinder.com/Berlin-(Bln).html
 * mon=MONitor: https://www.abbreviations.com/abbreviation/Monitor
 * ID=IDentification: https://www.abbreviations.com/abbreviation/identification
 * Vals=VALueS: https://www.abbreviations.com/abbreviation/value */\
 ! defined TU_Bln361095SMARTmon__attributes__SMARTuniqueIDandVals_hpp
   #define TU_Bln361095SMARTmon__attributes__SMARTuniqueIDandVals_hpp

///"Standard C" library header files:
 #include <stdint.h>///uint64_t

///_This_ repository's files:
 //#include "SMARTaccessBase.hpp"
 #include "SMARTattr.hpp"///class TU_Bln361095::SMARTmon::SMARTattr
 #include "SMARTuniqueID.hpp"///struct TU_Bln361095SMARTmonNmSpc::SMARTuniqueID
 ///TU_Bln361095SMARTmonNmSpcBgn, TU_Bln361095SMARTmonNmSpcEnd
 #include "SMARTmon_ID_prefix.h"

///Stefan Gebauer's(TU Berlin matricul.number:361095) ~"cmnSrc"repository files:
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint
 ///TU_Bln361095hardwareATA_SMARTnumSMARTrawValB
 #include <hardware/dataCarrier/ATA3Std.h>
 ///TU_Bln361095dataCarrierNVMeSMARTattrNumRawValB
 #include <hardware/dataCarrier/NVMe/NVMe_SMART_attr.h>
///TU_Bln361095AtomicXchgTyp

//class SMARTaccessBase;

using namespace TU_Bln361095SMARTmonNmSpc;

TU_Bln361095SMARTmonNmSpcBgn

class SMARTuniqueIDandValues
{
private:
  SMARTuniqueID m_SMARTuniqueID;
public:
//  /*uint64_t*/ long int m_SMARTrawValues[NUM_DIFFERENT_SMART_ENTRIES];
//  long int m_successfullyReadSMARTrawValue[NUM_DIFFERENT_SMART_ENTRIES];
//  long int m_timeStampOfRetrieval[NUM_DIFFERENT_SMART_ENTRIES];
  TU_Bln361095SMARTmonNmSpc::SMARTattr m_SMARTattrs[
    /**Allocate even more than needed for NVMe, just to not make a distinction
     * between NVMe and (S)ATA */
    TU_Bln361095numMaxATA_SMART_IDsPlus1];
  SMARTuniqueIDandValues (const SMARTuniqueID & _SMARTuniqueID);
  SMARTuniqueIDandValues () {}
  SMARTuniqueIDandValues( const SMARTuniqueIDandValues &);
  virtual ~SMARTuniqueIDandValues ();

  const SMARTuniqueID & getSMARTuniqueID() const { return m_SMARTuniqueID; }
  ///Version for possibility to change the object.
  SMARTuniqueID & getSMARTuniqueID(){ return m_SMARTuniqueID; }
  void SetDataCarrierID(const SMARTuniqueID & sMARTuniqueID) {
    m_SMARTuniqueID = sMARTuniqueID;
    setNumAttrRawValBytes();
  }
  void setNumAttrRawValBytes()
  {
    switch(m_SMARTuniqueID.getBusType() )
    {
    case TU_Bln361095::hardware::bus::NVMe :
      for(TU_Bln361095::CPU::faststUint idx = 0; idx < 
        TU_Bln361095dataCarrierNumNVMeSMART_IDsPlus1; idx++)
        m_SMARTattrs[idx].getAllVals().SetCapacity(
          TU_Bln361095dataCarrierNVMeSMARTattrNumRawValB);
      break;
    default:
      for(TU_Bln361095::CPU::faststUint idx = 0; idx < 
        TU_Bln361095numMaxATA_SMART_IDsPlus1; idx++)
        m_SMARTattrs[idx].getAllVals().SetCapacity(
          TU_Bln361095hardwareATA_SMARTnumSMARTrawValB);
      break;
    }
  }
  /** Important if no S.M.A.R.T. attributes to read exist yet.
   * Therefore get S.M.A.R.T. values at least once in order to set the 
   * SMARTattr::m_successfullyReadSMARTrawValue */
  void setSMART_IDsToReadFromSuccSMARTrawValUpd8(){
    fastestUnsignedDataType SMART_IDsToReadArrIdx = 0;
    for(fastestUnsignedDataType sMARTvalsIdx = 1; sMARTvalsIdx <
      TU_Bln361095dataCarrierNumSATA_SMARTattrIDs; sMARTvalsIdx++)
      if(m_SMARTattrs[sMARTvalsIdx].m_successfullyReadSMARTrawValue)
        m_SMARTuniqueID.m_SMART_IDsToRd[SMART_IDsToReadArrIdx++] = sMARTvalsIdx;
    m_SMARTuniqueID.m_SMART_IDsToRd[SMART_IDsToReadArrIdx] = 0;
  }
  int getSMARTattr(const TU_Bln361095::CPU::faststUint SMARTattrID,
    TU_Bln361095SMARTmonNmSpc::SMARTattr &);
};

bool operator < (const SMARTuniqueIDandValues & left,
                 const SMARTuniqueIDandValues & right);

TU_Bln361095SMARTmonNmSpcEnd

#endif///include guard
