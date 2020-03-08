/** File:   ConfigLoader.cpp
 * Author: sg
 * Created on 27. Dezember 2016, 20:14 */
#include <stdlib.h>

#include "ConfigLoader.hpp" //class tinyxml2::ConfigLoader
#include "FileSystem/File/GetAbsoluteFilePath.hpp" 
#include <Controller/character_string/ConvertStdStringToTypename.hpp>
#include <tinyxml2.h>
#include <SMARTmonitorBase.hpp>

namespace tinyxml2
{
ConfigLoader::ConfigLoader(
  std::set<SMARTentry> & smartAttributesToObserve,
  SMARTmonitorBase & r_userInterface)
  : ConfigurationLoaderBase(smartAttributesToObserve, r_userInterface)
{
  
}

//ConfigLoader::ConfigLoader(const ConfigLoader& orig) {
//}

ConfigLoader::~ConfigLoader() {
}

  /** Affects both client and server. */
  tinyxml2::XMLElement * ConfigLoader::ReadNetworkConfig(
    tinyxml2::XMLElement * p_tinyxml2XMLelement)
  {
    unsigned portNumber;
    //TODO
    /** Both client and server need to use the same port. */
    p_tinyxml2XMLelement = p_tinyxml2XMLelement->
      FirstChildElement("networkConnection");
    if( p_tinyxml2XMLelement )
    {
      portNumber = p_tinyxml2XMLelement->IntAttribute("portNumber", 0);
      if( portNumber != 0 )
      {
        m_r_SMARTmonitorBase.m_socketPortNumber = portNumber;
        LOGN("setting port number to " << portNumber)
      }
      const char * p_chAttributeValue = p_tinyxml2XMLelement->Attribute("serviceHostName", 
        /** Value: specify '0' in order to retrieve the value */ NULL);
      if( p_chAttributeValue )
      {
        m_r_SMARTmonitorBase.m_stdstrServiceHostName = p_chAttributeValue;
        LOGN("setting service host name to " << p_chAttributeValue)
      }
    }
    return p_tinyxml2XMLelement;
  }
  
  void ConfigLoader::GetSMARTattributesToObserve(
    const tinyxml2::XMLElement * p_tinyxml2XMLelement)
  {
//    const tinyxml2::XMLElement * SMART_parameters_to_observe =
    p_tinyxml2XMLelement = p_tinyxml2XMLelement->FirstChildElement(
      "SMART_parameters_to_observe");
    if( ! p_tinyxml2XMLelement )
      return;
    //TODO better make a copy of the array (else it is being changed)?
    /*const*/ char * SMARTparamsToObserve = (char *) p_tinyxml2XMLelement->GetText();
    const int SMARTparamsToObserveCharLen = strlen(SMARTparamsToObserve);
    int SMARTattributeID;
    std::istringstream std_iss;
    //std::string std_strSMARTparamsToObserve(SMARTparamsToObserve);
    for (int charIndex = 0; charIndex < SMARTparamsToObserveCharLen; ++charIndex)
    {
      if( SMARTparamsToObserve[charIndex] == ',' )
      {
        SMARTparamsToObserve[charIndex] = '\0';
        if( ConvertCharStringToTypename(SMARTattributeID, SMARTparamsToObserve) )
        {
          //std_iss >> SMARTattributeID;
//          SMARTattributeID = strtol(SMARTparamsToObserveCharLen, SMARTparamsToObserve);
//          if(  )
          {
            LOGN_DEBUG("adding SMART attribute with ID " << SMARTattributeID
              << " as SMART parameter to observe/monitor")
            m_r_SMARTmonitorBase.m_IDsOfSMARTattributesToObserve.insert(SMARTattributeID);
          }
        }
      }
    }
  }

  bool ConfigLoader::LoadSMARTparametersConfiguration(
    //libconfig::Setting & root
    const std::wstring & stdwstrWorkingDirWithConfigFilePrefix )
  {
    std::string stdstrFullConfigFilePath;

    /** IMPORTANT: Need to hold the XMLDocument object here (outside of call to 
     *  OpenConfigFile) */
    tinyxml2::XMLDocument tinyXML2Doc;
    tinyxml2::XMLElement * p_tinyxml2XMLelement = OpenConfigFile(
      stdwstrWorkingDirWithConfigFilePrefix, stdstrFullConfigFilePath,
      tinyXML2Doc);
    if( ! p_tinyxml2XMLelement )
      return false;
    
    //const char * arch = p_tinyxml2XMLelement->Value();
//    m_smartAttributesToObserve.clear();
    mp_smartAttributes->clear();

      SMARTentry sMARTentry;
        //libConfig.lookupValue("testImageFilePath", m_cfgData.testImageFilePath);

      ReadNetworkConfig(p_tinyxml2XMLelement);
      GetSMARTattributesToObserve(p_tinyxml2XMLelement);

      p_tinyxml2XMLelement = p_tinyxml2XMLelement->FirstChildElement(
        "SMART_parameters");
      if( ! p_tinyxml2XMLelement )
      {
        m_r_SMARTmonitorBase.ShowMessage("no \"SMART_parameters\" element in XML file");
        return false;
      }
      p_tinyxml2XMLelement = p_tinyxml2XMLelement->FirstChildElement(
        "SMART_parameter");
      if( ! p_tinyxml2XMLelement )
      {
        m_r_SMARTmonitorBase.ShowMessage("no \"SMART_parameters\" element in XML file");
        return false;
      }
//      const int numCritical_SMART_parameters = p_tinyxml2XMLelement->.getLength();
//      std::tstring std_tstr;
      bool criticalSMARTattribute = false;
      
//      for(int SMARTparameterIndex = 0; SMARTparameterIndex
//        < numCritical_SMART_parameters; ++ SMARTparameterIndex)
      char * p_ch;
      int smartAttributeID;
      std::string std_strCritical;
      std::string std_strAttribName, std_strAttribDetails;
      do
      {
        smartAttributeID = p_tinyxml2XMLelement->IntAttribute("Id", 0);
        if( smartAttributeID != 0 )
        {
          p_ch = (char *) p_tinyxml2XMLelement->Attribute("Name", 
            /** Value: specify '0' in order to retrieve the value */ NULL);
          if( p_ch)
            std_strAttribName = p_ch;
//            const bool successfullyLookedUpDetails = critical_SMART_parameter.
//              lookupValue("Details", std_strAttribDetails);
          p_ch = (char *) p_tinyxml2XMLelement->Attribute("Critical", 
            /** Value: specify '0' in order to retrieve the value */ NULL);
          criticalSMARTattribute = false;
          if( p_ch )
          {
            LOGN_DEBUG("found \"Critical\" attribute.Value is:" << 
              std_strCritical )
            if( std_strCritical == "yes" )
              criticalSMARTattribute = true;
          }

          sMARTentry.SetCritical(criticalSMARTattribute);
          LOGN_DEBUG("Critical value for " << 
            smartAttributeID << ":" << criticalSMARTattribute )
          if( ! std_strAttribName.empty() )
          {
            sMARTentry.SetAttributeID(smartAttributeID);
//            attributeNamesFromConfigFile.insert(std_strAttribName);
//            std::set<std::string>::const_iterator citer =
//              attributeNamesFromConfigFile.find(std_strAttribName);
//            if( citer != attributeNamesFromConfigFile.end() )
              //use pointer from member var instead of from stack.
              sMARTentry.SetName(std_strAttribName.c_str() );

            /*m_smartAttributesToObserve.*/ mp_smartAttributes->insert(
              /*std::make_pair(smartAttributeID,*/ sMARTentry//)
              );
          LOGN_DEBUG( "using SMART entry at address " << 
            &  (* mp_smartAttributes->find(sMARTentry) ) )
            LOGN("adding SMART ID" << smartAttributeID << "to " 
                << mp_smartAttributes)
          }
        }
        else
        {
          std::ostringstream std_oss;
          std_oss << "warning: no \"Id\" parameter provided for SMART "
            "parameter in file \"" << stdstrFullConfigFilePath << "\"";
//            << ":" << critical_SMART_parameter.getSourceLine();
          m_r_SMARTmonitorBase.ShowMessage(std_oss.str().c_str() );
        }
        p_tinyxml2XMLelement = p_tinyxml2XMLelement->NextSiblingElement();
      }while(p_tinyxml2XMLelement );
    return true;
  }

  void ConfigLoader::ReadServiceConnectionSettings(
    const std::wstring & stdwstrWorkingDirWithConfigFilePrefix)
  {
    if( stdwstrWorkingDirWithConfigFilePrefix.empty() )
      return;
    std::string stdstrFullConfigFilePath = GetStdString_Inline(stdwstrWorkingDirWithConfigFilePrefix);
    /** IMPORTANT: Need to hold the XMLDocument object here (outside of call to 
     *  OpenConfigFile) */
    tinyxml2::XMLDocument tinyXML2Doc;
    tinyxml2::XMLElement * p_tinyxml2XMLelement = OpenConfigFile(
      stdstrFullConfigFilePath,
      tinyXML2Doc);
    if( ! p_tinyxml2XMLelement )
      return;
    
    ReadNetworkConfig(p_tinyxml2XMLelement);
  }

  tinyxml2::XMLElement * ConfigLoader::OpenConfigFile(
    const std::string & stdstrFullConfigFilePath,
    tinyxml2::XMLDocument & tinyXML2Doc)
  {
    // Read the file. If there is an error, report it and exit.
    const tinyxml2::XMLError xmlResult = tinyXML2Doc.LoadFile(
      stdstrFullConfigFilePath.c_str() );

    if( xmlResult != XML_SUCCESS)
    {
      std::string stdstrAbsFullConfigFilePath;
      FileSystem::GetAbsolutePathA(
        stdstrFullConfigFilePath.c_str(), stdstrAbsFullConfigFilePath );
      std::ostringstream std_oss;
      switch(xmlResult) {
        case XML_ERROR_FILE_NOT_FOUND :
          std_oss << "File \"" << stdstrAbsFullConfigFilePath << "\" not found";
          LOGN_ERROR(std_oss.str() )
          break;
        case XML_ERROR_FILE_COULD_NOT_BE_OPENED :
          std_oss << "Could not open file \"" << stdstrFullConfigFilePath << "\"";
          break;
        case XML_ERROR_FILE_READ_ERROR :
          std_oss << "Error reading from file \"" << stdstrFullConfigFilePath << "\"";
          break;
        default :
          if( xmlResult > XML_ERROR_FILE_READ_ERROR && 
              xmlResult < XML_ERROR_COUNT )
          {
            std_oss << "parse error reading from file \"" << 
              stdstrFullConfigFilePath << "\"";
          }
          else
            std_oss << "OTHER error reading from file \"" << 
              stdstrFullConfigFilePath << "\"";
          break;
      }
//    }
//    if( ! std_oss.str().empty() )
//    {
      std_oss << ":\n" /*<< tinyXML2Doc.GetErrorStr1()
        << "\n"*/ << tinyXML2Doc./*GetErrorStr2()*/ErrorStr();
      LOGN_ERROR(std_oss.str() )
      m_r_SMARTmonitorBase.ShowMessage(std_oss.str().c_str() );
      return NULL;
    }
    if( xmlResult > XML_ERROR_FILE_READ_ERROR && xmlResult < XML_ERROR_COUNT )
    {
//        const int errorlineNumber = ...getLine();
//        ParseConfigFileException parseConfigFileException(
//          configFilePathWithoutFileExtension.c_str(), errorlineNumber);
//        throw parseConfigFileException;
//        stdoss << "Parse error in file \n\"" << filePath << "\"\n, line " 
//          << errorlineNumber << " : " << libConfigParseException.getError() ;
      return NULL;
    }
    tinyxml2::XMLElement * p_tinyxml2XMLelement = tinyXML2Doc.RootElement();
    return p_tinyxml2XMLelement;
  }
  
  /** Also needed for the service: if e.g. only critical SMART parameters 
   *  should be observed. */
  tinyxml2::XMLElement * ConfigLoader::OpenConfigFile(
    const std::wstring & configFilePathWithoutFileExtension,
    std::string & stdstrFullConfigFilePath,
    tinyxml2::XMLDocument & tinyXML2Doc)
  {
  //  if(IsDebuggerPresent()==FALSE)
//      {
//      }
    std::wstring fullConfigFilePath = configFilePathWithoutFileExtension + L"xml";

    stdstrFullConfigFilePath = GetStdString_Inline(fullConfigFilePath);
    
    return OpenConfigFile(stdstrFullConfigFilePath, tinyXML2Doc);
    //throw FileException(fullConfigFilePath.c_str() );
  }
} //end namespace tinyxml2
