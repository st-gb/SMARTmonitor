/** Author: Stefan Gebauer, M.Sc. Comp. Sc.
 * Created on 17. November 2016, 13:05 */

///Standard C/C++ header files:
#include <unistd.h>///sleep(unsigned)

///Stefan Gebauer's common_sourcecode git repository:
/** Include 1st to avoid MinGW GCC (9.2.0) "warning: #warning Please include
 *  winsock2.h before windows.h [-Wcpp]" */
#include <OperatingSystem/BSD/socket/socket.h>///CloseSocket()
#include <Controller/character_string/stdtstr.hpp>///GetStdWstring(...)
#include <Controller/Logger/LogFileAccessException.hpp>
typedef double TimeCountInSecType;///for GetTimeCountInSeconds(...)
#include <Controller/time/GetTickCount.hpp>
#include <FileSystem/File/GetAbsoluteFilePath.hpp>///GetAbsoluteFilePath(...)
#include <FileSystem/GetCurrentWorkingDir.hpp> //OperatingSystem::GetCurrentWorkingDirA_inl(...)
#include <FileSystem/PathSeperatorChar.hpp>///FileSystem::dirSeperatorChar
#include <FileSystem/path_seperator.h>///PATH_SEPERATOR_CHAR_STRING
#include <hardware/CPU/atomic/AtomicExchange.h>///AtomicExchange(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
///OperatingSystem::GetCurrentTime(...)
#include <OperatingSystem/time/GetCurrentTime.hpp>

#include "SMARTmonitorBase.hpp"///class SMARTmonitorBase
#include <SMARTaccessBase.hpp> //class SMARTaccessBase
#include <tinyxml2/ConfigLoader.hpp> //class tinyxml2::ConfigLoader
#include <ConfigLoader/ParseConfigFileException.hpp>

/** Static/Class members must be defined once in a source file. Do this here */
dataCarrierID2devicePath_type SMARTmonitorBase::s_dataCarrierID2devicePath;
unsigned SMARTmonitorBase::s_numberOfMilliSecondsToWaitBetweenSMARTquery = 10000;
AtomicExchType SMARTmonitorBase::s_updateSMARTvalues = 1;
extern const char FileSystem::dirSeperatorChar;
CommandLineOption SMARTmonitorBase::s_commandLineOptions [] = {
  {"logfilefolder", "<absolute or relative log file FOLDER>, e.g. \"/run/\" "
    "for writing log files to tmpfs/RAM drives"},
  {"svcConnConfFile", "<absolute or relative service config file FOLDER>, e.g. \"server.xml"},
  {""}
};

/** Initialized with default values. */
std::wstring SMARTmonitorBase::s_programOptionValues[beyondLastProgramOptionName] = {
  L".", L""//service."
};

//SMARTmonitorBase::HandleOptionLogFilePath()
//{
//  
//}

SMARTmonitorBase::SMARTmonitorBase()
  ///Use port number > well-known ports (1024) for less privileges.
  : m_socketPortNumber(2000),
//    mp_cfgLoader(NULL),
    m_cfgLoader((SMARTmonitor::SMARTattrDefsType /*&*/) SMARTaccessBase::
      getSMARTattrDefs(), *this),
    m_cmdLineArgStrings(NULL),
    m_ar_stdwstrCmdLineArgs(NULL),
  m_timeOutInSeconds(30)
#ifdef directSMARTaccess
  , m_SMARTaccess(SMARTuniqueIDsAndValues)
#endif
{
  /** For calling ::UpdateSMARTparameterValuesThreadFunc(void *) */
  m_getSMARTvaluesFunctionParams.p_SMARTmonitorBase = this;
#ifdef directSMARTaccess
  mp_SMARTaccess = & m_SMARTaccess;
#endif
  setDfltSMARTattrDef();
//  mp_cfgLoader = new tinyxml2::ConfigLoader(
//    (SMARTattrDefsType /*&*/) SMARTaccessBase::getSMARTattrDefs(), * this);
  //  InitializeLogger();
  
  //TODO
//  s_programOptionName2handler.insert(std::make_pair(L",));
}

SMARTmonitorBase::~SMARTmonitorBase() {
//  if( mp_cfgLoader )
//    delete mp_cfgLoader;
  if(m_cmdLineArgStrings )
    delete [] m_cmdLineArgStrings;
  if( m_ar_stdwstrCmdLineArgs)
    delete [] m_ar_stdwstrCmdLineArgs;
}

//https://gcc.gnu.org/onlinedocs/cpp/Stringification.html#Stringification
#define xstringify(s) stringify(s)
#define stringify(s) #s

///from https://stackoverflow.com/questions/14421656/is-there-widely-available-wide-character-variant-of-file
#define makeWchar_concat(x) L##x
#define makeWchar(x) makeWchar_concat(x)

/** Rationale: even If the S.M.A.R.T. attribute definition configuration file is
 * missing parameter names are possible. */
void SMARTmonitorBase::setDfltSMARTattrDef(){
  SMARTattrDefAccss::Set(1, "Read Error Rate");
  SMARTattrDefAccss::Set(2, "Throughput Performance");
  SMARTattrDefAccss::Set(3, "Spin-Up Time");
  SMARTattrDefAccss::Set(4, "Start/Stop Count");
  SMARTattrDefAccss::Set(5, "Reallocated Sectors Count");
  SMARTattrDefAccss::Set(6, "Read Channel Margin");
  SMARTattrDefAccss::Set(7, "Seek Error Rate");
  SMARTattrDefAccss::Set(8, "Seek Time Performances");
  SMARTattrDefAccss::Set(9, "Power-On Time");
  SMARTattrDefAccss::Set(10, "Spin Retry Count");
  SMARTattrDefAccss::Set(11, "Recalibration Retries or Calibration Retry Count");
  SMARTattrDefAccss::Set(12, "Power Cycle Count");
  
  SMARTattrDefAccss::Set(100, "Data Erased");
  
  SMARTattrDefAccss::Set(168, "SATA Physical Error Count");
  SMARTattrDefAccss::Set(169, "Bad Block Count");
  
  SMARTattrDefAccss::Set(171, "Program Fail Count");
  SMARTattrDefAccss::Set(172, "SSD Erase Fail Count");
  
  SMARTattrDefAccss::Set(174, "Unexpected Power Loss Count");
  SMARTattrDefAccss::Set(175, "Power Loss Protection Failure");
  SMARTattrDefAccss::Set(176, "Erase Fail Count");
  SMARTattrDefAccss::Set(177, "Wear Range Delta");
  SMARTattrDefAccss::Set(178, "Used Reserved Block Count (Chip)");
  SMARTattrDefAccss::Set(179, "Used Reserved Block Count Total");
  SMARTattrDefAccss::Set(180, "Unused Reserved Block Count Total");
  SMARTattrDefAccss::Set(181, "Program Fail Count");
  SMARTattrDefAccss::Set(182, "Erase Fail Count");
  SMARTattrDefAccss::Set(183, "Runtime Bad Blocks");
  SMARTattrDefAccss::Set(184, "End to End Error");
  
  SMARTattrDefAccss::Set(187, "Reported Uncorrectable Errors");
  SMARTattrDefAccss::Set(188, "Command Timeout");
  SMARTattrDefAccss::Set(189, "High Fly Writes");
  SMARTattrDefAccss::Set(190, "Temperature Difference or Airflow Temperature");
  SMARTattrDefAccss::Set(191, "G-sense Error Rate");
  SMARTattrDefAccss::Set(192, "Power-off/Emergency Retract Count");
  SMARTattrDefAccss::Set(193, "Load(/Unload) Cycle Count");
  SMARTattrDefAccss::Set(194, "Temperature Celsius");
  SMARTattrDefAccss::Set(195, "Hardware ECC Recovered");
  SMARTattrDefAccss::Set(196, "Reallocation Event Count");
  SMARTattrDefAccss::Set(197, "Current Pending Sector Count");
  SMARTattrDefAccss::Set(198, "Uncorrectable Sector Count");
  SMARTattrDefAccss::Set(199, "UltraDMA CRC Error Count");
  SMARTattrDefAccss::Set(200, "Multi-Zone Error Rate or Write Error Rate");
  SMARTattrDefAccss::Set(201, "Soft Read Error Rate or TA Counter Detected");
  
  SMARTattrDefAccss::Set(204, "Soft ECC Correction");
  
  SMARTattrDefAccss::Set(212, "Shock During Write");
  
  SMARTattrDefAccss::Set(220, "Disk Shift");
 
  SMARTattrDefAccss::Set(223, "Load/Unload Retry Count");
  
  SMARTattrDefAccss::Set(234, "Average erase count AND Maximum Erase Count");
  SMARTattrDefAccss::Set(235, "Good Block Count AND System(Free) Block Count");
  
  SMARTattrDefAccss::Set(240, "Head Flying Hours/Transfer Error Rate");
  SMARTattrDefAccss::Set(241, "Total Data Written");
  SMARTattrDefAccss::Set(242, "Total Data Read");
  
  SMARTattrDefAccss::Set(250, "Read Error Retry Rate");
  
  SMARTattrDefAccss::Set(254, "Free Fall Protection");
}

void SMARTmonitorBase::sigHandler(int signo){}
void SMARTmonitorBase::registerSignalHandler(){
  ///https://en.wikipedia.org/wiki/C_signal_handling
  ///Needed, else program exits when calling raise(SIGUSR1).
  signal(SIGUSR1, sigHandler);
}

void SMARTmonitorBase::SetCommandLineArgs(int argc, char ** argv) {
  /** IMPORTANT: creating the arrays can't be done in the constructor of this
    class as "argc" is "0" there. So do this from or after "OnInit()" */
  LOGN("number of program arguments passed:" << argc)
  m_cmdLineArgStrings = new const wchar_t * [argc];
  m_ar_stdwstrCmdLineArgs = new std::wstring[argc];
  //TODO move to "common_sourcecode"
  for (fastestUnsignedDataType index = 0; index < argc; ++index) {
    m_ar_stdwstrCmdLineArgs[index] = GetStdWstring(argv[index]);
    LOGN((index + 1) << ". program argument:" << m_ar_stdwstrCmdLineArgs[index])
    m_cmdLineArgStrings[index] = m_ar_stdwstrCmdLineArgs[index].c_str();
  }
  m_commandLineArgs.Set(argc, (wchar_t **) m_cmdLineArgStrings);
}

/** Return the file name part (without folder names) of this executable. */
std::wstring GetExeFileName(const wchar_t * const ar_wchFullProgramPath) {
  std::wstring stdwstrFullProgramPath(ar_wchFullProgramPath);
  int charIndexOfLastDirSepChar = stdwstrFullProgramPath.rfind(FileSystem::
    dirSeperatorChar);
  if (charIndexOfLastDirSepChar != std::wstring::npos) {
    return stdwstrFullProgramPath.substr(charIndexOfLastDirSepChar + 1);
  }
  return stdwstrFullProgramPath;
}

/** \brief adds S.M.A.R.T. attributes to observe from all successfully retrieved
 *  S.M.A.R.T. values from all data carriers to get S.M.A.R.T. values from.*/
void SMARTmonitorBase::SetSMARTattributesToObserve(
  std::set<SMARTuniqueIDandValues> & SMARTuniqueIDandValuesContainer)
{
  //  const std::set<SkSmartAttributeParsedData> & SMARTattributesToObserve =
  //    wxGetApp().mp_SMARTaccess->getSMARTattributesToObserve();
  //TODO add SMART attribute IDs to SMARTattributesToObserve
  m_IDsOfSMARTattrsToObserve.clear();
  std::set<SMARTuniqueIDandValues>::const_iterator iter =
          SMARTuniqueIDandValuesContainer.begin();
  for (; iter != SMARTuniqueIDandValuesContainer.end(); iter++) {
    const SMARTuniqueIDandValues & SMARTuniqueIDandValues = *iter;
    LOGN_DEBUG("address of SMARTuniqueIDandValues obj:" << &SMARTuniqueIDandValues)
    for (int SMARTattributeID = 0; SMARTattributeID <
      numDifferentSMART_IDsPlus1; SMARTattributeID++) {
      //    SMARTattributesToObserve.insert();
      if(SMARTuniqueIDandValues.m_SMARTvalues[SMARTattributeID].
           m_successfullyReadSMARTrawValue)
      {
        m_IDsOfSMARTattrsToObserve.insert(SMARTattributeID);
        LOGN_DEBUG("adding SMART attribute ID " << SMARTattributeID)
      }
    }
  }
}

void SMARTmonitorBase::OutputUsage() {
  std::ostringstream stdoss;
  stdoss << "Usage (options) either:\n"
    "-pass 2 command line arguments for each in this order:\n"
     "<command line option NAME> <command line option VALUE>):\n"
    "-or as 1 command line argument:<command line option NAME>=<command line "
     "option VALUE>\n\n"
    "avaiable options:\n";
  for (fastestUnsignedDataType index = 0;
    s_commandLineOptions[index].optionName[0] != '\0'; ++index) {
    CommandLineOption & commandLineOption = s_commandLineOptions[index];
    stdoss << "-" << commandLineOption.optionName << " " <<
    commandLineOption.possibleOptionValue << "";
    stdoss << "\n";
  }
  std::string str = stdoss.str();
//  std::cout << str << std::endl;
  ShowMessage(str.c_str() );
}

std::wstring SMARTmonitorBase::GetCommandLineOptionValue(
  const wchar_t * const cmdLineOptionName)
{
  unsigned programArgumentIndex = m_commandLineArgs.contains(
    cmdLineOptionName);

  if (programArgumentIndex != UINT_MAX) {
    std::wstring cmdLineOptionName;
    std::wstring cmdLineOptionValue;
    GetCmdLineOptionNameAndValue(
            programArgumentIndex,
            cmdLineOptionName,
            cmdLineOptionValue
            );
    return cmdLineOptionValue;
  }
  return L"";
}

void SMARTmonitorBase::GetCmdLineOptionNameAndValue(
  fastestUnsignedDataType & programArgumentIndex,
  std::wstring & cmdLineOptionName,
  std::wstring & cmdLineOptionValue
  )
{
  const wchar_t * pwchCmdLineOption = m_commandLineArgs.GetArgument(
    programArgumentIndex);
  std::wstring std_wstrCmdLineOption = pwchCmdLineOption;
  //  std::wstring cmdLineOptionName;
  int charPosOfEqualSign = std_wstrCmdLineOption.find('=');
  //  wchar_t * pwcschr(pwchCmdLineOption, L'=');
  if (charPosOfEqualSign != std::wstring::npos) {
    cmdLineOptionName = std_wstrCmdLineOption.substr(0, charPosOfEqualSign);
    cmdLineOptionValue = std_wstrCmdLineOption.substr(charPosOfEqualSign + 1);
    programArgumentIndex++;
  } else {
    cmdLineOptionName = std_wstrCmdLineOption;
    cmdLineOptionValue = GetCommandOptionValue(programArgumentIndex);
    programArgumentIndex += 2;
  }
}

std::wstring SMARTmonitorBase::GetCommandOptionName(std::wstring & cmdLineArg) {
  std::wstring cmdLineOptionName;
  int charPosOfEqualSign = cmdLineArg.find('=');
  if (charPosOfEqualSign != std::wstring::npos) {
    cmdLineOptionName = cmdLineArg.substr(0, charPosOfEqualSign);
  } else
    cmdLineOptionName = cmdLineArg;
  return cmdLineOptionName;
}

//TODO
std::wstring SMARTmonitorBase::GetCommandOptionValue(//const wchar_t * const str
        unsigned programArgumentIndex) {
  //const int index = m_commandLineArgs.contains(str);
  std::wstring cmdLineOptionName = m_commandLineArgs.GetArgument(
          programArgumentIndex);
  //  int charPosOfEqualSign = cmdLineOptionName.find('=');
  //  if( charPosOfEqualSign != std::wstring::npos )
  //  {
  //    cmdLineOptionName = cmdLineOptionName.substr(charPosOfEqualSign + 1);
  //    return cmdLineOptionName.substr(charPosOfEqualSign + 1);
  //  }
  for (fastestUnsignedDataType cmdLineOptionIndex = 0;
          s_commandLineOptions[cmdLineOptionIndex].optionName[0] != '\0';
          ++cmdLineOptionIndex) {
    CommandLineOption & commandLineOption = s_commandLineOptions[cmdLineOptionIndex];

    if (GetStdString_Inline(cmdLineOptionName) == commandLineOption.optionName) {
      return m_commandLineArgs.GetArgument(programArgumentIndex + 1);
    }
  }

  //  if( index != UINT_MAX && m_commandLineArgs.GetArgumentCount() > index + 1 )
  //  {
  //  }
  return std::wstring();
}

//TODO
/** This function should be the program option name -> handler function mapping.*/
void SMARTmonitorBase::HandleLogFileFolderProgramOption(
  std::wstring & cmdLineOptionValue)
{
  std::wstring stdwstrLogfilepathCmdLineArg = cmdLineOptionValue;
  if (stdwstrLogfilepathCmdLineArg.size() > 0) {
    const wchar_t lastChar = stdwstrLogfilepathCmdLineArg.at(
            stdwstrLogfilepathCmdLineArg.size() - 1);
    if (lastChar != FileSystem::dirSeperatorChar)
      stdwstrLogfilepathCmdLineArg += FileSystem::dirSeperatorChar;
  }
  m_stdstrLogFilePath = GetStdString_Inline(stdwstrLogfilepathCmdLineArg);
  m_stdstrLogFilePath += GetStdString_Inline(//m_stdwstrProgramePath);
    GetExeFileName(m_commandLineArgs.GetProgramPath()) );
}

fastestUnsignedDataType SMARTmonitorBase::ProcessCommandLineArgs()
{
  unsigned programArgumentCount = m_commandLineArgs.GetArgumentCount();
  bool showUsage = false;
  if (programArgumentCount > 1)
  {
    //    int charPosOfEqualSign;
    //TODO command line options may appear multiple times
    bool atLeast1UnknwonCmdLineOption = false;

    std::wstring cmdLineOptionName, cmdLineOptionValue;
    std::wstring cmdLineOption;
    const wchar_t * pwchCmdLineOption;
    for (unsigned programArgumentIndex =
      /** Start with the 2nd argument as the 1st one is the executable path 
       * and thus can't be a program option. */
      CommandLineArgs<wchar_t>::FirstProgramArg;
      programArgumentIndex < programArgumentCount;
      /* programArgumentIndex += 2 */)
    {
      //      cmdLineOptionName = GetCommandOptionName(cmdLineOption);
      GetCmdLineOptionNameAndValue(programArgumentIndex, cmdLineOptionName,
              cmdLineOptionValue);

      //      cmdLineOptionValue = GetCommandOptionValue(/*cmdLineOptionName.c_str()*/
      //        programArgumentIndex );
      if (cmdLineOptionValue == L"") {
        std::wcout << L"unknown command line option \"" << cmdLineOptionName
                << L"\"" << std::endl;
        atLeast1UnknwonCmdLineOption = true;
      }
      //TODO do here: execute cmdLineOption[cmdLineOptIdx].pfn
      else if (cmdLineOptionName == L"svcConnConfFile") {
        s_programOptionValues[serviceConnectionConfigFile] = cmdLineOptionValue;
      }
      else if (cmdLineOptionName == L"logfilefolder") {
        HandleLogFileFolderProgramOption(cmdLineOptionValue);
      }
      else if (cmdLineOptionName == L"loglevel")
        g_logger.SetLogLevel(GetStdString_Inline(cmdLineOptionValue) );
    }
    if(atLeast1UnknwonCmdLineOption)
      showUsage = true;
  }
  else
    showUsage = true;
  if(showUsage){
    OutputUsage();
    return 1;
  }
  return 0;
}

bool SMARTmonitorBase::InitializeLogger() {
  LogLevel::CreateLogLevelStringToNumberMapping();

  bool success = false;
  if( m_stdstrLogFilePath.empty() )
  {
    //std::string stdstrLogFilePath("SMARTmonitor.txt");
    m_stdwstrProgramePath = GetExeFileName(m_commandLineArgs.GetProgramPath());
    m_stdwstrProgramePath += L"_log.txt";
    m_stdstrLogFilePath += GetStdString_Inline(m_stdwstrProgramePath);
  }
  else///May be specified on command line/in configuration file
    m_stdstrLogFilePath += "_log.txt";
#ifdef _DEBUG
  g_logger.SetLogLevel("debug"/*LogLevel::debug*/);
#else
  g_logger.SetLogLevel("warning"/*LogLevel::warning*/);
#endif
  try {
    success = g_logger.OpenFileA(m_stdstrLogFilePath, "log4j", 4000, LogLevel:://debug
      info);
  }catch (const LogFileAccessException & lfae){
    //TODO does not respond anymore after this exception.
    ShowMessage(lfae.GetErrorMessageA().c_str() );
    std::cout << lfae.GetErrorMessageA() << std::endl;
  }
  return success;
}

void SMARTmonitorBase::ConstructConfigFilePathFromExeFilePath(
  const std::wstring & stdwstrAbsoluteFilePath,
  const std::wstring & stdwstrThisExecutable_sFilePath)
{
  std::wstring fullConfigFilePathWithoutExtension;
  //    //wxstrThisExecutablesFilePath
  //    wxString fileNameWithoutExtension;
  const int indexOfLastDot = stdwstrAbsoluteFilePath.rfind(L".");
  //const char ps = PATH_SEPERATOR_CHAR;
  std::wstring stdwstrPathSeperatorChar = GetStdWstring(std::string(
    PATH_SEPERATOR_CHAR_STRING));
  const int indexOfLastFileSepChar = stdwstrAbsoluteFilePath.rfind(
          //_T(xstringify(PATH_SEPERATOR_CHAR)  )
          stdwstrPathSeperatorChar);
  std::string configFileNameWithoutExtension;
  std::wstring exeFileName;
  if (indexOfLastFileSepChar != -1
          //Else this may happen: /home.git/executable"
          //&& indexOfLastFileSepChar < indexOfLastFileSepChar
          ) {
    int thisExecutable_sFilePathLen = //wcslen(wchThisExecutable_sFilePath);
            stdwstrThisExecutable_sFilePath.length();
    exeFileName = stdwstrAbsoluteFilePath.substr(indexOfLastFileSepChar + 1,
            thisExecutable_sFilePathLen);
  }
  const int indexOfExeFileNameDot = exeFileName.rfind(L".");
  if (indexOfExeFileNameDot == -1) /** If no file name extension like ".exe" */ {
    //        fullConfigFilePath = fullFilePathOfThisExecutable + wxT(".");
    if (indexOfLastFileSepChar != -1) {
      fullConfigFilePathWithoutExtension = //currentWorkingDir /*+ wxFILE_SEP_PATH */+
              stdwstrAbsoluteFilePath.substr(0, indexOfLastFileSepChar + 1) +
              exeFileName + L".";
    }
  } else {
    const std::wstring fullFilePathOfThisExecutableWoutExt =
            stdwstrThisExecutable_sFilePath.substr(0, indexOfLastDot + 1);
    fullConfigFilePathWithoutExtension = fullFilePathOfThisExecutableWoutExt;
  }
}

void  SMARTmonitorBase::EnsureSMARTattrToObsExist()
{
  if(m_IDsOfSMARTattrsToObserve.empty() )
  {
#ifdef directSMARTaccess
    //TODO add supported S.M.A.R.T. IDs to IDs of S.M.A.R.T. attributes to 
    // observe;multiple devices should be possible
//    mp_SMARTaccess->GetSupportedSMART_IDs(device, SMARTattributeNamesAndIDs);
#else
    for(int SMARTattributeID = 0; SMARTattributeID <
      numDifferentSMART_IDsPlus1; SMARTattributeID++) {
      m_IDsOfSMARTattrsToObserve.insert(SMARTattributeID);
      LOGN_DEBUG("adding SMART attribute ID " << SMARTattributeID)
    }
#endif
  }
}

#ifdef directSMARTaccess
///Reads directly from the devices
fastestUnsignedDataType SMARTmonitorBase::Upd8SMARTvalsDrctlyThreadSafe()
{
  LOGN_DEBUG("begin")
  DWORD dwRetVal = mp_SMARTaccess->ReadSMARTValuesForAllDrives(
    m_SMARTattrIDsToObs, s_dataCarrierID2devicePath);
  SMARTaccess_type & SMARTaccess = *mp_SMARTaccess;
  if (dwRetVal == SMARTaccessBase::success)
  {
    unsigned lineNumber = 0;
    bool atLeast1NonNullValue = false;
    //    //loop over drives
    //    long double timeInS;
    //    for(unsigned ucT1 = 0, ucT4 = 0; ucT1 < m_SMARTvalueProcessor.m_ucDrivesWithInfo; ++ ucT1)
    //    {
    const fastestUnsignedDataType numberOfDifferentDrives = SMARTaccess.
      GetNumberOfDifferentDrives();
    std::set<SMARTuniqueIDandValues> & SMARTuniqueIDsAndValues = SMARTaccess.
            GetSMARTuniqueIDandValues();
    LOGN("address:" << &SMARTuniqueIDsAndValues)
    std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
            SMARTuniqueIDsAndValues.begin();
    fastestUnsignedDataType SMARTattributeID;
    uint64_t SMARTrawValue;
            //TODO crashed in loop header at "iter++"
            bool consistent;
    for (fastestUnsignedDataType currentDriveIndex = 0 /*, ucT4 = 0*/;
            currentDriveIndex < numberOfDifferentDrives;
            ++currentDriveIndex, SMARTuniqueIDandValuesIter++) {
      //    pDriveInfo = m_SMARTvalueProcessor.GetDriveInfo(currentDriveIndex);
      //TODO only read intersection with supported S.M.A.R.T. attributes?
      SMARTattrToObsType::const_iterator
        SMARTattrsToObserveIter = m_IDsOfSMARTattrsToObserve.begin();
      uint64_t currentSMARTrawValue;
      //TODO crashes here (iterator-related?!-> thread access problem??)
      //TODO why this loop?? multithread-safe (AtomicExchange(...) ) already in
      // SMARTaccessBase::ReadSMARTValuesForAllDrives(...)
      for (; SMARTattrsToObserveIter != m_IDsOfSMARTattrsToObserve.end();
              SMARTattrsToObserveIter++) {
        SMARTattributeID = * SMARTattrsToObserveIter;

        consistent = SMARTuniqueIDandValuesIter->m_SMARTvalues[
                SMARTattributeID].IsConsistent(m_arSMARTrawValue[lineNumber]);
        if (consistent) {
          //          AtomicExchange( (long int *) & m_arSMART_ID[lineNumber]
          LOGN(m_arSMARTrawValue[lineNumber] << " " << currentSMARTrawValue)
          AtomicExchange(
                  & m_arTickCountOfLastQueryInMilliSeconds[lineNumber]
                  , GetTickCount() /*long val*/);
          //FileTi
          //              OperatingSystem::GetTimeCountInSeconds(timeInS);

          if (/*SMARTattributesToObserveIter->pretty_value*/
                  m_arSMARTrawValue[lineNumber])
            atLeast1NonNullValue = true;
          ++lineNumber;
          //            }
        }
      }
    }
    OperatingSystem::GetCurrentTime(m_timeOfLastSMARTvaluesUpdate);
    //    }
  } else // e.g. SMARTaccessBase::accessDenied
    ;
  return dwRetVal;
}
#endif

inline void SMARTmonitorBase::WaitForSignalOrTimeout(
  const fastestUnsignedDataType ms)
{
  LOGN_DEBUG("begin--" << ms << "ms")
  I_Condition::state st = waitOrSignalEvt.WaitForSignalOrTimeoutInMs(ms);
/*#ifdef __linux__
  timespec ts{numberOfSecondsToWaitBetweenSMARTquery,
    numberOfMilliSecondsToWaitBetweenSMARTquery * 1000000};
    
  pthread_cond_timedwait(&cond, &mutex, &ts);
#endif*/
  LOGN_DEBUG("end--return value of WaitForSignalOrTimeoutInMs:" << st)
}

inline void SMARTmonitorBase::WaitForTimeout(
  fastestUnsignedDataType numberOfSecondsToWaitBetweenSMARTquery,
  const fastestUnsignedDataType numberOfMilliSecondsToWaitBetweenSMARTquery)
{
  numberOfSecondsToWaitBetweenSMARTquery =
    numberOfMilliSecondsToWaitBetweenSMARTquery / 1000;
  while (numberOfSecondsToWaitBetweenSMARTquery-- &&
          SMARTmonitorBase::s_updateSMARTvalues) {
    /**If this program ends it needs max. the sleep time to terminate.
    * The lower the argument for sleep the more responsive this 
    * program gets.
    * Alternatives (needn't wait the whole time):
    * -WaitForSingleObjectEx(...) for MS Windows API
    * -pthread_cond_timedwait(&cond, &mutex, &ts); for Linux */
    sleep(1);///http://linux.die.net/man/3/sleep
  }
  //Sleep in microseconds (1/1000th of a millisecond))
  usleep(numberOfMilliSecondsToWaitBetweenSMARTquery % 1000 * 1000);
}

///Function that can be used by clients and service.
///It shouldn't be called from the UI thread because getting S.M.A.R.T. infos
/// needs some milliseconds->blocks the UI in this time.
/// SMARTmonitorBase::BeforeWait(...) can be used to display the values
/// (asynchronously in the UI thread via messages etc.).
DWORD THREAD_FUNCTION_CALLING_CONVENTION UpdateSMARTparameterValuesThreadFunc(
  void * p_v)
{
  LOGN_DEBUG("begin")
  struct GetSMARTvaluesFunctionParams * p_getSMARTvalsFnParams =
    (struct GetSMARTvaluesFunctionParams *) p_v;
  try{
#ifdef multithread
  I_Thread::SetCurrentThreadName("upd8 SMART");
#endif
//  SMARTmonitorBase * p_SMARTmonitorBase = (SMARTmonitorBase *) p_v;
  if( ! /*p_SMARTmonitorBase*/ p_getSMARTvalsFnParams)
    return 1;//TODO return enum
  {
    SMARTmonitorBase * p_SMARTmonitorBase = p_getSMARTvalsFnParams->
      p_SMARTmonitorBase;
    if( ! p_SMARTmonitorBase )
      return 2;//TODO return enum
    
    const unsigned numberOfMilliSecondsToWaitBetweenSMARTquery =
      SMARTmonitorBase::GetNumberOfMilliSecondsToWaitBetweenSMARTquery();
    fastestUnsignedDataType numberOfSecondsToWaitBetweenSMARTquery;
      
    GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType 
      p_getSMARTvaluesFunction = 
      p_getSMARTvalsFnParams->p_getSMARTvaluesFunction;
            
//    std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesSet = 
//      p_SMARTmonitorBase->mp_SMARTaccess->GetSMARTuniqueIDandValues();
    int res;
    do
    {
//      int res = p_SMARTmonitorBase->UpdateSMARTvaluesThreadSafe();
//      int res = ( (SMARTmonitorClient *) p_SMARTmonitorBase)->
//        GetSMARTvaluesFromServer(sMARTuniqueIDandValuesSet);
      res = (*p_SMARTmonitorBase.*p_getSMARTvaluesFunction)();
      if( res == 0 )
        p_SMARTmonitorBase->BeforeWait();
      else{
        LOGN_ERROR("get S.M.A.R.T. values function returned non-0")
        break;
      }
      /*p_SMARTmonitorBase->WaitForTimeout(
        numberOfSecondsToWaitBetweenSMARTquery,
        numberOfMilliSecondsToWaitBetweenSMARTquery);*/
      p_SMARTmonitorBase->WaitForSignalOrTimeout(
        numberOfMilliSecondsToWaitBetweenSMARTquery);
    } while (SMARTmonitorBase::s_updateSMARTvalues);
//    if(SMARTmonitorBase::s_updateSMARTvalues)
      /** The SMARTmonitor object may have already been destroyed if 
       *  SMARTmonitorBase::s_updateSMARTvalues is false->invalid pointer.*/
      p_SMARTmonitorBase->AfterGetSMARTvaluesLoop(res);
    if(p_getSMARTvaluesFunction ==
        //p_SMARTmonitorBase->GetSMARTattrValsFromSrv()
        & SMARTmonitorBase::GetSMARTattrValsFromSrv )
      OperatingSystem::BSD::sockets::CloseSocket();
  }
  }///E.g. if rolling file appender and permission denied for the new file.
  catch(LogFileAccessException & lfae){
    LOGN_ERROR("Log file access exception")
    //TODO does not respond anymore after this exception.
    p_getSMARTvalsFnParams->p_SMARTmonitorBase->ShowMessage(lfae.
      GetErrorMessageA().c_str(), UserInterface::MessageType::error);
  }
  LOGN_DEBUG("return 0--update S.M.A.R.T. values?:" << SMARTmonitorBase::
    s_updateSMARTvalues)
  return 0;
}

#ifdef multithread
void SMARTmonitorBase::StartAsyncUpdateThread(
  GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType
    getSMARTvaluesFunctionType
  )
{
  LOGN_DEBUG("begin")
  /** Preconditions */
  if (m_IDsOfSMARTattrsToObserve.size() > 0) {
    m_getSMARTvaluesFunctionParams.p_getSMARTvaluesFunction = 
      getSMARTvaluesFunctionType;
    m_updateSMARTparameterValuesThread.start(
      UpdateSMARTparameterValuesThreadFunc, /*this*/
      & m_getSMARTvaluesFunctionParams
      );
  }
  else
    ShowMessage("No S.M.A.R.T. attributes to observe", MessageType::error);
  LOGN_DEBUG("end")
}

void SMARTmonitorBase::StartAsyncUpdateThread(
//  GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType
//    getSMARTvaluesFunctionType
  struct GetSMARTvaluesFunctionParams & getSMARTvaluesFunctionParams
  )
{
  LOGN_DEBUG("# S.M.A.R.T. attributes to observe:" <<
    m_IDsOfSMARTattrsToObserve.size() )
  /** Preconditions */
  if (m_IDsOfSMARTattrsToObserve.size() > 0) {
//    struct GetSMARTvaluesFunctionParams
//      getSMARTvaluesFunctionParams = {this, getSMARTvaluesFunctionType };
    m_updateSMARTparameterValuesThread.start(
      UpdateSMARTparameterValuesThreadFunc, /*this*/
      & getSMARTvaluesFunctionParams
      );
  }
}
#endif

void SMARTmonitorBase::ConstructConfigFilePathFromExeDirPath(
  const std::wstring & stdwstrAbsoluteFilePath,
  std::wstring & fullConfigFilePathWithoutExtension)
{
  //const char ps = PATH_SEPERATOR_CHAR;
  std::wstring stdwstrPathSeperatorChar = GetStdWstring(std::string(
    PATH_SEPERATOR_CHAR_STRING));
  const int indexOfLastPathSepChar = stdwstrAbsoluteFilePath.rfind(
          //_T(xstringify(PATH_SEPERATOR_CHAR)  )
          stdwstrPathSeperatorChar);
  /** If char found in string. */
  if (indexOfLastPathSepChar != std::basic_string<wchar_t>::npos
      /** Else this may happen: /home.git/executable" */
      //&& indexOfLastFileSepChar < indexOfLastFileSepChar
    )
  {
    std::wstring stdwstrAbsoluteDirPath = stdwstrAbsoluteFilePath.substr(0,
      indexOfLastPathSepChar + 1);
    fullConfigFilePathWithoutExtension = stdwstrAbsoluteDirPath
      /* + L"SMARTmonitor."*/;
  }
}

/** adds configuration dir suffix to \param stdwstrCfgFilePathWoutExt
 *  \param stdwstrCfgFilePathWoutExt
 *   gets path from curr work dir if it is empty */
void SMARTmonitorBase::ConstructConfigFilePath(
  std::wstring & stdwstrCfgFilePathWoutExt)
{
  /** Not necessarily an absolute file path! e.g. also "./executable.elf" is
   *possible */
//  const wchar_t * wchThisExecutable_sFilePath = m_commandLineArgs.
//    GetProgramPath();
//  std::wstring stdwstrCurrExeFilePath(wchThisExecutable_sFilePath);
  
//  std::string currExePath;
  std::string stdstrCfgPathBaseDir;
  if(//OperatingSystem::Process::GetCurrExePath(currExePath) 
     stdwstrCfgFilePathWoutExt.empty() )
  {
//    std::wstring stdwstrCurrExeFilePath = GetStdWstring(currExePath);
//    LOGN("this exe's file path: \"" << stdwstrCurrExeFilePath << "\"")

    OperatingSystem::GetCurrentWorkingDirA_inl(stdstrCfgPathBaseDir);
    stdwstrCfgFilePathWoutExt = GetStdWstring(stdstrCfgPathBaseDir);
  //  std::wstring stdwstrAbsoluteFilePath = GetAbsoluteFilePath(
  //    GetStdWstring(currentWorkingDir), 
  //    GetStdWstring(wchThisExecutable_sFilePath) );
//  std::string stdstrAbsoluteFilePath;
//  FileSystem::GetAbsolutePathA(GetStdString(stdwstrCurrExeFilePath).c_str(),
//    stdstrAbsoluteFilePath);
//  std::wstring stdwstrAbsoluteFilePath = GetStdWstring(
      //stdstrAbsoluteFilePath
//    stdstrCurrWorkDir);
//    LOGN("this exe's absolute file path: \"" << //stdwstrAbsoluteFilePath
//      stdwstrCurrExeFilePath << "\"")
    LOGN("this exe's current working dir: \"" <<stdwstrCfgFilePathWoutExt <<
      "\"")
  }
    //TODO This code needs to be reworked. All cases [ (no) dot in file name, ]
    //have to be taken into account
  std::wstring configFilePathCmdLineValue = GetCommandLineOptionValue(L"configfilepath");
  if (configFilePathCmdLineValue == L"") /** NO config file path passed. */ {
    //    ConstructConfigFilePathFromExeFilePath(
    //      stdwstrAbsoluteFilePath,
    //      stdwstrThisExecutable_sFilePath);
//    ConstructConfigFilePathFromExeDirPath(
//      stdwstrAbsoluteFilePath,
//      stdwstrConfigPathWithoutExtension);
    
    stdwstrCfgFilePathWoutExt += PATH_SEPERATOR_WCHAR +
      std::wstring(L"config") + PATH_SEPERATOR_WCHAR;
  }
  //  else /** At least 1 program argument passed. */
  //  {
  //  //      if( )
  //    if( false /*isDirectoryPath(argv[1])*/ )
  //    {
  //    const wxString directoryForConfigFile = /*wxGetCwd()*/ argv[1];
  //    const int indexOfLastBackSlash = stdwstrThisExecutable_sFilePath.rfind(
  //      wxFILE_SEP_PATH);
  //    const int fileNameLen = stdwstrThisExecutable_sFilePath.size() - indexOfLastBackSlash - 1;
  //    const wxString workingDirWithConfigFilePrefix = directoryForConfigFile +
  //      stdwstrThisExecutable_sFilePath.substr(indexOfLastBackSlash,
  //      fileNameLen - 2 /* - 3 chars extension + file separator char */);
  //    fullConfigFilePathWithoutExtension = workingDirWithConfigFilePrefix;
  //    }
  //    else
  //      fullConfigFilePathWithoutExtension = argv[1];
  //  }
  //  //  wxWidgets::wxSMARTreader smartReader;
  //  /*std::wstring*/ stdwstrConfigPathWithoutExtension =
  //    wxWidgets::GetStdWstring_Inline(fullConfigFilePathWithoutExtension);
  LOGN("using config file path: \"" << stdwstrCfgFilePathWoutExt << "\"")
}

/** tries to load from 2 paths:
*   -given path
*   -path relative to curr work dir */
bool SMARTmonitorBase::tryCfgFilePaths(
  const wchar_t fileName[],
  loadFuncType loadFunc
  )
{
  enum InitSMARTretCode initSMARTretCode = success;

//TODO pass this folder via CMake argument so it is the same as Debian package
// (via CPack) installation path
#if defined( __linux__) && defined(buildService)
  std::wstring stdwstrWorkDirWithCfgFilePrefix =//L"/usr/local/SMARTmonitor";
    makeWchar(resourcesFSpath);
#else
  std::wstring stdwstrWorkDirWithCfgFilePrefix;
#endif
  std::wstring origPath = stdwstrWorkDirWithCfgFilePrefix;
  ConstructConfigFilePath(stdwstrWorkDirWithCfgFilePrefix);
  stdwstrWorkDirWithCfgFilePrefix += fileName;
  
  std::string stdstrFullConfigFilePath;
  //TODO just for compilation
  bool successfullyLoadedCfgFile = /*mp_cfgLoader.LoadSMARTCfg*/
    (m_cfgLoader.*loadFunc)(& stdwstrWorkDirWithCfgFilePrefix, 
      & stdstrFullConfigFilePath, NULL);

  if(! successfullyLoadedCfgFile)
    if(origPath != L""){
      stdwstrWorkDirWithCfgFilePrefix = L"";
      successfullyLoadedCfgFile = /*mp_cfgLoader->LoadSMARTCfg*/
        (m_cfgLoader.*loadFunc)(& stdwstrWorkDirWithCfgFilePrefix,
          & stdstrFullConfigFilePath, NULL);
    }
  if(! successfullyLoadedCfgFile)
    initSMARTretCode = readingConfigFileFailed;
}

fastestUnsignedDataType SMARTmonitorBase::InitializeSMART(){
  enum InitSMARTretCode initSMARTretCode = success;

  try{
    tryCfgFilePaths(L"en/SMARTattrDefs.", & CfgLoaderType::readSMARTattrDefs);
    tryCfgFilePaths(L"SMARTdataCarrierDefs.", & CfgLoaderType::
      ReadSMARTdataCarrierDefs);
    tryCfgFilePaths(L"SMARTsrvCnnctn.", & CfgLoaderType::ReadSrvCnnctnCfg);
    
    //    {
    //      //wxMessageBox(wxT("failed reading config file \"") + workingDirWithConfigFilePrefix + wxT("\""));
    //      return false;
    //    }
    //if( smartReader.m_oSMARTDetails.size())
#ifdef directSMARTaccess
    DWORD dwRetVal = mp_SMARTaccess->ReadSMARTValuesForAllDrives(
      m_SMARTattrIDsToObs, s_dataCarrierID2devicePath);
    switch (dwRetVal) {
     case SMARTaccessBase::accessDenied:
       ShowMessage("direct access denied to S.M.A.R.T.->get values via server "
         "or restart this program  as an administrator in order to get the "
         "S.M.A.R.T. values directly",
        UserInterface::MessageType::error);
        initSMARTretCode = accessToSMARTdenied;
        break;
      case SMARTaccessBase::noSingleSMARTdevice:
//        ShowMessage("no S.M.A.R.T. capable device present\n->program exits "
//                "(after showing this message)");
        initSMARTretCode = noSMARTcapableDevicePresent;
        break;
      case SMARTaccessBase::success:
        LOGN("SMART successfully accessed")
        break;
    }
#endif
  } catch (const ParseConfigFileException & e) {
    LOGN("parse config file exc")
    ShowMessage("There was at least 1 error reading the configuration file\n"
            "->this application will exit now.");
    initSMARTretCode = readingConfigFileFailed;
    throw e;
  }
  LOGN("end--returning " << initSMARTretCode)
  return initSMARTretCode;
}

void SMARTmonitorBase::ShowMessage(const char * const msg) const {
  LOGN(msg)
  std::cout << msg << std::endl;
}

void SMARTmonitorBase::ShowMessage(const char * const msg,
  UserInterface::MessageType::messageTypes msgType) const
{
  switch(msgType){/**Output to standard output and standard error because
    * logging to a file may be unavailable/the messages can be seen easier.*/
   case MessageType::error: /*LOGN_ERROR(msg)*/ std::cerr << msg; break;
   case MessageType::warning: /*LOGN_WARNING(msg)*/ std::cout << msg; break;
   case MessageType::success: /*LOGN_SUCCESS(msg)*/ std::cout << msg; break;
  }
}
