/** File:  ConfigLoader.cpp
 * Author: Stefan Gebauer,M.Sc.Comp.Sc./Informatik (TU Berlin)
 * Created on 27. Dezember 2016, 20:14 */
#include <stdlib.h>

#include "ConfigLoader.hpp" //class tinyxml2::ConfigLoader
///FileSystem::GetAbsolutePathA(...)
#include <FileSystem/File/GetAbsoluteFilePath.hpp>
#include <Controller/character_string/ConvertStdStringToTypename.hpp>
#include <tinyxml2.h>
#include <SMARTmonitorBase.hpp>
#include <attributes/ModelAndFirmware.hpp>///class ModelAndFirmware

namespace tinyxml2
{
ConfigLoader::ConfigLoader(
  SMARTmonitorBase::SMARTattrDefsType /*&*/ SMARTattrDefs,
  SMARTmonitorBase & r_userInterface)
  : ConfigurationLoaderBase(SMARTattrDefs, r_userInterface)
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
  const tinyxml2::XMLElement * p_tinyxml2XMLele)
{
//    const tinyxml2::XMLElement * SMART_parameters_to_observe =
  p_tinyxml2XMLele = p_tinyxml2XMLele->FirstChildElement(
    "SMART_parameters_to_observe");
  if( ! p_tinyxml2XMLele )
    return;
  //TODO better make a copy of the array (else it is being changed)?
  /*const*/ char * SMARTparamsToObs = (char *) p_tinyxml2XMLele->GetText();
  const int SMARTparamsToObserveCharLen = strlen(SMARTparamsToObs);
  char * SMARTparamsToObsNumBgn = SMARTparamsToObs;
  int SMARTattributeID;
  std::istringstream std_iss;
  //std::string std_strSMARTparamsToObserve(SMARTparamsToObserve);
  fastestUnsignedDataType SMARTattrIDtoObsIdx = 0;
  for (int charIndex = 0; charIndex < SMARTparamsToObserveCharLen; ++charIndex)
  {
    if( SMARTparamsToObs[charIndex] == ',' )
    {
      SMARTparamsToObs[charIndex] = '\0';
      if( ConvertCharStringToTypename(SMARTattributeID, SMARTparamsToObsNumBgn) )
      {
        //std_iss >> SMARTattributeID;
//          SMARTattributeID = strtol(SMARTparamsToObserveCharLen, SMARTparamsToObserve);
//          if(  )
        {
          LOGN_DEBUG("adding SMART attribute with ID " << SMARTattributeID
            << " as SMART parameter to observe/monitor")
        m_r_SMARTmonitorBase.m_IDsOfSMARTattrsToObserve.insert(SMARTattributeID);

        if(SMARTattrIDtoObsIdx < numDifferentSMART_IDs)
          m_r_SMARTmonitorBase.m_SMARTattrIDsToObs[SMARTattrIDtoObsIdx++] = 
            SMARTattributeID;
        }
      }
      SMARTparamsToObsNumBgn = SMARTparamsToObs + charIndex + 1;
    }
  }
  if(SMARTattrIDtoObsIdx < numDifferentSMART_IDs)
    m_r_SMARTmonitorBase.m_SMARTattrIDsToObs[SMARTattrIDtoObsIdx] = 0;
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
  SMARTattrDefAccss::clearSMARTattrDefs();

      SMARTattrDef sMARTattrDef;
        //libConfig.lookupValue("testImageFilePath", m_cfgData.testImageFilePath);

      ReadNetworkConfig(p_tinyxml2XMLelement);
  readModelAndFirmwareCfg(p_tinyxml2XMLelement);
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
      int sMARTattrID;
      std::string std_strCritical;
      std::string std_strAttribName, std_strAttribDetails;
      do
      {
        sMARTattrID = p_tinyxml2XMLelement->IntAttribute("Id", 0);
        if( sMARTattrID != 0 )
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
            std_strCritical = p_ch;
            LOGN_DEBUG("found \"Critical\" attribute.Value is:" << 
              std_strCritical )
            if( std_strCritical == "yes" )
              criticalSMARTattribute = true;
          }

          sMARTattrDef.SetCritical(criticalSMARTattribute);
          LOGN_DEBUG("Critical value for " << 
            sMARTattrID << ":" << criticalSMARTattribute )
          if( ! std_strAttribName.empty() )
          {
            sMARTattrDef.SetAttributeID(sMARTattrID);
//            attributeNamesFromConfigFile.insert(std_strAttribName);
//            std::set<std::string>::const_iterator citer =
//              attributeNamesFromConfigFile.find(std_strAttribName);
//            if( citer != attributeNamesFromConfigFile.end() )
              //use pointer from member var instead of from stack.
              sMARTattrDef.SetName(std_strAttribName.c_str() );

        SMARTattrDefAccss::Add(sMARTattrDef);
          LOGN_DEBUG( "using SMART entry at address " << 
        SMARTattrDefAccss::getSMARTattrDef(sMARTattrID) )
            LOGN("adding SMART ID" << sMARTattrID /*<< "to " 
              << mp_smartAttributes*/)
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

/** Model and firmware for a HDD/SSD determine the S.M.A.R.T. parameter units .
 * : if the firmware changes for a model then the parameter unit may change. */
void ConfigLoader::readModelAndFirmwareCfg(const tinyxml2::XMLElement *
  const p_rootXMLele)
{
  const tinyxml2::XMLElement * p_dataCarrierXMLele = p_rootXMLele->
    FirstChildElement("data_carrier");
  while(p_dataCarrierXMLele)
  {
    /** Value: specify '0' in order to retrieve the value */ //NULL);
    const char * p_chModel = p_dataCarrierXMLele->Attribute("model", NULL);
    const char * p_chFirmware = p_dataCarrierXMLele->Attribute("firmware",
      NULL);
    ModelAndFirmware modelAndFirmware;
    if(p_chModel && p_chFirmware){
      modelAndFirmware.Set(p_chModel, p_chFirmware);
      m_r_SMARTmonitorBase.m_modelAndFirmwareTuples.insert(modelAndFirmware);
    }
    const tinyxml2::XMLElement * p_tinyxml2XMLelement;
    p_tinyxml2XMLelement = p_dataCarrierXMLele->FirstChildElement("SMART");
    while(p_tinyxml2XMLelement){
      const int paramID = p_tinyxml2XMLelement->IntAttribute("paramID", 0);
      const char * p_chUnit = p_tinyxml2XMLelement->Attribute("unit", NULL);
      if(paramID != 0 && p_chUnit)
      {
        modelAndFirmware.setParamUnit(paramID, p_chUnit);
        LOGN("setting SMART param unit for ID " << paramID << "to " << p_chUnit)
      }
      ///Next sibling element must be called from sibling:
      /// http://www.grinninglizard.com/tinyxmldocs/classTiXmlHandle.html#f0643f8683f3f2b779b8c9d78c67b2c0
      p_tinyxml2XMLelement = p_tinyxml2XMLelement->NextSiblingElement("SMART");
    }
    p_dataCarrierXMLele = p_dataCarrierXMLele->NextSiblingElement("data_carrier");
  }
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
      m_r_SMARTmonitorBase.ShowMessage(std_oss.str().c_str(), UserInterface::
        MessageType::warning);
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
