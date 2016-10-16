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
#include <set> //std::set
#include <attributes/SMARTuniqueIDandValues.hpp> //class SMARTuniqueIDandValues

class SMARTaccessBase
{
private:
//  std::set<SkIdentifyParsedData> m_SMARTuniqueIDs;
public:
  typedef std::set<SkSmartAttributeParsedData> SMARTattributesType;
  typedef const SMARTattributesType constSMARTattributesType;
protected:
  /*std::set<SkSmartAttributeParsedData>*/ SMARTattributesType SMARTattributesToObserve;
  std::set<SMARTuniqueIDandValues> m_SMARTuniqueIDandValues;
public:
  enum retCodes { success = 0, accessDenied};
  SMARTaccessBase ();
  virtual
  ~SMARTaccessBase ();

  /** Implement functionality in subclasses because the SMART access part is
   *  specific to Linux, Windows or used SMART library and so on.*/
  virtual enum retCodes ReadSMARTValuesForAllDrives() = 0;
  const std::set<SkSmartAttributeParsedData> & getSMARTattributesToObserve() const
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
};

#endif /* SMARTACCESSBASE_HPP_ */
