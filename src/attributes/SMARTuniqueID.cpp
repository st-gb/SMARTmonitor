/** SMARTuniqueID.cpp
 * Created on: 05.08.2016
 * Author:Stefan Gebauer, M.Sc.Comp.Sc.*/

///C/C++ standard library files:
#include <sstream>///class std::ostringstream
#include <string.h>///strcmp

///this repository's header files:
#include "SMARTuniqueID.hpp"///struct SMARTuniqueID

///Stefan Gebauer's(TU Berlin mat.#361095)~"common_sourcecode" repository files:
 ///TU_Bln361095::CstdLib::strcmp::EqlStrs
#include <C_standard_library/string_compare_return_codes.h>
 /**TU_Bln361095hardwareSMARTnumFWbytes, TU_Bln361095hardwareSMARTnumModelBytes,
  * TU_Bln361095hardwareSMARTnumSNbytes, numDifferentSMART_IDs */
#include <hardware/dataCarrier/ATA3Std.h>

bool operator < (const SMARTuniqueID & left,
                 const SMARTuniqueID & right)
{
  int retVal = strcmp(left.m_modelName, right.m_modelName);
  if( retVal < 0 )
    return true;
  else if( retVal == TU_Bln361095::CstdLib::strcmp::EqlStrs )
  {
    retVal = strcmp(left.m_firmWareName, right.m_firmWareName);
    if( retVal < 0)
      return true;
    else if( retVal == TU_Bln361095::CstdLib::strcmp::EqlStrs)
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
    strncpy(m_modelName, modelName, TU_Bln361095hardwareSMARTnumModelBytes+1);
  }
  void SMARTuniqueID::SetFirmwareName(const char * firmwareName) {
    strncpy(m_firmWareName, firmwareName, TU_Bln361095hardwareSMARTnumFWbytes+1);
  }
  void SMARTuniqueID::SetSerialNumber(const char * serialNumber) {
    strncpy(m_serialNumber, serialNumber, TU_Bln361095hardwareSMARTnumSNbytes);
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
  strncpy(m_firmWareName, orig.m_firmWareName,
    TU_Bln361095hardwareSMARTnumFWbytes + 1);
  strncpy(m_modelName, orig.m_modelName,
    TU_Bln361095hardwareSMARTnumModelBytes + 1);
  strncpy(m_serialNumber, orig.m_serialNumber,
    TU_Bln361095hardwareSMARTnumSNbytes + 1);
  
  copyArr(orig.m_SMART_IDsToRd, m_SMART_IDsToRd);
  copyArr(orig.supportedSMART_IDs, supportedSMART_IDs );
  m_busType = orig.getBusType();
  return *this;
}
