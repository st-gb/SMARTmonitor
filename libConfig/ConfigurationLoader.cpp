/*
 * ConfigurationLoader.cpp
 *
 *  Created on: 28.12.2014
 *      Author: mr.sys
 */

#include <libConfig/ConfigurationLoader.hpp>
#include <libconfig.h++>
#include <FileSystem/File/FileException.hpp>
#include <iostream> //std::err

namespace libConfig
{

  ConfigurationLoader::ConfigurationLoader(SMARTDETAILSMAP & oSMARTDetails)
    : ConfigurationLoaderBase(oSMARTDetails)
  {
    // TODO Auto-generated constructor stub

  }

  ConfigurationLoader::~ConfigurationLoader()
  {
    // TODO Auto-generated destructor stub
  }

  bool ConfigurationLoader::LoadConfiguration(
      const std::wstring & fullFilePathOfThisExecutable)
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
        szFullFilePathOfThisExecutable;
        TCHAR * lastBackslash = _tcsrchr(szFullFilePathOfThisExecutable, _T('\\'));
        if( lastBackslash != NULL )
        {
          szFileNameOfThisExecutable = lastBackslash + 1;
        }
//        fullFilePathOfThisExecutable.resize(
//            fullFilePathOfThisExecutable.size() - 3);
//        fullFilePathOfThisExecutable += L"cfg";
      }
    if( nSmartAttribs == 0)
      throw FileException(szFullFilePathOfThisExecutable);
    libconfig::Config libConfig;

    // Read the file. If there is an error, report it and exit.
    try
    {
//      libConfig.readFile(filePath);

      // Get the store name.
      try {
        ST_SMART_DETAILS stSD;
          //libConfig.lookupValue("testImageFilePath", m_cfgData.testImageFilePath);

        const libconfig::Setting & root = libConfig.getRoot();
          const libconfig::Setting & critical_SMART_parameters =
              root["critical_SMART_parameters"];
          const int numFeatures = critical_SMART_parameters.getLength();
          for(int i = 0; i < numFeatures; ++i)
          {
              const libconfig::Setting & critical_SMART_parameter =
                  critical_SMART_parameters;
              stSD.m_bCritical = true;
              int attribID;
              std::string std_strAttribName, std_strAttribDetails;
              if( ( critical_SMART_parameter.lookupValue("Id", attribID)
                   && critical_SMART_parameter.lookupValue("Name", std_strAttribName)
                   && critical_SMART_parameter.lookupValue("Details", std_strAttribDetails)
                   )
                )
              {
                stSD.m_ucAttribId = attribID;
//                stSD.m_csAttribName = std_strAttribName.c_str();
//                stSD.m_csAttribDetails = std_strAttribDetails;
                m_oSMARTDetails.insert(
                    SMARTDETAILSMAP::value_type(stSD.m_ucAttribId, stSD));
              }
          }
      }
      catch (const libconfig::SettingNotFoundException &nfex)
      {
        std::cerr << "No 'name' setting in configuration file." << std::endl;
        //setDefaultValues = true;
      }
    }
    catch (const libconfig::FileIOException &fioex)
    {
      std::cerr << "I/O error while reading file." << std::endl;
      //return(EXIT_FAILURE);
      throw FileException(szFullFilePathOfThisExecutable);
    }
    catch (const libconfig::ParseException &pex)
    {
      std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
        << " - " << pex.getError() << std::endl;
      //return(EXIT_FAILURE);
    }
    return nSmartAttribs != 0;
  }

} /* namespace libConfig */
