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
#include "libConfig/ConfigurationLoader.hpp"
#include <wx/taskbar.h>
#include <wx/filefn.h> //wxFILE_SEP_PATH
#include <iostream> //class std::cerr
#include <ConfigLoader/ParseConfigFileException.hpp>
#include <Windows/HideMinGWconsoleWindow.h>
#include <FileSystem/File/FileException.hpp>
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

//class MyDialog;
/*static*/ MyDialog *gs_dialog = NULL;

const wxString wxSMARTmonitorApp::appName = wxT("wxSMARTmonitor");
unsigned wxSMARTmonitorApp::s_numberOfMilliSecondsToWaitBetweenSMARTquery = 10000;

wxSMARTmonitorApp::wxSMARTmonitorApp()
{
  // TODO Auto-generated constructor stub

}

wxSMARTmonitorApp::~wxSMARTmonitorApp()
{
  // TODO Auto-generated destructor stub
}

IMPLEMENT_APP(wxSMARTmonitorApp)

bool wxSMARTmonitorApp::OnInit()
{
//  std::cerr << "ggg" << std::endl;
//  if ( ! wxApp::OnInit() )
//  {
////    std::cerr << "ggg" << std::endl;
//    return false;
//  }

#if wxMAJOR_VERSION > 2 && wxMINOR_VERSION > 8 // since wxWidgets 2.9.0
  if ( ! wxTaskBarIcon::IsAvailable() )
  {
    wxMessageBox(
      wxT("There appears to be no system tray support in your current "
        "environment. This application may not behave as expected."),
      wxT("Warning")
      , wxOK | wxICON_EXCLAMATION
    );
  }
#endif
  try
  {
    /** Show a dialog. Else when displaying messages no icon appears in the
     *  task bar and this not able to switch there with alt+Tab.*/
    gs_dialog = new MyDialog(wxT("wxS.M.A.R.T. monitor"), m_wxSMARTvalueProcessor);
    gs_dialog->Show(true);
    m_wxSMARTvalueProcessor.Init();
    const wxString fullFilePathOfThisExecutable = argv[0];
    wxString fullConfigFilePath;
    if(argc == 1)
    {
      const int indexOfLastDot = fullFilePathOfThisExecutable.rfind(wxT("."));
      const wxString fullFilePathOfThisExecutableWoutExt =
          fullFilePathOfThisExecutable.substr(0, indexOfLastDot + 1);
      fullConfigFilePath = fullFilePathOfThisExecutableWoutExt;
    }
    else
    {
      const wxString directoryForConfigFile = /*wxGetCwd()*/ argv[1];
      const int indexOfLastBackSlash = fullFilePathOfThisExecutable.rfind(
        wxFILE_SEP_PATH);
      const int fileNameLen = fullFilePathOfThisExecutable.size() - indexOfLastBackSlash - 1;
      const wxString workingDirWithConfigFilePrefix = directoryForConfigFile +
        fullFilePathOfThisExecutable.substr(indexOfLastBackSlash,
        fileNameLen - 2 /* - 3 chars extension + file separator char */);
      fullConfigFilePath = workingDirWithConfigFilePrefix;
    }
//  wxWidgets::wxSMARTreader smartReader;
    std::wstring stdwstrWorkingDirWithConfigFilePrefix =
      wxWidgets::GetStdWstring_Inline(fullConfigFilePath);

//  std::set<SkSmartAttributeParsedData> smartAttributesToObserve;
  libConfig::ConfigurationLoader configurationLoader(
    /*smartAttributesToObserve*/ (SMARTaccessBase::SMARTattributesType &) m_SMARTaccess.getSMARTattributesToObserve(),
    *this);

    try
    {
        //TODO just for compilation
      const bool successfullyLoadedConfigFile = configurationLoader.
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
    DWORD dwRetVal = m_SMARTaccess.ReadSMARTValuesForAllDrives();
    if( dwRetVal == SMARTaccessBase::accessDenied )
    {
      wxMessageBox( wxT("access denied to S.M.A.R.T.\n->restart this program as an"
        " administrator\n->programs exits after closing this message box") );
      return false;
    }
    if( m_SMARTaccess.GetNumberOfSMARTparametersToRead() > 0 )
    {
      m_taskBarIcon = new MyTaskBarIcon();
      // Create the main window
  #ifdef __MINGW32__
      HideMinGWconsoleWindow();
  #endif
    }
    else
    {
      ShowMessage("0 SMART parameters to read -> exiting");
      return false;
    }
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
