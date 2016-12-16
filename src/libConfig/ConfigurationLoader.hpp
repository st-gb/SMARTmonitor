/*
 * ConfigurationLoader.hpp
 *
 *  Created on: 28.12.2014
 *      Author: mr.sys
 */

#ifndef LIBCONFIG_CONFIGURATIONLOADER_HPP_
#define LIBCONFIG_CONFIGURATIONLOADER_HPP_

#include "ConfigLoader/ConfigurationLoaderBase.hpp"
//#include <UserInterface/UserInterface.hpp> //Base class UserInterface
//#include <SMARTmonitorBase.hpp> //Base class SMARTmonitorBase
#include <set> //class std::set
#include <string> //class std::string

class SMARTmonitorBase;
namespace libconfig {
  class Setting;
}

namespace libConfig
{
  class ConfigurationLoader
    : public ConfigurationLoaderBase
  {
  private:
    SMARTmonitorBase & m_r_SMARTmonitorBase;
    std::set<std::string> attributeNamesFromConfigFile;
  public:
    ConfigurationLoader(
      std::set</*SkSmartAttributeParsedData*/SMARTentry> & smartAttributesToObserve,
      SMARTmonitorBase & r_userInterface);
    virtual
    ~ConfigurationLoader();
    
    void ReadNetworkConfig(const libconfig::Setting & root);
    bool LoadConfiguration(const std::wstring & fullFilePathOfThisExecutable);
    void GetSMARTattributesToObserve(const libconfig::Setting & root);
  };
} /* namespace libConfig */

#endif /* LIBCONFIG_CONFIGURATIONLOADER_HPP_ */
