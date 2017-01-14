/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConfigLoader.hpp
 * Author: sg
 *
 * Created on 27. Dezember 2016, 20:14
 */

#ifndef CONFIGLOADER_HPP
#define CONFIGLOADER_HPP

#include <ConfigLoader/ConfigurationLoaderBase.hpp>

/** Forward declarations (compiles faster than #including header file(s). */
namespace tinyxml2
{
  class XMLElement;
  class XMLDocument;
}

namespace tinyxml2
{
  class ConfigLoader : public ConfigurationLoaderBase
  {
  public:
    ConfigLoader(
      std::set<SMARTentry> & smartAttributesToObserve,
      SMARTmonitorBase & r_userInterface);
    ConfigLoader(const ConfigLoader& orig);
    virtual ~ConfigLoader();
    
    void GetSMARTattributesToObserve(const tinyxml2::XMLElement * );
    tinyxml2::XMLElement * ReadNetworkConfig(tinyxml2::XMLElement * );
    bool LoadSMARTparametersConfiguration(
      const std::wstring & stdwstrWorkingDirWithConfigFilePrefix );
    tinyxml2::XMLElement * OpenConfigFile(
      const std::wstring & configFilePathWithoutFileExtension,
      std::string & stdstrFullConfigFilePath,
      tinyxml2::XMLDocument & tinyXML2Doc);
  private:

  };
}

#endif /* CONFIGLOADER_HPP */
