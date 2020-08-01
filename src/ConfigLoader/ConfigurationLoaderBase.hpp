/*
 * ConfigurationLoaderBase.hpp
 *
 *  Created on: 24.10.2014
 * Author: Stefan Gebauer,M.Sc.Comp.Sc. */

#ifndef CONFIGURATIONLOADERBASE_HPP_
#define CONFIGURATIONLOADERBASE_HPP_

//#include <SMARTDETAILSMAP.hpp>
#include <stddef.h> // size_t in <atasmart.h>
#include <set> //class std::set
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN
#include <SMARTmonitorBase.hpp>///SMARTmonitorBase::SMARTattrDefType

/** Forward declaration. */
class SMARTmonitorBase;

class ConfigurationLoaderBase
{
protected:
  SMARTmonitorBase & m_r_SMARTmonitorBase;
public:
//  std::set<SMARTentry> & m_smartAttributesToObserve;
  SMARTmonitorBase::SMARTattrDefsType * mp_SMARTattrDefs;

  ConfigurationLoaderBase(/*const*/
    SMARTmonitorBase::SMARTattrDefsType & SMARTattrDefs,
    SMARTmonitorBase & r_SMARTmonitorBase
    )
//      m_smartAttributesToObserve(smartAttributesToObserve)
    : m_r_SMARTmonitorBase(r_SMARTmonitorBase)
  {
    //TODO logging here causes crashs (if executed before opening log file?) ?
//    LOGN(smartAttributesToObserve.size() )
    mp_SMARTattrDefs = & SMARTattrDefs;
  }
  virtual ~ConfigurationLoaderBase() {}
  
  virtual void ReadServiceConnectionSettings(const std::wstring & ) {};
  virtual bool LoadSMARTparametersConfiguration(const std::wstring & ) { 
    return false; };
};

#endif /* CONFIGURATIONLOADERBASE_HPP_ */
