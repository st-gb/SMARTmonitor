/* wxSMARTmonitorApp.cpp
 * Created on: 26.11.2013
 *  Author: mr.sys
 *  adapted from the wxWidgets "tbtest" sample  */
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
#include "compiler/GCC/enable_disable_warning.h"
/** Prevent GCC/g++ warning "warning: deprecated conversion from string constant 
 *  to ‘char*’" when including the "xpm" file */
GCC_DIAG_OFF(write-strings)
#include "../S.M.A.R.T._OK.xpm"
#include "../S.M.A.R.T._warning.xpm"
GCC_DIAG_ON(write-strings)

#include <iostream> //class std::cerr
#include <OperatingSystem/multithread/GetCurrentThreadNumber.hpp>
#include <ConfigLoader/ConfigurationLoaderBase.hpp> //class ConfigurationLoaderBase
#include <Windows/HideMinGWconsoleWindow.h>
#include <FileSystem/File/FileException.hpp>
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(..)
#include "ConnectToServerDialog.hpp"

// global variables
/*static*/ SMARTdialog *gs_dialog = NULL;

/** definitions of static class members. */
fastestUnsignedDataType wxSMARTmonitorApp::s_GUIthreadID;
wxIcon wxSMARTmonitorApp::s_SMARTokIcon;
wxIcon wxSMARTmonitorApp::s_SMARTwarningIcon;

//from https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#The_Normal_Case
//const wxEventType AfterConnectToServerEventType = wxNewEventType();
DEFINE_LOCAL_EVENT_TYPE(AfterConnectToServerEventType)
DEFINE_LOCAL_EVENT_TYPE(ShowMessageEventType)
DEFINE_LOCAL_EVENT_TYPE(StartServiceConnectionCountDownEventType)

BEGIN_EVENT_TABLE(wxSMARTmonitorApp, wxApp)
  EVT_COMMAND(wxID_ANY, AfterConnectToServerEventType, wxSMARTmonitorApp::OnAfterConnectToServer)
  EVT_COMMAND(wxID_ANY, ShowMessageEventType, wxSMARTmonitorApp::OnShowMessage)
  EVT_COMMAND(wxID_ANY, StartServiceConnectionCountDownEventType, 
    wxSMARTmonitorApp::OnStartServiceConnectionCountDown)
  EVT_TIMER(TIMER_ID, wxSMARTmonitorApp ::OnTimer)
END_EVENT_TABLE()

//const wxString wxSMARTmonitorApp::appName = wxT("wxSMARTmonitor");

wxSMARTmonitorApp::wxSMARTmonitorApp()
  : m_taskBarIcon(NULL) //,
//    m_SMARTaccess(m_wxSMARTvalueProcessor.getSMARTaccess() ),
//    configurationLoader(
//      /*smartAttributesToObserve*/ (SMARTaccessBase::SMARTattributesType &)
//      m_SMARTaccess.getSMARTattributesToObserve(),
//      * this)
  , m_pConnectToServerDialog(NULL)
  , m_wxtimer(this, TIMER_ID)
{
  s_GUIthreadID = GetCurrentThreadNumber();
  //InitializeLogger(); 
  //mp_SMARTaccess = & m_wxSMARTvalueProcessor.getSMARTaccess();
  //LOGN("SMART access pointer:" << mp_SMARTaccess)
}

wxSMARTmonitorApp::~wxSMARTmonitorApp()
{
}

IMPLEMENT_APP(wxSMARTmonitorApp)

void wxSMARTmonitorApp::CreateTaskBarIcon()
{
  /** wxTaskBarIcon::IsAvailable exists since wxWidgets 2.9.0 */
#if wxMAJOR_VERSION > 1 && wxMINOR_VERSION > 8
  if ( wxTaskBarIcon::IsAvailable() )
    wxGetApp().m_taskBarIcon = new TaskBarIcon();
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
  wxGetApp().m_taskBarIcon = new TaskBarIcon();
#endif
}

void wxSMARTmonitorApp::OnStartServiceConnectionCountDown(
  wxCommandEvent & event)
{
  m_serviceConnectionCountDownInSeconds = event.GetInt();
  m_wxtimer.Start(1000);
}

void wxSMARTmonitorApp::StartServiceConnectionCountDown(
  const fastestUnsignedDataType countDownInSeconds)
{
  if( GetCurrentThreadNumber() == s_GUIthreadID )
  {
    m_serviceConnectionCountDownInSeconds = countDownInSeconds;
    m_wxtimer.Start(1000);
  }
  else
  {
    wxCommandEvent startServiceConnectionCountDown( StartServiceConnectionCountDownEventType );
    startServiceConnectionCountDown.SetInt(countDownInSeconds);
    wxPostEvent(this, startServiceConnectionCountDown);
  }
}

void wxSMARTmonitorApp::ReBuildUserInterface() { 
  //SetSMARTdriveID();
//  SetSMARTattribIDandNameLabel();
  gs_dialog->ReBuildUserInterface();
}

void wxSMARTmonitorApp::ShowStateAccordingToSMARTvalues(bool atLeast1CriticalNonNullValue)
{
  if( atLeast1CriticalNonNullValue )
    ShowSMARTwarningIcon();
  else
    ShowSMARTokIcon();
}

void wxSMARTmonitorApp::OnAfterConnectToServer(wxCommandEvent & e)
{
  if( m_pConnectToServerDialog )
  {
    m_pConnectToServerDialog->Close(true);
    delete m_pConnectToServerDialog;
    m_pConnectToServerDialog = NULL;
  }
  int connectResult = e.GetInt();
  if( connectResult == 0)
  {
//    SuccessfullyConnectedToClient();
    /*if( !*/ GetSMARTvaluesAndUpdateUI(); //)
//      StartServiceConnectionCountDown(countDownInSeconds);
    gs_dialog->EnableServerInteractingControls(connectResult);
  }
  else
  {
    /** If not closing the socket then socket file descriptor number increases?*/
    close(m_socketFileDesc);
    gs_dialog->EnableServerInteractingControls(connectResult);
    HandleConnectionError("");
    fastestUnsignedDataType countDownInSeconds = 60;
//    gs_dialog->StartCountDown(countDownInSeconds);
//    m_wxtimer.StartOnce(countDownInSeconds * 1000);
    StartServiceConnectionCountDown(countDownInSeconds);
  }
}

/** Should only be called from the UI thread?!, else program crash? */
void wxSMARTmonitorApp::AfterConnectToServer(int connectResult)
{
  /** To execute in UI thread.
   * https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#The_Normal_Case */
  wxCommandEvent AfterConnectToServerEvent( AfterConnectToServerEventType );
  AfterConnectToServerEvent.SetInt(connectResult);
  wxPostEvent(this, AfterConnectToServerEvent);
//    return errorConnectingToService;
//  OnAfterConnectToServer();
}

void wxSMARTmonitorApp::BeforeWait()
{
  /** This function is usually called from a non-GUI thread. So we have to send
   *  an event to let the GUI update happen in the UI thread to avoid a program 
   * crash. */
  wxCommandEvent UpdateSMARTvaluesEvent( UpdateSMARTparameterValuesInGUIEventType );
  wxPostEvent(gs_dialog, UpdateSMARTvaluesEvent);
}

void wxSMARTmonitorApp::ChangeState(enum serverConnectionState newState)
{
  //TODO ensure to/must be called in GUI thread
  gs_dialog->SetState(newState);
}

void wxSMARTmonitorApp::GetTextFromUser(
  const char * label, 
  std::string & std_strValue)
{
  const wxString wxstrLabel = wxWidgets::GetwxString_Inline(label);
  const wxString defaultValue = wxWidgets::GetwxString_Inline(std_strValue);
  wxString wxstrText = wxGetTextFromUser(
    wxstrLabel //message,
    , wxGetTextFromUserPromptStr// const wxString & 	caption = wxGetTextFromUserPromptStr,
    , defaultValue //const wxString & 	default_value = wxEmptyString,
    , NULL //wxWindow * 	parent = NULL,
    );
  std_strValue = wxWidgets::GetStdString_Inline(wxstrText);
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

void wxSMARTmonitorApp::OnTimer(wxTimerEvent& event)
{
//  if( m_serverConnectionState = connectedToService)
//  else
  if( m_serviceConnectionCountDownInSeconds --)
  {
    wxString wxstrServiceHostName = m_stdstrServiceHostName;
    gs_dialog->SetStatus( wxString::Format(wxT("connection attempt to %s, port %u in %u seconds"),
      wxstrServiceHostName.c_str(), 
      m_socketPortNumber, 
      m_serviceConnectionCountDownInSeconds ));
  }
  else
  {
    gs_dialog->SetStatus(wxT("") );
    m_wxtimer.Stop();
    ConnectToServerAndGetSMARTvalues();
  }
}

/** http://docs.wxwidgets.org/trunk/classwx_app_console.html#a99953775a2fd83fa2456e390779afe15 : 
 *  "This must be provided by the application, and will usually create the 
 *  application's main window, optionally calling SetTopWindow()."  */
bool wxSMARTmonitorApp::OnInit()
{
  CreateCommandLineArgsArrays();
  CreateTaskBarIcon();
  /** Show a dialog. Else when displaying messages no icon appears in the
   *  task bar and this not able to switch there with alt+Tab.*/
  gs_dialog = new SMARTdialog(wxT("wxS.M.A.R.T. monitor"), //m_wxSMARTvalueProcessor
    m_SMARTvalueProcessor);
  gs_dialog->Show(true);
  GetSMARTokayIcon(s_SMARTokIcon);
  GetSMARTwarningIcon(s_SMARTwarningIcon);  

  ProcessCommandLineArgs(); /** May display messages. */
  InitializeLogger();
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
    const fastestUnsignedDataType initSMARTresult = InitializeSMART();
    std::wstring stdwstrServiceConnectionConfigFile = 
      s_programOptionValues[serviceConnectionConfigFile];
    mp_configurationLoader->ReadServiceConnectionSettings(
      stdwstrServiceConnectionConfigFile );

    ShowSMARTokIcon();
    //TODO execute after OnInit(): else no dialog is shown?
    if( ! stdwstrServiceConnectionConfigFile.empty() )
    {
      m_wxtimer.StartOnce(1000);
//      ConnectToServerAndGetSMARTvalues();
    }
    else 
      if( initSMARTresult == SMARTaccessBase::success)
    {
      //TODO exchange by wxGetApp().StartAsyncUpdateThread();
      gs_dialog->StartAsyncUpdateThread();
    }
//    else if( result == SMARTaccessBase::noSingleSMARTdevice )
//      return false;
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

//int wxSMARTmonitorApp::OnRun ()
//{
//}

bool wxSMARTmonitorApp::GetIcon(
  wxIcon & icon,
  wxString iconFileName, 
  char * inMemoryIcon [] )
{
  /** http://docs.wxwidgets.org/trunk/classwx_bitmap.html:
  *  "wxMSW supports BMP and ICO files, BMP and ICO resources;
  *  wxGTK supports XPM files;" */
  wxBitmapType wxbitmapType;
  #ifdef _WIN32
    wxbitmapType = wxBITMAP_TYPE_ICO;
    iconFileName += wxT("ico")
  #else //#ifdef __WXGTK__ //wxGTK
    wxbitmapType = wxBITMAP_TYPE_XPM;
    iconFileName += wxT("xpm");
  #endif

  bool bIconFileSuccessfullyLoaded = icon.LoadFile(
    iconFileName,
    wxbitmapType);
  if( ! bIconFileSuccessfullyLoaded)
  {
    wxMessageBox( wxT("Loading icon file \n\"") + wxGetCwd() +
      wxFILE_SEP_PATH + iconFileName + wxT( "\" failed") );
    /** Loading a (custom) icon from file failed, so provide a pre-defined one.*/
    icon = wxIcon(inMemoryIcon);
  }
  return bIconFileSuccessfullyLoaded;
}

//TODO better call this function only once at startup?!
bool wxSMARTmonitorApp::GetSMARTokayIcon(wxIcon & icon)
{
  return GetIcon(icon, wxT("S.M.A.R.T._OK."), S_M_A_R_T__OK_xpm);
}

//TODO better call this function only once at startup?!
bool wxSMARTmonitorApp::GetSMARTwarningIcon(wxIcon & icon)
{
  return GetIcon(icon, wxT("S.M.A.R.T._warning."), S_M_A_R_T__warning_xpm);
}

void wxSMARTmonitorApp::SetAttribute(
  const SMARTuniqueID & sMARTuniqueID, 
  fastestUnsignedDataType SMARTattributeID,
  const enum ColumnIndices::columnIndices & columnIndex,
  const std::string & std_strValue)
{
  wxString wxstrValue = wxWidgets::GetwxString_Inline(std_strValue );
  //TODO: Maybe implement this in class wxWidgets::SMARTtableListCtrl as this 
  //should avoid 2 times a pointer dereference -> performs better or in 
  //SMARTmonitorDialog as param. "sMARTuniqueID" needs to be taken into account
  gs_dialog->m_pwxlistctrl->SetSMARTattribValue(
    /*lineNumber*/ SMARTattributeID, //long index
    columnIndex /** column #/ index */,
    wxstrValue);
}
  
void wxSMARTmonitorApp::ShowConnectionState(const char * const pchServerAddress, 
  int connectTimeOutInSeconds)
{
  m_pConnectToServerDialog = new ConnectToServerDialog(
    pchServerAddress, m_socketPortNumber, connectTimeOutInSeconds, m_socketFileDesc);
  m_pConnectToServerDialog->Show();
  /** Ensures "connect" can't be pressed a second time */
//  m_pConnectToServerDialog->ShowModal();
}

void wxSMARTmonitorApp::OnShowMessage(wxCommandEvent & event)
{
    wxString wxstrMessage = event.GetString();
//    wxMessageBox(wxstrMessage, m_appName );
    gs_dialog->ShowText(wxstrMessage);  
}

void wxSMARTmonitorApp::ShowMessage(const char * const str) const
{
  unsigned currentThreadNumber = GetCurrentThreadNumber();
  /** Only call UI functions in UI thread, else gets SIGABORT error when calling
   *  "wxMessageBox(...)" . */
  if( currentThreadNumber == s_GUIthreadID )
  { 
    wxString wxstrMessage = wxWidgets::GetwxString_Inline(str);
//    wxMessageBox(wxstrMessage, m_appName );
    gs_dialog->ShowText(wxstrMessage);
  }
  else
  {
    /** Post an event with the message as content to show 
    *   this message in the GUI thread */
    wxCommandEvent wxcommand_event(ShowMessageEventType);
    wxcommand_event.SetString(GetwxString_Inline(str));
    wxPostEvent( (wxSMARTmonitorApp *) this, wxcommand_event);
  }
}

void wxSMARTmonitorApp::ShowIcon(const wxIcon & icon, const wxString & message )
{
  wxString serviceLocation = wxWidgets::GetwxString_Inline(m_stdstrServiceHostName);

  if( serviceLocation == wxT("") )
    serviceLocation = wxT("direct SMART access");
  wxString tooltip = wxString::Format( wxT("for %s:\n%s"), 
    serviceLocation, message );
  
    //if ( wxGetApp().m_taskBarIcon == NULL )  
  if ( ! m_taskBarIcon->SetIcon( icon, tooltip ) )
    wxMessageBox(wxT("Could not set new taskbar icon."), wxT("wxSMARTmonitor") );
  if( gs_dialog)
    gs_dialog->SetIcon(icon);
}

wxIcon wxSMARTmonitorApp::ShowSMARTokIcon()
{
  ShowIcon(s_SMARTokIcon, wxT("raw value for all critical SMART parameters is 0") );
  return s_SMARTokIcon;
}

wxIcon wxSMARTmonitorApp::ShowSMARTwarningIcon()
{
  ShowIcon(s_SMARTwarningIcon, wxT("raw value for at least 1 critical SMART "
    "parameter is > 0") );
  return s_SMARTwarningIcon;
}
