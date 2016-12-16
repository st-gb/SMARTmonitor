/*
 * SMARTreaderBase.hpp
 *
 *  Created on: 31.07.2016
 *      Author: sg
 */

#ifndef SMARTACCESSBASE_HPP_
#define SMARTACCESSBASE_HPP_

#include <fastest_data_type.h>
#include <stddef.h> //size_t for <atasmart.h>
#include <atasmart.h> //struct SkSmartAttributeParsedData
#include <set> //class std::set
#include <vector> //class std::vector
#include <attributes/SMARTuniqueIDandValues.hpp> //class SMARTuniqueIDandValues
#include <attributes/SMARTattributeNameAndID.hpp> //class SMARTattributeNameAndID
#include <attributes/SMARTentry.hpp> //class SMARTentry

class SMARTaccessBase
{
private:
//  std::set<SkIdentifyParsedData> m_SMARTuniqueIDs;
public:
  typedef std::set<SMARTentry> SMARTattributesContainerType;
  typedef const SMARTattributesContainerType constSMARTattributesType;
  static fastestUnsignedDataType s_sizeOfLongIntInBytes;
protected:
  SMARTattributesContainerType SMARTattributesToObserve;
  std::set<SMARTuniqueIDandValues> m_SMARTuniqueIDandValues;
public:
  enum retCodes { success = 0, accessDenied};
  SMARTaccessBase ();
  virtual
  ~SMARTaccessBase ();

  /** Implement functionality in subclasses because the SMART access part is
   *  specific to Linux, Windows or used SMART library and so on.*/
  virtual enum retCodes ReadSMARTValuesForAllDrives() = 0;
  const std::set<SMARTentry> & getSMARTattributes() const
  {
    return SMARTattributesToObserve;
  }
  fastestUnsignedDataType GetNumberOfSMARTparametersToRead() const {
    return SMARTattributesToObserve.size();
  }

  fastestUnsignedDataType GetNumberOfDifferentDrives() const {
    return m_SMARTuniqueIDandValues.size();
  }
  std::set<SMARTuniqueIDandValues> & GetSMARTuniqueIDandValues() {
    return m_SMARTuniqueIDandValues;
  }
  fastestUnsignedDataType GetNumSMARTattributesToObserve() const {
    return SMARTattributesToObserve.size(); }
  
  virtual int GetSupportedSMART_IDs(
    const char * const device,
    std::vector<SMARTattributeNameAndID> & SMARTattributeNamesAndIDs) {};
};

#endif /* SMARTACCESSBASE_HPP_ */
