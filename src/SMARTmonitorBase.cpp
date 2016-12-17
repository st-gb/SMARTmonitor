/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMARTmonitorBase.cpp
 * Author: sg
 * 
 * Created on 17. November 2016, 13:05
 */

#include "SMARTmonitorBase.hpp"
#include <FileSystem/GetCurrentWorkingDir.hpp> //OperatingSystem::GetCurrentWorkingDirA_inl(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <Controller/character_string/stdtstr.hpp> //GetStdWstring(...)
#include <FileSystem/File/GetAbsoluteFilePath.hpp> // GetAbsoluteFilePath(...)
#include <FileSystem/path_seperator.hpp>
#include <Controller/Logger/LogFileAccessException.hpp>
#include <SMARTaccessBase.hpp>
#include <ConfigLoader/ParseConfigFileException.hpp>
#include <hardware/CPU/atomic/AtomicExchange.h>
#include <Controller/time/GetTickCount.hpp>
#include <FileSystem/PathSeperatorChar.hpp>

unsigned SMARTmonitorBase::s_numberOfMilliSecondsToWaitBetweenSMARTquery = 10000;
fastestSignedDataType SMARTmonitorBase::s_updateSMARTvalues = 1;
extern const char FileSystem::dirSeperatorChar;
CommandLineOption SMARTmonitorBase::s_commandLineOptions [] = { 
  {"logfilefolder", "<absolute or relative log file FOLDER>, e.g. \"/run/\" "
    "for writing log files to tmpfs/RAM drives"}, {""} };

SMARTmonitorBase::SMARTmonitorBase() 
  : m_socketPortNumber(1000)
{
  mp_SMARTaccess = & m_SMARTvalueProcessor.getSMARTaccess();
  mp_configurationLoader = new libConfig::ConfigurationLoader(
    (SMARTaccessBase::SMARTattributesContainerType &) mp_SMARTaccess->getSMARTattributes(), * this);
//  InitializeLogger();
}

SMARTmonitorBase::~SMARTmonitorBase() {
}

//https://gcc.gnu.org/onlinedocs/cpp/Stringification.html#Stringification
#define xstringify(s) stringify(s)
#define stringify(s) #s

void SMARTmonitorBase::SetCommandLineArgs(int argc, char ** argv)
{
  /** IMPORTANT: creating the arrays can't be done in the constructor of this
    class as "argc" is "0" there. So do this from or after "OnInit()" */
  LOGN("number of program arguments passed:" << argc)
  m_cmdLineArgStrings = new const wchar_t * [argc];
  m_ar_stdwstrCmdLineArgs = new std::wstring[argc];
  //TODO move to "common_sourcecode"
  for(fastestUnsignedDataType index = 0; index < argc; ++index)
  {
    m_ar_stdwstrCmdLineArgs[index] = GetStdWstring(argv[index]);
    LOGN( (index+1) << ". program argument:" << m_ar_stdwstrCmdLineArgs[index])
    m_cmdLineArgStrings[index] = m_ar_stdwstrCmdLineArgs[index].c_str();
  }
  m_commandLineArgs.Set(argc, (wchar_t **) m_cmdLineArgStrings);
}

  std::wstring GetExeFileName(const wchar_t * const ar_wchFullProgramPath)
  {
    std::wstring stdwstrFullProgramPath(ar_wchFullProgramPath);
    int charIndexOfLastDirSepChar = stdwstrFullProgramPath.rfind(FileSystem::
      dirSeperatorChar);
    if( charIndexOfLastDirSepChar != std::wstring::npos)
    {
      return stdwstrFullProgramPath.substr(charIndexOfLastDirSepChar + 1);
    }
    return stdwstrFullProgramPath;
  }

void SMARTmonitorBase::SetSMARTattributesToObserve(
  std::set<SMARTuniqueIDandValues> & SMARTuniqueIDandValuesContainer)
{
//  const std::set<SkSmartAttributeParsedData> & SMARTattributesToObserve =
//    wxGetApp().mp_SMARTaccess->getSMARTattributesToObserve();
  //TODO add SMART attribute IDs to SMARTattributesToObserve
  m_IDsOfSMARTattributesToObserve.clear();
  std::set<SMARTuniqueIDandValues>::const_iterator iter = 
    SMARTuniqueIDandValuesContainer.begin();
  for( ; iter != SMARTuniqueIDandValuesContainer.end(); iter++ )
  {
    const SMARTuniqueIDandValues & SMARTuniqueIDandValues = *iter;
    LOGN_DEBUG("address of SMARTuniqueIDandValues obj:" << & SMARTuniqueIDandValues)
    for( int SMARTattributeID = 0; SMARTattributeID < 
        NUM_DIFFERENT_SMART_ENTRIES; SMARTattributeID ++ )
    {
//    SkSmartAttributeParsedData skSmartAttributeParsedData;
//    skSmartAttributeParsedData.id 
//    SMARTattributesToObserve.insert();
      if( SMARTuniqueIDandValues.m_SMARTvalues[SMARTattributeID].
          m_successfullyReadSMARTrawValue )
      {
        m_IDsOfSMARTattributesToObserve.insert(SMARTattributeID);
        LOGN_DEBUG("adding SMART attribute ID " << SMARTattributeID )
      }
    }
  }
}

void SMARTmonitorBase::OutputUsage()
{
  std::ostringstream stdoss;
  stdoss << "Usage (options) (pass 2 command line arguments for each in this order:\n"
    << "<command line option NAME> <command line option VALUE>) :\n";
  for( fastestUnsignedDataType index = 0; 
    s_commandLineOptions[index].optionName[0] != '\0'; ++ index )
  {
    CommandLineOption & commandLineOption = s_commandLineOptions[index];
    stdoss << commandLineOption.optionName << " " << 
      commandLineOption.possibleOptionValue << "";
    stdoss << "\n";
  }
  std::string str = stdoss.str();
  std::cout << str << std::endl;
}

std::wstring SMARTmonitorBase::GetCommandOptionValue(const wchar_t * const 
  cmdLineOptionName)
{
  const int index = m_commandLineArgs.contains(cmdLineOptionName);
  return m_commandLineArgs.GetArgument(index);
}

std::wstring SMARTmonitorBase::GetCommandOptionValue(//const wchar_t * const str
  unsigned programArgumentIndex)
{
  //const int index = m_commandLineArgs.contains(str);
  std::wstring cmdLineOptionName = m_commandLineArgs.GetArgument(
    programArgumentIndex);
  for( fastestUnsignedDataType cmdLineOptionIndex = 0; 
    s_commandLineOptions[cmdLineOptionIndex].optionName[0] != '\0'; 
    ++ cmdLineOptionIndex )
  {
    CommandLineOption & commandLineOption = s_commandLineOptions[cmdLineOptionIndex];
    if( GetStdString_Inline(cmdLineOptionName) == commandLineOption.optionName )
      return m_commandLineArgs.GetArgument(programArgumentIndex + 1);      
  }
//  if( index != UINT_MAX && m_commandLineArgs.GetArgumentCount() > index + 1 )
//  {
//  }
  return std::wstring();
}

void SMARTmonitorBase::InitializeLogger()
{
  LogLevel::CreateLogLevelStringToNumberMapping();
  //std::string stdstrLogFilePath("SMARTmonitor.txt");
  std::wstring stdwstrProgramePath = GetExeFileName(m_commandLineArgs.GetProgramPath() );
    stdwstrProgramePath += L"_log.txt";
    
  std::string stdstrLogFilePath;
  //std::string stdstrLogFileDirectory;
  unsigned programArgumentCount = m_commandLineArgs.GetArgumentCount();
  if( programArgumentCount > 1 )
  {
    bool atLeast1UnknwonCmdLineOption = false;
    for( unsigned programArgumentIndex = 1; programArgumentIndex < programArgumentCount;
        programArgumentIndex += 2)
    {
      std::wstring cmdLineOptionName = m_commandLineArgs.GetArgument(programArgumentIndex);
      if( GetCommandOptionValue(/*cmdLineOptionName.c_str()*/
            programArgumentIndex ) == L"" )
      {
        std::wcout << L"unknown command line option \"" << cmdLineOptionName << L"\"" << std::endl;
        atLeast1UnknwonCmdLineOption = true;
      }
      if( cmdLineOptionName == L"logfilefolder" )
      {
        std::wstring stdwstrLogfilepathCmdLineArg = GetCommandOptionValue(
          /* L"logfilefolder" */ programArgumentIndex );
        if( stdwstrLogfilepathCmdLineArg.size() > 0 )
        {
          const wchar_t lastChar = stdwstrLogfilepathCmdLineArg.at(
            stdwstrLogfilepathCmdLineArg.size() - 1 );
          if( lastChar != FileSystem::dirSeperatorChar )
            stdwstrLogfilepathCmdLineArg += FileSystem::dirSeperatorChar;
        }
        stdstrLogFilePath = GetStdString_Inline( stdwstrLogfilepathCmdLineArg );
      }
    }
    if( atLeast1UnknwonCmdLineOption )
      OutputUsage();
  }  
  stdstrLogFilePath += GetStdString_Inline(stdwstrProgramePath);
  
  try
  {
    g_logger.OpenFileA(stdstrLogFilePath, "log4j", 4000, LogLevel::debug);
  }
  catch(const LogFileAccessException & lfae)
  {
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
    PATH_SEPERATOR_CHAR_STRING) );
  const int indexOfLastFileSepChar = stdwstrAbsoluteFilePath.rfind(
    //_T(xstringify(PATH_SEPERATOR_CHAR)  )
    stdwstrPathSeperatorChar );
  std::string configFileNameWithoutExtension;
  std::wstring exeFileName;
  if( indexOfLastFileSepChar != -1
    //Else this may happen: /home.git/executable"
    //&& indexOfLastFileSepChar < indexOfLastFileSepChar
    )
  {
    int thisExecutable_sFilePathLen = //wcslen(wchThisExecutable_sFilePath);
      stdwstrThisExecutable_sFilePath.length();
    exeFileName = stdwstrAbsoluteFilePath.substr(indexOfLastFileSepChar + 1,
      thisExecutable_sFilePathLen );
  }
  const int indexOfExeFileNameDot = exeFileName.rfind( L"." );
  if( indexOfExeFileNameDot == -1 ) /** If no file name extension like ".exe" */
  {
//        fullConfigFilePath = fullFilePathOfThisExecutable + wxT(".");
    if( indexOfLastFileSepChar != -1 )
    {
      fullConfigFilePathWithoutExtension = //currentWorkingDir /*+ wxFILE_SEP_PATH */+
        stdwstrAbsoluteFilePath.substr(0, indexOfLastFileSepChar + 1) +
        exeFileName + L".";
    }
  }
  else
  {
    const std::wstring fullFilePathOfThisExecutableWoutExt =
      stdwstrThisExecutable_sFilePath.substr(0, indexOfLastDot + 1);
    fullConfigFilePathWithoutExtension = fullFilePathOfThisExecutableWoutExt;
  }
}

std::string SMARTmonitorBase::Format(
  fastestUnsignedDataType SMARTattributeID, 
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  switch(SMARTattributeID)
  {
    case 9:
    {
      stdoss.precision(1);
      //stdoss << std::fixed << (float) SMARTrawValue / 3600000 << " h";
      const float hours = (float) SMARTrawValue / 3600000.0f;
      const fastestUnsignedDataType numDays = hours / 24;
      const float hoursRemainder = hours - (numDays*24);
      stdoss << std::fixed << numDays << "d" << hoursRemainder << "h";
    }
      break;
    default:
      stdoss << SMARTrawValue;
      break;
  }
  return stdoss.str();
}

void SMARTmonitorBase::UpdateSMARTvaluesThreadSafe()
{
  DWORD dwRetVal = mp_SMARTaccess->ReadSMARTValuesForAllDrives();
  SMARTaccess_type & SMARTaccess = * mp_SMARTaccess;
  if( dwRetVal == SMARTaccessBase::success )
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
      LOGN("address:" << & SMARTuniqueIDsAndValues )
      std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
        SMARTuniqueIDsAndValues.begin();
      fastestUnsignedDataType SMARTattributeID;
      uint64_t SMARTrawValue;
      const std::set<SMARTentry> & SMARTattributesToObserve =
        SMARTaccess.getSMARTattributes();
      LOGN( "# SMART attributes to observe:" << SMARTattributesToObserve.size() )
      //TODO crashed in loop header at "iter++"
      bool consistent;
      for(fastestUnsignedDataType currentDriveIndex = 0 /*, ucT4 = 0*/;
        currentDriveIndex < numberOfDifferentDrives;
        ++ currentDriveIndex, SMARTuniqueIDandValuesIter ++)
      {
    //    pDriveInfo = m_SMARTvalueProcessor.GetDriveInfo(currentDriveIndex);
        std::set<SMARTentry>::const_iterator
          SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
        uint64_t currentSMARTrawValue;
        //TODO crashes here (iterator-related?!-> thread access problem??)
        for( ; SMARTattributesToObserveIter != SMARTattributesToObserve.end();
            SMARTattributesToObserveIter ++)
        {
          SMARTattributeID = SMARTattributesToObserveIter->GetAttributeID();
          
          consistent = SMARTuniqueIDandValuesIter->m_SMARTvalues[
            SMARTattributeID].IsConsistent( m_arSMARTrawValue[lineNumber]);
          if(consistent)
          {
//          AtomicExchange( (long int *) & m_arSMART_ID[lineNumber]
          LOGN(m_arSMARTrawValue[lineNumber] << " " << currentSMARTrawValue)
          AtomicExchange(
            & m_arTickCountOfLastQueryInMilliSeconds[lineNumber]
            , GetTickCount() /*long val*/ );
          //FileTi
//              OperatingSystem::GetTimeCountInSeconds(timeInS);

          if( /*SMARTattributesToObserveIter->pretty_value*/
              m_arSMARTrawValue[lineNumber] )
            atLeast1NonNullValue = true;
          ++ lineNumber;
//            }
          }
        }
      }
//    }
  }
  else // e.g. SMARTaccessBase::accessDenied
    ;
}

DWORD THREAD_FUNCTION_CALLING_CONVENTION UpdateSMARTparameterValuesThreadFunc(void * p_v)
{
  SMARTmonitorBase * p_SMARTmonitorBase = (SMARTmonitorBase *) p_v;
  const unsigned numberOfMilliSecondsToWaitBetweenSMARTquery =
    SMARTmonitorBase::GetNumberOfMilliSecondsToWaitBetweenSMARTquery();
  fastestUnsignedDataType numberOfSecondsToWaitBetweenSMARTquery;
  if(p_SMARTmonitorBase)
  {
    do
    {
      p_SMARTmonitorBase->UpdateSMARTvaluesThreadSafe();
      p_SMARTmonitorBase->BeforeWait();

      numberOfSecondsToWaitBetweenSMARTquery =
        numberOfMilliSecondsToWaitBetweenSMARTquery / 1000;
      while( numberOfSecondsToWaitBetweenSMARTquery -- && 
        SMARTmonitorBase::s_updateSMARTvalues)
      {
        sleep(1);
      }
      //Sleep in mikroseconds (1/1000 of a millisecond))
      usleep(numberOfMilliSecondsToWaitBetweenSMARTquery % 1000 * 1000);
    }while(SMARTmonitorBase::s_updateSMARTvalues);
    
    p_SMARTmonitorBase->AfterGetSMARTvaluesLoop();    
  }
  return 0;
}

void SMARTmonitorBase::StartAsyncUpdateThread()
{
  if( mp_SMARTaccess->GetNumSMARTattributesToObserve() > 0 )
  {
    m_updateSMARTparameterValuesThread.start(
      UpdateSMARTparameterValuesThreadFunc, this);
  }
}

void SMARTmonitorBase::ConstructConfigFilePathFromExeDirPath(
  const std::wstring & stdwstrAbsoluteFilePath,
  std::wstring & fullConfigFilePathWithoutExtension)
{
  //const char ps = PATH_SEPERATOR_CHAR;
  std::wstring stdwstrPathSeperatorChar = GetStdWstring(std::string(
    PATH_SEPERATOR_CHAR_STRING) );
  const int indexOfLastPathSepChar = stdwstrAbsoluteFilePath.rfind(
    //_T(xstringify(PATH_SEPERATOR_CHAR)  )
    stdwstrPathSeperatorChar );
  if( indexOfLastPathSepChar != -1
    //Else this may happen: /home.git/executable"
    //&& indexOfLastFileSepChar < indexOfLastFileSepChar
    )
  {
    std::wstring stdwstrAbsoluteDirPath = stdwstrAbsoluteFilePath.substr(0, 
      indexOfLastPathSepChar + 1);
    fullConfigFilePathWithoutExtension = stdwstrAbsoluteDirPath + L"SMARTmonitor.";
  }
}

void SMARTmonitorBase::ConstructConfigFilePath(
  std::wstring & stdwstrConfigPathWithoutExtension)
{
  // Not necessarily an absolute file path! e.g. also "./executable.elf" is possible
  const wchar_t * wchThisExecutable_sFilePath = m_commandLineArgs.
    GetProgramPath();
  std::wstring stdwstrThisExecutable_sFilePath (wchThisExecutable_sFilePath);
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
  std::wstring configFilePathCmdLineValue = GetCommandOptionValue(L"configfilepath");
  if( configFilePathCmdLineValue == L"" ) /** NO config file path passed. */
  {
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

fastestUnsignedDataType SMARTmonitorBase::InitializeSMART()
{
  enum InitSMARTretCode initSMARTretCode = success;
  std::wstring stdwstrWorkingDirWithConfigFilePrefix;
  ConstructConfigFilePath(stdwstrWorkingDirWithConfigFilePrefix);

  try
  {
    //TODO just for compilation
    const bool successfullyLoadedConfigFile = mp_configurationLoader->
      LoadConfiguration(stdwstrWorkingDirWithConfigFilePrefix);
    
    if( ! successfullyLoadedConfigFile )
      initSMARTretCode = readingConfigFileFailed;
//    {
//      //wxMessageBox(wxT("failed reading config file \"") + workingDirWithConfigFilePrefix + wxT("\""));
//      return false;
//    }
    //if( smartReader.m_oSMARTDetails.size())
    DWORD dwRetVal = mp_SMARTaccess->ReadSMARTValuesForAllDrives();
    if( dwRetVal == SMARTaccessBase::accessDenied )
    {
      ShowMessage("access denied to S.M.A.R.T.\n->restart this program as an"
        " administrator\n->program exits (after showing this message)");
      initSMARTretCode = accessToSMARTdenied;
    }
    else
      LOGN("SMART successfully accessed")
  }catch(const ParseConfigFileException & e)
  {
    LOGN("parse config file exc")
    ShowMessage("There was at least 1 error reading the configuration file\n"
      "->this application will exit now.");
    initSMARTretCode = readingConfigFileFailed;
    throw e;
  }
  LOGN("end")
  return initSMARTretCode;
}

void SMARTmonitorBase::ShowMessage(const char * const msg) const
{
  LOGN(msg)
  std::cout << msg << std::endl;
}