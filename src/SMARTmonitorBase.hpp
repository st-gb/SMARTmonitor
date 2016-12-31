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
//#include "libConfig/ConfigurationLoader.hpp"
#include <SMARTvalueProcessorBase.hpp> //
#include <UserInterface/UserInterface.hpp> //base class UserInterface
#include <OperatingSystem/multithread/nativeThreadType.hpp>
#include <set> //class std::set

struct CommandLineOption
{
  const char * optionName;
  const char * possibleOptionValue;
};

/** Forward declarations: */
class ConfigurationLoaderBase;

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
  
  enum programOptionNames { logFileFolder = 0, 
    /** This is used for service clients (i.e. TUIs/GUIs) to specify the 
     *  service address etc.*/
    serviceConnectionConfigFile,
    beyondLastProgramOptionName};
  static std::wstring s_programOptionValues[beyondLastProgramOptionName];
  typedef void ( * CommandLineOptionHandler_type)();
  std::map<std::wstring,CommandLineOptionHandler_type> s_programOptionName2handler;
  
  std::wstring m_stdwstrProgramePath;
  std::string m_stdstrLogFilePath;
  enum InitSMARTretCode { success = 0, readingConfigFileFailed, 
    accessToSMARTdenied, noSMARTcapableDevicePresent};
  
  void ConstructConfigFilePath(std::wstring & stdwstrConfigPathWithoutExtension);
  void ProcessCommandLineArgs();
  void HandleLogFileFolderProgramOption();
  fastestUnsignedDataType InitializeSMART();
  /** Must be declared virtual, else it cannot be overriden in a(n) (indirect) 
   *  subclass?! */
  virtual void ShowMessage(const char * const msg) const;
  SMARTaccess_type * mp_SMARTaccess;
  SMARTvalueProcessorBase m_SMARTvalueProcessor;
  void SetCommandLineArgs(int argc, char ** argv);
  void StartAsyncUpdateThread();
  void UpdateSMARTvaluesThreadSafe();
  virtual void BeforeWait() { }
  virtual void AfterGetSMARTvaluesLoop() { }
  static unsigned GetNumberOfMilliSecondsToWaitBetweenSMARTquery() {
    return s_numberOfMilliSecondsToWaitBetweenSMARTquery;
    }
  std::string FormatHumanReadable(fastestUnsignedDataType SMARTattributeID, 
    const uint64_t & SMARTrawValue);
  
  static fastestSignedDataType s_updateSMARTvalues;
  static CommandLineOption s_commandLineOptions [] ;
  /*DWORD*/ uint64_t m_arSMARTrawValue[255]; //provide space for up t 255 SMART attribute values
//  long int m_arSMART_ID[255]; //provide space for up t 255 SMART attribute values
  long int m_arTickCountOfLastQueryInMilliSeconds[255];
  fastestUnsignedDataType m_socketPortNumber;
  //TODO this member isn't needed for the service
  std::string m_stdstrServiceHostName;
  fastestUnsignedDataType m_retryWaitTimeInS;
  void InitializeLogger();
  std::set<int> m_IDsOfSMARTattributesToObserve;
  void SetSMARTattributesToObserve(std::set<SMARTuniqueIDandValues> & );
  void OutputUsage();
  CommandLineArgs<wchar_t> GetCommandLineArgs() const { return m_commandLineArgs;}
  std::wstring GetCommandOptionValue(const wchar_t * const 
    cmdLineOptionName);
  void GetCommandOptionNameAndValue(
    fastestUnsignedDataType & programArgumentIndex,
    std::wstring & cmdLineOptionName,
    std::wstring & cmdLineOptionValue
    );
  std::wstring GetCommandOptionName(std::wstring & cmdLineArg);
  std::wstring GetCommandOptionValue(/*const wchar_t * const str*/ unsigned);
  typedef std::map<SMARTuniqueID,std::string> dataCarrierID2devicePath_type;
  static dataCarrierID2devicePath_type s_dataCarrierID2devicePath;
protected:
  static unsigned s_numberOfMilliSecondsToWaitBetweenSMARTquery;
  const wchar_t ** m_cmdLineArgStrings;
  std::wstring * m_ar_stdwstrCmdLineArgs;
  /** Must persist the OnInit() method because the strings are referred from
   *  libATAsmart's SMART access class. So create the config loader on heap.*/
//  libConfig::ConfigurationLoader configurationLoader;
  ConfigurationLoaderBase * mp_configurationLoader;
  CommandLineArgs</*charType*/ wchar_t> m_commandLineArgs;
  nativeThread_type m_updateSMARTparameterValuesThread;

  //std::wstring m_cmdLineArgVector;
private:
  void ConstructConfigFilePathFromExeDirPath(
    const std::wstring & stdwstrAbsoluteFilePath,
    std::wstring & fullConfigFilePathWithoutExtension);
  void ConstructConfigFilePathFromExeFilePath(
    const std::wstring & stdwstrAbsoluteFilePath,
    const std::wstring & stdwstrThisExecutable_sFilePath);
};

#endif /* SMARTMONITORBASE_HPP */

