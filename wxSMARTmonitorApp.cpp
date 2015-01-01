/*
 * wxSMARTmonitorApp.cpp
 *
 *  Created on: 26.11.2013
 *      Author: mr.sys
 *
 *  adapted from the wxWidgets "tbtest" sample
 */
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//// the application icon (under Windows and OS/2 it is in resources)
//#ifndef wxHAS_IMAGES_IN_RESOURCES
//    #include "../sample.xpm"
//#endif
//
//#include "smile.xpm"

#include "wxSMARTmonitorApp.hpp"
#include "wxSMARTmonitorDialog.hpp" // class MyDialog
#include <wx/taskbar.h>
#include <wx/filefn.h> //wxFILE_SEP_PATH
#include <iostream> //class std::cerr
#include <Windows/HideMinGWconsoleWindow.h>
#include <FileSystem/File/FileException.hpp>
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

//class MyDialog;
/*static*/ MyDialog *gs_dialog = NULL;

wxSMARTmonitorApp::wxSMARTmonitorApp()
{
  // TODO Auto-generated constructor stub

}

wxSMARTmonitorApp::~wxSMARTmonitorApp()
{
  // TODO Auto-generated destructor stub
}

IMPLEMENT_APP(wxSMARTmonitorApp)

bool wxSMARTmonitorApp::OnInit()
{
//  std::cerr << "ggg" << std::endl;
  if ( ! wxApp::OnInit() )
  {
//    std::cerr << "ggg" << std::endl;
    return false;
  }

#if wxMAJOR_VERSION > 2 && wxMINOR_VERSION > 8 // since wxWidgets 2.9.0
  if ( ! wxTaskBarIcon::IsAvailable() )
  {
    wxMessageBox(
      wxT("There appears to be no system tray support in your current "
        "environment. This sample may not behave as expected."),
      wxT("Warning")
      , wxOK | wxICON_EXCLAMATION
    );
  }
#endif
  try
  {
    wxString workingDirectory = wxGetWorkingDirectory();
    wxString fullFilePathOfThisExecutable = argv[0];
    const int indexOfLastBackSlash = fullFilePathOfThisExecutable.rfind(
      wxFILE_SEP_PATH);
    const int fileNameLen = fullFilePathOfThisExecutable.size() - indexOfLastBackSlash - 1;
    wxString workingDirWithConfigFilePrefix = workingDirectory +
      fullFilePathOfThisExecutable.substr(indexOfLastBackSlash,
      fileNameLen - 2 /* - 3 chars extension + file separator char */);
//  wxWidgets::wxSMARTreader smartReader;

  if( ! smartReader.GetSMARTparameterIDsToWatch() )
  {
    wxMessageBox(wxT("failed reading config file.") );
    return false;
  }
  //if( smartReader.m_oSMARTDetails.size())
  DWORD dwRetVal = smartReader.ReadSMARTValuesForAllDrives();
  if( dwRetVal == ERROR_ACCESS_DENIED )
  {
    wxMessageBox( wxT("access denied to S.M.A.R.T.\n->restart this program as an"
      " administrator\n->programs exits after closing this message box") );
    return false;
  }
  m_taskBarIcon = new MyTaskBarIcon();
  // Create the main window
  gs_dialog = new MyDialog(wxT("wxS.M.A.R.T. monitor"), smartReader);
  HideMinGWconsoleWindow();
  }
  catch(const FileException & fe)
  {
    TCHAR * filePath = fe.GetFilePath();
    wxMessageBox(wxT("failed to open file ") + wxWidgets::getwxString_inline(
      //GetStdWstring( ))
      filePath ) );
    return false;
  }
//  gs_dialog->Show(true);

 return true;
}

bool wxSMARTmonitorApp::GetSMARTokayIcon(wxIcon & icon)
{
//  wxIcon icon; //(/*smile_xpm*/  );
  #ifdef _WIN32
  wxString wxstrIconFileName = wxT("S.M.A.R.T._OK.ico");
  #endif
  bool bLoadFileRetVal = icon.LoadFile(
    //http://docs.wxwidgets.org/trunk/classwx_bitmap.html:
    //"wxMSW supports BMP and ICO files, BMP and ICO resources;
    // wxGTK supports XPM files;"
    #ifdef _WIN32
      wxstrIconFileName,
      wxBITMAP_TYPE_ICO
    #endif
    );
  if( ! bLoadFileRetVal)
    wxMessageBox( wxT("Loading icon file \n\"") + wxGetCwd() +
      wxFILE_SEP_PATH + wxstrIconFileName + wxT( "\" failed") );
  return bLoadFileRetVal;
}

bool wxSMARTmonitorApp::GetSMARTwarningIcon(wxIcon & icon)
{
//  wxIcon icon; //(/*smile_xpm*/  );
  #ifdef _WIN32
  wxString wxstrIconFileName = wxT("S.M.A.R.T._warning.ico");
  #endif
  bool bLoadFileRetVal = icon.LoadFile(
    //http://docs.wxwidgets.org/trunk/classwx_bitmap.html:
    //"wxMSW supports BMP and ICO files, BMP and ICO resources;
    // wxGTK supports XPM files;"
    #ifdef _WIN32
      wxstrIconFileName,
      wxBITMAP_TYPE_ICO
    #endif
    );
  if( ! bLoadFileRetVal)
    wxMessageBox( wxT("Loading icon file \n\"") + wxGetCwd() +
      wxFILE_SEP_PATH + wxstrIconFileName + wxT( "\" failed") );
  return bLoadFileRetVal;
}
