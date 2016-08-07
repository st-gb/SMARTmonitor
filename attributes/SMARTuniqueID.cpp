/*
 * SMARTuniqueID.cpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#include <string.h> //strcmp
#include "SMARTuniqueID.hpp" //struct SMARTuniqueID

#define STRING_ARE_IDENTICAL 0

bool operator < (const SMARTuniqueID & left,
                 const SMARTuniqueID & right)
{
  int retVal = strcmp(left.model, right.model);
  if( retVal < 0 )
    return true;
  else if( retVal == STRING_ARE_IDENTICAL )
  {
    retVal = strcmp(left.firmware, right.firmware);
    if( retVal < 0)
      return true;
    else if( retVal == STRING_ARE_IDENTICAL )
    {
      if( strcmp(left.serial, right.serial) < 0)
        return true;
    }
  }
  return false;
}

//see https://en.wikipedia.org/wiki/Assignment_operator_(C%2B%2B)
SMARTuniqueID & SMARTuniqueID::operator = (const SMARTuniqueID & l)
{
//  SMARTuniqueID copy;
  strncpy(firmware, l.firmware, 9);
  strncpy(model, l.model, 41);
  strncpy(serial, l.serial, 21);
  return *this;
}
