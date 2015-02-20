/*
 * ConfigurationLoader.cpp
 *
 *  Created on: 28.12.2014
 *      Author: mr.sys
 */

#include <libConfig/ConfigurationLoader.hpp>
#include <libconfig.h++>

#include <ConfigLoader/ParseConfigFileException.hpp>

#include <FileSystem/File/FileException.hpp>
#include <iostream> //std::err
#include <sstream> //std::ostringstream

namespace libConfig
{

  ConfigurationLoader::ConfigurationLoader(
    SMARTDETAILSMAP & oSMARTDetails ,
    const UserInterface & r_userInterface
    )
    : ConfigurationLoaderBase(oSMARTDetails)
    , m_r_userInterface(r_userInterface)
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
    TCHAR * szFileNameOfThisExecutable;
    int nC1, nSmartAttribs;
    ST_SMART_DETAILS stSD;

    m_oSMARTDetails.clear();

  //  if(IsDebuggerPresent()==FALSE)
      {
      }
    std::wstring fullConfigFilePath = fullFilePathOfThisExecutable + L"cfg";
    libconfig::Config libConfig;

    std::string stdstrFullConfigFilePath = GetStdString_Inline(fullConfigFilePath);
    // Read the file. If there is an error, report it and exit.
    try
    {
      libConfig.readFile(stdstrFullConfigFilePath.c_str());

      // Get the store name.
      try {
        ST_SMART_DETAILS stSD;
          //libConfig.lookupValue("testImageFilePath", m_cfgData.testImageFilePath);

        const libconfig::Setting & root = libConfig.getRoot();
          const libconfig::Setting & critical_SMART_parameters =
              root["critical_SMART_parameters"];
        const int numCritical_SMART_parameters = critical_SMART_parameters.getLength();
        std::tstring std_tstr;
        for(int criticalSMARTparameterIndex = 0; criticalSMARTparameterIndex
          < numCritical_SMART_parameters; ++ criticalSMARTparameterIndex)
        {
          const libconfig::Setting & critical_SMART_parameter =
              critical_SMART_parameters[criticalSMARTparameterIndex];
          stSD.m_bCritical = true;
          int attribID;
          std::string std_strAttribName, std_strAttribDetails;
          const bool successfullyLookedUpID = critical_SMART_parameter.
            lookupValue("Id", attribID);
          if( successfullyLookedUpID )
          {
            const bool successfullyLookedUpName = critical_SMART_parameter.
              lookupValue("Name", std_strAttribName);
//            const bool successfullyLookedUpDetails = critical_SMART_parameter.
//              lookupValue("Details", std_strAttribDetails);

            if( //successfullyLookedUpID &&
                successfullyLookedUpName
                //&& successfullyLookedUpDetails
              )
            {
              stSD.m_ucAttribId = attribID;
              std_tstr = GetStdTstring_Inline( std_strAttribName.c_str() );
              stSD.m_csAttribName = CString(std_tstr);
              //stSD.m_csAttribDetails = std_strAttribDetails;
              m_oSMARTDetails.insert(
                  SMARTDETAILSMAP::value_type(stSD.m_ucAttribId, stSD));
            }
          }
          else
          {
            std::ostringstream std_oss;
            std_oss << "warning: no \"Id\" parameter provided for SMART "
              "parameter at " << critical_SMART_parameter.getSourceFile()
              << ":" << critical_SMART_parameter.getSourceLine();
            m_r_userInterface.ShowMessage(std_oss.str().c_str() );
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
      throw FileException(fullConfigFilePath.c_str() );
    }
    catch (const libconfig::ParseException &pex)
    {
      std::ostringstream stdoss;
      stdoss << "Parse error at " << pex.getFile() << ":" << pex.getLine()
            << " - " << pex.getError() ;
      std::cerr << stdoss.str() << std::endl;
      //return false;
      m_r_userInterface.ShowMessage(stdoss.str().c_str() );
      ParseConfigFileException parseConfigFileException;
      throw parseConfigFileException;
      //return(EXIT_FAILURE);
    }
    return true;
  }

} /* namespace libConfig */
