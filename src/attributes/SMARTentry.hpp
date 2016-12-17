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
#include <fastest_data_type.h> //fastestUnsignedDataType
#include <string> //class std::string

//typedef struct SkSmartAttributeParsedData SMARTentry;

class SMARTentry
{
  bool m_criticalAttribute;
  fastestUnsignedDataType attributeID;
  std::string attributeName;
public:
  SMARTentry() : m_criticalAttribute(false) { }
  
  fastestUnsignedDataType GetAttributeID() const { return attributeID; }
  void SetAttributeID(const fastestUnsignedDataType attributeID) {
    this->attributeID = attributeID;
    }
  void SetName(const char * const name ) { attributeName = name; }
  void SetCritical(const bool criticalAttribute) { 
    m_criticalAttribute = criticalAttribute; }
  bool IsCritical() const { return m_criticalAttribute; }
  const char * const GetName() const { return attributeName.c_str(); }
};

bool operator < (const SMARTentry & left, const SMARTentry & right);

#endif /* ATTRIBUTES_SMARTENTRY_HPP_ */