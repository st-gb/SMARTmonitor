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

SMARTmonitorBase::SMARTmonitorBase() 
{
  mp_SMARTaccess = & m_SMARTvalueProcessor.getSMARTaccess();
  mp_configurationLoader = new libConfig::ConfigurationLoader(
    (SMARTaccessBase::SMARTattributesType &) mp_SMARTaccess->getSMARTattributesToObserve(), * this);
  InitializeLogger();
}

SMARTmonitorBase::~SMARTmonitorBase() {
}

//https://gcc.gnu.org/onlinedocs/cpp/Stringification.html#Stringification
#define xstringify(s) stringify(s)
#define stringify(s) #s

void SMARTmonitorBase::InitializeLogger()
{
  LogLevel::CreateLogLevelStringToNumberMapping();
  std::string stdstrLogFilePath("SMARTmonitor.txt");
  try
  {
    g_logger.OpenFileA(stdstrLogFilePath, "log4j", 4000, LogLevel::debug);
  }
  catch(const LogFileAccessException & lfae)
  {
    std::cout << lfae.GetErrorMessageA() << std::endl;
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
  if(m_commandLineArgs.GetArgumentCount() == 1) /** NO program arguments passed. */
  {
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
      int thisExecutable_sFilePathLen = wcslen(wchThisExecutable_sFilePath);
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

void SMARTmonitorBase::InitializeSMART()
{
  std::wstring stdwstrWorkingDirWithConfigFilePrefix;
  ConstructConfigFilePath(stdwstrWorkingDirWithConfigFilePrefix);

  try
  {
    //TODO just for compilation
    const bool successfullyLoadedConfigFile = mp_configurationLoader->
      LoadConfiguration(stdwstrWorkingDirWithConfigFilePrefix);
    
//    if( ! successfullyLoadedConfigFile )
//    {
//      //wxMessageBox(wxT("failed reading config file \"") + workingDirWithConfigFilePrefix + wxT("\""));
//      return false;
//    }
    //if( smartReader.m_oSMARTDetails.size())
    DWORD dwRetVal = mp_SMARTaccess->ReadSMARTValuesForAllDrives();
    if( dwRetVal == SMARTaccessBase::accessDenied )
    {
      ShowMessage("access denied to S.M.A.R.T.\n->restart this program as an"
        " administrator\n->program exits after closing this message box");
//      return false;
    }
    else
      LOGN("SMART successfully accessed")
  }catch(const ParseConfigFileException & e)
  {
    LOGN("parse config file exc")
    ShowMessage("There was at least 1 error reading the configuration file\n"
      "->this application will exit now.");
    throw e;
  }
  LOGN("end")
}

void SMARTmonitorBase::ShowMessage(const char * const msg) const
{
  LOGN(msg)
}
