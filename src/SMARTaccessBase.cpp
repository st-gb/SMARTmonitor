/** SMARTaccessBase.cpp
 *  Created on: 31.07.2016
 *  Author: Stefan Gebauer, M.Sc. Comp.Sc./Informatik (TU Berlin) */

#include <SMARTaccessBase.hpp>
///dataCarrier::getNumBwrittenSinceOSstart(...)
#include <hardware/dataCarrier/getNumBwrittenSinceOSstart.hpp>
///dataCarrier::getNumB_readSinceOSstart(...)
#include <hardware/dataCarrier/getNumB_readSinceOSstart.hpp>
#include <OperatingSystem/time/GetUpTime.h>///OperatingSystem::GetUptimeInS(...)

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
   case SMARTattributeNames::PowerOnTime:{
    long int uptimeInNs;
    OperatingSystem::GetUptimeInNs(uptimeInNs);
    sMARTuniqueID.guessUnit(SMARTattrID, SMARTrawVal, uptimeInNs
      /**to ms*//1000000);
    }
    break;
    /** The unit for "Total Data/LBAs Written/Read" differs among models. For
     * HFS256G39TND-N210A, firmware:30001P10 (serial:EJ7CN55981080CH09)
     * Solid State Device (SSD) it seems to be GiB rather than LBAs.*/
   case SMARTattributeNames::TotalDataWritten:{
    //get device path belonging to sMARTuniqueID.
  //  const char * dataCarrierPath = //getDevicePath(sMARTuniqueID);
    ///For SSDs may be more than written by OS due to.wear levelling.
    const uint64_t numBwrittenSinceOSstart = /*OperatingSystem::*/dataCarrier::
      getNumBwrittenSinceOSstart(stdstrDataCarrierPath);
    sMARTuniqueID.guessUnit(SMARTattrID, SMARTrawVal, numBwrittenSinceOSstart);
    }
    break;
   case SMARTattributeNames::TotalDataRead:{
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
    const uint64_t numBreadSinceOSstart = /*OperatingSystem::*/dataCarrier::
      getNumB_readSinceOSstart(stdstrDataCarrierPath);
    sMARTuniqueID.guessUnit(SMARTattrID, SMARTrawVal, numBreadSinceOSstart);
    }
    break;
  }
}
