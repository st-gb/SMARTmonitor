/** File:ProcessSMARTdata.hpp
 * Author: Stefan Gebauer, M.Sc.Comp.Sc.
 * Created on 19. Juli 2020, 16:19 */

#ifndef PROCESSSMARTDATA_HPP
#define PROCESSSMARTDATA_HPP

#include <tinyxml2.h>///tinyxml2::XMLElement etc.

///Forward declarations(compiles faster than to #include the declaration)
TU_Bln361095SMARTmonNmSpcBgn
  class SMARTuniqueIDandValues;
TU_Bln361095SMARTmonNmSpcEnd

void GetSMARTrawValues(
  tinyxml2::XMLElement *,
  TU_Bln361095SMARTmonNmSpc::SMARTuniqueIDandValues &);
void GetSMARTuniqueID(tinyxml2::XMLElement *,
  TU_Bln361095SMARTmonNmSpc::SMARTuniqueIDandValues &);
tinyxml2::XMLElement * getXMLdoc(
  uint8_t SMARTdataXML[],
  fastestSignedDataType numBytesToRead,
  tinyxml2::XMLDocument & tinyXML2Doc);

namespace tinyxml2{
class SrvDataProcessor
//  : public SMARTmon::SrvDataProcessor
{
  tinyxml2::XMLDocument tinyXML2Doc;
  tinyxml2::XMLElement * p_tinyxml2XMLele;
public:
  bool Begin(const uint8_t SMARTdataXML[],
    fastestSignedDataType numBytesToRead);
  void GetSMARTrawValues(TU_Bln361095SMARTmonNmSpc::SMARTuniqueIDandValues &);
  void GetSMARTuniqueID(TU_Bln361095SMARTmonNmSpc::SMARTuniqueIDandValues &);
};
}

#endif /* PROCESSSMARTDATA_HPP */
