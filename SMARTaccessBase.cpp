/*
 * SMARTreaderBase.cpp
 *
 *  Created on: 31.07.2016
 *      Author: sg
 */

#include <SMARTaccessBase.hpp>

/** E.g. 32 bit Linux: size of long int is 4 bytes*/
fastestUnsignedDataType SMARTaccessBase::s_sizeOfLongIntInBytes = sizeof(long int);

SMARTaccessBase::SMARTaccessBase ()
{
  /** Atomic assignment operations relay on the size of a long int. */
  //s_sizeOfLongInt = sizeof(long int);
}

SMARTaccessBase::~SMARTaccessBase ()
{
  // TODO Auto-generated destructor stub
}
