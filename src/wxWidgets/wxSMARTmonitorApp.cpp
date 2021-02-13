/* wxSMARTmonitorApp.cpp
 * Created on: 26.11.2013
 *  Author:Stefan Gebauer, M. Sc. Comp. Sc.
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

///wxWidgets header files:
#include <wx/filefn.h> //wxFILE_SEP_PATH
#include <wx/log.h>///class wxLogNull
#include <wx/taskbar.h>

///Stefan Gebauer's "common_sourcecode" repository header files:
///GCC_DIAG_ON(...), GCC_DIAG_OFF(...) preprocessor macros
#include "compiler/GCC/enable_disable_warning.h"
#include <FileSystem/File/FileException.hpp>
#include <FileSystem/PathSeperatorChar.hpp>///dirSeperatorCharW
#include <OperatingSystem/multithread/GetCurrentThreadNumber.hpp>
#ifdef __MINGW32__
#include <OperatingSystem/Windows/HideMinGWconsoleWindow.h>
#endif
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(..)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

///This repository's header files:
/** Prevent GCC/g++ warning "warning: deprecated conversion from string constant 
 *  to ‘char*’" when including the "xpm" file */
GCC_DIAG_OFF(write-strings)
#include "../icons/S.M.A.R.T._OK.xpm"
#include "../icons/S.M.A.R.T._unknown.xpm"
#include "../icons/S.M.A.R.T._warning.xpm"
GCC_DIAG_ON(write-strings)
#include "SetSMARTattrEvent.hpp"///class SetSMARTattrEvent
#include "wxSMARTmonitorApp.hpp"
#include "ConnectToServerDialog.hpp"
#include <ConfigLoader/ConfigurationLoaderBase.hpp> //class ConfigurationLoaderBase
//#include "libConfig/ConfigurationLoader.hpp"

///Standard C(++) header files:
#include <iostream> //class std::cerr

// global variables
/*static*/ SMARTdialog * gs_dialog = NULL;

/** definitions of static class members. */
wxIcon wxSMARTmonitorApp::s_SMARTokIcon;
wxIcon wxSMARTmonitorApp::s_SMARTstatusUnknownIcon;
wxIcon wxSMARTmonitorApp::s_SMARTwarningIcon;

//from https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#The_Normal_Case
//const wxEventType AfterConnectToServerEventType = wxNewEventType();
DEFINE_LOCAL_EVENT_TYPE(AfterConnectToServerEventType)
DEFINE_LOCAL_EVENT_TYPE(ChangeStateEvtType)
DEFINE_LOCAL_EVENT_TYPE(CnnctToSrvrEvtType)
DEFINE_LOCAL_EVENT_TYPE(ShowCurrentActionEventType)
DEFINE_LOCAL_EVENT_TYPE(ShowMessageEventType)
DEFINE_LOCAL_EVENT_TYPE(StartServiceConnectionCountDownEventType)
DEFINE_LOCAL_EVENT_TYPE(StartCnnctCntDownEvtType)

BEGIN_EVENT_TABLE(wxSMARTmonitorApp, wxApp)
  EVT_COMMAND(wxID_ANY, AfterConnectToServerEventType, wxSMARTmonitorApp::
    OnAfterConnectToServer)
  EVT_COMMAND(wxID_ANY, ChangeStateEvtType, wxSMARTmonitorApp::OnChangeState)
  EVT_COMMAND(wxID_ANY, CnnctToSrvrEvtType, wxSMARTmonitorApp::OnCnnctToSrvr)
  EVT_COMMAND(wxID_ANY, ShowMessageEventType, wxSMARTmonitorApp::OnShowMessage)
  EVT_COMMAND(wxID_ANY, StartCnnctCntDownEvtType, wxSMARTmonitorApp::
    OnStartSrvCnnctnCntDown)
  EVT_COMMAND(wxID_ANY, ShowCurrentActionEventType, wxSMARTmonitorApp::
    OnShowCurrentAction)
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
  , m_p_cnnctToSrvDlg(NULL)
  , m_wxtimer(this, TIMER_ID)
{
  s_UIthreadID = OperatingSystem::GetCurrentThreadNumber();
#ifdef multithread
  I_Thread::SetCurrentThreadName("UI");
#endif
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
#if wxMAJOR_VERSION > 1 && wxMINOR_VERSION > 8 || wxMAJOR_VERSION > 2
  if (///https://docs.wxwidgets.org/trunk/classwx_task_bar_icon.html#a287bb3303f01651f50c8de17e314a147;
     /** "Since 2.9.0*/ wxTaskBarIcon::IsAvailable() )
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

void wxSMARTmonitorApp::OnStartSrvCnnctnCntDown(wxCommandEvent & event){
  ///Only needs to be done in case connect dialog is not already shown.
  wxGetApp().DisableSrvUIctrls();
  if(! m_p_cnnctToSrvDlg){
    ShwCnnctToSrvrDlg(m_stdstrServiceHostName);
  }
  else{
    m_p_cnnctToSrvDlg->EndCnnctnAttemptTimer();
    m_p_cnnctToSrvDlg->StartSrvCnnctnCntDown();
  }
}

void wxSMARTmonitorApp::OnStartServiceConnectionCountDown(
  wxCommandEvent & event)
{
//  m_wxtimer.Start(1000);
  if(m_p_cnnctToSrvDlg)
    m_p_cnnctToSrvDlg->StartSrvCnnctnAttmptCntDown(event.GetInt() );
}

void wxSMARTmonitorApp::StartSrvCnnctnAttmptCntDown(
  const fastestUnsignedDataType countDownInSeconds)
{
  if(OperatingSystem::GetCurrentThreadNumber() == s_UIthreadID)
  {
//    m_wxtimer.Start(1000);
    if(m_p_cnnctToSrvDlg)
      m_p_cnnctToSrvDlg->StartSrvCnnctnAttmptCntDown(countDownInSeconds);
  }
  else
  {
    wxCommandEvent startServiceConnectionCountDown(
      StartServiceConnectionCountDownEventType);
    startServiceConnectionCountDown.SetInt(countDownInSeconds);
    wxPostEvent(this, startServiceConnectionCountDown);
  }
}

void wxSMARTmonitorApp::ReBuildUserInterface(){
  LOGN_DEBUG("begin")
  //SetSMARTdriveID();
//  SetSMARTattribIDandNameLabel();
  gs_dialog->ReBuildUserInterface();
}

void wxSMARTmonitorApp::ShowStateAccordingToSMARTvalues(
  const SMARTvalueRating entireSMARTvalRating)
{
  switch(entireSMARTvalRating){
   case SMARTvalueWarning:
    ShowSMARTwarningIcon();
    break;
  case SMARTvalueOK:
    ShowSMARTokIcon();
    break;
  }
}

/** Usually called via wxWidgets events (from another thread) to run in user 
 * interface thread.*/
void wxSMARTmonitorApp::OnAfterConnectToServer(wxCommandEvent & commandEvent)
{
  ///May be "errno" from calling "connect" or "select"
  int connectResult = commandEvent.GetInt();
  if(connectResult == 0)
    m_srvrCnnctnState = connectedToService;
  else
    m_srvrCnnctnState = unconnectedFromService;
  //TODO The following could go into a "AfterCnnctToSrvInUIthread" function
  // usable by all subclasses of SMARTmonitorClient.
  if( connectResult == /*connectedToService*/ 0)
  {
//    connectedToSrv();
    if(m_p_cnnctToSrvDlg)
    {
      m_p_cnnctToSrvDlg->End();///Only close connect dialog if connected
      m_p_cnnctToSrvDlg = NULL;
    }
//    SuccessfullyConnectedToClient();
#if execGetSMARTvalsAndUpd8UIinUIthread
    GetSMARTvaluesAndUpdateUI();
#else
    /*m_GetSMARTvalsAndUpd8UIthread*/m_updateSMARTparameterValuesThread.start(
      GetSMARTvaluesAndUpdateUIthreadFn,
      /** Need to upcast to class SMARTmonitorClient else the pointer casted to
       * class SMARTmonitorClient from "void *" parameter in SMARTmonitorClient
       * ::GetSMARTvaluesAndUpdateUIthreadFn is wrong. */
      (SMARTmonitorClient *)this);
#endif
//      StartServiceConnectionCountDown(countDownInSeconds);
    gs_dialog->EnableServerInteractingControls(connectResult);
  }
  else
  {
    /** If not closing the socket then socket file descriptor number increases?*/
    close(m_socketFileDesc);
    if(m_p_cnnctToSrvDlg)
      m_p_cnnctToSrvDlg->EndCnnctnTimeoutTimer();
    gs_dialog->EnableServerInteractingControls(connectResult);
    HandleConnectionError("", connectResult);
//    gs_dialog->StartCountDown(countDownInSeconds);
//    m_wxtimer.StartOnce(countDownInSeconds * 1000);
    StartSrvCnnctnAttmptCntDown(m_srvCnnctnCntDownInSec);
  }
}

void wxSMARTmonitorApp::OnChangeState(wxCommandEvent & commandEvent)
{
  ChangeConnectionState((enum serverConnectionState) commandEvent.GetInt());
}

void wxSMARTmonitorApp::OnCnnctToSrvr(wxCommandEvent & commandEvent)
{
  CnnctToSrvAndGetSMARTvals(asynCnnct);
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
  LOGN_DEBUG("begin")
  /** This function is usually called from a non-GUI thread. So we have to send
   *  an event to let the GUI update happen in the UI thread to avoid a program 
   * crash. */
  wxCommandEvent UpdateSMARTvaluesEvent( UpdateSMARTparamValsInGUIevtType );
  wxPostEvent(gs_dialog, UpdateSMARTvaluesEvent);
}

void wxSMARTmonitorApp::ChangeConnectionState(enum serverConnectionState newState)
{
  //TODO ensure to/must be called in GUI thread
  if(OperatingSystem::GetCurrentThreadNumber() == s_UIthreadID)
    gs_dialog->SetState(newState);
  else{
    /** Create event To execute UI operations in UI thread.
     * https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#The_Normal_Case */
    wxCommandEvent changeStateEvent(ChangeStateEvtType);
    changeStateEvent.SetInt(newState);
    wxPostEvent(this, changeStateEvent);
  }
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
  { //TODO accessing "argv[]" and converting to std::wstring be too inefficient.
    m_ar_stdwstrCmdLineArgs[index] = wxWidgets::GetStdWstring_Inline(
      ///"wxAppConsoleBase.argv" has type "wxCmdLineArgsArray" if wxUSE_UNICODE
      /// is defined (in wxWidgets 3.0?) wxCmdLineArgsArray::operator [] returns
      /// a wxString from wxArrayString::operator []
      argv[/**wxCmdLineArgsArray's operator []" has parameter type "size_t" or
       * "int", so cast.*/ (size_t) index].c_str()
      );
    LOGN( (index+1) << ". program argument:" << m_ar_stdwstrCmdLineArgs[index])
    m_cmdLineArgStrings[index] = m_ar_stdwstrCmdLineArgs[index].c_str();
  }
  m_commandLineArgs.Set(argc, (wchar_t **) m_cmdLineArgStrings);
}

void wxSMARTmonitorApp::OnTimer(wxTimerEvent& event)
{
//  if( m_serverConnectionState = connectedToService)
//  else
  wxString wxstrServiceHostName = m_stdstrServiceHostName;
  if(m_srvCnnctnCntDownInSec --)
  {
    /** Create title as local variable for easier debugging. */
    wxString status = wxString::Format(
      wxT("conn. attempt to \"%s\",port %u in %u s"),
      wxstrServiceHostName.c_str(), 
      m_socketPortNumber, 
      m_srvCnnctnCntDownInSec);
    gs_dialog->SetStatus(status);
  }
  else
  {
    //TODO move status line creation to base class SMARTmonitorClient
    wxString status = wxString::Format(
      wxT("conn. attempt to \"%s\",port %u"),
      wxstrServiceHostName.c_str(), 
      m_socketPortNumber);
    gs_dialog->SetStatus(status);
    m_wxtimer.Stop();
    CnnctToSrvAndGetSMARTvals(asynCnnct);
    if(m_p_cnnctToSrvDlg)
      m_p_cnnctToSrvDlg->StartSrvCnnctnCntDown();
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
  gs_dialog = new SMARTdialog(wxT("wxS.M.A.R.T. monitor")//,
    //m_wxSMARTvalueProcessor
    /*m_SMARTvalueProcessor*/);
  gs_dialog->Show(true);
  GetSMARTokayIcon(s_SMARTokIcon);
  GetSMARTstatusUnknownIcon(s_SMARTstatusUnknownIcon);
  GetSMARTwarningIcon(s_SMARTwarningIcon);  

  LogLevel::CreateLogLevelStringToNumberMapping();
  ProcessCommandLineArgs(); /** May display messages. */
  const bool succInitedLogger = InitializeLogger();
  if(! succInitedLogger)
//TODO Crashes at ntdll!RtlImageDirectoryEntryToData ()
//    ntdll!RtlEnterCriticalSection ()
//    OperatingSystem::Windows::CriticalSection::Enter 
//  (this=<error reading variable: Cannot access memory at address 0x10>)
//  after returning from here.
    return false;
  LOGN_DEBUG("Logger initialized")///For testing if logging is running.
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
    m_cfgLoader.ReadServiceConnectionSettings(
      stdwstrServiceConnectionConfigFile );

//    ShowSMARTokIcon();
    ShowSMARTstatusUnknownIcon();
    bool drctSMARTaccess = false;
    if( ! stdwstrServiceConnectionConfigFile.empty() )
    {
//      m_wxtimer.StartOnce(1000);
      ///Execute after OnInit() finished to enable cancelling in GUI?
      wxCommandEvent cnnctToSrvrEvt(CnnctToSrvrEvtType );
      wxPostEvent(this, cnnctToSrvrEvt);
    }
#ifdef directSMARTaccess
    else
      if( initSMARTresult == SMARTaccessBase::success)
    {
      EnsureSMARTattrToObsExist();
      gs_dialog->ReBuildUserInterface();
      //TODO execute after OnInit(): else no dialog is shown?
      gs_dialog->StartAsyncDrctUpd8Thread();
      drctSMARTaccess = true;
    }
#endif
    if(drctSMARTaccess == false)
      if(initSMARTresult == accessToSMARTdenied)
        gs_dialog->disableDrctSMARTaccss(wxT("access to SMART denied\n"
          "(maybe due to insufficient rights\n"
          "--start as administrator to enable)"));
      else
        gs_dialog->disableDrctSMARTaccss(wxT("no built-in direct SMART access"));
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

void wxSMARTmonitorApp::DisableSrvUIctrls(){
  gs_dialog->m_p_ConnectAndDisconnectButton->Enable(false);
}

void wxSMARTmonitorApp::EnableSrvUIctrls(){
  gs_dialog->m_p_ConnectAndDisconnectButton->Enable(true);
}

inline void createIconFilePath(wxString & iconFilePath, const wxString &
  iconFileName)
{
  if(iconFilePath.size() > 0 && iconFilePath.Last() != wxFILE_SEP_PATH)
    iconFilePath += wxFILE_SEP_PATH;
  iconFilePath += wxString(wxT("icons") ) + wxFILE_SEP_PATH + iconFileName;
}

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
    iconFileName += wxT("ico");
  #else //#ifdef __WXGTK__ //wxGTK
    wxbitmapType = wxBITMAP_TYPE_XPM;
    iconFileName += wxT("xpm");
  #endif

  ///https://forums.wxwidgets.org/viewtopic.php?t=1696
  wxLogNull noDbgMsgs;///Disable wxWidgets (debug) messages when loading file.

  bool bIconFileSuccessfullyLoaded = false;
  wxString defaultIconFilePath;
#ifdef __linux__
  ///Path after installing via deb package manager.
  //TODO maybe pass this path from cmake to also use in createDebPkg.cmake
  defaultIconFilePath = //wxT("/usr/local/SMARTmonitor");
    /** Use a preprocessor macro in order to use the same value for creating
     *  Debian package.
     *  Pass "-DresourcesFSpath=STRING:>>resourcesFSpath<< when using CMake.
     *  Needs preprocessor macro with name "resourcesFSpath".
     * Get value for preprocessor macro and make wxT() literal of it. */
    wxSTRINGIZE(resourcesFSpath);
#else
  defaultIconFilePath = wxGetCwd();
#endif
  createIconFilePath(defaultIconFilePath, iconFileName);
  bIconFileSuccessfullyLoaded = icon.LoadFile(
    defaultIconFilePath,
    wxbitmapType);
  if(! bIconFileSuccessfullyLoaded)
  {
    /**The 1st program argument Is not always the _full_ path of the executable
     * ?, e.g. no directory when in "PATH" environment variable?
     * Alternative: OperatingSystem::Process::GetCurrExePath(); */
    std::wstring exeFilePath = m_commandLineArgs.GetProgramPath();
    std::wstring exeFileDir = exeFilePath.substr(0,
      exeFilePath.rfind(FileSystem::dirSeperatorCharW) +1);
    wxString iconFilePathFromExe = getwxString_inline(exeFileDir);
    createIconFilePath(iconFilePathFromExe, iconFileName);

    bIconFileSuccessfullyLoaded = icon.LoadFile(iconFilePathFromExe, wxbitmapType);
    if(! bIconFileSuccessfullyLoaded){
      wxMessageBox( wxT("Loading icon file(s)\n-\"") +
        defaultIconFilePath + wxT("\"\n-\"") + iconFilePathFromExe +
        wxT("\"\n failed") );
      ///Loading a (custom) icon from file failed, so provide a pre-defined one.
      icon = wxIcon(inMemoryIcon);
    }
  }
  return bIconFileSuccessfullyLoaded;
}

//TODO better call this function only once at startup?!
bool wxSMARTmonitorApp::GetSMARTokayIcon(wxIcon & icon)
{
  return GetIcon(icon, wxT("S.M.A.R.T._OK."), S_M_A_R_T__OK_xpm);
}

bool wxSMARTmonitorApp::GetSMARTstatusUnknownIcon(wxIcon & icon)
{
  return GetIcon(icon, wxT("S.M.A.R.T._unknown."), S_M_A_R_T__unknown_xpm);
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
  const std::string & std_strValue,
  const enum SMARTvalueRating sMARTvalueRating,
  void * data/** Which list control to use (for supported S.M.A.R.T. IDs dialog
    it is non-NULL */
  )
{
  wxString wxstrValue = wxWidgets::GetwxString_Inline(std_strValue );
  //TODO: Maybe implement this in class wxWidgets::SMARTtableListCtrl as this 
  //should avoid 2 times a pointer dereference -> performs better or in 
  //SMARTmonitorDialog as param. "sMARTuniqueID" needs to be taken into account
  
  if(OperatingSystem::GetCurrentThreadNumber() != s_UIthreadID){
    SMARTattrs _SMARTattrs(
      sMARTuniqueID,
      SMARTattributeID,
      columnIndex,
      std_strValue,
      sMARTvalueRating,
      data//,
      //SetSMARTattrEventType
      );
    SetSMARTattrEvent event(_SMARTattrs);
    wxPostEvent(this, event);
  }
  else{
  wxWidgets::SMARTtableListCtrl * wxSMARTtableListCtrl;
  if(data == NULL){
    PerDataCarrierPanel * perDataCarrierPanel = gs_dialog->
      m_SMARTuniqueID2perDataCarrierPanel[sMARTuniqueID];
    wxSMARTtableListCtrl = perDataCarrierPanel->m_pwxlistctrl;
  }
  else
    wxSMARTtableListCtrl = (wxWidgets::SMARTtableListCtrl *) data;
  wxSMARTtableListCtrl->SetSMARTattribValue(
    /*lineNumber*/ SMARTattributeID, //long index
    columnIndex /** column #/ index */,
    wxstrValue,
    sMARTvalueRating);
  }
}

void wxSMARTmonitorApp::SetGetDirectSMARTvals()
{
  wxString status = wxString::Format(
    wxT("direct S.M.A.R.T. values in %u ms interval"),
    s_numberOfMilliSecondsToWaitBetweenSMARTquery);
  gs_dialog->SetStatus(status);
  SetGetSMARTvalsMode(SMARTmonitorBase::directSMARTvals);
}

void wxSMARTmonitorApp::SetGetSMARTvalsMode(const enum GetSMARTvalsMode mode)
{
  m_getSMARTvalsMode = mode;
  switch(mode){
   case directSMARTvals :
//    gs_dialog->DisableGetSMARTvalsDrctly();
     break;
  }
}

void wxSMARTmonitorApp::ShowConnectionState(const char * const pchServerAddress, 
  int connectTimeOutInSeconds)
{
#ifdef _DEBUG
  int currentThreadNumber = OperatingSystem::GetCurrentThreadNumber();
  if(currentThreadNumber = s_UIthreadID)
    ;
#endif
//  m_p_cnnctToSrvDlg = new ConnectToServerDialog(
//    pchServerAddress, m_socketPortNumber, connectTimeOutInSeconds,
//    m_socketFileDesc);
  //TODO dialog appears in foreground
//  m_p_cnnctToSrvDlg->Show();
//  m_pConnectToServerDialog->ShowWindowModal();
  /** Ensures "connect" can't be pressed a second time */
//  m_pConnectToServerDialog->ShowModal();
}

void wxSMARTmonitorApp::ShwCnnctToSrvrDlg(const std::string & srvAddr){
  if(! m_p_cnnctToSrvDlg){
    m_p_cnnctToSrvDlg = new ConnectToServerDialog(
      ///Alternative: pass (pointer to) _this_ object and assign in dialog c'tor
      m_stdstrServiceHostName.c_str(),
      m_socketPortNumber,
      m_timeOutInSeconds,
      m_socketFileDesc);
    m_p_cnnctToSrvDlg->Show();
  }
}

void wxSMARTmonitorApp::OnShowCurrentAction(wxCommandEvent & evt)
{
  const enum CurrentAction currAction = (enum CurrentAction) evt.GetInt();
  gs_dialog->ShowCurrentAction(currAction);
}

void wxSMARTmonitorApp::SetCurrentAction(enum CurrentAction currAction)
{
  if(OperatingSystem::GetCurrentThreadNumber() == s_UIthreadID)
    gs_dialog->ShowCurrentAction(currAction);
  else{
    /** To execute in UI thread.
     * https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#The_Normal_Case */
    wxCommandEvent showCurrentActionEvent(ShowCurrentActionEventType);
    showCurrentActionEvent.SetInt(currAction);
    //TODO set thread ID, if blocking or non-blocking connect and show this in UI
    // showCurrentActionEvent.SetClientData SetClientObject
    wxPostEvent(this, showCurrentActionEvent);
  }
}

void wxSMARTmonitorApp::startSrvCnnctCntDown()
{
//  if(m_p_cnnctToSrvDlg){
    wxCommandEvent wxcommand_event(StartCnnctCntDownEvtType);
//    wxPostEvent( (ConnectToServerDialog *) m_p_cnnctToSrvDlg, wxcommand_event);
    wxPostEvent(this, wxcommand_event);
//  }
}

void wxSMARTmonitorApp::OnShowMessage(wxCommandEvent & event)
{
    wxString wxstrMessage = event.GetString();
//    wxMessageBox(wxstrMessage, m_appName );
    gs_dialog->ShowMessage(wxstrMessage, 
      (UserInterface::MessageType::messageTypes) event.GetInt() );
}

void wxSMARTmonitorApp::ShowMessage(
  const char * const message, 
  enum MessageType::messageTypes msgType) const
{
  unsigned currentThreadNumber = OperatingSystem::GetCurrentThreadNumber();
  /** Only call UI functions in UI thread, else gets SIGABORT error when calling
   *  "wxMessageBox(...)" . */
  if(currentThreadNumber == s_UIthreadID)
  {
    wxString wxstrMessage = wxWidgets::GetwxString_Inline(message);
    gs_dialog->ShowMessage(wxstrMessage, msgType);
  }
  else
  {
    /** Post an event with the message as content to show 
    *   this message in the GUI thread */
    wxCommandEvent wxcommand_event(ShowMessageEventType);
    wxcommand_event.SetString(GetwxString_Inline(message));
    wxcommand_event.SetInt(msgType);
    wxPostEvent( (wxSMARTmonitorApp *) this, wxcommand_event);
  }
}

void wxSMARTmonitorApp::ShowMessage(const char * const str) const
{
  unsigned currentThreadNumber = OperatingSystem::GetCurrentThreadNumber();
  /** Only call UI functions in UI thread, else gets SIGABORT error when calling
   *  "wxMessageBox(...)" . */
  if( currentThreadNumber == s_UIthreadID )
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
    wxcommand_event.SetInt(-1);//TODO neccessary?
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
  
  if(m_taskBarIcon///May be NULL if wxTaskBarIcon::IsAvailable() returns false
    && ! m_taskBarIcon->SetIcon(icon, tooltip) )
    wxMessageBox(wxT("Could not set new taskbar icon."), wxT("wxSMARTmonitor") );
  if( gs_dialog)
    gs_dialog->SetIcon(icon);
}

wxIcon wxSMARTmonitorApp::ShowSMARTokIcon()
{
  ShowIcon(s_SMARTokIcon, wxT("values for (all) processed (critical) S.M.A.R.T."
    " parameters are in a safe range") );
  return s_SMARTokIcon;
}

wxIcon wxSMARTmonitorApp::ShowSMARTstatusUnknownIcon()
{
  ShowIcon(s_SMARTstatusUnknownIcon, wxT("unknown S.M.A.R.T. status") );
  return s_SMARTstatusUnknownIcon;
}

wxIcon wxSMARTmonitorApp::ShowSMARTwarningIcon()
{
  ShowIcon(s_SMARTwarningIcon, wxT("value for at least 1 processed (critical) "
    "S.M.A.R.T. parameter is not in a safe range") );
  return s_SMARTwarningIcon;
}
