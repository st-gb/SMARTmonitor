/** Author: sg
 * Created on 27. Dezember 2016, 20:14 */
#ifndef CONFIGLOADER_HPP
#define CONFIGLOADER_HPP

#include <ConfigLoader/ConfigurationLoaderBase.hpp>///base class
#include <attributes/SMARTattrDefsType.hpp>///typedef SMARTattrDefType

/** Forward declarations (compiles faster than #including header file(s). */
namespace tinyxml2
{
  class XMLElement;
  class XMLDocument;
}

typedef tinyxml2::XMLElement * loaderParamType;///For loadFuncType

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
    bool LoadSMARTcfg(
      const std::wstring & stdwstrWorkingDirWithConfigFilePrefix );
    
  tinyxml2::XMLElement * openCfgFile(
    const std::wstring & stdwstrWorkingDirWithConfigFilePrefix,
    tinyxml2::XMLDocument & tinyXML2Doc);
    tinyxml2::XMLElement * OpenConfigFile(
      const std::wstring & configFilePathWithoutFileExtension,
      std::string & stdstrFullConfigFilePath,
      tinyxml2::XMLDocument & tinyXML2Doc);
    tinyxml2::XMLElement * OpenConfigFile(
      const std::string & stdstrFullConfigFilePath,
      tinyxml2::XMLDocument & tinyXML2Doc);
  bool ReadSrvCnnctnCfg(
    std::wstring * stdwstrWorkingDirWithConfigFilePrefix = NULL,
    std::string * stdstrFullConfigFilePath = NULL,
    tinyxml2::XMLElement * p_tinyxml2XMLele = NULL
    );
  bool readSMARTattrDefs(
    std::wstring * p_stdwstrWorkingDirWithConfigFilePrefix = NULL,
    std::string * p_stdstrFullConfigFilePath = NULL,
    tinyxml2::XMLElement * p_tinyxml2XMLele = NULL
    );
  bool ReadSMARTdataCarrierDefs(
    std::wstring * p_stdwstrWorkingDirWithConfigFilePrefix = NULL,
    std::string * p_stdstrFullConfigFilePath = NULL,
    tinyxml2::XMLElement * p_tinyxml2XMLele = NULL);
    void ReadServiceConnectionSettings(const std::wstring & );
  private:

};
}

#endif /* CONFIGLOADER_HPP */

