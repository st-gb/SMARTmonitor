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
  NcursesUI() {}
  NcursesUI(const int argumentCount, char * argumentVector [] );
  NcursesUI(const NcursesUI& orig);
  virtual ~NcursesUI();
  
  typedef std::map<fastestUnsignedDataType, fastestUnsignedDataType> attrID2Line_type;
  static NcursesUI * s_p_NcursesUI;
  void BeforeWait();// { }
  static void CreateUI();
  void CreateCommandLineArgsArrays(const int argumentCount, char * argumentVector []);
  static void ServerAddressForm();
  static void MenuLoop();
  static struct _win_st * s_menuBar, * s_bodyWindow, * s_statusBar;
  void ReBuildUserInterface();
//  void UpdateSMARTvaluesUI();
  void LayoutUserInterface();
private:
  int m_lastLineNumber;
  static fastestUnsignedDataType s_GUIthreadID;
  attrID2Line_type attributeID2LineMapping;
  static Curses::Menu s_mainMenu, s_programMenu, s_serverMenu;
//  static Ncurses::Form s_connToSrvForm;
  void ChangeState(enum state newState);
  static void CreateMenus();
  static void ConnectToServer();
  static void Exit();
  void GetTextFromUser(const char * label, std::string & );
  void SetSMARTdriveID();
  void SetAttribute(
    const SMARTuniqueID &, 
    fastestUnsignedDataType SMARTattributeID,
    const enum columnIndices &,
    const std::string &);
  void ShowMessage(const char * const msg) const;
  static void ShowProgramMenu();
  static void ShowServerMenu();
};

#endif /* NCURSESUI_HPP */
