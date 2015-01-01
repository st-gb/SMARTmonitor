/*
 * ConfigurationLoader.cpp
 *
 *  Created on: 24.10.2014
 *      Author: mr.sys
 */

#include <windows.h> //GetModuleFileName, ...
#include "ConfigurationLoader.hpp"
//#include <windef.h> //MAX_PATH
#include <FileSystem/File/FileException.hpp>

namespace Windows
{

  ConfigurationLoader::ConfigurationLoader(const SMARTDETAILSMAP & oSMARTDetails)
    : ConfigurationLoaderBase( (SMARTDETAILSMAP &) oSMARTDetails)
  {
    // TODO Auto-generated constructor stub

  }

  ConfigurationLoader::~ConfigurationLoader()
  {
    // TODO Auto-generated destructor stub
  }

  bool ConfigurationLoader::LoadConfiguration(const std::string & s)
  {
    TCHAR szFullFilePathOfThisExecutable[MAX_PATH] =
      { 0 }, szKeyName[MAX_PATH] =
      { 0 }, szValue[1024] =
      { 0 };
    TCHAR * szFileNameOfThisExecutable;
    int nC1, nSmartAttribs;
    ST_SMART_DETAILS stSD;

    m_oSMARTDetails.clear();

  //  if(IsDebuggerPresent()==FALSE)
      {
        ::GetModuleFileName(NULL, szFullFilePathOfThisExecutable, MAX_PATH);
        TCHAR * lastBackslash = _tcsrchr(szFullFilePathOfThisExecutable, _T('\\'));
        if( lastBackslash != NULL )
        {
          szFileNameOfThisExecutable = lastBackslash + 1;
        }
        szFullFilePathOfThisExecutable[lstrlen(szFullFilePathOfThisExecutable) - 3] = 0;
        lstrcat(szFullFilePathOfThisExecutable, _T("ini") );
      }
  //  else
  //    wsprintf(szINIFileName,"D:\\Saneesh\\Projects\\Helpers\\SMART\\Smart.ini");
    nSmartAttribs = GetPrivateProfileInt(_T("General"), _T("Max Attributes"), 0,
      /*szFileNameOfThisExecutable*/ szFullFilePathOfThisExecutable );
    if( nSmartAttribs == 0)
      throw FileException(szFullFilePathOfThisExecutable);
    for (nC1 = 0; nC1 < nSmartAttribs; ++nC1)
      {
        wsprintf(szKeyName, _T("Attrib%d"), nC1);
        stSD.m_ucAttribId = GetPrivateProfileInt(szKeyName, _T("Id"), 0,
            szFullFilePathOfThisExecutable);
        stSD.m_bCritical = GetPrivateProfileInt(szKeyName, _T("Critical"), 0,
            szFullFilePathOfThisExecutable);
        GetPrivateProfileString(szKeyName, _T("Name"), _T(""), szValue, 1024,
            szFullFilePathOfThisExecutable);
        stSD.m_csAttribName = szValue;
        GetPrivateProfileString(szKeyName, _T("Details"), _T(""), szValue, 1024,
            szFullFilePathOfThisExecutable);
        stSD.m_csAttribDetails = szValue;
        m_oSMARTDetails.insert(
            SMARTDETAILSMAP::value_type(stSD.m_ucAttribId, stSD));
      }
    return nSmartAttribs != 0;
  }
} /* namespace Windows */
