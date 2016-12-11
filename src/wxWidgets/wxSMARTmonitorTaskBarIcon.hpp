/*
 * wxSMARTmonitorTaskBarIcon.hpp
 *
 *  Created on: 26.11.2013
 *      Author: mr.sys
 */

#ifndef WXSMARTMONITORTASKBARICON_HPP_
#define WXSMARTMONITORTASKBARICON_HPP_

#include <wx/taskbar.h> //class wxTaskBarIcon
#ifdef _WIN32
//#include <shellapi.h> //Shell_NotifyIconGetRect
//#include <winuser.h> //GetSystemMetrics
#include <windows.h> //GetSystemMetrics
#include <wx/msw/winundef.h>
#endif

class MyTaskBarIcon
  : public wxTaskBarIcon
{
public:
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    MyTaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE)
    :   wxTaskBarIcon(iconType)
#else
    MyTaskBarIcon()
#endif
    {
//      //http://msdn.microsoft.com/en-us/library/windows/desktop/dd378426%28v=vs.85%29.aspx
//      NOTIFYICONIDENTIFIER identifier;
//      RECT iconLocation;
//      HRESULT hres = ::Shell_NotifyIconGetRect(
//        identifier, //_In_   const NOTIFYICONIDENTIFIER *identifier,
//        iconLocation //_Out_  RECT *iconLocation
//      );
//      //http://msdn.microsoft.com/en-us/library/windows/desktop/bb775701%28v=vs.85%29.aspx
//      LoadIconMetric(
//        //To load a predefined icon or a standalone icon file, set this parameter to NULL.
//        NULL, //_In_   HINSTANCE hinst,
//         _In_   PCWSTR pszName,
//         _In_   int lims,
//         _Out_  HICON *phico
//         );
#ifdef _WIN32
      int x = GetSystemMetrics(SM_CXICON);
      x = GetSystemMetrics(SM_CXSMICON);
      int y = GetSystemMetrics(SM_CYICON);
#endif
    }

//    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnLeftButtonClick(wxTaskBarIconEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuSetNewIcon(wxCommandEvent&);
    void OnMenuCheckmark(wxCommandEvent&);
    void OnMenuUICheckmark(wxUpdateUIEvent&);
    void OnMenuSub(wxCommandEvent&);
    void OnSize(wxSizeEvent &);
    virtual wxMenu *CreatePopupMenu();

    DECLARE_EVENT_TABLE()
};

#endif /* WXSMARTMONITORTASKBARICON_HPP_ */

