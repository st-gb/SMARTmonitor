/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMARTmonitorBase.hpp
 * Author: sg
 *
 * Created on 17. November 2016, 13:05
 */

#ifndef SMARTMONITORBASE_HPP
#define SMARTMONITORBASE_HPP

#include <string> //std::wstring
#include <Process/CommandLineArgs.hpp> //class CommandLineArgs
#include "libConfig/ConfigurationLoader.hpp"
#include <SMARTvalueProcessorBase.hpp> //
#include <UserInterface/UserInterface.hpp> //base class UserInterface

/** Use character type in order to pass to to CommandLineArgs member variable*/
/*template<typename charType>*/ class SMARTmonitorBase
  : public UserInterface
{
public:
  SMARTmonitorBase();
  //template<typename charType>
  /*SMARTmonitorBase(int argc, charType ** argv) {
    m_commandLineArgs.Set(argc, argv);
    }*/
  virtual ~SMARTmonitorBase();
  
  void ConstructConfigFilePath(std::wstring & stdwstrConfigPathWithoutExtension);
  void InitializeSMART();
  void ShowMessage(const char * const msg) const;
  SMARTaccess_type * mp_SMARTaccess;
  SMARTvalueProcessorBase m_SMARTvalueProcessor;
protected:
  /** Must persist the OnInit() method because the strings are referred from
   *  libATAsmart's SMART access class. So create the config loader on heap.*/
//  libConfig::ConfigurationLoader configurationLoader;
  libConfig::ConfigurationLoader * mp_configurationLoader;
  void InitializeLogger();
  CommandLineArgs</*charType*/ wchar_t> m_commandLineArgs;
  //std::wstring m_cmdLineArgVector;
private:

};

#endif /* SMARTMONITORBASE_HPP */

