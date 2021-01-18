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
#include <attributes/SMARTattrDefsType.hpp>///SMARTmonitor::SMARTattrDefsType

/** Forward declaration. */
class SMARTmonitorBase;

class ConfigurationLoaderBase
{
protected:
  SMARTmonitorBase & m_r_SMARTmonitorBase;
public:
//  std::set<SMARTentry> & m_smartAttributesToObserve;
  SMARTmonitor::SMARTattrDefsType * mp_SMARTattrDefs;

  ConfigurationLoaderBase(/*const*/
    SMARTmonitor::SMARTattrDefsType & SMARTattrDefs,
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
  virtual bool LoadSMARTcfg(const std::wstring & ) { 
    return false; };
};

#endif /* CONFIGURATIONLOADERBASE_HPP_ */
