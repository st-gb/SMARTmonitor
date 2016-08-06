/*
 * SMARTuniqueIDandValues.cpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#include <attributes/SMARTuniqueIDandValues.hpp>

SMARTuniqueIDandValues::SMARTuniqueIDandValues ()
{
  // TODO Auto-generated constructor stub

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
