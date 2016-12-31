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
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN

/** Forward declaration. */
class SMARTmonitorBase;

class ConfigurationLoaderBase
{
protected:
  SMARTmonitorBase & m_r_SMARTmonitorBase;
public:
//  std::set<SMARTentry> & m_smartAttributesToObserve;
  std::set<SMARTentry> * mp_smartAttributes;

  ConfigurationLoaderBase(/*const*/
    std::set<SMARTentry> & smartAttributesToObserve,
    SMARTmonitorBase & r_SMARTmonitorBase
    )
//      m_smartAttributesToObserve(smartAttributesToObserve)
    : m_r_SMARTmonitorBase(r_SMARTmonitorBase)
  {
    LOGN(smartAttributesToObserve.size() )
    mp_smartAttributes = & smartAttributesToObserve;
  }
  virtual ~ConfigurationLoaderBase() {}
  
  virtual void ReadServiceConnectionSettings(const std::wstring & ) {};
  virtual bool LoadSMARTparametersConfiguration(const std::wstring & ) { 
    return false; };
};

#endif /* CONFIGURATIONLOADERBASE_HPP_ */
