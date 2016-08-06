/*
 * ConfigurationLoaderBase.hpp
 *
 *  Created on: 24.10.2014
 *      Author: mr.sys
 */

#ifndef CONFIGURATIONLOADERBASE_HPP_
#define CONFIGURATIONLOADERBASE_HPP_

//#include <SMARTDETAILSMAP.hpp>
#include <stddef.h> // size_t in <atasmart.h>
#include <set> //class std::set
#include <attributes/SMARTentry.hpp>

class ConfigurationLoaderBase
{
//protected:
public:
//  SMARTDETAILSMAP & m_oSMARTDetails;
  std::set<SMARTentry> & m_smartAttributesToObserve;

  ConfigurationLoaderBase(/*const*/ //SMARTDETAILSMAP & oSMARTDetails
    std::set<SkSmartAttributeParsedData> & smartAttributesToObserve )
    : //m_oSMARTDetails(oSMARTDetails)
      m_smartAttributesToObserve(smartAttributesToObserve)
  {

  }
};

#endif /* CONFIGURATIONLOADERBASE_HPP_ */
