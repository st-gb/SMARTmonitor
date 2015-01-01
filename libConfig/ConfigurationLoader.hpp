/*
 * ConfigurationLoader.hpp
 *
 *  Created on: 28.12.2014
 *      Author: mr.sys
 */

#ifndef LIBCONFIG_CONFIGURATIONLOADER_HPP_
#define LIBCONFIG_CONFIGURATIONLOADER_HPP_

#include <ConfigurationLoaderBase.hpp>

namespace libConfig
{

  class ConfigurationLoader : public ConfigurationLoaderBase
  {
  public:
    ConfigurationLoader(SMARTDETAILSMAP & oSMARTDetails);
    virtual
    ~ConfigurationLoader();
    bool LoadConfiguration(const std::wstring & fullFilePathOfThisExecutable);
  };

} /* namespace libConfig */

#endif /* LIBCONFIG_CONFIGURATIONLOADER_HPP_ */
