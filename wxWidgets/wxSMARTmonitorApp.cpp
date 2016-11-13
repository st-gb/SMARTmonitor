/*
 * wxSMARTmonitorApp.cpp
 *
 *  Created on: 26.11.2013
 *      Author: mr.sys
 *
 *  adapted from the wxWidgets "tbtest" sample
 */
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//// the application icon (under Windows and OS/2 it is in resources)
//#ifndef wxHAS_IMAGES_IN_RESOURCES
//    #include "../sample.xpm"
//#endif
//
//#include "smile.xpm"

#include "wxSMARTmonitorApp.hpp"
#include "wxSMARTmonitorDialog.hpp" // class MyDialog
//#include "libConfig/ConfigurationLoader.hpp"
#include <wx/taskbar.h>
#include <wx/filefn.h> //wxFILE_SEP_PATH
#include <iostream> //class std::cerr
#include <ConfigLoader/ParseConfigFileException.hpp>
#include <Windows/HideMinGWconsoleWindow.h>
#include <FileSystem/File/FileException.hpp>
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include <Controller/Logger/LogFileAccessException.hpp>
#include <FileSystem/File/GetAbsoluteFilePath.hpp> // GetAbsoluteFilePath(...)

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

//class MyDialog;
/*static*/ SMARTdialog *gs_dialog = NULL;

const wxString wxSMARTmonitorApp::appName = wxT("wxSMARTmonitor");
unsigned wxSMARTmonitorApp::s_numberOfMilliSecondsToWaitBetweenSMARTquery = 10000;

wxSMARTmonitorApp::wxSMARTmonitorApp()
  : m_taskBarIcon(NULL) //,
//    m_SMARTaccess(m_wxSMARTvalueProcessor.getSMARTaccess() ),
//    configurationLoader(
//      /*smartAttributesToObserve*/ (SMARTaccessBase::SMARTattributesType &)
//      m_SMARTaccess.getSMARTattributesToObserve(),
//      * this)
{
  mp_SMARTaccess = & m_wxSMARTvalueProcessor.getSMARTaccess();
  mp_configurationLoader = new libConfig::ConfigurationLoader(
      (SMARTaccessBase::SMARTattributesType &) mp_SMARTaccess->getSMARTattributesToObserve(), * this);
  LogLevel::CreateLogLevelStringToNumberMapping();
  std::string stdstrLogFilePath("wxSMARTmonitor.txt");
  try
  {
    g_logger.OpenFileA(stdstrLogFilePath, "log4j", 4000, LogLevel::debug);
  }
  catch(const LogFileAccessException & lfae)
  {
    std::cout << lfae.GetErrorMessageA() << std::endl;
  }
  LOGN("SMART acc ptr:" << mp_SMARTaccess)
}

wxSMARTmonitorApp::~wxSMARTmonitorApp()
{
  // TODO Auto-generated destructor stub
}

IMPLEMENT_APP(wxSMARTmonitorApp)

void wxSMARTmonitorApp::ConstructConfigFilePath(
  std::wstring & stdwstrConfigPathWithoutExtension)
{
  // Not necessarily an absolute file path! e.g. also "./executable.elf" is possible
  const wxString wxstrThisExecutablesFilePath = argv[0];
  LOGN("this exe's file path: \"" << wxWidgets::GetStdWstring_Inline(
    wxstrThisExecutablesFilePath) << "\"")

  const wxString currentWorkingDir = wxGetCwd();
  
  std::wstring stdwstrAbsoluteFilePath = GetAbsoluteFilePath(
    wxWidgets::GetStdWstring_Inline(currentWorkingDir), 
    wxWidgets::GetStdWstring_Inline(wxstrThisExecutablesFilePath) );
  LOGN("this exe's absolute file path: \"" << stdwstrAbsoluteFilePath << "\"")

  LOGN("this exe's current working dir: \"" << wxWidgets::GetStdWstring_Inline(
    currentWorkingDir) << "\"")
  
  wxString fullConfigFilePathWithoutExtension;
  if(argc == 1) /** NO program arguments passed. */
  {

    //wxstrThisExecutablesFilePath
    wxString fileNameWithoutExtension;
    const int indexOfLastDot = stdwstrAbsoluteFilePath.rfind(wxT("."));
    const int indexOfLastFileSepChar = stdwstrAbsoluteFilePath.rfind(
      wxFILE_SEP_PATH);
    wxString configFileNameWithoutExtension;
    wxString exeFileName;
    if( indexOfLastFileSepChar != -1
      //Else this may happen: /home.git/executable"
      //&& indexOfLastFileSepChar < indexOfLastFileSepChar
      )
    {
      exeFileName = stdwstrAbsoluteFilePath.substr(indexOfLastFileSepChar,
        wxstrThisExecutablesFilePath.length() );
    }
    const int indexOfExeFileNameDot = exeFileName.rfind(wxT("."));
    if( indexOfExeFileNameDot == -1 ) /** If no file name extension like ".exe" */
    {
  //        fullConfigFilePath = fullFilePathOfThisExecutable + wxT(".");
      if( indexOfLastFileSepChar != -1 )
      {
        fullConfigFilePathWithoutExtension = //currentWorkingDir /*+ wxFILE_SEP_PATH */+
          stdwstrAbsoluteFilePath.substr(0, indexOfLastFileSepChar + 1) +
          exeFileName + wxT(".");
      }
    }
    else
    {
      const wxString fullFilePathOfThisExecutableWoutExt =
        wxstrThisExecutablesFilePath.substr(0, indexOfLastDot + 1);
      fullConfigFilePathWithoutExtension = fullFilePathOfThisExecutableWoutExt;
    }
  }
  else /** At least 1 program argument passed. */
  {
  //      if( )
    if( false /*isDirectoryPath(argv[1])*/ )
    {
    const wxString directoryForConfigFile = /*wxGetCwd()*/ argv[1];
    const int indexOfLastBackSlash = wxstrThisExecutablesFilePath.rfind(
      wxFILE_SEP_PATH);
    const int fileNameLen = wxstrThisExecutablesFilePath.size() - indexOfLastBackSlash - 1;
    const wxString workingDirWithConfigFilePrefix = directoryForConfigFile +
      wxstrThisExecutablesFilePath.substr(indexOfLastBackSlash,
      fileNameLen - 2 /* - 3 chars extension + file separator char */);
    fullConfigFilePathWithoutExtension = workingDirWithConfigFilePrefix;
    }
    else
      fullConfigFilePathWithoutExtension = argv[1];
  }
  //  wxWidgets::wxSMARTreader smartReader;
  /*std::wstring*/ stdwstrConfigPathWithoutExtension =
    wxWidgets::GetStdWstring_Inline(fullConfigFilePathWithoutExtension);
  LOGN("using config file path: \"" << stdwstrConfigPathWithoutExtension << "\"")
}
bool wxSMARTmonitorApp::OnInit()
{
//  std::cerr << "ggg" << std::endl;
//  if ( ! wxApp::OnInit() )
//  {
////    std::cerr << "ggg" << std::endl;
//    return false;
//  }

  /** wxTaskBarIcon::IsAvailable exists since wxWidgets 2.9.0 */
#if wxMAJOR_VERSION > 1 && wxMINOR_VERSION > 8
  if ( wxTaskBarIcon::IsAvailable() )
    wxGetApp().m_taskBarIcon = new MyTaskBarIcon();
  else
  {
    wxMessageBox(
      wxT("There appears to be no system tray support in your current "
        "environment. This application may not behave as expected."),
      wxT("Warning")
      , wxOK | wxICON_EXCLAMATION
    );
  }
#else
  wxGetApp().m_taskBarIcon = new MyTaskBarIcon();
#endif
  try
  {
//    if( m_SMARTaccess.GetNumberOfSMARTparametersToRead() > 0 )
    {
//      m_taskBarIcon = new MyTaskBarIcon();
      // Create the main window
  #ifdef __MINGW32__
      HideMinGWconsoleWindow();
  #endif
    }
//    else
//    {
//      ShowMessage("0 SMART parameters to read -> exiting");
//      return false;
//    }
    /** Show a dialog. Else when displaying messages no icon appears in the
     *  task bar and this not able to switch there with alt+Tab.*/
    gs_dialog = new SMARTdialog(wxT("wxS.M.A.R.T. monitor"), m_wxSMARTvalueProcessor);
    gs_dialog->Show(true);
    m_wxSMARTvalueProcessor.Init();

    std::wstring stdwstrWorkingDirWithConfigFilePrefix;
    ConstructConfigFilePath(stdwstrWorkingDirWithConfigFilePrefix);

//  std::set<SkSmartAttributeParsedData> smartAttributesToObserve;
//  libConfig::ConfigurationLoader configurationLoader(
//    /*smartAttributesToObserve*/ (SMARTaccessBase::SMARTattributesType &)
//    m_SMARTaccess.getSMARTattributesToObserve(),
//    * this);

    try
    {
        //TODO just for compilation
      const bool successfullyLoadedConfigFile = mp_configurationLoader->
        LoadConfiguration(stdwstrWorkingDirWithConfigFilePrefix);
  //    ! smartReader.LoadSMARTparameterConfigFile(
  //            stdwstrWorkingDirWithConfigFilePrefix, this);
      if( ! successfullyLoadedConfigFile )
      {
        //wxMessageBox(wxT("failed reading config file \"") + workingDirWithConfigFilePrefix + wxT("\""));
        return false;
      }
    }catch(const ParseConfigFileException & e)
    {
      ShowMessage("There was at least 1 error reading the configuration file\n"
        "->this application will exit now.");
    }

    //if( smartReader.m_oSMARTDetails.size())
    DWORD dwRetVal = mp_SMARTaccess->ReadSMARTValuesForAllDrives();
    if( dwRetVal == SMARTaccessBase::accessDenied )
    {
      wxMessageBox( wxT("access denied to S.M.A.R.T.\n->restart this program as an"
        " administrator\n->program exits after closing this message box") );
      return false;
    }
    gs_dialog->StartAsyncUpdateThread();
  }
  catch(const FileException & fe)
  {
    TCHAR * filePath = fe.GetFilePath();
    wxMessageBox(wxT("failed to open file: ") + wxWidgets::getwxString_inline(
      //GetStdWstring( ))
      filePath ) );
    return false;
  }
  //  gs_dialog->Show(true);
  return true;
}

bool wxSMARTmonitorApp::GetSMARTokayIcon(wxIcon & icon)
{
//  wxIcon icon; //(/*smile_xpm*/  );
  #ifdef _WIN32
  wxString wxstrIconFileName = wxT("S.M.A.R.T._OK.ico");
  #else
  wxString wxstrIconFileName = wxT("S.M.A.R.T._OK.xpm");
  #endif
  bool bLoadFileRetVal = icon.LoadFile(
    //http://docs.wxwidgets.org/trunk/classwx_bitmap.html:
    //"wxMSW supports BMP and ICO files, BMP and ICO resources;
    // wxGTK supports XPM files;"
    #ifdef _WIN32
      wxstrIconFileName,
      wxBITMAP_TYPE_ICO
    #else //#ifdef __WXGTK__ //wxGTK
      wxstrIconFileName,
      wxBITMAP_TYPE_XPM
    #endif
    );
  if( ! bLoadFileRetVal)
    wxMessageBox( wxT("Loading icon file \n\"") + wxGetCwd() +
      wxFILE_SEP_PATH + wxstrIconFileName + wxT( "\" failed") );
  return bLoadFileRetVal;
}

bool wxSMARTmonitorApp::GetSMARTwarningIcon(wxIcon & icon)
{
//  wxIcon icon; //(/*smile_xpm*/  );
  #ifdef _WIN32
  wxString wxstrIconFileName = wxT("S.M.A.R.T._warning.ico");
  #else
  wxString wxstrIconFileName = wxT("S.M.A.R.T._warning.xpm");
  #endif
  bool bLoadFileRetVal = icon.LoadFile(
    //http://docs.wxwidgets.org/trunk/classwx_bitmap.html:
    //"wxMSW supports BMP and ICO files, BMP and ICO resources;
    // wxGTK supports XPM files;"
    #ifdef _WIN32
      wxstrIconFileName,
      wxBITMAP_TYPE_ICO
    #else //#ifdef __WXGTK__ //wxGTK
      wxstrIconFileName,
      wxBITMAP_TYPE_XPM
    #endif
    );
  if( ! bLoadFileRetVal)
    wxMessageBox( wxT("Loading icon file \n\"") + wxGetCwd() +
      wxFILE_SEP_PATH + wxstrIconFileName + wxT( "\" failed") );
  return bLoadFileRetVal;
}

void wxSMARTmonitorApp::ShowMessage(const char * const str) const
{
  wxString wxstrMessage = wxWidgets::GetwxString_Inline(str);
  wxMessageBox(wxstrMessage, appName );
}
