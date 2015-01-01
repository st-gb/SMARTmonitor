/*
 * ConfigurationLoaderBase.hpp
 *
 *  Created on: 24.10.2014
 *      Author: mr.sys
 */

#ifndef CONFIGURATIONLOADERBASE_HPP_
#define CONFIGURATIONLOADERBASE_HPP_

#include <SMARTDETAILSMAP.hpp>

class ConfigurationLoaderBase
{
//protected:
public:
  SMARTDETAILSMAP & m_oSMARTDetails;

  ConfigurationLoaderBase(/*const*/ SMARTDETAILSMAP & oSMARTDetails)
    : m_oSMARTDetails(oSMARTDetails)
  {

  }
};

#endif /* CONFIGURATIONLOADERBASE_HPP_ */
