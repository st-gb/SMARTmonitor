/** SMARTaccessBase.cpp
 *  Created on: 31.07.2016
 *  Author: Stefan Gebauer, M.Sc. Comp.Sc./Informatik (TU Berlin) */

///This repository's header files:
#include <SMARTaccessBase.hpp>

///Stefan Gebauer's(TU Berlin matr.#361095)~"common_sourcecode"repository files:
///dataCarrier::getNumBwrittenSinceOSstart(...)
#include <OperatingSystem/hardware/dataCarrier/getNumBwrittenSinceOSstart.h>
///dataCarrier::getNumB_readSinceOSstart(...)
#include <OperatingSystem/hardware/dataCarrier/getNumB_readSinceOSstart.h>

 ///for Windows' GetTimeCountInSeconds(...) in "GetTimeCount.h"
 typedef double TU_Bln361095tmCntInSecTyp;///TiMe CouNT IN SEConds TYPe
 ///for GetTimeCountInNanoSeconds(...)
 typedef long double TU_Bln361095tmCntInNsTyp;///TiMe CouNT IN NanoSeconds TYPe
 ///OperatingSystem::GetUptimeInNs(...)
 #include <OperatingSystem/time/GetUpTime.h>

TU_Bln361095SMARTmonNmSpcBgn

/** E.g. 32 bit Linux: size of long int is 4 bytes*/
fastestUnsignedDataType SMARTaccessBase::s_sizeOfLongIntInBytes = sizeof(long int);

SMARTaccessBase::SMARTaccessBase(SMARTuniqueIDandValsContType & 
  sMARTuniqueIDsandVals)
  : m_SMARTuniqueIDsandVals(sMARTuniqueIDsandVals)
{
  /** Atomic assignment operations relay on the size of a long int. */
  //s_sizeOfLongInt = sizeof(long int);
}

SMARTaccessBase::~SMARTaccessBase ()
{
  // TODO Auto-generated destructor stub
}

void SMARTaccessBase::possiblyAutoDetectUnit(
  const fastestUnsignedDataType SMARTattrID,
  const uint64_t & SMARTrawVal,
  SMARTuniqueID & sMARTuniqueID,
  const std::string & stdstrDataCarrierPath)
{
  switch(SMARTattrID){
  case TU_Bln361095::dataCarrier::SMART::Attr::PowerOnTime:{
    /** "long int" may not be sufficient for uptime in ns (especially for 32
     * bit) */
    TU_Bln361095tmCntInNsTyp uptimeInNs;
    OperatingSystem::GetUptimeInNs(uptimeInNs);
    sMARTuniqueID.guessUnit(SMARTattrID, SMARTrawVal, uptimeInNs
      /**to ms*//1000000);
    }
    break;
   /** see TU Berlin OS Design slides winter term 2014/2015, SSDs lecture about
    * how many data is erased by a write operation.*/
   case TU_Bln361095::dataCarrier::SMART::Attr::GiB_Erased:
	 /** The unit for "GiB/data erased" (attr. ID 100) could be approximated via
	  * low level IO functions by OS that determine the written sector/offset
	  * + length of write operations.*/
//     OperatingSystem::getNumDifferentBlocksWritten();
     break;
    /** The unit for "Total Data/LBAs Written/Read" differs among models. For
     * HFS256G39TND-N210A, firmware:30001P10 (serial:EJ7CN55981080CH09)
     * Solid State Device (SSD) it seems to be GiB rather than LBAs.*/
   case TU_Bln361095::dataCarrier::SMART::Attr::TotalDataWritten:{
    //get device path belonging to sMARTuniqueID.
  //  const char * dataCarrierPath = //getDevicePath(sMARTuniqueID);
    /**For SSDs in reality there may be more written by its SSD controller than
     * by operating system because of wear levelling and write amplification.*/
    uint64_t numWrittenBsinceOSstart;
    const enum TU_Bln361095::OpSys::dataCarrier::
     getNumWrittenBsinceOSstart::Rslt getNumWrittenBsinceOSstartRslt =
      TU_Bln361095::OpSys::dataCarrier::GetNumWrittenBsinceOSstart(
        stdstrDataCarrierPath.c_str(),
        & numWrittenBsinceOSstart);
    if(getNumWrittenBsinceOSstartRslt == TU_Bln361095::OpSys::dataCarrier::
      getNumWrittenBsinceOSstart::Success)
      sMARTuniqueID.guessUnit(SMARTattrID, SMARTrawVal, numWrittenBsinceOSstart);
    }
    break;
   case TU_Bln361095::dataCarrier::SMART::Attr::TotalDataRead:{
    ///Use 64 bit value because it can get very high (>409257094144)
    /**Caching may take effect on data carrier side so that less bytes are read
    * than assumed.->Caching must be disabled before?*/
    /** Under Linux for "Total Data Read" this can be tested via 
     * "sudo dd if=device skip=numBlockToSkip of=/dev/zero count=numBlockToRead"
     * where
     * -device:the whole data carrier or a partion, safest is using 
     *   a free partition or a disabled swap partion.
     * -numBlockToSkip: num blocks to skip to overcome caching
     * -numBlockToRead: use ca. 1000000 for 512 B block size and unit GiB*/
     uint64_t numBreadSinceOSstart;
    const enum TU_Bln361095::OpSys::dataCarrier::
     getNumReadBsinceOSstart::Rslt getNumReadBsinceOSstartRslt =
      TU_Bln361095::OpSys::dataCarrier::GetNumReadBsinceOSstart(
        stdstrDataCarrierPath.c_str(),
        & numBreadSinceOSstart);
    if(getNumReadBsinceOSstartRslt == TU_Bln361095::OpSys::dataCarrier::
       getNumReadBsinceOSstart::Success)
      sMARTuniqueID.guessUnit(SMARTattrID, SMARTrawVal, numBreadSinceOSstart);
    }
    break;
  }
}

TU_Bln361095SMARTmonNmSpcEnd
