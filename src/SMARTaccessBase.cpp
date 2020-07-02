/** SMARTaccessBase.cpp
 *  Created on: 31.07.2016
 *  Author: Stefan Gebauer, M.Sc. Comp.Sc./Informatik (TU Berlin) */

#include <SMARTaccessBase.hpp>
///dataCarrier::getNumBwrittenSinceOSstart(...)
#include <hardware/dataCarrier/getNumBwrittenSinceOSstart.hpp>

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
  //get device path belonging to sMARTuniqueID.
//  const char * dataCarrierPath = //getDevicePath(sMARTuniqueID);
  const uint64_t numBwrittenSinceOSstart = /*OperatingSystem::*/dataCarrier::
    getNumBwrittenSinceOSstart(stdstrDataCarrierPath);
  sMARTuniqueID.guessUnit(SMARTattrID, SMARTrawVal, numBwrittenSinceOSstart);
}
