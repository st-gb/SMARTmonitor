/** SMARTentry.hpp
 *  Created on: 06.08.2016
 * @author Stefan Gebauer(TU Berlin matriculation number 361095)*/

///Include guard, see http://en.wikipedia.org/wiki/Include_guard :

/**Bln=BerLiN:http://www.acronymfinder.com/Berlin-(Bln).html
 * Prgm=PRaGMa
 * Incl=INCLude:http://www.abbreviations.com/abbreviation/include
 * Grd=GuaRD:http://www.abbreviations.com/abbreviation/guard */
#ifdef TU_Bln361095usePrgmInclGrd
/**Non-standard include guard:supported by many, but not all industry compilers:
 * see http://en.wikipedia.org/wiki/Pragma_once#Portability */
  #pragma once
#endif
#if defined TU_Bln361095usePrgmInclGrd ||\
/**Include guard supported by (nearly) all industry compilers:*/\
/**Bln=BerLiN: https://www.acronymfinder.com/Berlin-(Bln).html
 * mon=MONitor: http://www.abbreviations.com/abbreviation/monitor
 * attr=ATTRibute: http://www.abbreviations.com/abbreviation/Attribute
 * Def=DEFinition: http://www.abbreviations.com/abbreviation/definition */\
 ! defined TU_Bln361095SMARTmon__SMARTattrDef_hpp
   #define TU_Bln361095SMARTmon__SMARTattrDef_hpp

///C(++) standard library files:
 #include <string>///class std::string

///Stefan Gebauer's(TU Berlin matricul. num. 361095) ~"cmnSrc" repository files:
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint

class SMARTattrDef///def=definition: https://en.wiktionary.org/wiki/def
{
  bool m_criticalAttribute;
  TU_Bln361095::CPU::faststUint m_attributeID;
  std::string attributeName;
public:
  SMARTattrDef() : m_criticalAttribute(false), m_attributeID(0) { }
  /** For easy searching purposes within an STL container.: 
      "[...].find( SMARTentry(197) );" */
  SMARTattrDef(const TU_Bln361095::CPU::faststUint attributeID)
    : m_attributeID(attributeID)
  {
  }
  SMARTattrDef(
    const TU_Bln361095::CPU::faststUint attrID,
    const char attrName [])
  {
    m_attributeID = attrID;
    attributeName = attrName;
  }
  
  TU_Bln361095::CPU::faststUint GetAttributeID() const { return m_attributeID;}
  void SetAttributeID(const TU_Bln361095::CPU::faststUint attributeID) {
    m_attributeID = attributeID;
    }
  void SetName(const char * const name ) { attributeName = name; }
  void SetCritical(const bool criticalAttribute) { 
    m_criticalAttribute = criticalAttribute; }
  bool IsCritical() const { return m_criticalAttribute; }
  const char * const GetName() const { return attributeName.c_str(); }
};

bool operator < (const SMARTattrDef & left, const SMARTattrDef & right);

#endif///include guard
