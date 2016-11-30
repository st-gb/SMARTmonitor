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
#include <Controller/character_string/stdtstr.hpp>

#include "SMARTmonitorBase.hpp" //GetStdString_Inline

namespace libConfig
{

  ConfigurationLoader::ConfigurationLoader(
    std::set<SkSmartAttributeParsedData> & smartAttributesToObserve,
    SMARTmonitorBase & r_userInterface
    )
    : ConfigurationLoaderBase(/*oSMARTDetails*/ smartAttributesToObserve)
    , m_r_SMARTmonitorBase(r_userInterface)
  {
    // TODO Auto-generated constructor stub
  }

  ConfigurationLoader::~ConfigurationLoader()
  {
    // TODO Auto-generated destructor stub
  }

  /*void Create(
    std::set<SkSmartAttributeParsedData> & smartAttributesToObserve,
    const UserInterface & r_userInterface)
  {

  }*/
  
  void ConfigurationLoader::ReadNetworkConfig(const libconfig::Setting & root)
  {
    unsigned portNumber;
    const libconfig::Setting & netWorkConnection =
      root["networkConnection"];
    if( netWorkConnection.lookupValue("portNumber", portNumber) )
    {
      m_r_SMARTmonitorBase.m_socketPortNumber = portNumber;
      LOGN("setting port number to " << portNumber)
    }
    std::string serviceHostName;
    if( netWorkConnection.lookupValue("serviceHostName", serviceHostName) )
    {
      LOGN("setting service host name to " << serviceHostName)
      m_r_SMARTmonitorBase.m_stdstrServiceHostName = serviceHostName;
    }
  }
  
  void ConfigurationLoader::GetSMARTattributesToObserve(const libconfig::Setting & root)
  {
    const libconfig::Setting & SMART_parameters_to_observe =
      root["SMART_parameters_to_observe"];
    int numSMARTparamsToObserve = SMART_parameters_to_observe.getLength();
    int SMARTattributeID;
    for (int i = 0; i < numSMARTparamsToObserve; ++i)
    {
      const libconfig::Setting & SMARTattributeIDSetting = SMART_parameters_to_observe[i];
      libconfig::Setting::Type type = SMARTattributeIDSetting.getType();
      if( type == libconfig::Setting::TypeInt )
      {
        SMARTattributeID = (int) SMARTattributeIDSetting;
        LOGN("adding SMART attribute with ID " << (int) SMARTattributeIDSetting << 
          " as SMART parameter to observe/monitor")
          m_r_SMARTmonitorBase.m_IDsOfSMARTattributesToObserve.insert(SMARTattributeID);
      }
    }
  }

  bool ConfigurationLoader::LoadConfiguration(
      const std::wstring & fullFilePathOfThisExecutable)
  {
    TCHAR * szFileNameOfThisExecutable;
    int nC1, nSmartAttribs;

//    m_smartAttributesToObserve.clear();
    mp_smartAttributes->clear();

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
//        ST_SMART_DETAILS stSmartDetails;
        SkSmartAttributeParsedData skSmartAttributeParsedData;
          //libConfig.lookupValue("testImageFilePath", m_cfgData.testImageFilePath);

        const libconfig::Setting & root = libConfig.getRoot();
        
        ReadNetworkConfig(root);
        GetSMARTattributesToObserve(root);
                
        const libconfig::Setting & SMART_parameters =
          root["SMART_parameters"];        
        const int numCritical_SMART_parameters = SMART_parameters.getLength();
        std::tstring std_tstr;
        for(int SMARTparameterIndex = 0; SMARTparameterIndex
          < numCritical_SMART_parameters; ++ SMARTparameterIndex)
        {
          const libconfig::Setting & critical_SMART_parameter =
            SMART_parameters[SMARTparameterIndex];
//          stSmartDetails.m_bCritical = true;
          int smartAttributeID;
          std::string std_strAttribName, std_strAttribDetails;
          const bool successfullyLookedUpID = critical_SMART_parameter.
            lookupValue("Id", smartAttributeID);
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
              skSmartAttributeParsedData.id = smartAttributeID;
              attributeNamesFromConfigFile.insert(std_strAttribName);
              std::set<std::string>::const_iterator citer =
                attributeNamesFromConfigFile.find(std_strAttribName);
              if( citer != attributeNamesFromConfigFile.end() )
                //use pointer from member var instead of from stack.
                skSmartAttributeParsedData.name = std_strAttribName.c_str();

              /*m_smartAttributesToObserve.*/ mp_smartAttributes->insert(
                /*std::make_pair(smartAttributeID,*/ skSmartAttributeParsedData//)
                );
              LOGN("adding SMART ID" << smartAttributeID << "to " 
                  << mp_smartAttributes)
            }
          }
          else
          {
            std::ostringstream std_oss;
            std_oss << "warning: no \"Id\" parameter provided for SMART "
              "parameter at " << critical_SMART_parameter.getSourceFile()
              << ":" << critical_SMART_parameter.getSourceLine();
            m_r_SMARTmonitorBase.ShowMessage(std_oss.str().c_str() );
          }
        }
      }
      catch (const libconfig::SettingNotFoundException & nfex)
      {
        std::ostringstream std_oss;
        std_oss << "\"" << nfex.getPath() << "\" setting not found in "
          "configuration file \n\"" << stdstrFullConfigFilePath << "\"\n"
          << "cause: " << nfex.what();
        std::cerr << std_oss.str() << std::endl;
        LOGN_ERROR(std_oss.str() )
        m_r_SMARTmonitorBase.ShowMessage(std_oss.str().c_str() );
        //setDefaultValues = true;
      }
    }
    catch (const libconfig::FileIOException & libConfigFileIOexc)
    {
      std::cerr << "I/O error while reading file \"" <<
        stdstrFullConfigFilePath << " \"" << std::endl;
      //return(EXIT_FAILURE);
      throw FileException(fullConfigFilePath.c_str() );
    }
    catch (const libconfig::ParseException & libConfigParseException)
    {
      const int errorlineNumber = libConfigParseException.getLine();
      const char * filePath = libConfigParseException.getFile();
      std::ostringstream stdoss;
      stdoss << "Parse error in file \n\"" << filePath << "\"\n, line " 
        << errorlineNumber << " : " << libConfigParseException.getError() ;
      LOGN_ERROR( stdoss.str() );
      //return false;
      m_r_SMARTmonitorBase.ShowMessage(stdoss.str().c_str() );
      ParseConfigFileException parseConfigFileException(
        fullFilePathOfThisExecutable.c_str(), errorlineNumber);
      throw parseConfigFileException;
      //return(EXIT_FAILURE);
    }
    return true;
  }

} /* namespace libConfig */
