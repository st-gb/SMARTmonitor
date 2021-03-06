/* Author: sg
 * Created on 11. Januar 2017, 14:30 */
#include "NcursesUI.hpp" //this class's header file
#include "MainWindow.hpp" //class Curses::MainWindow
#include "Controller/character_string/stdtstr.hpp"
#include "OperatingSystem/multithread/GetCurrentThreadNumber.hpp"
#include <libraries/curses/EventQueue.hpp> //class Curses::EventQueue
//#include <menu.h>
//#include <libraries/ncurses/form/Form.hpp>
#include <libraries/curses/textfield.h> //editText(...))
#include <libraries/curses/MessageBox.hpp> //class Ncurses::MessageBox
#include <libraries/curses/color.h> //A_ATTR
#include <ConfigLoader/ConfigurationLoaderBase.hpp> //class ConfigurationLoaderBase

#define NUM_MENU_BAR_LINES 1

/** Adapted from PDcurses3.4/demos/tui.c : */
#define BODY_WINDOW_COLOR 6
#define BUTTON_COLOR      5
#define STATUS_BAR_COLOR  3
#define SMART_ATTRIBUTE_COLOR 1
#define ATTR_INVERTED_COLOR   2
#define GREEN_ON_BLACK 7
#define BLACK_ON_GREEN 8
#define RED_ON_BLACK 9
#define SMART_VALUE_OK GREEN_ON_BLACK
#define SMART_VALUE_OK_INVERTED BLACK_ON_GREEN
#define SMART_VALUE_WARNING RED_ON_BLACK
//#define A_ATTR  (A_ATTRIBUTES ^ A_COLOR)  /* A_BLINK, A_REVERSE, A_BOLD */

/** Definitions of static variables. */
Curses::Menu NcursesUI::s_mainMenu, NcursesUI::s_programMenu, 
  NcursesUI::s_serverMenu;
//Ncurses::Form NcursesUI::s_connToSrvForm;
struct _win_st * NcursesUI::s_menuBar, * NcursesUI::s_bodyWindow, 
  * NcursesUI::s_statusBar;
NcursesUI * NcursesUI::s_p_NcursesUI;
fastestUnsignedDataType NcursesUI::s_GUIthreadID;
//static NcursesUI gs_ncursesUI;

extern Curses::EventQueue g_eventQueue;

NcursesUI::NcursesUI(const int argumentCount, char * argumentVector [] )
  : SMARTmonitorClient() //Call base class constructor.
  , s_connectionStatusWindow(/*1,2*/)
  , m_lastLineNumber(0)
//    m_cmdLineArgStrings(argumentVector)
{
  CreateCommandLineArgsArrays(argumentCount, argumentVector);
//  m_commandLineArgs.Set(argumentCount, (wchar_t **) argumentVector);
  s_p_NcursesUI = this;
  CreateUI();
}

//NcursesUI::NcursesUI(const NcursesUI& orig) {
//}

NcursesUI::~NcursesUI() {
  endwin(); /** End curses mode. */
}

void NcursesUI::CreateCommandLineArgsArrays(
  const int argumentCount, char * argumentVector [] )
{
//  LOGN("number of program arguments passed:" << argumentCount)
  m_cmdLineArgStrings = new const wchar_t * [argumentCount];
  m_ar_stdwstrCmdLineArgs = new std::wstring[argumentCount];
  //TODO move to "common_sourcecode"
  for(fastestUnsignedDataType index = 0; index < argumentCount; ++index)
  {
    m_ar_stdwstrCmdLineArgs[index] = GetStdWstring(argumentVector[index]);
//    LOGN( (index+1) << ". program argument:" << m_ar_stdwstrCmdLineArgs[index])
    m_cmdLineArgStrings[index] = m_ar_stdwstrCmdLineArgs[index].c_str();
  }
  m_commandLineArgs.Set(argumentCount, (wchar_t **) m_cmdLineArgStrings);
}

void NcursesUI::Exit()
{
  /** Force program exit. */
  s_mainMenu.SetEnterLeavesMenu(true);
}

void NcursesUI::ServerAddressForm()
{
//  Ncurses::Form form;
//  form.addField(4);
//  form.CreateForm(s_bodyWindow);
//  form.InsideForm(); 
  s_p_NcursesUI->SMARTmonitorClient::ConnectToServer();
}

void NcursesUI::GetTextFromUser(const char * label, std::string & std_str )
{
  int oldYCursorPosOfBodyWindow, oldXcursorPosOfBodyWindow;
  getyx(NcursesUI::s_bodyWindow, oldYCursorPosOfBodyWindow, 
    oldXcursorPosOfBodyWindow);
  const int bodyWindowWidth = getmaxx(NcursesUI::s_bodyWindow);
  int yBeginOfBodyWindow, xBeginOfBodyWindow;
  getbegyx(NcursesUI::s_bodyWindow, yBeginOfBodyWindow, xBeginOfBodyWindow);
  
  const int textFieldCharWidth = 29;
  int windowHeight = 5, maxLineWidth = strlen(label) + textFieldCharWidth;
//  const int windowHeight = numLines + /** box chars + line for OK button*/ 3;
  /** If window was created with derwin(...) or subwin(...) it isn't cleared 
   *  from screen after wdelete(...). So use newwin(...) instead. */
  WINDOW * p_MessageWindow = newwin(
//      windowToShowMessageIn, 
    windowHeight , 
    bodyWindowWidth, /** y begin*/ 1 , /** first column */ 0 );
  colorBox(p_MessageWindow, BODY_WINDOW_COLOR, 1);
  mvwaddstr(p_MessageWindow, 1, 1, label);
  
  if( editText(p_MessageWindow, std_str, 29) == '\n')
    ;
  delwin(p_MessageWindow);
  touchwin(NcursesUI::s_bodyWindow);
//    wmove(windowToShowMessageIn, oldYCursorPosOfBodyWindow, oldXcursorPosOfBodyWindow);
  wrefresh(NcursesUI::s_bodyWindow);
}

void NcursesUI::ConnectToServer()
{
  ServerAddressForm();
}

/*static*/ void initColor(void)
{
#ifdef A_COLOR
  if (has_colors())
      start_color();

  /** init_pair(...) (ncurses):parameters
   * 1. number of colour pair,
   * 2. foreground colour number,
   * 3. background colour number
   *   "& ~A_ATTR" : remove possible attributes like "A_BOLD" */
  init_pair(BODY_WINDOW_COLOR    & ~A_ATTR, COLOR_WHITE, COLOR_BLUE);
  init_pair(STATUS_BAR_COLOR    & ~A_ATTR, COLOR_WHITE, COLOR_CYAN);
  init_pair(EDITBOXCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
  init_pair(SMART_ATTRIBUTE_COLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
  init_pair(ATTR_INVERTED_COLOR & ~A_ATTR, COLOR_BLACK, COLOR_WHITE);
  init_pair(SMART_VALUE_OK & ~A_ATTR, COLOR_RED, COLOR_WHITE);
  init_pair(GREEN_ON_BLACK & ~A_ATTR, COLOR_GREEN, COLOR_BLACK);
  init_pair(BLACK_ON_GREEN & ~A_ATTR, COLOR_BLACK, COLOR_GREEN);
  init_pair(BUTTON_COLOR & ~A_ATTR, COLOR_WHITE, COLOR_RED);
#endif
}

void UpdateSMARTvaluesUI_()
{
  NcursesUI::s_p_NcursesUI->UpdateSMARTvaluesUI();
}

void NcursesUI::BeforeWait()
{
  g_eventQueue.addEvent( UpdateSMARTvaluesUI_ );
}

void NcursesUI::CreateUI()
{
  /** The GUI thread id is determined by the thread that creates the windows.
      So this ID is set to the thread that calls this function. */
  s_GUIthreadID = OperatingSystem::GetCurrentThreadNumber();
  /** see https://bbs.archlinux.org/viewtopic.php?id=94780 */
  std::cout << "in Linux/Unix the environment variable \"TERM\" must be set "
    "in order for initscr() to succeed. " << std::endl;
  /** http://pubs.opengroup.org/onlinepubs/007908775/xcurses/initscr.html :
   *  "Upon successful completion, initscr() returns a pointer to stdscr. 
   *   Otherwise, it does not return." */
  WINDOW * p_stdScrWin = initscr();
	cbreak();              /* direct input (no newline required)... */
	noecho();              /* ... without echoing */
	curs_set(0);           /* hide cursor (if possible) */
	nodelay(stdscr, TRUE);  /* don't wait for input... */
	halfdelay(10);          /* ...well, no more than a second, anyway */
	keypad(stdscr, TRUE);   /* enable cursor keys */
	scrollok(stdscr, TRUE); /* enable scrolling in main window */
  
  s_menuBar = newwin(NUM_MENU_BAR_LINES, COLS, 0, 0);
  s_bodyWindow = newwin(LINES - NUM_MENU_BAR_LINES, COLS, 1, 0 /**start x*/);
  initColor();
  if( has_colors() )
  {
    start_color();
    int maxNumColours = COLORS;
    //int maxNumColourPairs = COLOR_PAIR;
    wattrset(s_menuBar, BODY_WINDOW_COLOR ); //COLOR_PAIR(color & A_CHARTEXT) | attr);
    chtype attr = BODY_WINDOW_COLOR & A_ATTR;  /* extract Bold, Reverse, Blink bits */
    wbkgd(s_menuBar, attr);
    colorBox(s_menuBar, BODY_WINDOW_COLOR, 0);
    colorBox(s_bodyWindow, BODY_WINDOW_COLOR, 1);
  //  CreateWindows();
  }
  CreateMenus();
//  CreateForm();
}

//TODO unify with wxSMARTmonitorApp::OnInit() in base class (
//  SMARTmonitorClient or SMARTmonitorBase)
void NcursesUI::Init()
{
//  gs_ncursesUI.SetCommandLineArgs(argc, argv);
  
//  ncursesUI.CreateCommandLineArgsArrays();
  ProcessCommandLineArgs();
  
  std::wstring stdwstrServiceConnectionConfigFile = GetProgramOptionValue(
    serviceConnectionConfigFile);
  
  mp_configurationLoader->ReadServiceConnectionSettings(
    s_programOptionValues[serviceConnectionConfigFile] );
  
  if( ! stdwstrServiceConnectionConfigFile.empty() )
    ConnectToServerAndGetSMARTvalues();
//  ncursesUI.InitializeLogger();
  fastestUnsignedDataType initSMARTretCode = InitializeSMART();
//  if( initSMARTretCode == SMARTmonitorBase::success )
  {
//  NcursesUI::ServerAddressForm();
//  NcursesUI::CreateUI();
    Curses::MainWindow win(*this);
    NcursesUI::MenuLoop();
  }
}

void NcursesUI::BeforeConnectToServer()
{
//  Curses::MessageBox messageBox(BODY_WINDOW_COLOR, BUTTON_COLOR);
  s_connectionStatusWindow.SetSocketFileDescriptor(m_socketFileDesc);
  s_connectionStatusWindow.EventLoop();
}

void NcursesUI::LayoutUserInterface()
{
  wresize(s_menuBar, NUM_MENU_BAR_LINES, COLS);
  wresize(s_bodyWindow, LINES - NUM_MENU_BAR_LINES, COLS);
  colorBox(s_bodyWindow, BODY_WINDOW_COLOR, 1);
  wrefresh(s_bodyWindow);
}

void NcursesUI::ReBuildUserInterface()
{
  SetSMARTdriveID();
  int lineNumber = 1;

  fastestUnsignedDataType maxNumCharsNeeded, column = 0;
  const char * p_chAttrName;
  for( fastestUnsignedDataType attributeType = ColumnIndices::SMART_ID;
      attributeType < ColumnIndices::beyondLast; attributeType ++ )
  {
    p_chAttrName = SMARTmonitorClient::s_columnAttriuteNames[attributeType];
    mvwaddstr(s_bodyWindow, lineNumber, column, p_chAttrName );
    maxNumCharsNeeded = SMARTmonitorClient::s_maxNumCharsNeededForDisplay[
      attributeType];
    column += maxNumCharsNeeded;
  }
  m_lastLineNumber = 2;
  SetSMARTattribIDandNameLabel();
  m_lastLineNumber = 2;
  wrefresh(s_bodyWindow);
}

//void NcursesUI::UpdateSMARTvaluesUI()
//{
//}

void NcursesUI::SetAttribute(
  const SMARTuniqueID & sMARTuniqueID, 
  fastestUnsignedDataType SMARTattributeID,
  const enum ColumnIndices::columnIndices & columnIndex,
  const std::string & std_strValue,
  enum SMARTvalueRating sMARTvalueRating)
{
  attrID2Line_type::const_iterator iter = attributeID2LineMapping.find(
    SMARTattributeID);
  unsigned lineNumber;
  switch( sMARTvalueRating )
  {
    case noCriticalValue :
      /** Alternate (inverted/complementary) colours for better readability: 
       *  So the value for the next column can be displayed directly near/right of
       *  the current value. */
      setcolor( s_bodyWindow, columnIndex % 2 == 0 ? SMART_ATTRIBUTE_COLOR : 
        ATTR_INVERTED_COLOR);
      break;
    case SMARTvalueOK :
      /** Alternate (inverted/complementary) colours for better readability: 
       *  So the value for the next column can be displayed directly near/right of
       *  the current value. */
      setcolor( s_bodyWindow, columnIndex % 2 == 0 ? SMART_VALUE_OK : 
        SMART_VALUE_OK_INVERTED);
      break;
    case SMARTvalueWarning:
      /** Alternate (inverted/complementary) colours for better readability: 
       *  So the value for the next column can be displayed directly near/right of
       *  the current value. */
      setcolor( s_bodyWindow, columnIndex % 2 == 0 ? SMART_ATTRIBUTE_COLOR : 
        ATTR_INVERTED_COLOR);
      break;
  }
  const int maxx = getmaxx(s_bodyWindow);
  if( iter != attributeID2LineMapping.end() ) /** mapping exists/found */
  {
    lineNumber = iter->second;
  }
  else
  {
    lineNumber = m_lastLineNumber;
    attributeID2LineMapping.insert(std::make_pair(SMARTattributeID, m_lastLineNumber) );
    ++ m_lastLineNumber;
  }
  /** Prevent text wrap around after line end (begins at x pos 0 in next line) */
  if( s_charPosOAttrNameBegin[columnIndex] < maxx )
    mvwaddnstr(s_bodyWindow, lineNumber, s_charPosOAttrNameBegin[columnIndex], 
      std_strValue.c_str(), maxx - s_charPosOAttrNameBegin[columnIndex] );
  wrefresh(s_bodyWindow);
}

void NcursesUI::SetSMARTdriveID()
{
//  LOGN("begin")
  std::set<SMARTuniqueIDandValues> & SMARTuniqueIDsAndValues = mp_SMARTaccess->
    GetSMARTuniqueIDandValues();
  std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
    SMARTuniqueIDsAndValues.begin();
  for(fastestUnsignedDataType currentDriveIndex = 0 /*, ucT4 = 0*/;
//    currentDriveIndex < numberOfDifferentDrives; /*++ currentDriveIndex*/
    SMARTuniqueIDandValuesIter != SMARTuniqueIDsAndValues.end() ;
    SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueID & SMARTuniqueID = SMARTuniqueIDandValuesIter->getSMARTuniqueID();
//    SMARTuniqueID.
    std::ostringstream oss;
    oss << "model:" << SMARTuniqueID.m_modelName;
    oss << " firmware:" << SMARTuniqueID.m_firmWareName;
    oss << " serial:" << SMARTuniqueID.m_serialNumber;
    std::string mediaInfo = oss.str();
    mvwaddstr(s_bodyWindow, 0, 0, mediaInfo.c_str() );
    wrefresh(s_bodyWindow);
//    std::set<SkSmartAttributeParsedData>::const_iterator
//      SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
  }
}

void NcursesUI::ShowMessage(const char * const message) const
{
  //TODO: 
  DWORD currentThreadNumber = OperatingSystem::GetCurrentThreadNumber();
  const bool currentThreadIsUIthread = currentThreadNumber == s_GUIthreadID;
  if( currentThreadIsUIthread )
  {
    Curses::MessageBox messageBox(BODY_WINDOW_COLOR, BUTTON_COLOR);
    messageBox.ShowMessage(message, s_bodyWindow);
  }
//  else//if the thread is another than the GUI thread then 
  {
    //TODO
//    MessageEvent(message);
//    g_eventQueue.addEvent();
  }
}

void SuccessfullyConnectedToServer()
{
  NcursesUI::s_p_NcursesUI->ChangeState(SMARTmonitorClient::connectedToService);
  NcursesUI::s_p_NcursesUI->GetSMARTvaluesAndUpdateUI();
}

void NcursesUI::AfterConnectToServer(int connectResult)
{
  s_connectionStatusWindow.Close();
  if( connectResult == 0 )
    g_eventQueue.addEvent(SuccessfullyConnectedToServer);
}

void UpdateConnTimeout()
{
  int f;
  mvwprintw(NcursesUI::s_statusBar, 0, 0, 
    "connecting to \"\" %u",f);
  wrefresh(NcursesUI::s_statusBar);
}

void NcursesUI::ShowConnectionState(const char * const pch, int timeOutInS)
{
  std::ostringstream oss;
  oss << "connecting to \"" << pch << "\",timeout in ca." << timeOutInS << "s";
//  if( ! s_connectionStatusWindow.IsShown() )
    s_connectionStatusWindow.ShowMessage(oss.str().c_str(), s_bodyWindow);
  //TODO make interruptable by user (e.g. via "cancel" button)
//  while(timeOutInS --)
//  {
//    sleep(1);
    //g_eventQueue.addEvent( UpdateConnTimeout );
    
//  mvwaddstr(NcursesUI::s_statusBar, 0, 0, oss.str().c_str() );
//  wrefresh(NcursesUI::s_statusBar);
//  }
    //  std::ostringstream oss;
    //  oss << "connecting to " << pch << " , timeout:" << timeOut << "seconds";
    //  ShowMessage(oss.str().c_str() );
}
  
void NcursesUI::ShowProgramMenu()
{
  s_programMenu.InsideMenu(true, s_bodyWindow);
}

void NcursesUI::ShowServerMenu()
{
  s_serverMenu.InsideMenu(true, s_bodyWindow);
}

void NcursesUI::ChangeState(enum serverConnectionState newState)
{
  colorBox(s_statusBar, STATUS_BAR_COLOR, 0);
//  setcolor(s_statusBar, STATUS_BAR_COLOR);
  switch( newState )
  {
  case connectedToService :
//  mvwaddstr(s_menuBar, 0, 30, m_stdstrServerAddress.c_str() );
    mvwprintw(s_statusBar, 0, 0, 
      "connected to \"%s\"", //"--last update:%s (local time)", 
      m_stdstrServiceHostName.c_str() );
    break;
  case unconnectedFromService :
    //If got at
    if( m_timeOfLastSMARTvaluesUpdate.tm_sec == 100 )
    {
      mvwprintw(s_statusBar, 0, 0, 
        "disconnected from %s--never got S.M.A.R.T values",
        m_stdstrServiceHostName.c_str() );
    }
    else
    {
      std::string timeString = GetTimeAsString(m_timeOfLastSMARTvaluesUpdate);
      mvwprintw(s_statusBar, 0, 0, 
        "disconnected from %s--last update:%s (local time)",
        m_stdstrServiceHostName.c_str(), timeString.c_str() );
    }
    break;
  }
//  touchwin(s_statusBar);
  wrefresh(s_statusBar);
}
  
void NcursesUI::CreateMenus(){
  s_programMenu.addMenuItem("About", Exit);
  s_programMenu.addMenuItem("Exit", Exit);
  s_programMenu.createMenu();
  
  s_serverMenu.addMenuItem("connect", ConnectToServer);
  s_serverMenu.addMenuItem("disconnect", ConnectToServer);
  s_serverMenu.createMenu();
  
  s_mainMenu.addMenuItem("Program", ShowProgramMenu);
  s_mainMenu.addMenuItem("Server", ShowServerMenu);
  s_mainMenu.createMenu(Curses::Menu::Horizontal);
  s_statusBar = newwin(NUM_MENU_BAR_LINES, 
    COLS - s_mainMenu.GetNumColumnsNeeded(), 0, s_mainMenu.GetNumColumnsNeeded());
//  if( has_colors() )
//  {
//    chtype attr = BODYCOLOR & A_ATTR;  /* extract Bold, Reverse, Blink bits */
//    wattrset(s_statusBar, BODYCOLOR ); //COLOR_PAIR(color & A_CHARTEXT) | attr);
//    wbkgd(s_statusBar, attr);
//  }
//  s_inputProcessorStack.add(s_mainMenu);
}

void NcursesUI::MenuLoop()
{
  s_mainMenu.InsideMenu(false, s_menuBar);
}

int main(int argc, char * argv [])
{
  NcursesUI ncursesUI(argc, argv);
  ncursesUI.Init();
  return 0;
}
