/*
 * SMARTuniqueIDandValues.cpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#include <attributes/SMARTuniqueIDandValues.hpp>
#include <fastest_data_type.h>

SMARTuniqueIDandValues::SMARTuniqueIDandValues (const SMARTuniqueID & _SMARTuniqueID)
{
  m_SMARTuniqueID = _SMARTuniqueID;
}

SMARTuniqueIDandValues::SMARTuniqueIDandValues( const SMARTuniqueIDandValues & obj)
{
  m_SMARTuniqueID = obj.getSMARTuniqueID();
  for(fastestUnsignedDataType arrayIndex = 0; arrayIndex < 255; arrayIndex ++)
  {
    m_SMARTrawValues[arrayIndex] = obj.m_SMARTrawValues[arrayIndex];
  }
}

SMARTuniqueIDandValues::~SMARTuniqueIDandValues ()
{
  // TODO Auto-generated destructor stub
}

bool operator < (const SMARTuniqueIDandValues & left,
                 const SMARTuniqueIDandValues & right)
{
  return left.getSMARTuniqueID() < right.getSMARTuniqueID();
}
