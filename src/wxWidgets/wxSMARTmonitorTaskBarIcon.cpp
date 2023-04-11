/** wxSMARTmonitorTaskBarIcon.cpp
 *  Created on: 26.11.2013
 *  Author: Stefan Gebauer, M.Sc. Comp.Sc. */

/**Prevents <winsock.h> to be included in <windows.h>. <Winsock.h> and
 * <Winsock2.h> have the same definitions.->re-definition happens if <winsock.h>
 * is included before <Winsock2.h> */
#define _WINSOCKAPI_

/** Include at 1st in Windows build to avoid:
 * "#warning Please include winsock2.h before windows.h" */
#include "wxSMARTmonitorApp.hpp"
#include "wxSMARTmonitorTaskBarIcon.hpp"///class wxSMARTmonitorTaskBarIcon
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

BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, TaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    TaskBarIcon::OnMenuExit)
    EVT_MENU(PU_NEW_ICON,TaskBarIcon::OnMenuSetNewIcon)
    EVT_MENU(PU_CHECKMARK,TaskBarIcon::OnMenuCheckmark)
    EVT_UPDATE_UI(PU_CHECKMARK,TaskBarIcon::OnMenuUICheckmark)
//    EVT_TASKBAR_LEFT_DCLICK  (MyTaskBarIcon::OnLeftButtonDClick)
    EVT_TASKBAR_LEFT_DOWN  (TaskBarIcon::OnLeftButtonClick)
    EVT_MENU(PU_SUB1, TaskBarIcon::OnMenuSub)
    EVT_MENU(PU_SUB2, TaskBarIcon::OnMenuSub)
    EVT_SIZE(TaskBarIcon::OnSize)
END_EVENT_TABLE()

void ShowTaskBarIcon()
{
//    // we should be able to show up to 128 characters on recent Windows versions
//    // (and 64 on Win9x)
//    if ( ! m_taskBarIcon->SetIcon(
//        wxICON(sample),
//         wxT("wxTaskBarIcon Sample\n"
//         "With a very, very, very, very\n"
//         "long tooltip whose length is\n"
//         "greater than 64 characters.")
//        )
//       )
//    {
//        wxLogError(wxT("Could not set icon."));
//    }

#if defined(__WXOSX__) && wxOSX_USE_COCOA
    m_dockIcon = new TaskBarIcon(wxTBI_DOCK);
    if ( !m_dockIcon->SetIcon(wxICON(sample)) )
    {
        wxLogError(wxT("Could not set icon."));
    }
#endif
}

void TaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
    gs_dialog->Show(true);
}

void TaskBarIcon::OnMenuExit(wxCommandEvent& )
{
    gs_dialog->Close(true);
}

static bool check = true;

void TaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
{
    check = !check;
}

void TaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
{
    event.Check(check);
}

void TaskBarIcon::OnMenuSetNewIcon(wxCommandEvent&)
{
//    wxIcon icon(smile_xpm);
//
//    if ( ! SetIcon(icon, wxT("wxTaskBarIcon Sample - a different icon")))
//        wxMessageBox(wxT("Could not set new icon."));
}

void TaskBarIcon::OnMenuSub(wxCommandEvent&)
{
    wxMessageBox(wxT("You clicked on a submenu!"));
}

// Overridables
wxMenu * TaskBarIcon::CreatePopupMenu()
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

void TaskBarIcon::OnSize(wxSizeEvent & evt)
{
  wxSize s = evt.GetSize();
  int i = s.x;
}

//void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
//{
//    gs_dialog->Show(true);
//}

void TaskBarIcon::OnLeftButtonClick(wxTaskBarIconEvent&)
{
  bool dialogIsShown = gs_dialog->IsShown();
  gs_dialog->Show( ! dialogIsShown);
  //TODO bring window to front
//  gs_dialog->B
}
