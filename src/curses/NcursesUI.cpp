/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NcursesUI.cpp
 * Author: root
 * 
 * Created on 11. Januar 2017, 14:30
 */

#include "NcursesUI.hpp"
#include "Controller/character_string/stdtstr.hpp"
//#include <menu.h>
//#include <libraries/ncurses/form/Form.hpp>
#include <libraries/curses/textfield.h>

#define NUM_MENU_BAR_LINES 1
//from PDcurses demo, tui.c:
# define BODYCOLOR        6
# define BUTTON_COLOR     5
//#define A_ATTR  (A_ATTRIBUTES ^ A_COLOR)  /* A_BLINK, A_REVERSE, A_BOLD */

/** Definitions of static variables. */
Ncurses::Menu NcursesUI::s_mainMenu, NcursesUI::s_programMenu, 
  NcursesUI::s_serverMenu;
//Ncurses::Form NcursesUI::s_connToSrvForm;
struct _win_st * NcursesUI::s_menuBar, * NcursesUI::s_bodyWindow;
NcursesUI * NcursesUI::s_p_NcursesUI;

namespace Ncurses
{
  int MainWindow::HandleAction(const int ch)
  {
//      if( ch != ERR )
//        mvprintw(7, 3, "%d %c", ch, ch );
    int ret = -2;
    bool consumedInput = true;
    switch(ch)
    {
      case KEY_RESIZE :
        m_ncursesUI.LayoutUserInterface();
        m_ncursesUI.ReBuildUserInterface();
        break;
      default:
        ret = Ncurses::Window::inputNotHandled;
    }
//    if( consumedInput )
//      wrefresh(windowToShowMenuIn);
    return ret;
  }
}

NcursesUI::NcursesUI(const int argumentCount, char * argumentVector [] )
//    m_cmdLineArgStrings(argumentVector)
{
  CreateCommandLineArgsArrays(argumentCount, argumentVector);
//  m_commandLineArgs.Set(argumentCount, (wchar_t **) argumentVector);
  s_p_NcursesUI = this;
  CreateUI();
}

NcursesUI::NcursesUI(const NcursesUI& orig) {
}

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
  mvwaddstr(NcursesUI::s_bodyWindow, 5, 3, label);
  wrefresh(NcursesUI::s_bodyWindow);
  if( editText(NcursesUI::s_bodyWindow, std_str, 29) == '\n')
    ;
}

void NcursesUI::ConnectToServer()
{
  ServerAddressForm();
}

/*static*/ void initcolor(void)
{
#ifdef A_COLOR
  if (has_colors())
      start_color();

  /** init_pair(...) (ncurses):parameters
   * 1. number of colour pair,
   * 2. foreground colour number,
   * 3. background colour number
   *   "& ~A_ATTR" : remove possible attributes like "A_BOLD" */
  init_pair(BODYCOLOR    & ~A_ATTR, COLOR_WHITE, COLOR_BLUE);
  init_pair(EDITBOXCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
  init_pair(BUTTON_COLOR & ~A_ATTR, COLOR_WHITE, COLOR_RED);
#endif
}

void NcursesUI::CreateUI()
{
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
  initcolor();
  if( has_colors() )
  {
    start_color();
    int maxNumColours = COLORS;
    //int maxNumColourPairs = COLOR_PAIR;
    wattrset(s_menuBar, BODYCOLOR ); //COLOR_PAIR(color & A_CHARTEXT) | attr);
    chtype attr = BODYCOLOR & A_ATTR;  /* extract Bold, Reverse, Blink bits */
    wbkgd(s_menuBar, attr);
    colorBox(s_menuBar, BODYCOLOR, 0);
    colorBox(s_bodyWindow, BODYCOLOR, 1);
  //  CreateWindows();
  }
  CreateMenus();
//  CreateForm();
}

void NcursesUI::LayoutUserInterface()
{
  wresize(s_menuBar, NUM_MENU_BAR_LINES, COLS);
  wresize(s_bodyWindow, LINES - NUM_MENU_BAR_LINES, COLS);
  colorBox(s_bodyWindow, BODYCOLOR, 1);
  wrefresh(s_bodyWindow);
}

void NcursesUI::ReBuildUserInterface()
{
  SetSMARTdriveID();
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
//    pDriveInfo = m_SMARTvalueProcessor.GetDriveInfo(currentDriveIndex);

//    std::set<SkSmartAttributeParsedData>::const_iterator
//      SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
  }
}

void NcursesUI::ShowMessage(const char * const message) const
{
//  MessageWindow msgWin;
  //TODO Make the following code generic and move it to a separate class 
  // "Ncurses::MessageBox" derived from Ncurses::Window
  const char * currentMessageChar = message;
  int currentLineWidth = 0, maxLineWidth = 0, numLines = 1;
  std::vector<fastestUnsignedDataType> m;
  while( * currentMessageChar != '\0' )
  {
    if( * currentMessageChar == '\n' )
    {
      m.push_back(currentLineWidth);
      if( currentLineWidth > maxLineWidth)
        maxLineWidth = currentLineWidth;
      currentLineWidth = 0;
      numLines ++;
    }
    else
      currentLineWidth ++;
    currentMessageChar ++;
  }
  if( currentLineWidth)
    m.push_back(currentLineWidth);
  if( currentLineWidth > maxLineWidth)
    maxLineWidth = currentLineWidth;
  const int windowHeight = numLines + /** box chars + line for OK button*/ 3;
  WINDOW * p_MessageWindow = derwin(s_bodyWindow, windowHeight , 
    maxLineWidth + 2, 1, 1 );
  colorBox(p_MessageWindow, BODYCOLOR, 1);
  mvwaddstr(p_MessageWindow, 0, 1, "message");
//  mvwaddstr(p_win, 1, 1, message);
  const char * lineBegin = message, * formatString;
  for( int i = 0; i < m.size(); i++ )
  {
    currentLineWidth = m.at(i);
    std::ostringstream oss;
    oss << "%." << currentLineWidth << "s"; //-> e.g. "%20s"
    formatString = oss.str().c_str();
    mvwprintw(p_MessageWindow, 1 + i , 1, formatString, lineBegin );
    lineBegin += currentLineWidth + 1;
  }
  WINDOW * p_OKwindow = derwin(p_MessageWindow, 1 , 2, windowHeight - 2, 
    maxLineWidth / 2 );
  colorBox(p_OKwindow, BUTTON_COLOR, 1);
  mvwaddstr(p_OKwindow, 0, 0, "OK");
  wrefresh(p_OKwindow);
//  touchwin(s_bodyWindow);
  wrefresh(p_MessageWindow);
  int ch;
  do
  {
    ch = wgetch(p_MessageWindow);
    /** https://linux.die.net/man/3/wgetch :
     * "In no-delay mode, if no input is waiting, the value ERR is returned."
     * ERR = no valid input. */
//    if( ch != ERR )
  }while(ch != '\n' );
  delwin(p_OKwindow);
  delwin(p_MessageWindow);
  wrefresh(s_bodyWindow);
}

void NcursesUI::ShowProgramMenu()
{
  s_programMenu.InsideMenu(true, s_bodyWindow);
}

void NcursesUI::ShowServerMenu()
{
  s_serverMenu.InsideMenu(true, s_bodyWindow);
}

void NcursesUI::ChangeState(enum state newState)
{
//  mvwaddstr(s_menuBar, 0, 30, m_stdstrServerAddress.c_str() );
  mvwprintw(s_menuBar, 0, 30, "connected to %s", m_stdstrServiceHostName.c_str() );
  wrefresh(s_menuBar);
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
  s_mainMenu.createMenu(Ncurses::Menu::Horizontal);
//  s_inputProcessorStack.add(s_mainMenu);
}

void NcursesUI::MenuLoop()
{
  s_mainMenu.InsideMenu(false, s_menuBar);
}

int main(int argc, char * argv [])
{
  NcursesUI ncursesUI(argc, argv);
  
//  ncursesUI.CreateCommandLineArgsArrays();
  ncursesUI.ProcessCommandLineArgs();
//  ncursesUI.InitializeLogger();
  fastestUnsignedDataType initSMARTretCode = ncursesUI.
    InitializeSMART();
//  if( initSMARTretCode == SMARTmonitorBase::success )
  {
//  NcursesUI::ServerAddressForm();
//  NcursesUI::CreateUI();
    Ncurses::MainWindow win(ncursesUI);
    NcursesUI::MenuLoop();
  }
  return 0;
}
