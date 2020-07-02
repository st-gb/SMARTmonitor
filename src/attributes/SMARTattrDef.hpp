/*
 * SMARTentry.hpp
 *
 *  Created on: 06.08.2016
 *      Author: sg
 */

#ifndef ATTRIBUTES_SMARTENTRY_HPP_
#define ATTRIBUTES_SMARTENTRY_HPP_

#include <stddef.h> //size_t in <atasmart.h>
//#include <atasmart.h> //struct SkSmartAttributeParsedData
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType
#include <string> //class std::string

//typedef struct SkSmartAttributeParsedData SMARTentry;

class SMARTattrDef///def=definition: https://en.wiktionary.org/wiki/def
{
  bool m_criticalAttribute;
  fastestUnsignedDataType m_attributeID;
  std::string attributeName;
public:
  SMARTattrDef() : m_criticalAttribute(false), m_attributeID(0) { }
  /** For easy searching purposes within an STL container.: 
      "[...].find( SMARTentry(197) );" */
  SMARTattrDef(const fastestUnsignedDataType attributeID) {
    m_attributeID = attributeID;
  }
  
  fastestUnsignedDataType GetAttributeID() const { return m_attributeID; }
  void SetAttributeID(const fastestUnsignedDataType attributeID) {
    m_attributeID = attributeID;
    }
  void SetName(const char * const name ) { attributeName = name; }
  void SetCritical(const bool criticalAttribute) { 
    m_criticalAttribute = criticalAttribute; }
  bool IsCritical() const { return m_criticalAttribute; }
  const char * const GetName() const { return attributeName.c_str(); }
};

bool operator < (const SMARTattrDef & left, const SMARTattrDef & right);

#endif /* ATTRIBUTES_SMARTENTRY_HPP_ */
