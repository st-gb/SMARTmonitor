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
#include <Windows/HideMinGWconsoleWindow.h>
#include <FileSystem/File/FileException.hpp>
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

//class MyDialog;
/*static*/ SMARTdialog *gs_dialog = NULL;

const wxString wxSMARTmonitorApp::appName = wxT("wxSMARTmonitor");

wxSMARTmonitorApp::wxSMARTmonitorApp()
  : m_taskBarIcon(NULL) //,
//    m_SMARTaccess(m_wxSMARTvalueProcessor.getSMARTaccess() ),
//    configurationLoader(
//      /*smartAttributesToObserve*/ (SMARTaccessBase::SMARTattributesType &)
//      m_SMARTaccess.getSMARTattributesToObserve(),
//      * this)
{
  //InitializeLogger(); 
  //mp_SMARTaccess = & m_wxSMARTvalueProcessor.getSMARTaccess();
  LOGN("SMART access pointer:" << mp_SMARTaccess)
}

wxSMARTmonitorApp::~wxSMARTmonitorApp()
{
  delete mp_configurationLoader;
  delete [] m_cmdLineArgStrings;
  delete [] m_ar_stdwstrCmdLineArgs;
}

IMPLEMENT_APP(wxSMARTmonitorApp)

void wxSMARTmonitorApp::CreateTaskBarIcon()
{
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
}

void wxSMARTmonitorApp::CreateCommandLineArgsArrays()
{
  /** IMPORTANT: creating the arrays can't be done in the constructor of this
    class as "argc" is "0" there. So do this from or after "OnInit()" */
  LOGN("number of program arguments passed:" << argc)
  m_cmdLineArgStrings = new const wchar_t * [argc];
  m_ar_stdwstrCmdLineArgs = new std::wstring[argc];
  //TODO move to "common_sourcecode"
  for(fastestUnsignedDataType index = 0; index < argc; ++index)
  {
    m_ar_stdwstrCmdLineArgs[index] = wxWidgets::GetStdWstring_Inline(argv[index]);
    LOGN( (index+1) << ". program argument:" << m_ar_stdwstrCmdLineArgs[index])
    m_cmdLineArgStrings[index] = m_ar_stdwstrCmdLineArgs[index].c_str();
  }
  m_commandLineArgs.Set(argc, (wchar_t **) m_cmdLineArgStrings);
}

bool wxSMARTmonitorApp::OnInit()
{
  CreateCommandLineArgsArrays();
  CreateTaskBarIcon();
  //m_wxSMARTvalueProcessor.Init();
  //InitializeSMART();
  try
  {
//    if( m_SMARTaccess.GetNumberOfSMARTparametersToRead() > 0 )
    {
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
    gs_dialog = new SMARTdialog(wxT("wxS.M.A.R.T. monitor"), //m_wxSMARTvalueProcessor
      m_SMARTvalueProcessor);
    gs_dialog->Show(true);
    InitializeSMART();
    //TODO exchange by wxGetApp().StartAsyncUpdateThread();
    gs_dialog->StartAsyncUpdateThread();
  }
  /** Thrown by mp_configurationLoader->LoadConfiguration(..:) */
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
