/**(c) from 2022 Stefan Gebauer,Computer Science Master(from TU Berlin)
 * Author:Stefan Gebauer(TU Berlin matriculation number 361095)
 * Created on 29.Oct.2022, 20:44(Berlin, Germany time zone)*/

///Include guard,see http://en.wikipedia.org/wiki/Include_guard
#ifdef TU_Bln361095usePrgmInclGrd
/**Non-standard include guard:supported by many, but not all industry compilers:
 * see http://en.wikipedia.org/wiki/Pragma_once#Portability */
  #pragma once
#endif
#if defined TU_Bln361095usePrgmInclGrd ||\
  /**Include guard supported by (nearly) all industry compilers:*/\
 ! defined TU_Bln361095SMARTmon_wx_UI_about_hpp
   #define TU_Bln361095SMARTmon_wx_UI_about_hpp

///Standard C(++) header files:
#include <sstream>///class std::ostringstream

///wxWidgets header files:
#include <wx/dialog.h>///base class wxDialog
#include <wx/msgdlg.h>///wxMessageBox(...)
#include <wx/sizer.h>///class wxBoxSizer
#include <wx/taskbar.h>///class wxTaskBarIcon::IsAvailable()
#include <wx/textctrl.h>///class wxTextCtrl

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
///FileSystem::GetCurrentWorkingDir
#include <FileSystem/GetCurrentWorkingDir.hpp>
///OperatingSystem::Process::GetCurrExePath(...)
#include <OperatingSystem/Process/FileSys/GetCurrExePath.hpp>

///wxWidgets::GetwxString_Inline(...)
#include <wxWidgets/charStr/wxStringHelper.hpp>
#include "../wxSMARTmonitorApp.hpp"///wxGetApp()
#include <bldTmCharArr.h>///TU_Bln361095SMARTmonBldTmANSIcharArr

namespace wxWidgets
{
///Dialog with non-editable text control
class AboutDlg: public wxDialog
{
public:
  AboutDlg(const wxString & str, const wxChar * const title)
  : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, //wxDefaultSize,
      wxSize(400,400),
      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
  {
    wxTextCtrl * p_wxTxtCtrl = new wxTextCtrl(this, wxID_ANY, str,
      wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    p_wxTxtCtrl->SetEditable(false);
    wxBoxSizer * p_sizer = new wxBoxSizer(wxVERTICAL);

    p_sizer->Add(
      p_wxTxtCtrl,
      /** "proportion": "in the main orientation of the wxBoxSizer - where 0
       * stands for not changeable" */
      1 //wxEXPAND///To not need to scroll vertically for large messages.
      , wxEXPAND //int flag=0
      , /*int border*/ 0);
    SetSizer/*AndFit*/(p_sizer);
    /** https://wiki.wxwidgets.org/Beech:Using_custom_dialogs_with_sizers :
     * "SetSizeHints() directs the sizer to set the minimal size of the window to
     * match the sizer's minimal size." */
//    p_sizer->SetSizeHints(this);
    Centre();
//    Fit();
  }
};

inline void showAboutDlg(wxWindow * parent)
{
  /**"static" within function means: this is only created/assigned at the 1st
   * time.*/
  static const wxChar * const title = wxT("About wxS.M.A.R.T.monitor");
  static wxString message =
    wxT("a tool to monitor (CRITICAL) S.M.A.R.T. parameters\n"
    "(see http://en.wikipedia.org/wiki/S.M.A.R.T.)\n"
    "\n(c) ca. Nov 26 2013-"/*__DATE__*/)
    + TU_Bln361095::wxWidgets::GetwxString_inln(
        TU_Bln361095SMARTmonBldDtANSIcharArr)
    + wxT(" ")
    + TU_Bln361095::wxWidgets::GetwxString_inln(
        TU_Bln361095SMARTmonBldTmOfD_ANSIcharArr)
    + wxT("(Berlin, Germany time zone) by Stefan Gebauer(Computer Science "
    "Master from TU Berlin--matriculation number 361095), Berlin, Germany");

  wxString buildID;
  buildID += "\nbuild type: ";
  buildID += wxSTRINGIZE(BUILD_TYPE);
  buildID += wxT("\nBuild configuration:\nC++ compiler: \"");
  buildID += wxSTRINGIZE(CXX_COMPILER);
  buildID += " ";
  buildID += wxSTRINGIZE(CXX_COMPILER_VERSION);
  buildID += "\"\nC++ flags: \"";
  buildID += wxSTRINGIZE(CXX_FLAGS);
  buildID += "\"\n";

//  wxString wxVer = wxSTRINGIZE(wxMAJOR_VERSION) + wxString(wxT(".") );
//  wxVer += wxSTRINGIZE(wxMINOR_VERSION);
//  wxVer += wxT(".");
//  wxVer += wxSTRINGIZE(wxRELEASE_NUMBER);
  wxString wxVer = wxVERSION_NUM_DOT_STRING_T;
  buildID += wxT("Using wxWidgets version ") + wxVer + wxT("\n");

#if defined multithread || defined TU_Bln361095mltThrd
  buildID += "Built with multithread support\n";
#endif
#ifdef TU_Bln361095useBSDskt
  buildID += "Built with BSD sockets support\n";
#endif

  buildID += wxT("Unit detection:");
#ifdef untDtctnNarrowRng
  buildID += "narrow unit range";
#endif
#ifdef untDtctnWidnRng
  buildID += "widen unit range";
#endif
#ifdef untDtctnLeastMtrcDiffsBetwnRawValInc
  buildID += "least metric differences between raw value increases";
#endif
  std::tstring currWorkDir, currExeDir;
  const int getCurrWorkDirRslt = TU_Bln361095::FileSys::GetCurrWorkngDir_inln(
    currWorkDir);
  const bool getCurrExePathSccss = TU_Bln361095::OpSys::Process::FileSys::
    GetExeFileName_inln(currExeDir);

  /**The current working directory is relevant for reading configuration files.
   * So display it to let a user know where they are searched.*/
  wxString aboutString = message + wxString("\n") + buildID
    + wxString("\n\ncurrent working directory:\n")
    + (getCurrExePathSccss ? TU_Bln361095::wxWidgets::GetwxString_inln(
      currWorkDir) : wxT("Getting path failed") )
    + wxString("\nFile system path of this executable:\n")
    + (getCurrWorkDirRslt == 0 ? TU_Bln361095::wxWidgets::
       GetwxString_inln(currExeDir) : wxT("") );

#if wxMAJOR_VERSION > 1 && wxMINOR_VERSION > 8 || wxMAJOR_VERSION > 2
  if(///https://docs.wxwidgets.org/trunk/classwx_task_bar_icon.html#a287bb3303f01651f50c8de17e314a147;
     /** "Since 2.9.0*/ ! wxTaskBarIcon::IsAvailable() )
    aboutString += wxT("\n\nThere appears to be no task bar/system tray support"
      " in your current environment. So you aren't able to hide the main window"
      " (and re-show it by clicking on the task bar icon) while running this "
      "application.");
#endif

//#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
//  wxGetApp().m_taskBarIcon->ShowBalloon(title, message, 15000,
//    wxICON_INFORMATION);
//#else // !__WXMSW__
  
//  wxMessageBox(aboutString, title, wxICON_INFORMATION | wxOK, parent);
  AboutDlg aboutDlg(aboutString, title);
  aboutDlg.ShowModal();
//#endif // __WXMSW__/!__WXMSW__
}
}///End namespace
#endif///include guard
