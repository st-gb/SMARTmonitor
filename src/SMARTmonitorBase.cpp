/** Author: sg
 * Created on 17. November 2016, 13:05 */
#include "SMARTmonitorBase.hpp" //class SMARTmonitorBase
#include "client/SMARTmonitorClient.h" //class SMARTmonitorClient
#include <FileSystem/GetCurrentWorkingDir.hpp> //OperatingSystem::GetCurrentWorkingDirA_inl(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <Controller/character_string/stdtstr.hpp> //GetStdWstring(...)
#include <FileSystem/File/GetAbsoluteFilePath.hpp> // GetAbsoluteFilePath(...)
#include <FileSystem/path_seperator.h> //PATH_SEPERATOR_CHAR_STRING
#include <Controller/Logger/LogFileAccessException.hpp>
#include <SMARTaccessBase.hpp> //class SMARTaccessBase
#include <tinyxml2/ConfigLoader.hpp> //class tinyxml2::ConfigLoader
#include <ConfigLoader/ParseConfigFileException.hpp>
#include <hardware/CPU/atomic/AtomicExchange.h>
#include <Controller/time/GetTickCount.hpp>
#include <FileSystem/PathSeperatorChar.hpp> //FileSystem::dirSeperatorChar

/** Static/Class members must be defined once in a source file. Do this here */
SMARTmonitorBase::dataCarrierID2devicePath_type SMARTmonitorBase::s_dataCarrierID2devicePath;
unsigned SMARTmonitorBase::s_numberOfMilliSecondsToWaitBetweenSMARTquery = 10000;
fastestSignedDataType SMARTmonitorBase::s_updateSMARTvalues = 1;
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
  : m_socketPortNumber(1000),
    mp_configurationLoader(NULL),
    m_cmdLineArgStrings(NULL),
    m_ar_stdwstrCmdLineArgs(NULL)
{
  /** For calling ::UpdateSMARTparameterValuesThreadFunc(void *) */
  m_getSMARTvaluesFunctionParams.p_SMARTmonitorBase = this;
  mp_SMARTaccess = &m_SMARTvalueProcessor.getSMARTaccess();
  mp_configurationLoader = new tinyxml2::ConfigLoader(
    (SMARTaccessBase::SMARTattributesContainerType &) mp_SMARTaccess->getSMARTattributes(), * this);
  //  InitializeLogger();
  
  //TODO
//  s_programOptionName2handler.insert(std::make_pair(L",));
}

SMARTmonitorBase::~SMARTmonitorBase() {
  if( mp_configurationLoader )
    delete mp_configurationLoader;
  if(m_cmdLineArgStrings )
    delete [] m_cmdLineArgStrings;
  if( m_ar_stdwstrCmdLineArgs)
    delete [] m_ar_stdwstrCmdLineArgs;
}

//https://gcc.gnu.org/onlinedocs/cpp/Stringification.html#Stringification
#define xstringify(s) stringify(s)
#define stringify(s) #s

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

void SMARTmonitorBase::SetSMARTattributesToObserve(
        std::set<SMARTuniqueIDandValues> & SMARTuniqueIDandValuesContainer) {
  //  const std::set<SkSmartAttributeParsedData> & SMARTattributesToObserve =
  //    wxGetApp().mp_SMARTaccess->getSMARTattributesToObserve();
  //TODO add SMART attribute IDs to SMARTattributesToObserve
  m_IDsOfSMARTattributesToObserve.clear();
  std::set<SMARTuniqueIDandValues>::const_iterator iter =
          SMARTuniqueIDandValuesContainer.begin();
  for (; iter != SMARTuniqueIDandValuesContainer.end(); iter++) {
    const SMARTuniqueIDandValues & SMARTuniqueIDandValues = *iter;
    LOGN_DEBUG("address of SMARTuniqueIDandValues obj:" << &SMARTuniqueIDandValues)
    for (int SMARTattributeID = 0; SMARTattributeID <
            NUM_DIFFERENT_SMART_ENTRIES; SMARTattributeID++) {
      //    SMARTattributesToObserve.insert();
      if(SMARTuniqueIDandValues.m_SMARTvalues[SMARTattributeID].
           m_successfullyReadSMARTrawValue)
      {
        m_IDsOfSMARTattributesToObserve.insert(SMARTattributeID);
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
    GetCommandOptionNameAndValue(
            programArgumentIndex,
            cmdLineOptionName,
            cmdLineOptionValue
            );
    return cmdLineOptionValue;
  }
  return L"";
}

void SMARTmonitorBase::GetCommandOptionNameAndValue(
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
  m_stdstrLogFilePath += GetStdString_Inline(m_stdwstrProgramePath);
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
      GetCommandOptionNameAndValue(programArgumentIndex, cmdLineOptionName,
              cmdLineOptionValue);

      //      cmdLineOptionValue = GetCommandOptionValue(/*cmdLineOptionName.c_str()*/
      //        programArgumentIndex );
      if (cmdLineOptionValue == L"") {
        std::wcout << L"unknown command line option \"" << cmdLineOptionName
                << L"\"" << std::endl;
        atLeast1UnknwonCmdLineOption = true;
      }
      else if (cmdLineOptionName == L"svcConnConfFile") {
        s_programOptionValues[serviceConnectionConfigFile] = cmdLineOptionValue;
      }
      else if (cmdLineOptionName == L"logfilefolder") {
        HandleLogFileFolderProgramOption(cmdLineOptionValue);
      }
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

void SMARTmonitorBase::InitializeLogger() {
  LogLevel::CreateLogLevelStringToNumberMapping();
  //std::string stdstrLogFilePath("SMARTmonitor.txt");
  m_stdwstrProgramePath = GetExeFileName(m_commandLineArgs.
          GetProgramPath());
  m_stdwstrProgramePath += L"_log.txt";

  //std::string stdstrLogFileDirectory;
  m_stdstrLogFilePath += GetStdString_Inline(m_stdwstrProgramePath);
  try {
    g_logger.OpenFileA(m_stdstrLogFilePath, "log4j", 4000, LogLevel::debug);
  }  catch (const LogFileAccessException & lfae) {
    std::cout << lfae.GetErrorMessageA() << std::endl;
  }
}

void SMARTmonitorBase::ConstructConfigFilePathFromExeFilePath(
  const std::wstring & stdwstrAbsoluteFilePath,
  const std::wstring & stdwstrThisExecutable_sFilePath)
{
  std::wstring fullConfigFilePathWithoutExtension;
  //    //wxstrThisExecutablesFilePath
  //    wxString fileNameWithoutExtension;
  const int indexOfLastDot = stdwstrAbsoluteFilePath.rfind(_T("."));
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

fastestUnsignedDataType SMARTmonitorBase::UpdateSMARTvaluesThreadSafe()
{
  DWORD dwRetVal = mp_SMARTaccess->ReadSMARTValuesForAllDrives();
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
    const std::set<SMARTentry> & SMARTattributesToObserve =
            SMARTaccess.getSMARTattributes();
    LOGN("# SMART attributes to observe:" << SMARTattributesToObserve.size())
            //TODO crashed in loop header at "iter++"
            bool consistent;
    for (fastestUnsignedDataType currentDriveIndex = 0 /*, ucT4 = 0*/;
            currentDriveIndex < numberOfDifferentDrives;
            ++currentDriveIndex, SMARTuniqueIDandValuesIter++) {
      //    pDriveInfo = m_SMARTvalueProcessor.GetDriveInfo(currentDriveIndex);
      std::set<SMARTentry>::const_iterator
      SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
      uint64_t currentSMARTrawValue;
      //TODO crashes here (iterator-related?!-> thread access problem??)
      for (; SMARTattributesToObserveIter != SMARTattributesToObserve.end();
              SMARTattributesToObserveIter++) {
        SMARTattributeID = SMARTattributesToObserveIter->GetAttributeID();

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
    //    }
  } else // e.g. SMARTaccessBase::accessDenied
    ;
  return dwRetVal;
}

DWORD THREAD_FUNCTION_CALLING_CONVENTION UpdateSMARTparameterValuesThreadFunc(
  void * p_v)
{
//  SMARTmonitorBase * p_SMARTmonitorBase = (SMARTmonitorBase *) p_v;
  struct GetSMARTvaluesFunctionParams * getSMARTvaluesFunctionParams =
    (struct GetSMARTvaluesFunctionParams *) p_v;
  if( ! /*p_SMARTmonitorBase*/ getSMARTvaluesFunctionParams)
    return 1;
  {
    SMARTmonitorBase * p_SMARTmonitorBase = getSMARTvaluesFunctionParams->
      p_SMARTmonitorBase;
    if( ! p_SMARTmonitorBase )
      return 2;
    
    const unsigned numberOfMilliSecondsToWaitBetweenSMARTquery =
      SMARTmonitorBase::GetNumberOfMilliSecondsToWaitBetweenSMARTquery();
    fastestUnsignedDataType numberOfSecondsToWaitBetweenSMARTquery;    
      
    GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType 
      p_getSMARTvaluesFunction = 
      getSMARTvaluesFunctionParams->p_getSMARTvaluesFunction;
            
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
      else
        break;

      numberOfSecondsToWaitBetweenSMARTquery =
              numberOfMilliSecondsToWaitBetweenSMARTquery / 1000;
      while (numberOfSecondsToWaitBetweenSMARTquery-- &&
              SMARTmonitorBase::s_updateSMARTvalues) {
        sleep(1);
      }
      //Sleep in microseconds (1/1000th of a millisecond))
      usleep(numberOfMilliSecondsToWaitBetweenSMARTquery % 1000 * 1000);
    } while (SMARTmonitorBase::s_updateSMARTvalues);

    p_SMARTmonitorBase->AfterGetSMARTvaluesLoop(res);
  }
  return 0;
}

void SMARTmonitorBase::StartAsyncUpdateThread(
  GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType
    getSMARTvaluesFunctionType
  )
{
  LOGN_DEBUG("begin")
  /** Preconditions */
  if (mp_SMARTaccess->GetNumSMARTattributesToObserve() > 0) {
    m_getSMARTvaluesFunctionParams.p_getSMARTvaluesFunction = 
      getSMARTvaluesFunctionType;
    m_updateSMARTparameterValuesThread.start(
      UpdateSMARTparameterValuesThreadFunc, /*this*/
      & m_getSMARTvaluesFunctionParams
      );
  }
  LOGN_DEBUG("end")
}

void SMARTmonitorBase::StartAsyncUpdateThread(
//  GetSMARTvaluesFunctionParams::GetSMARTvaluesFunctionType
//    getSMARTvaluesFunctionType
  struct GetSMARTvaluesFunctionParams & getSMARTvaluesFunctionParams
  )
{
  /** Preconditions */
  if (mp_SMARTaccess->GetNumSMARTattributesToObserve() > 0) {
//    struct GetSMARTvaluesFunctionParams
//      getSMARTvaluesFunctionParams = {this, getSMARTvaluesFunctionType };
    m_updateSMARTparameterValuesThread.start(
      UpdateSMARTparameterValuesThreadFunc, /*this*/
      & getSMARTvaluesFunctionParams
      );
  }
}

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
    fullConfigFilePathWithoutExtension = stdwstrAbsoluteDirPath + L"SMARTmonitor.";
  }
}

void SMARTmonitorBase::ConstructConfigFilePath(
        std::wstring & stdwstrConfigPathWithoutExtension) {
  // Not necessarily an absolute file path! e.g. also "./executable.elf" is possible
  const wchar_t * wchThisExecutable_sFilePath = m_commandLineArgs.
          GetProgramPath();
  std::wstring stdwstrThisExecutable_sFilePath(wchThisExecutable_sFilePath);
  LOGN("this exe's file path: \"" << std::wstring(wchThisExecutable_sFilePath) << "\"")

  std::string currentWorkingDir;
  OperatingSystem::GetCurrentWorkingDirA_inl(currentWorkingDir);

  //  std::wstring stdwstrAbsoluteFilePath = GetAbsoluteFilePath(
  //    GetStdWstring(currentWorkingDir), 
  //    GetStdWstring(wchThisExecutable_sFilePath) );
  std::string stdstrAbsoluteFilePath;
  FileSystem::GetAbsolutePathA(GetStdString(stdwstrThisExecutable_sFilePath).c_str(),
          stdstrAbsoluteFilePath);
  std::wstring stdwstrAbsoluteFilePath = GetStdWstring(stdstrAbsoluteFilePath);
  LOGN("this exe's absolute file path: \"" << stdwstrAbsoluteFilePath << "\"")

  LOGN("this exe's current working dir: \"" << GetStdWstring(
    currentWorkingDir) << "\"")

    //TODO This code needs to be reworked. All cases [ (no) dot in file name, ]
    //have to be taken into account
    std::wstring fullConfigFilePathWithoutExtension;
  std::wstring configFilePathCmdLineValue = GetCommandLineOptionValue(L"configfilepath");
  if (configFilePathCmdLineValue == L"") /** NO config file path passed. */ {
    //    ConstructConfigFilePathFromExeFilePath(
    //      stdwstrAbsoluteFilePath,
    //      stdwstrThisExecutable_sFilePath);
    ConstructConfigFilePathFromExeDirPath(
            stdwstrAbsoluteFilePath,
            fullConfigFilePathWithoutExtension);
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
  stdwstrConfigPathWithoutExtension = fullConfigFilePathWithoutExtension;
  LOGN("using config file path: \"" << stdwstrConfigPathWithoutExtension << "\"")
}

fastestUnsignedDataType SMARTmonitorBase::InitializeSMART() {
  enum InitSMARTretCode initSMARTretCode = success;
  std::wstring stdwstrWorkingDirWithConfigFilePrefix;
  ConstructConfigFilePath(stdwstrWorkingDirWithConfigFilePrefix);

  try {
    //TODO just for compilation
    const bool successfullyLoadedConfigFile = mp_configurationLoader->
      LoadSMARTparametersConfiguration(stdwstrWorkingDirWithConfigFilePrefix);

    if (!successfullyLoadedConfigFile)
      initSMARTretCode = readingConfigFileFailed;
    //    {
    //      //wxMessageBox(wxT("failed reading config file \"") + workingDirWithConfigFilePrefix + wxT("\""));
    //      return false;
    //    }
    //if( smartReader.m_oSMARTDetails.size())
    DWORD dwRetVal = mp_SMARTaccess->ReadSMARTValuesForAllDrives();
    switch (dwRetVal) {
      case SMARTaccessBase::accessDenied:
        ShowMessage("access denied to S.M.A.R.T.\n->restart this program as an"
          " administrator\nin order to get the S.M.A.R.T. values directly");
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
