/*
 * ConfigurationLoader.hpp
 *
 *  Created on: 24.10.2014
 *      Author: mr.sys
 */

#ifndef CONFIGURATIONLOADER_HPP_
#define CONFIGURATIONLOADER_HPP_

#include <ConfigurationLoaderBase.hpp>

namespace Windows
{

  class ConfigurationLoader
    : public ConfigurationLoaderBase
  {
  public:
    ConfigurationLoader(const SMARTDETAILSMAP & oSMARTDetails);
    virtual
    ~ConfigurationLoader();

    bool LoadConfiguration(const std::string & s);
  };

} /* namespace Windows */

#endif /* CONFIGURATIONLOADER_HPP_ */
