/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NcursesUI.hpp
 * Author: root
 *
 * Created on 11. Januar 2017, 14:30
 */

#ifndef NCURSESUI_HPP
#define NCURSESUI_HPP

#include <client/SMARTmonitorClient.h> //Base class SMARTmonitorClient
//#include <libraries/ncurses/form/Form.hpp>
#include <libraries/ncurses/menu/Menu.hpp>

struct _win_st;

/** This class is called NcursesUI because is intended to use ncurses menus */
class NcursesUI
  : public SMARTmonitorClient
{
public:
  NcursesUI(const int argumentCount, char * argumentVector [] );
  NcursesUI(const NcursesUI& orig);
  virtual ~NcursesUI();
  
  static NcursesUI * s_p_NcursesUI;
  static void CreateUI();
  void CreateCommandLineArgsArrays(const int argumentCount, char * argumentVector []);
  static void ServerAddressForm();
  static void MenuLoop();
  static struct _win_st * s_menuBar, * s_bodyWindow;
  void ReBuildUserInterface();
  void LayoutUserInterface();
private:
  static Ncurses::Menu s_mainMenu, s_programMenu, s_serverMenu;
//  static Ncurses::Form s_connToSrvForm;
  void ChangeState(enum state newState);
  static void CreateMenus();
  static void ConnectToServer();
  static void Exit();
  void GetTextFromUser(const char * label, std::string & );
  void SetSMARTdriveID();
  void ShowMessage(const char * const msg) const;
  static void ShowProgramMenu();
  static void ShowServerMenu();
};

namespace Ncurses {
  class MainWindow 
    : public Window
  {
    NcursesUI & m_ncursesUI;
  public:
    MainWindow(NcursesUI & ncursesUI) : m_ncursesUI(ncursesUI) {
      Ncurses::Window::s_inputProcessorStack.add(this); }
    int HandleAction(const int ch);
  };
}

#endif /* NCURSESUI_HPP */

