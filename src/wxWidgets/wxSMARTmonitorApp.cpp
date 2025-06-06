/* wxSMARTmonitorApp.cpp
 * Created on: 26.11.2013
 *  Author:Stefan Gebauer, M. Sc. Comp. Sc.
 *  adapted from the wxWidgets "tbtest" sample  */

/**Prevents <winsock.h> to be included in <windows.h>. <Winsock.h> and
 * <Winsock2.h> have the same definitions.->re-definition happens if <winsock.h>
 * is included before <Winsock2.h> */
#define _WINSOCKAPI_

/** Include at 1st in Windows build to avoid:
 * "#warning Please include winsock2.h before windows.h" */
#include "wxSMARTmonitorApp.hpp"

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
 #include <wx/filefn.h>///wxFILE_SEP_PATH
 #include <wx/log.h>///class wxLogNull
 #include <wx/taskbar.h>///wxTaskBarIcon::IsAvailable()

///Stefan Gebauer's "common_sourcecode" repository header files:
 /**TU_Bln361095GCCenableWarn(...), TU_Bln361095GCCdisableWarn(...) preprocessor
  * macros */
 #include <compiler/C,C++/enableAndDisableWarn.h>
 ///class TU_Bln361095::FileSys::FileException
 #include <FileSystem/File/FileException.hpp>
 ///TU_Bln361095::FileSys::DirSepCharW
 #include <FileSystem/PathSeperatorChar.hpp>
 ///TU_Bln361095::OpSys::GetCurrThreadNum()
 #include <OperatingSystem/multithread/getCurrThreadNum.h>
#ifdef __MINGW32__
#include <OperatingSystem/Windows/HideMinGWconsoleWindow.h>
#endif
 ///LOGN(...),LOGN_DEBUG(...)
 #include <preprocessor_macros/logging_preprocessor_macros.h>
 /**TU_Bln361095::wxWidgets::GetStdString_inln(...),
  * TU_Bln361095::wxWidgets::GetwxString_inln(...) */
 #include <wxWidgets/charStr/wxStringHelper.hpp>

///_This_ project's (repository) (header) files:
 /** Prevent GCC/g++ warning "warning: deprecated conversion from string constant 
  *  to ‘char*’" when including the "xpm" file */
 TU_Bln361095GCCdisableWarn(write-strings)
 #include "../icons/S.M.A.R.T._OK.xpm"///S_M_A_R_T__OK_xpm
 #include "../icons/S.M.A.R.T._unknown.xpm"///S_M_A_R_T__unknown_xpm
 #include "../icons/S.M.A.R.T._warning.xpm"///S_M_A_R_T__warning_xpm
 TU_Bln361095GCCenableWarn(write-strings)
 #include "SetSMARTattrEvent.hpp"///class SetSMARTattrEvent
 #include "ConnectToServerDialog.hpp"///class ConnectToServerDialog
 ///class ConfigurationLoaderBase
 #include <ConfigLoader/ConfigurationLoaderBase.hpp>

///Standard C(++) header files:
#include <iostream> //class std::cerr

// global variables
/*static*/ SMARTdialog * gs_dialog = NULL;

/** definitions of static class members. */
wxIcon wxSMARTmonitorApp::s_SMARTokIcon;
wxIcon wxSMARTmonitorApp::s_SMARTstatusUnknownIcon;
wxIcon wxSMARTmonitorApp::s_SMARTwarningIcon;

/**from
 * http://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#The_Normal_Case
 */
//const wxEventType AfterConnectToServerEventType = wxNewEventType();
///Cnct=connect:http://www.allacronyms.com/connect/abbreviated
///Srv=server:http://www.abbreviations.com/abbreviation/server
///Evt=event:http://www.allacronyms.com/event/abbreviated
DEFINE_LOCAL_EVENT_TYPE(AfterCnctToSrvEvtType)
DEFINE_LOCAL_EVENT_TYPE(ChangeStateEvtType)
DEFINE_LOCAL_EVENT_TYPE(CnnctToSrvrEvtType)
DEFINE_LOCAL_EVENT_TYPE(ShowCurrentActionEventType)
DEFINE_LOCAL_EVENT_TYPE(ShowMessageEventType)
DEFINE_LOCAL_EVENT_TYPE(StartSrvCnnctnAttmptCntDownEvtType)
DEFINE_LOCAL_EVENT_TYPE(StartCnnctCntDownEvtType)

BEGIN_EVENT_TABLE(wxSMARTmonitorApp, wxApp)
#ifdef TU_Bln361095useClntSrv
  EVT_COMMAND(wxID_ANY, AfterCnctToSrvEvtType, wxSMARTmonitorApp::
    OnAfterConnectToServer)
  EVT_COMMAND(wxID_ANY, ChangeStateEvtType, wxSMARTmonitorApp::OnChangeState)
  EVT_COMMAND(wxID_ANY, CnnctToSrvrEvtType, wxSMARTmonitorApp::OnCnnctToSrvr)
#endif
  EVT_COMMAND(wxID_ANY, ShowMessageEventType, wxSMARTmonitorApp::OnShowMessage)
  EVT_COMMAND(wxID_ANY, ShowCurrentActionEventType, wxSMARTmonitorApp::
    OnShowCurrentAction)
#ifdef TU_Bln361095useClntSrv
  EVT_COMMAND(wxID_ANY, StartCnnctCntDownEvtType, wxSMARTmonitorApp::
    OnStartSrvCnnctnCntDown)
  EVT_COMMAND(wxID_ANY, StartSrvCnnctnAttmptCntDownEvtType, 
    wxSMARTmonitorApp::OnStartServiceConnectionCountDown)
  EVT_TIMER(TIMER_ID, wxSMARTmonitorApp ::OnTimer)
#endif
END_EVENT_TABLE()

//const wxString wxSMARTmonitorApp::appName = wxT("wxSMARTmonitor");

wxSMARTmonitorApp::wxSMARTmonitorApp()
  : m_taskBarIcon(NULL) //,
//    m_SMARTaccess(m_wxSMARTvalueProcessor.getSMARTaccess() ),
//    configurationLoader(
//      /*smartAttributesToObserve*/ (SMARTaccessBase::SMARTattributesType &)
//      m_SMARTaccess.getSMARTattributesToObserve(),
//      * this)
#ifdef TU_Bln361095useClntSrv
  , m_p_cnnctToSrvDlg(NULL)
#endif
  , m_wxtimer(this, TIMER_ID)
{
  s_UIthreadID = TU_Bln361095::OpSys::GetCurrThreadNum();
#ifdef TU_Bln361095SMARTmonMultithread
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
    /** Do not show a message at startup because this hinders displaying of the
     *  main window/S.M.A.R.T. OK/warning icon (these is displayed after closing
     *  message window at first). Especially if automatically started at OS boot
     *  this is annoying.*/
  }
#else
  wxGetApp().m_taskBarIcon = new TaskBarIcon();
#endif
}

void wxSMARTmonitorApp::ReBuildUserInterface(){
  LOGN_DEBUG("begin")
  //SetSMARTdriveID();
//  SetSMARTattribIDandNameLabel();
  gs_dialog->ReBuildUserInterface();
}

void wxSMARTmonitorApp::ShowStateAccordingToSMARTvalues(
  const SMARTvals::Rating::E entireSMARTvalRating)
{
  switch(entireSMARTvalRating){
   case SMARTvals::Rating::atLeast1Warn:
    ShowSMARTwarningIcon();
    break;
   case SMARTvals::Rating::OK:
    ShowSMARTokIcon();
    break;
  }
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
  if(TU_Bln361095::OpSys::GetCurrThreadNum() == s_UIthreadID){
    gs_dialog->SetState(newState);
    m_srvrCnnctnState = newState;
  }
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
  const wxString wxstrLabel = TU_Bln361095::wxWidgets::GetwxString_inln(label);
  const wxString defaultValue = TU_Bln361095::wxWidgets::GetwxString_inln(
    std_strValue);
  wxString wxstrText = wxGetTextFromUser(
    wxstrLabel //message,
    , wxGetTextFromUserPromptStr// const wxString & 	caption = wxGetTextFromUserPromptStr,
    , defaultValue //const wxString & 	default_value = wxEmptyString,
    , NULL //wxWindow * 	parent = NULL,
    );
  std_strValue = TU_Bln361095::wxWidgets::GetStdString_inln(wxstrText);
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
    m_ar_stdwstrCmdLineArgs[index] = TU_Bln361095::wxWidgets::GetStdWstring_inln(
      ///"wxAppConsoleBase.argv" has type "wxCmdLineArgsArray" if wxUSE_UNICODE
      /// is defined (in wxWidgets 3.0?) wxCmdLineArgsArray::operator [] returns
      /// a wxString from wxArrayString::operator []
      argv[/**wxCmdLineArgsArray's operator []" has parameter type "size_t" or
       * "int", so cast.*/ (size_t) index].c_str()
      );
    LOGN( (index+1) << ". program argument:" << m_ar_stdwstrCmdLineArgs[index])
    m_cmdLineArgStrings[index] = m_ar_stdwstrCmdLineArgs[index].c_str();
  }
  m_commandLineArgs.set(argc, (wchar_t **) m_cmdLineArgStrings);
}

/** http://docs.wxwidgets.org/trunk/classwx_app_console.html#a99953775a2fd83fa2456e390779afe15 : 
 *  "This must be provided by the application, and will usually create the 
 *  application's main window, optionally calling SetTopWindow()."
 *  @return "Return true to continue processing, false to exit the application
 *    immediately." */
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

  const fastestUnsignedDataType prcssCmdLneArgsRtrnVal =
    ProcessCommandLineArgs(); /** May display messages. */
  if(prcssCmdLneArgsRtrnVal == SMARTmonitorBase::calledHelp)
    return false;
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
      //TODO Hides a "CMD" console although not started by this application?
      HideMinGWconsoleWindow();
  #endif
    }
//    else
//    {
//      ShowMessage("0 SMART parameters to read -> exiting");
//      return false;
//    }
    const fastestUnsignedDataType initSMARTresult = InitializeSMART();
    /**Ensure read config error message is shown (and not overwritten by another
     * message)*/
    if(initSMARTresult != readingConfigFileFailed)
    {
    std::wstring stdwstrServiceConnectionConfigFile = 
      s_programOptionValues[serviceConnectionConfigFile];
    std::string errorMsg;
    m_cfgLoader.ReadServiceConnectionSettings(
      stdwstrServiceConnectionConfigFile, errorMsg);
    if(errorMsg != "")
      ShowMessage(errorMsg.c_str () );

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
#ifdef TU_Bln361095SMARTmonDrctSMARTaccss
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
        gs_dialog->disableDrctSMARTaccss(wxT("direct (without client/server)\n"
          "access to S.M.A.R.T. denied\n"
          "(maybe due to insufficient rights\n"
          "--start as administrator to enable)"));
      else
        gs_dialog->disableDrctSMARTaccss(wxT("no built-in direct SMART access"));
//    else if( result == SMARTaccessBase::noSingleSMARTdevice )
//      return false;
    }
  }
  /** Thrown by mp_configurationLoader->LoadConfiguration(..:) */
  catch(const TU_Bln361095::FileSys::FileException & fileExc)
  {
    TCHAR * filePath = fileExc.GetFilePath();
    wxMessageBox(wxT("failed to open file: ") + TU_Bln361095::wxWidgets::
     GetwxString_inln(
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

inline void createIconFilePath(wxString & iconFilePath, const wxString &
  iconFileName)
{
  if(iconFilePath.size() > 0 && iconFilePath.Last() != wxFILE_SEP_PATH)
    iconFilePath += wxFILE_SEP_PATH;
  iconFilePath += wxString(wxT("icons") ) + wxFILE_SEP_PATH + iconFileName;
}

/**@param iconFileName in: without file name extension
 *   out: with file name extension */
bool wxSMARTmonitorApp::GetIcon(
  wxIcon & icon,
  wxString & iconFileName,
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
    std::wstring exeFilePath = m_commandLineArgs.getProgPath();
    std::wstring exeFileDir = exeFilePath.substr(0,
      exeFilePath.rfind(TU_Bln361095::FileSys::DirSepCharW) +1);
    wxString iconFilePathFromExe = TU_Bln361095::wxWidgets::GetwxString_inln(
      exeFileDir);
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
  const enum colIndices::columnIndices & columnIndex,
  const std::string & std_strValue,
  const SMARTvalRatngTyp SMARTvalRatng,
  void * data/** Which list control to use (for supported S.M.A.R.T. IDs dialog
    it is non-NULL */
  )
{
  wxString wxstrValue = TU_Bln361095::wxWidgets::GetwxString_inln(std_strValue);
  //TODO: Maybe implement this in class wxWidgets::SMARTtableListCtrl as this 
  //should avoid 2 times a pointer dereference -> performs better or in 
  //SMARTmonitorDialog as param. "sMARTuniqueID" needs to be taken into account
  
  if(TU_Bln361095::OpSys::GetCurrThreadNum() != s_UIthreadID){
    SMARTattrs _SMARTattrs(
      sMARTuniqueID,
      SMARTattributeID,
      columnIndex,
      std_strValue,
      SMARTvalRatng,
      data//,
      //SetSMARTattrEventType
      );
    SetSMARTattrEvent event(_SMARTattrs);
    wxPostEvent(this, event);
  }
  else{
  wxWidgets::SMARTtableListCtrl * wxSMARTtableListCtrl;
  if(data == NULL){
#ifdef _DEBUG
    SMARTdialog::SMARTuniqueID2perDataCarrierPanelType &
      sMARTuniqueID2perDataCarrierPanel = gs_dialog->
      m_SMARTuniqueID2perDataCarrierPanel;
#endif
    PerDataCarrierPanel * perDataCarrierPanel = gs_dialog->
      m_SMARTuniqueID2perDataCarrierPanel[sMARTuniqueID];
    //TODO CrasheD here (pointer is NULL) after connected->cancel server
    // connection->connect to other server. Maybe because the Unique IDs here.
    // (2 elements, but nut [...] && known where the 2nd is inserted)
    // Make a (unit) test for this?
    wxSMARTtableListCtrl = perDataCarrierPanel->m_pwxlistctrl;
  }
  else
    wxSMARTtableListCtrl = (wxWidgets::SMARTtableListCtrl *) data;
  wxSMARTtableListCtrl->SetSMARTattribValue(
    /*lineNumber*/ SMARTattributeID, //long index
    columnIndex /** column #/ index */,
    wxstrValue,
    SMARTvalRatng);
  }
}

void wxSMARTmonitorApp::SetGetDirectSMARTvals()
{
  wxString status = wxString::Format(
    wxT("direct S.M.A.R.T. values in %u ms interval"),
    s_numberOfMilliSecondsToWaitBetweenSMARTquery);
  gs_dialog->SetStatus(status);
  setUI(drctSMARTaccss);
//  SetGetSMARTvalsMode(SMARTmonitorBase::directSMARTvals);
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

/** @param srvCnnctnState the User Interface (UI controls and dialogs) to set.*/
void wxSMARTmonitorApp::setUI(const enum serverConnectionState srvCnnctnState)
{
  switch(srvCnnctnState){
#ifdef TU_Bln361095useClntSrv
   case SMARTmonitorClient::cnnctdToSrv:
    gs_dialog->setUI(srvCnnctnState);
    break;
   case SMARTmonitorClient::connectToSrv:
     ///Disable because "connect to server" dialog is shown in "ConnectToServer"
     gs_dialog->DisableCnnctAndDiscnnctBtn();
    break;
   case SMARTmonitorClient::uncnnctdToSrv :
    if(wxGetApp().m_p_cnnctToSrvDlg)
      wxGetApp().m_p_cnnctToSrvDlg->EnableCnnctBtn();
    else///Enable "Connect..." button if connect to server dialog is not shown.
      gs_dialog->UnCnnctdToSrvUIctrls();
    break;
#endif
   case SMARTmonitorClient::drctSMARTaccss:
    gs_dialog->DrctSMARTaccssUIctrls();
    break;
   case SMARTmonitorClient::endedDrctSMART:
    gs_dialog->NoDrctSMARTaccessUIctrls();
    break;
  }
}

void wxSMARTmonitorApp::ShowConnectionState(const char * const pchServerAddress, 
  int connectTimeOutInSeconds)
{
#ifdef _DEBUG
  int currentThreadNumber = TU_Bln361095::OpSys::GetCurrThreadNum();
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

void wxSMARTmonitorApp::OnShowCurrentAction(wxCommandEvent & evt)
{
  const enum CurrentAction currAction = (enum CurrentAction) evt.GetInt();
  gs_dialog->ShowCurrentAction(currAction);
}

void wxSMARTmonitorApp::SetCurrentAction(enum CurrentAction currAction)
{
  if(TU_Bln361095::OpSys::GetCurrThreadNum() == s_UIthreadID)
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
  const TU_Bln361095::OpSys::ThreadNumTyp currThreadNum = TU_Bln361095::OpSys::
    GetCurrThreadNum();
  /** Only call UI functions in UI thread, else gets SIGABORT error when calling
   *  "wxMessageBox(...)" . */
  if(currThreadNum == s_UIthreadID)
  {
    wxString wxstrMessage = TU_Bln361095::wxWidgets::GetwxString_inln(message);
    gs_dialog->ShowMessage(wxstrMessage, msgType);
  }
  else
  {
    /** Post an event with the message as content to show 
    *   this message in the GUI thread */
    wxCommandEvent wxcommand_event(ShowMessageEventType);
    wxcommand_event.SetString(TU_Bln361095::wxWidgets::GetwxString_inln(message));
    wxcommand_event.SetInt(msgType);
    wxPostEvent( (wxSMARTmonitorApp *) this, wxcommand_event);
  }
}

//TODO to avoid double code/for unification better call ShowMessage(
// const char * const str, MessageType::info) than to implement the same!?
void wxSMARTmonitorApp::ShowMessage(const char * const str) const
{
  const TU_Bln361095::OpSys::ThreadNumTyp currThreadNum = TU_Bln361095::
    OpSys::GetCurrThreadNum();
  /** Only call UI functions in UI thread, else gets SIGABORT error when calling
   *  "wxMessageBox(...)" . */
  if(currThreadNum == s_UIthreadID)
  {
    wxString wxstrMessage = TU_Bln361095::wxWidgets::GetwxString_inln(str);
//    wxMessageBox(wxstrMessage, m_appName );
    gs_dialog->ShowText(wxstrMessage);
  }
  else
  {
    /** Post an event with the message as content to show 
    *   this message in the GUI thread */
    wxCommandEvent wxcommand_event(ShowMessageEventType);
    wxcommand_event.SetString(TU_Bln361095::wxWidgets::GetwxString_inln(str));
    wxcommand_event.SetInt(-1);//TODO neccessary?
    wxPostEvent( (wxSMARTmonitorApp *) this, wxcommand_event);
  }
}

void wxSMARTmonitorApp::ShowIcon(const wxIcon & icon, const wxString & message )
{
  wxString tooltip;
#ifdef TU_Bln361095SMARTmonDrctSMARTaccss
  if(/*serviceLocation == wxT("")*/ getsSMARTdataDrctly() )
    tooltip = wxString::Format(wxT("data via direct (local) SMART access:\n%s"),
      message);
  else
#endif
  {
    const wxString serviceLocation = TU_Bln361095::wxWidgets::GetwxString_inln(
      m_stdstrServiceHostName);
    /** Port number is relevant as e.g. with port forwarding done in a router
     * multiple computers/hosts may be addressable. */
    tooltip = wxString::Format( wxT("for %s, port %u:\n%s"), 
      serviceLocation, m_socketPortNumber, message );
  }
  if(m_taskBarIcon///May be NULL if wxTaskBarIcon::IsAvailable() returns false
    && ! m_taskBarIcon->SetIcon(icon, tooltip) )
    wxMessageBox(wxT("Could not set new taskbar icon."), wxT("wxSMARTmonitor") );
  if( gs_dialog)
    gs_dialog->SetIcon(icon);
}

wxIcon wxSMARTmonitorApp::ShowSMARTokIcon()
{
  ShowIcon(s_SMARTokIcon, wxT("values for (all) processed (critical) S.M.A.R.T."
    " parameters "
    //TODO display all data carriers (model, firmware, serial number) here?
    "are in a safe range") );
  return s_SMARTokIcon;
}

wxIcon wxSMARTmonitorApp::ShowSMARTstatusUnknownIcon()
{
  ShowIcon(s_SMARTstatusUnknownIcon, wxT("unknown (current) S.M.A.R.T. status")
    );
  return s_SMARTstatusUnknownIcon;
}

wxIcon wxSMARTmonitorApp::ShowSMARTwarningIcon()
{
  ShowIcon(s_SMARTwarningIcon, wxT("value for at least 1 processed (critical) "
    "S.M.A.R.T. parameter is not in a safe range") );
  return s_SMARTwarningIcon;
}
