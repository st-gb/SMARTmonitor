/*
 * wxSMARTmonitorTaskBarIcon.cpp
 *
 *  Created on: 26.11.2013
 *      Author: mr.sys
 */

#include "wxSMARTmonitorTaskBarIcon.hpp" //class wxSMARTmonitorTaskBarIcon
#include "wxSMARTmonitorApp.hpp"
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
//#include "smile.xpm"

#include "wxSMARTmonitorDialog.hpp"
//class MyDialog;
extern SMARTdialog * gs_dialog;

// ----------------------------------------------------------------------------
// MyTaskBarIcon implementation
// ----------------------------------------------------------------------------
enum
{
    PU_RESTORE = 10001,
    PU_NEW_ICON,
    PU_EXIT,
    PU_CHECKMARK,
    PU_SUB1,
    PU_SUB2,
    PU_SUBMAIN
};

BEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    MyTaskBarIcon::OnMenuExit)
    EVT_MENU(PU_NEW_ICON,MyTaskBarIcon::OnMenuSetNewIcon)
    EVT_MENU(PU_CHECKMARK,MyTaskBarIcon::OnMenuCheckmark)
    EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
//    EVT_TASKBAR_LEFT_DCLICK  (MyTaskBarIcon::OnLeftButtonDClick)
    EVT_TASKBAR_LEFT_DOWN  (MyTaskBarIcon::OnLeftButtonClick)
    EVT_MENU(PU_SUB1, MyTaskBarIcon::OnMenuSub)
    EVT_MENU(PU_SUB2, MyTaskBarIcon::OnMenuSub)
    EVT_SIZE(MyTaskBarIcon::OnSize)
END_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
    gs_dialog->Show(true);
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
    gs_dialog->Close(true);
}

static bool check = true;

void MyTaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
{
    check = !check;
}

void MyTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
{
    event.Check(check);
}

void MyTaskBarIcon::OnMenuSetNewIcon(wxCommandEvent&)
{
//    wxIcon icon(smile_xpm);
//
//    if ( ! SetIcon(icon, wxT("wxTaskBarIcon Sample - a different icon")))
//        wxMessageBox(wxT("Could not set new icon."));
}

void MyTaskBarIcon::OnMenuSub(wxCommandEvent&)
{
    wxMessageBox(wxT("You clicked on a submenu!"));
}

// Overridables
wxMenu * MyTaskBarIcon::CreatePopupMenu()
{
    wxMenu *menu = new wxMenu;
    menu->Append(PU_RESTORE, wxT("&Restore main window"));
    menu->AppendSeparator();
//    menu->Append(PU_NEW_ICON, wxT("&Set New Icon"));
//    menu->AppendSeparator();
//    menu->AppendCheckItem(PU_CHECKMARK, wxT("Test &check mark"));
//    menu->AppendSeparator();
//    wxMenu *submenu = new wxMenu;
//    submenu->Append(PU_SUB1, wxT("One submenu"));
//    submenu->AppendSeparator();
//    submenu->Append(PU_SUB2, wxT("Another submenu"));
//    menu->Append(PU_SUBMAIN, wxT("Submenu"), submenu);
    /* OSX has built-in quit menu for the dock menu, but not for the status item */
#ifdef __WXOSX__
    if ( OSXIsStatusItem() )
#endif
    {
        menu->AppendSeparator();
        menu->Append(PU_EXIT,    wxT("E&xit"));
    }
    return menu;
}

void MyTaskBarIcon::OnSize(wxSizeEvent & evt)
{
  wxSize s = evt.GetSize();
  int i = s.x;
}

//void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
//{
//    gs_dialog->Show(true);
//}

void MyTaskBarIcon::OnLeftButtonClick(wxTaskBarIconEvent&)
{
  bool dialogIsShown = gs_dialog->IsShown();
  gs_dialog->Show( ! dialogIsShown);
  //TODO bring window to front
//  gs_dialog->B
}
