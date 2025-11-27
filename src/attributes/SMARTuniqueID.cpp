/**(c) Stefan Gebauer(Computer Science Master 2019 from TU Berlin)
 * Created on: 05.08.2016
 * @author:Stefan Gebauer (TU Berlin matriculation number 361095) */

///C/C++ standard library files:
#include <sstream>///class std::ostringstream
#include <string.h>///strcmp

///this repository's header files:
#include "SMARTuniqueID.hpp"///struct SMARTuniqueID

///Stefan Gebauer's(TU Berlin mat.#361095)~"common_sourcecode" repository files:
 ///TU_Bln361095::CstdLib::strcmp::EqlStrs
#include <CstdLib/strcmpRtrnCds.h>
 /**TU_Bln361095hardwareSMARTnumFWbytes, TU_Bln361095hardwareSMARTnumModelBytes,
  * TU_Bln361095hardwareSMARTnumSNbytes, numDifferentSMART_IDs */
#include <hardware/dataCarrier/ATA3Std.h>

TU_Bln361095SMARTmonNmSpcBgn

/**This function is useful/necessary for Standard Template Library (STL)
 * containers like std::set.*/
bool operator < (const TU_Bln361095::SMARTmon::SMARTuniqueID & left,
                 const TU_Bln361095::SMARTmon::SMARTuniqueID & right)
{
  int retVal =
  /**To not compare with characters beyond the array size either:
   * -end the character string with string terminating '\0' character and use
   *  strcmp(...)
   * -use strncmp(...) */
    strncmp(left.m_modelName, right.m_modelName,
      TU_Bln361095hardwareSMARTnumModelBytes);
  /**If left character string is lexicographically less than right character
   * string.*/
  if(retVal < TU_Bln361095::CstdLib::strcmp::EqlStrs)
    return true;
  else if( retVal == TU_Bln361095::CstdLib::strcmp::EqlStrs )
  {
    retVal =
    /**To not compare with characters beyond the array size either:
     * -end the character string with string terminating '\0' character and use
     *  strcmp(...)
     * -use strncmp() */
      strncmp(left.m_firmWareName, right.m_firmWareName,
        TU_Bln361095hardwareSMARTnumFWbytes);
    /**If left character string is lexicographically less than right character
     * string.*/
    if(retVal < TU_Bln361095::CstdLib::strcmp::EqlStrs)
      return true;
    else if( retVal == TU_Bln361095::CstdLib::strcmp::EqlStrs)
    {
      if(
    /**To not compare with characters beyond the array size either:
     * -end the character string with string terminating '\0' character and use
     *  strcmp(...)
     * -use strncmp() */
        strncmp(left.m_serialNumber, right.m_serialNumber,
          TU_Bln361095hardwareSMARTnumSNbytes)
        /**If left character string is lexicographically less than right
         * character string.*/
        < TU_Bln361095::CstdLib::strcmp::EqlStrs)
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
void SMARTuniqueID::copyArr(
  const TU_Bln361095::CPU::faststUint orig [],
  TU_Bln361095::CPU::faststUint cpy [])
{
  for(TU_Bln361095::CPU::faststUint idx = 0;idx < m_numDiffrntSMARTattrIDs;
    idx++)
  {
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

TU_Bln361095SMARTmonNmSpcEnd
