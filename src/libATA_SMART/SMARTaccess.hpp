/** SMARTaccess.hpp
 *  Created on: 31.07.2016
 *  Author: Sefan Gebauer, M.Sc. Comp. Sc., TU Berlin */

#ifndef LIBATA_SMART_SMARTACCESS_HPP_
#define LIBATA_SMART_SMARTACCESS_HPP_

#include <SMARTaccessBase.hpp> //base class SMARTreaderBase
#include <string> //class std::string
#include <vector> //class std::vector
#include <string.h>///strncpy(...)
///#defines numSMART_FWbytes, numSMARTmodelBytes, numSMART_SNbytes
#include <hardware/dataCarrier/ATA3Std.h>

#ifdef __linux__
  #include <stddef.h>///size_t in "<atasmart.h>"
  #include <atasmart.h>///struct SkSmartAttributeParsedData
#endif

///http://0pointer.de/blog/projects/being-smart.html
///see https://github.com/Rupan/libatasmart/blob/master/atasmart.c
namespace libatasmart
{
//  std::string g_stdStrAttributeName;

  struct attr_helper {
    uint64_t *value;
    SkBool found;
    fastestUnsignedDataType IDtoLookFor;
    std::string attributeName;
  };

  int getSupportedSMART_IDs(const char * const,
    std::vector<SMARTattributeNameAndID> & );

  static void getSMARTrawValueCallback(
    SkDisk * d,
    const SkSmartAttributeParsedData * a,
    struct libatasmart::attr_helper *ah);

  int readAttribute(/*const char attributeName []*/
    fastestUnsignedDataType SMARTattributeID,
    SkDisk * p_skDisk, 
    uint64_t & value);

class SMARTaccess
  : public SMARTaccessBase
{
  public:
    SMARTaccess(SMARTuniqueIDandValsContType &);
    virtual
    ~SMARTaccess ();

  //TODO needs quality assurance (return values from libatasmart functions)
  // therefore it needs the doc./sources
  void fill(const char device[], SMARTuniqueID & sMARTuniqueID)
  {
    SkDisk * p_skDisk;
    int i = sk_disk_open(device, /* SkDisk **_d*/ & p_skDisk);
    if( i != -1)
    {
//      LOGN_DEBUG("Disk \"" << device << "\" successfully opened.")
  //    i = sk_init_smart( & skDisk);
//        sk_disk_check_power_mode(p_skDisk);
      i = sk_disk_smart_read_data(p_skDisk);
      if( i == 0)
      {
//        LOGN_DEBUG("sk_disk_smart_read_data for \"" << device << "\" succeeded.")
        SkBool diskIdentityAvailable;
        SkBool SMARTavailable;
        i = sk_disk_smart_is_available(p_skDisk, & SMARTavailable);
        i = sk_disk_identify_is_available(p_skDisk, & diskIdentityAvailable);
        if(i == 0 && diskIdentityAvailable)
        {
//          LOGN_DEBUG("sk_disk_identify_is_available for \"" << device << 
//            "\" succeeded.")
          const SkIdentifyParsedData * p_SkIdentifyParsedData;
          i = sk_disk_identify_parse(p_skDisk, & p_SkIdentifyParsedData);
          if( i == 0)///<=>successfuly got disk info
          {
            const SkIdentifyParsedData & skIDparsedData = 
              *p_SkIdentifyParsedData;
            strncpy(sMARTuniqueID.m_firmWareName, skIDparsedData.firmware,
              numSMART_FWbytes+1);
            strncpy(sMARTuniqueID.m_modelName, skIDparsedData.model,
              numSMARTmodelBytes+1);
            strncpy(sMARTuniqueID.m_serialNumber, skIDparsedData.serial,
              numSMART_SNbytes+1);
          }
        }
      }
      sk_disk_free(p_skDisk);
    }
//    LOGN("SMART unique ID:" << str() )
  }

  enum SMARTaccessBase::retCodes readSMARTforDevice(const char device [],
    SMARTuniqueID &);
  int GetSupportedSMART_IDs(
    const char * const device,
    suppSMART_IDsType & SMARTattributeNamesAndIDs);
  enum SMARTaccessBase::retCodes ReadSMARTValuesForAllDrives(
    const fastestUnsignedDataType sMARTattrIDsToRead[]);
  void copySMARTvalues(const  SkDisk *, //const SkIdentifyParsedData *
    SMARTuniqueID & sMARTuniqueID, const char device []);
  };

} /* namespace libatasmart */

#endif /* LIBATA_SMART_SMARTACCESS_HPP_ */
