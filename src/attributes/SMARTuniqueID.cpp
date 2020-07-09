/** SMARTuniqueID.cpp
 * Created on: 05.08.2016
 * Author:Stefan Gebauer, M.Sc.Comp.Sc.*/

///standard C/C++ libs
#include <sstream>///class std::ostringstream
#include <string.h> //strcmp

#include "SMARTuniqueID.hpp" //struct SMARTuniqueID
///numSMART_FWbytes, numSMARTmodelBytes, numSMART_FWbytes
#include <hardware/dataCarrier/ATA3Std.h>

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

//TODO add parameter for max items?
void SMARTuniqueID::copyArr(const fastestUnsignedDataType orig [],
  fastestUnsignedDataType cpy [])
{
  for(fastestUnsignedDataType idx = 0; idx < numDifferentSMART_IDs ; idx++ ){
    cpy[idx] = orig[idx];
    if(orig[idx] == 0)
      break;
  }
}

//see https://en.wikipedia.org/wiki/Assignment_operator_(C%2B%2B)
SMARTuniqueID & SMARTuniqueID::operator = (const SMARTuniqueID & orig)
{
//  SMARTuniqueID copy;
  strncpy(m_firmWareName, orig.m_firmWareName, numSMART_FWbytes+1);
  strncpy(m_modelName, orig.m_modelName, numSMARTmodelBytes+1);
  strncpy(m_serialNumber, orig.m_serialNumber, numSMART_SNbytes+1);
  
  copyArr(orig.m_SMART_IDsToRd, m_SMART_IDsToRd);
  copyArr(orig.supportedSMART_IDs, supportedSMART_IDs );
  return *this;
}
