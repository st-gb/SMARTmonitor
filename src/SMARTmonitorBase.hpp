/* File:   SMARTmonitorBase.hpp
 * Author: Stefan Gebauer, M.Sc. Comp.Sc.
 * Created on 17. November 2016, 13:05 */

#ifndef SMARTMONITORBASE_HPP
#define SMARTMONITORBASE_HPP

///Standard C(++) header files:
#include <set>///class std::set
#include <string>///class std::wstring

///Stefan Gebauer's(TU Berlin matr. # 361095) ~"cmnSrc" repository header files:
 ///TU_Bln361095::CPU::atomicXchgTyp
 #include <hardware/CPU/atomic/AtomicExchange.h>
 ///typedef TU_Bln361095::OpSys::nativeEvntTyp
 #include <OperatingSystem/multithread/nativeEvent_type.hpp>
 #ifdef TU_Bln361095SMARTmonMultithread
 ///typedef TU_Bln361095::OpSys::nativeThreadTyp
  #include <OperatingSystem/multithread/nativeThreadType.hpp>
 #endif
 ///class TU_Bln361095::OpSys::Process::CmdLineArgs
 #include <OperatingSystem/Process/cmdLineArgs.h>
 ///struct CommandLineOption
 #include <OperatingSystem/Process/CommandLineOption.hpp>

///_This_("SMARTmon") project's (repository) (header) files:

#include <attributes/ModelAndFirmware.hpp>///class ModelAndFirmware
#include <attributes/SMARTattrDefAccss.hpp>///base class SMARTattrDefAccss
#include <attributes/SMARTuniqueIDandValues.hpp>///class SMARTuniqueIDandValues
//#include "libConfig/ConfigurationLoader.hpp"
#include <SMARTvalueProcessorBase.hpp> //
#include <UserInterface/UserInterface.hpp>///base class UserInterface
 ///tinyxml2::ConfigLoader, TU_Bln361095::SMARTmon::CfgLoader::funcParamTyp
 #include <tinyxml2/ConfigLoader.hpp>

typedef tinyxml2::ConfigLoader CfgLoaderType;

typedef bool (CfgLoaderType::*loadFuncType)(
  std::string & errorMsg,///Reference param. must be before default arguments
  std::wstring * stdwstrWorkingDirWithConfigFilePrefix,
  std::string * stdstrFullConfigFilePath,
  TU_Bln361095::SMARTmon::CfgLoader::funcParamTyp,
  void * p_additionalParam);

///If the compiler definition exits.
#ifdef \
  /**<=>Build with direct S.M.A.R.T. access */\
  TU_Bln361095SMARTmonDrctSMARTaccss
  ///typedef TU_Bln361095::SMARTmon::SMARTaccessTyp
  #include <SMARTaccType.hpp>
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

std::wstring GetExeFileName(const wchar_t * const ar_wchFullProgramPath);

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
  enum GetSMARTvalsMode{directSMARTvals, noUpdate};
  enum GetSMARTvalsMode m_getSMARTvalsMode;
  enum prcssCmdLneArgsRtrnVals {sccssfllyParsedAllCmdLneArgs = 0,//dontShowUI = 1,
    calledHelp};
  const struct tm & GetLastSMARTvaluesUpdateTime() const {
    //TODO because "tm" is a struct with multiple fields/members:
    // changes non-atomically in get SMART values thread.
    // ->possible inconsistency
    return m_timeOfLastSMARTvaluesUpdate; }
  struct tm m_timeOfLastSMARTvaluesUpdate;
  ///attr=attribute Def=definition Cont=container
  typedef std::/*set<SMARTattrDef>*/ map<unsigned,SMARTattrDef> 
    SMARTattrDefContType;
  typedef const SMARTattrDefContType constSMARTattrDefContType;
  typedef SMARTattrDefContType::const_iterator 
    SMARTattrContConstIterType;
//  SMARTattrDefContType SMARTattrDefs;
  typedef std::set<SMARTuniqueIDandValues> SMARTuniqueIDandValsContType;
  typedef std::set<ModelAndFirmware> ModelAndFirmwareTuplesType;
  ModelAndFirmwareTuplesType m_modelAndFirmwareTuples;
protected:
  SMARTuniqueIDandValsContType SMARTuniqueIDsAndValues;
  bool asynCnnct = true;
  TU_Bln361095::OpSys::nativeEvntTyp waitOrSignalEvt;
  static std::string s_strAllLogLevels;
public:
  SMARTmonitorBase();
  //template<typename charType>
  /*SMARTmonitorBase(int argc, charType ** argv) {
    m_commandLineArgs.Set(argc, argv);
    }*/
  virtual ~SMARTmonitorBase();
  
  static void sigHandler(int signo);
  static void regCancelSelectSigHandler();

  static void setDfltSMARTattrDef();
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
    logLvlIdx,
    beyondLastProgramOptionName};
protected:
  /** This array is usually filled from:
   *  -command line arguments
   *  -or from config file */
  static std::wstring s_programOptionValues[beyondLastProgramOptionName];
public:
#ifdef TU_Bln361095SMARTmonMultithread
  TU_Bln361095::OpSys::nativeThreadTyp connectThread;
#endif
  virtual void startSrvCnnctCntDown(){};
  fastestUnsignedDataType m_cnnctTimeOutInSec;
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
   *  (Only methods of this class can be used there, but because the method
   *  is virtual it passes execution to the implementation in 
   *  SMARTmonitorClient.) */
  virtual fastestUnsignedDataType GetSMARTattrValsFromSrv() {return 1;}
  fastestUnsignedDataType ProcessCommandLineArgs();
  void HandleLogFileFolderProgramOption(std::wstring & cmdLineOptionValue);
  fastestUnsignedDataType InitializeSMART();
  /** Must be declared virtual, else it cannot be overriden in a(n) (indirect) 
   *  subclass?! */
  virtual void ShowMessage(const char * const msg) const;
  virtual void ShowMessage(const std::string & str) const{
    ShowMessage(str.c_str() );
  }
  /** Must be declared virtual, else it cannot be overriden in a(n) (indirect) 
   *  subclass?! */
  virtual void ShowMessage(const char * const msg, UserInterface::MessageType::messageTypes) const;
#ifdef TU_Bln361095SMARTmonDrctSMARTaccss
  TU_Bln361095::SMARTmon::SMARTaccessTyp * mp_SMARTaccess;
  TU_Bln361095::SMARTmon::SMARTaccessTyp m_SMARTaccess;
  SMARTvalueProcessorBase m_SMARTvalueProcessor;
#endif
  void SetCommandLineArgs(int argc, char ** argv);
#ifdef TU_Bln361095SMARTmonMultithread
  ///Should be usable by different targets, e.g. service, wxWidgets or curses
  /// client. Therefore the parameters.
  void StartAsyncUpdateThread(
    //GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType
    struct GetSMARTvaluesFunctionParams & getSMARTvaluesFunctionParams);
  void StartAsyncUpdateThread(
    GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType
      getSMARTvaluesFunctionType
    );
  bool upd8SMARTparamValsThrdIsRunning(){
    return m_updateSMARTparameterValuesThread.IsRunning();}
#endif
  bool tryCfgFilePaths(const wchar_t fileName[], loadFuncType,
    void * p_additionalParam);
#ifdef TU_Bln361095SMARTmonDrctSMARTaccss
  fastestUnsignedDataType Upd8SMARTvalsDrctlyThreadSafe();
#endif
  virtual void BeforeWait() { }
  virtual void AfterGetSMARTvaluesLoop(int getSMARTvaluesResult) { }
  static unsigned GetNumberOfMilliSecondsToWaitBetweenSMARTquery() {
    return s_numberOfMilliSecondsToWaitBetweenSMARTquery;
    }
  /* Use same data type as AtomExchange function from common_sourcecode repo
   * to avoid adress sanitizer errors. */
  static /*fastestSignedDataType*/ TU_Bln361095::CPU::atomicXchgTyp
    s_updateSMARTvalues;
  static CommandLineOption/*<char>*/ s_commandLineOptions [] ;
  /*DWORD*/ uint64_t m_arSMARTrawValue[255]; //provide space for up t 255 SMART attribute values
//  long int m_arSMART_ID[255]; //provide space for up t 255 SMART attribute values
  long int m_arTickCountOfLastQueryInMilliSeconds[255];
  fastestUnsignedDataType m_socketPortNumber;
  //TODO this member isn't needed for the service
  std::string m_stdstrServiceHostName;
  fastestUnsignedDataType m_retryWaitTimeInS;
  void GetUsage(std::ostringstream &);
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
  fastestUnsignedDataType GetNumSMARTattrToObs(){return m_IDsOfSMARTattrsToObserve.size(); }
  void SetSMARTattributesToObserve(std::set<SMARTuniqueIDandValues> & );
  TU_Bln361095::CPU::faststUint m_SMARTattrIDsToObs[
    TU_Bln361095dataCarrierNumSATA_SMARTattrIDs];
  void OutputUsage();
  virtual void SetGetSMARTvalMode(enum GetSMARTvalsMode){};
  TU_Bln361095::OpSys::Process::CmdLineArgs<wchar_t> GetCommandLineArgs() const
    { return m_commandLineArgs;}
  
  std::wstring GetCommandLineOptionValue(const wchar_t * const cmdLineOptionName);
  void GetCmdLineOptionNameAndValue(
    fastestUnsignedDataType & programArgumentIndex,
    std::wstring & cmdLineOptionName,
    std::wstring & cmdLineOptionValue
    );
  std::wstring GetCommandOptionName(std::wstring & cmdLineArg);
  std::wstring GetCommandOptionValue(/*const wchar_t * const str*/ unsigned);
  const ModelAndFirmware * getDataCarrierAttrDefs(const SMARTuniqueID &) const;
  
  static dataCarrierID2devicePath_type s_dataCarrierID2devicePath;
  inline void WaitForSignalOrTimeout(
    const fastestUnsignedDataType numberOfMilliSecondsToWaitBetweenSMARTquery);
  inline void WaitForTimeout(
    fastestUnsignedDataType numberOfSecondsToWaitBetweenSMARTquery,
    const fastestUnsignedDataType numberOfMilliSecondsToWaitBetweenSMARTquery);
protected:
  static unsigned s_numberOfMilliSecondsToWaitBetweenSMARTquery;
  const wchar_t ** m_cmdLineArgStrings;
  std::wstring * m_ar_stdwstrCmdLineArgs;
  /** Must persist the OnInit() method because the strings are referred from
   *  libATAsmart's SMART access class. So create the config loader on heap.*/
//  libConfig::ConfigurationLoader configurationLoader;
  CfgLoaderType m_cfgLoader;
  TU_Bln361095::OpSys::Process::CmdLineArgs</*charType*/ wchar_t>
    m_commandLineArgs;
#ifdef TU_Bln361095SMARTmonMultithread
  TU_Bln361095::OpSys::nativeThreadTyp m_updateSMARTparameterValuesThread;
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

extern SMARTmonitorBase * gp_SMARTmon;

#endif /* SMARTMONITORBASE_HPP */

