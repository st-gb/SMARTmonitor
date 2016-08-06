/*
 * ConfigurationLoader.hpp
 *
 *  Created on: 28.12.2014
 *      Author: mr.sys
 */

#ifndef LIBCONFIG_CONFIGURATIONLOADER_HPP_
#define LIBCONFIG_CONFIGURATIONLOADER_HPP_

#include <ConfigurationLoaderBase.hpp>
#include <UserInterface/UserInterface.hpp>
#include <set> //class std::set
#include <string> //class std::string

namespace libConfig
{

  class ConfigurationLoader
    : public ConfigurationLoaderBase
  {
  private:
    const UserInterface & m_r_userInterface;
    std::set<std::string> attributeNamesFromConfigFile;
  public:
    ConfigurationLoader(
//      SMARTDETAILSMAP & oSMARTDetails,
      std::set<SkSmartAttributeParsedData> & smartAttributesToObserve,
      const UserInterface & r_userInterface);
    virtual
    ~ConfigurationLoader();
    bool LoadConfiguration(const std::wstring & fullFilePathOfThisExecutable);
  };

} /* namespace libConfig */

#endif /* LIBCONFIG_CONFIGURATIONLOADER_HPP_ */
