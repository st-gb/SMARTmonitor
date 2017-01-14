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
//#include <menu.h>
#include <libraries/ncurses/form/Form.hpp>
#include <libraries/curses/textfield.h>

#define NUM_MENU_BAR_LINES 1
//from PDcurses demo, tui.c:
# define BODYCOLOR        6
#define A_ATTR  (A_ATTRIBUTES ^ A_COLOR)  /* A_BLINK, A_REVERSE, A_BOLD */

/** Definitions of static variables. */
Ncurses::Menu NcursesUI::s_mainMenu, NcursesUI::s_programMenu, 
  NcursesUI::s_serverMenu;
Ncurses::Form NcursesUI::s_connToSrvForm;
struct _win_st * NcursesUI::s_menuBar, * NcursesUI::s_bodyWindow;
NcursesUI * NcursesUI::s_p_NcursesUI;



NcursesUI::NcursesUI() {
  s_p_NcursesUI = this;
  CreateUI();
}

NcursesUI::NcursesUI(const NcursesUI& orig) {
}

NcursesUI::~NcursesUI() {
  endwin();
}

void NcursesUI::Exit()
{
  
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
     *   1. number of colour pair,
     *   2. foreground colour number,
     *   3. background colour number
     *  "& ~A_ATTR" : remove possible attributes like "A_BOLD" */
    init_pair(BODYCOLOR    & ~A_ATTR, COLOR_WHITE, COLOR_BLUE);
    init_pair(EDITBOXCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
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

void NcursesUI::ShowMessage(const char * const msg) const
{
  mvwaddstr(s_bodyWindow, 5, 3, msg);
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

int main(void )
{
  NcursesUI ncursesUI;
//  NcursesUI::ServerAddressForm();
//  NcursesUI::CreateUI();
  NcursesUI::MenuLoop();
  return 0;
}
