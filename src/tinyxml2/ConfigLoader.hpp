/** Author: sg
 * Created on 27. Dezember 2016, 20:14 */
#ifndef CONFIGLOADER_HPP
#define CONFIGLOADER_HPP

#include <ConfigLoader/ConfigurationLoaderBase.hpp>
#include <SMARTmonitorBase.hpp>///typedef SMARTmonitorBase::SMARTattrDefType

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
      SMARTmonitorBase::SMARTattrDefsType /*&*/ SMARTattrDefs,
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
    tinyxml2::XMLElement * OpenConfigFile(
      const std::string & stdstrFullConfigFilePath,
      tinyxml2::XMLDocument & tinyXML2Doc);
    void ReadServiceConnectionSettings(const std::wstring & );
  private:

  };
}

#endif /* CONFIGLOADER_HPP */

