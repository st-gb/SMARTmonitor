/* File:   SMARTmonitorBase.hpp
 * Author: sg
 * Created on 17. November 2016, 13:05 */

#ifndef SMARTMONITORBASE_HPP
#define SMARTMONITORBASE_HPP

///Standard C(++) header files:
#include <set>///class std::set
#include <string>///class std::wstring

///common_sourcecode repository header files:
#include <OperatingSystem/multithread/nativeThreadType.hpp>
#include <OperatingSystem/Process/CommandLineArgs.hpp> //class CommandLineArgs
///struct CommandLineOption
#include <OperatingSystem/Process/CommandLineOption.hpp>

#include <attributes/SMARTattrDefAccss.hpp>///base class SMARTattrDefAccss
#include <attributes/SMARTuniqueIDandValues.hpp>///class SMARTuniqueIDandValues
//#include "libConfig/ConfigurationLoader.hpp"
#include <SMARTvalueProcessorBase.hpp> //
#include <UserInterface/UserInterface.hpp> //base class UserInterface

#if directSMARTaccess
  #include <SMARTaccType.hpp>///typedef SMARTaccess_type
#endif

/** Forward declarations: */
class ConfigurationLoaderBase;
class SMARTmonitorBase;

/** Needed for passing class functions of SMARTmonitorBase and its derived 
 *  classes to ::UpdateSMARTparameterValuesThreadFunc(...) */
struct GetSMARTvaluesFunctionParams
{
  SMARTmonitorBase * p_SMARTmonitorBase;
  //pointer to member function of SMARTmonitorBase
  typedef fastestUnsignedDataType (SMARTmonitorBase::*GetSMARTvaluesFunctionType)();
  GetSMARTvaluesFunctionType p_getSMARTvaluesFunction = NULL;
};

/** Use character type in order to pass to to CommandLineArgs member variable*/
/*template<typename charType>*/
///Base class for all clients/services. Operations and attributes that can be 
// used by all are located here.
class SMARTmonitorBase
  : public UserInterface
  /** Make subclass of SMARTattrDefAccss and do not inherit SMARTaccessBase from
   * it so that no SMARTaccessBase-derived class is needed (for socket-only).*/ 
  , public SMARTattrDefAccss
{
public:
  ///attr=attribute Def=definition Cont=container
  typedef std::/*set<SMARTattrDef>*/ map<unsigned,SMARTattrDef> 
    SMARTattrDefContType;
  typedef SMARTattrDef * SMARTattrDefsType;
  typedef const SMARTattrDefContType constSMARTattrDefContType;
  typedef SMARTattrDefContType::const_iterator 
    SMARTattrContConstIterType;
//  SMARTattrDefContType SMARTattrDefs;
  typedef std::set<SMARTuniqueIDandValues> SMARTuniqueIDandValsContType;
protected:
  SMARTuniqueIDandValsContType SMARTuniqueIDsAndValues;
public:
  SMARTmonitorBase();
  //template<typename charType>
  /*SMARTmonitorBase(int argc, charType ** argv) {
    m_commandLineArgs.Set(argc, argv);
    }*/
  virtual ~SMARTmonitorBase();
  
  /** Needs to persist over the call to 
   *  ::UpdateSMARTparameterValuesThreadFunc(...) so making it a member 
   *  variable is feasible. 
   *  (-> creating it on stack before calling
   *  ::UpdateSMARTparameterValuesThreadFunc(...) is not possible because
   *  the stack content is popped and changed afterwards.) */
  struct GetSMARTvaluesFunctionParams m_getSMARTvaluesFunctionParams;
  SMARTuniqueIDandValsContType & GetSMARTuniqueIDsAndVals() { return 
    SMARTuniqueIDsAndValues; }
  
  enum programOptionNames { logFileFolder = 0, 
    /** This is used for service clients (i.e. TUIs/GUIs) to specify the 
     *  service address etc.*/
    serviceConnectionConfigFile,
    beyondLastProgramOptionName};
protected:
  /** This array is usually filled from:
   *  -command line arguments
   *  -or from config file */
  static std::wstring s_programOptionValues[beyondLastProgramOptionName];
public:
  std::wstring GetProgramOptionValue(const /*enum programOptionNames*/
    fastestUnsignedDataType programOptionName) {
//    const wchar_t * programOptionValue = L"";
    std::wstring * p_stdwstrProgramOptionValue;
    if( programOptionName < beyondLastProgramOptionName )
      p_stdwstrProgramOptionValue = & s_programOptionValues[programOptionName];
    return * p_stdwstrProgramOptionValue;
    };
  
  typedef void ( * CommandLineOptionHandler_type)();
  //TODO use/implement
  std::map<std::wstring,CommandLineOptionHandler_type> s_programOptionName2handler;
  
  std::wstring m_stdwstrProgramePath;
  std::string m_stdstrLogFilePath;
  enum InitSMARTretCode { success = 0, readingConfigFileFailed, 
    accessToSMARTdenied, noSMARTcapableDevicePresent};
  
  void ConstructConfigFilePath(std::wstring & stdwstrConfigPathWithoutExtension);
  /** Exists to enable SMARTmonitorClient::GetSMARTvaluesFromServer to be 
   *  executed in ::UpdateSMARTparameterValuesThreadFunc(...) 
   *  (Only methods of this class can be used there, but becase the method
   *  is virtual it passes execution to the implementation in 
   *  SMARTmonitorClient.) */
  virtual fastestUnsignedDataType GetSMARTattrValsFromSrv() {}
  fastestUnsignedDataType ProcessCommandLineArgs();
  void HandleLogFileFolderProgramOption(std::wstring & cmdLineOptionValue);
  fastestUnsignedDataType InitializeSMART();
  /** Must be declared virtual, else it cannot be overriden in a(n) (indirect) 
   *  subclass?! */
  virtual void ShowMessage(const char * const msg) const;
  /** Must be declared virtual, else it cannot be overriden in a(n) (indirect) 
   *  subclass?! */
  virtual void ShowMessage(const char * const msg, UserInterface::MessageType::messageTypes) const;
#ifdef directSMARTaccess
  SMARTaccess_type * mp_SMARTaccess;
  SMARTaccess_type m_SMARTaccess;
  SMARTvalueProcessorBase m_SMARTvalueProcessor;
#endif
  void SetCommandLineArgs(int argc, char ** argv);
#ifdef multithread
  ///Should be usable by different targets, e.g. service, wxWidgets or curses
  /// client. Therefore the parameters.
  void StartAsyncUpdateThread(
    //GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType
    struct GetSMARTvaluesFunctionParams & getSMARTvaluesFunctionParams);
  void StartAsyncUpdateThread(
    GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType
      getSMARTvaluesFunctionType
    );
#endif
  fastestUnsignedDataType Upd8SMARTvalsDrctlyThreadSafe();
  virtual void BeforeWait() { }
  virtual void AfterGetSMARTvaluesLoop(int getSMARTvaluesResult) { }
  static unsigned GetNumberOfMilliSecondsToWaitBetweenSMARTquery() {
    return s_numberOfMilliSecondsToWaitBetweenSMARTquery;
    }
  
  static fastestSignedDataType s_updateSMARTvalues;
  static CommandLineOption/*<char>*/ s_commandLineOptions [] ;
  /*DWORD*/ uint64_t m_arSMARTrawValue[255]; //provide space for up t 255 SMART attribute values
//  long int m_arSMART_ID[255]; //provide space for up t 255 SMART attribute values
  long int m_arTickCountOfLastQueryInMilliSeconds[255];
  fastestUnsignedDataType m_socketPortNumber;
  //TODO this member isn't needed for the service
  std::string m_stdstrServiceHostName;
  fastestUnsignedDataType m_retryWaitTimeInS;
  bool InitializeLogger();
  void EnsureSMARTattrToObsExist();
  ///A std::set ensures no double entries exist.
  /// Only SMART attr IDs in this container are read and updated in UI.
  typedef std::set<int> SMARTattrToObsType;
  //TODO this should be per data carrier? Because if nothing is specified it 
  // could get the S.M.A.R.T. IDs to observe from the supported S.M.A.R.T. 
  // attributes list (via direct S.M.A.R.T. access). And the supported 
  // S.M.A.R.T. IDs differ between drives.
  SMARTattrToObsType m_IDsOfSMARTattrsToObserve;
  fastestUnsignedDataType m_SMARTattrIDsToObs[numDifferentSMART_IDs];
  fastestUnsignedDataType GetNumSMARTattrToObs(){return m_IDsOfSMARTattrsToObserve.size(); }
  void SetSMARTattributesToObserve(std::set<SMARTuniqueIDandValues> & );
  void OutputUsage();
  CommandLineArgs<wchar_t> GetCommandLineArgs() const { return m_commandLineArgs;}
  
  std::wstring GetCommandLineOptionValue(const wchar_t * const cmdLineOptionName);
  void GetCmdLineOptionNameAndValue(
    fastestUnsignedDataType & programArgumentIndex,
    std::wstring & cmdLineOptionName,
    std::wstring & cmdLineOptionValue
    );
  std::wstring GetCommandOptionName(std::wstring & cmdLineArg);
  std::wstring GetCommandOptionValue(/*const wchar_t * const str*/ unsigned);
  
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
#ifdef multithread
  nativeThread_type m_updateSMARTparameterValuesThread;
#endif
  //std::wstring m_cmdLineArgVector;
private:
  void ConstructConfigFilePathFromExeDirPath(
    const std::wstring & stdwstrAbsoluteFilePath,
    std::wstring & fullConfigFilePathWithoutExtension);
  void ConstructConfigFilePathFromExeFilePath(
    const std::wstring & stdwstrAbsoluteFilePath,
    const std::wstring & stdwstrThisExecutable_sFilePath);
};
DWORD THREAD_FUNCTION_CALLING_CONVENTION UpdateSMARTparameterValuesThreadFunc(
  void * p_v);

#endif /* SMARTMONITORBASE_HPP */

