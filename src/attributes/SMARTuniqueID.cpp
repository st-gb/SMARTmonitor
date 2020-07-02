/** SMARTuniqueID.cpp
 * Created on: 05.08.2016
 * Author:Stefan Gebauer, M.Sc.Comp.Sc.*/

///standard C/C++ libs
#include <sstream>///class std::ostringstream
#include <string.h> //strcmp

#include "SMARTuniqueID.hpp" //struct SMARTuniqueID

#define STRING_ARE_IDENTICAL 0

bool operator < (const SMARTuniqueID & left,
                 const SMARTuniqueID & right)
{
  int retVal = strcmp(left.m_modelName, right.m_modelName);
  if( retVal < 0 )
    return true;
  else if( retVal == STRING_ARE_IDENTICAL )
  {
    retVal = strcmp(left.m_firmWareName, right.m_firmWareName);
    if( retVal < 0)
      return true;
    else if( retVal == STRING_ARE_IDENTICAL )
    {
      if( strcmp(left.m_serialNumber, right.m_serialNumber) < 0)
        return true;
    }
  }
  return false;
}

  SMARTuniqueID::~SMARTuniqueID()
  {
//    delete [] firmware; firmware = NULL;
//    delete [] model; model = NULL;
//    delete [] serial; serial = NULL;
  }

  void SMARTuniqueID::SetModelName(const char * modelName) {
    strncpy(m_modelName, modelName, 41);
  }
  void SMARTuniqueID::SetFirmwareName(const char * firmwareName) {
    strncpy(m_firmWareName, firmwareName, 9);
  }
  void SMARTuniqueID::SetSerialNumber(const char * serialNumber) {
    strncpy(m_serialNumber, serialNumber, 21);
  }

  std::string SMARTuniqueID::str() const
  {
    std::ostringstream ostr;
    ostr << *this;
    std::string str = ostr.str();
    return str; //ostr.str();
  }

  /*std::ostream & SMARTuniqueID::operator << ( std::ostream & ostr, const SMARTuniqueID & obj)
  {
    ostr << firmware << " " << model << " " << serial;
    return ostr;
  }*/

//see https://en.wikipedia.org/wiki/Assignment_operator_(C%2B%2B)
SMARTuniqueID & SMARTuniqueID::operator = (const SMARTuniqueID & l)
{
//  SMARTuniqueID copy;
  strncpy(m_firmWareName, l.m_firmWareName, 9);
  strncpy(m_modelName, l.m_modelName, 41);
  strncpy(m_serialNumber, l.m_serialNumber, 21);
  return *this;
}
