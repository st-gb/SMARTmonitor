/* File:   NcursesUI.hpp
 * Author: sg
 * Created on 11. Januar 2017, 14:30 */
#ifndef NCURSESUI_HPP
#define NCURSESUI_HPP

#include <client/SMARTmonitorClient.h> //Base class SMARTmonitorClient
//#include <libraries/ncurses/form/Form.hpp>
#include <libraries/ncurses/menu/Menu.hpp>
//#include <libraries/curses/MessageBox.hpp>
#include "ConnectToServerDialog.hpp"

/** Forward declaration.*/
struct _win_st;

/** This class is called NcursesUI because is intended to use ncurses menus */
class NcursesUI
  : public SMARTmonitorClient
{
public:
  NcursesUI() : s_connectionStatusWindow(/*1, 2*/) {}
  NcursesUI(const int argumentCount, char * argumentVector [] );
//  NcursesUI(const NcursesUI& orig);
  virtual ~NcursesUI();
  
  ConnectToServerDialog s_connectionStatusWindow;
  typedef std::map<fastestUnsignedDataType, fastestUnsignedDataType> attrID2Line_type;
  static NcursesUI * s_p_NcursesUI;
  void BeforeWait();// { }
  void BeforeConnectToServer();
  static void CreateUI();
  void CreateCommandLineArgsArrays(const int argumentCount, char * argumentVector []);
  void Init();
  static void ServerAddressForm();
  static void MenuLoop();
  static struct _win_st * s_menuBar, * s_bodyWindow, * s_statusBar;
  void ReBuildUserInterface();
  void AfterConnectToServer(int connectResult);
  void ChangeState(enum serverConnectionState newState);
  
//  void UpdateSMARTvaluesUI();
  void LayoutUserInterface();
private:
  int m_lastLineNumber;
  static fastestUnsignedDataType s_GUIthreadID;
  //TODO maybe use an 256 elements array here:
  // array[SMARTattribID ] = line #
  attrID2Line_type attributeID2LineMapping;
  static Curses::Menu s_mainMenu, s_programMenu, s_serverMenu;
//  static Ncurses::Form s_connToSrvForm;
  static void CreateMenus();
  static void ConnectToServer();
  static void Exit();
  void GetTextFromUser(const char * label, std::string & );
  void SetSMARTdriveID();
  void SetAttribute(
    const SMARTuniqueID &, 
    fastestUnsignedDataType SMARTattributeID,
    const enum ColumnIndices::columnIndices &,
    const std::string &);
  void ShowMessage(const char * const msg) const;
  void ShowConnectionState(const char * const pch, int timeOut);
  static void ShowProgramMenu();
  static void ShowServerMenu();
};

#endif /* NCURSESUI_HPP */
