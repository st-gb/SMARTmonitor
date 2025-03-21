/**(c) from 2017 by Stefan Gebauer(Computer Science Master from TU Berlin)
 * @author Stefan Gebauer(TU Berlin matriculation number 361095)
 * Created on 2. Januar 2017, 23:13 */

#ifndef TU_Bln361095SMARTmon_SMARTvalFormatter_hpp
#define TU_Bln361095SMARTmon_SMARTvalFormatter_hpp

///C,C++ standard header files:
 #include <stdint.h>///uint64_t
 #include <string>///class std::string

///Stefan Gebauer's(TU Berlin matricul.numb.361095) ~"cmnSrc" repository files:
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint
 ///TU_Bln361095dataCarrierATA_SMARTcurrTemp
 #include <hardware/dataCarrier/ATA3Std.h>

///_this_ repository's (header) files:
 ///TU_Bln361095SMARTmonNmSpcBgn, TU_Bln361095SMARTmonNmSpcEnd
 #include <SMARTmon_ID_prefix.h>
 ///TU_Bln361095SMARTmonNmSpc::SMARTattr::rawValTyp
 #include "attributes/SMARTattr.hpp"

///Forward declarations:
TU_Bln361095SMARTmonNmSpcBgn
class ModelAndFirmware;
class SMARTuniqueID;
TU_Bln361095SMARTmonNmSpcEnd

TU_Bln361095SMARTmonNmSpcBgn

class SMARTvalueFormatter
{
public:
  SMARTvalueFormatter();
  SMARTvalueFormatter(const SMARTvalueFormatter& orig);
  virtual ~SMARTvalueFormatter();

  ///@brief Formats \p SMARTrawValue human readable
  ///@param SMARTrawVal raw value read from S.M.A.R.T. attribute
  template<typename SMARTvalTyp> inline static std::string
    ///Fmt=ForMaT
    FmtHumanReadable(
      const SMARTuniqueID & sMARTuniqueID,
      TU_Bln361095::CPU::faststUint SMARTattrID,
      const SMARTvalTyp & SMARTrawValue,
      const bool unitKnown,
      const ModelAndFirmware * p_modelAndFirmware)
  {
    switch(sMARTuniqueID.getBusType() )
    {
    case TU_Bln361095::hardware::bus::NVMe:
      return FmtNVMeValHumanReadable(
        SMARTattrID,
        SMARTrawValue,
        unitKnown,
        p_modelAndFirmware
        );
      break;
    default:
      return FmtATAvalHumanReadable(
        SMARTattrID,
        SMARTrawValue,
        unitKnown,
        p_modelAndFirmware
        );
    }
  }
  ///ForMaT ATA VALue HUMAN READABLE
  template<typename SMARTvalTyp> static std::string FmtATAvalHumanReadable(
    TU_Bln361095::CPU::FaststUint SMARTattributeID, 
    const /*uint64_t*/SMARTvalTyp & SMARTrawValue,
    const bool unitKnown,
    const TU_Bln361095::SMARTmon::ModelAndFirmware *);
  ///ForMaT NVMe VALue HUMAN READABLE
  /*template<typename SMARTattrRawValTyp>*/ static std::string
   FmtNVMeValHumanReadable(
    const TU_Bln361095::CPU::FaststUint SMARTattrID,
    const /* uint8_t* */ /*SMARTattrRawValTyp*/
      TU_Bln361095SMARTmonNmSpc::SMARTattr::rawValTyp & SMARTval,
    const bool unitKnown,
    const TU_Bln361095::SMARTmon::ModelAndFirmware * p_modelAndFirmware);
  ///For (S)ATA (and not NVMe) S.M.A.R.T.:
  static std::string GetDegCfromCurr(const uint64_t & SMARTrawValue);
  static std::string GetDegCfromCurrMinMax(const uint64_t & SMARTrawValue);
  static std::string GetDegCfrom_mK(const uint64_t & SMARTrawValue);
  static std::string GetTimeFrom_h(const uint64_t & SMARTrawVal);
  static std::string GetTimeFrom_ms(const uint64_t & SMARTrawValue);
  static std::string GetNumberWithSIprefix(const uint64_t & SMARTrawVal);
  static std::string TempDiffOrAirflowTemp_DegCforCurrMinMax(const uint64_t &);
private:

};

#endif///include guard

TU_Bln361095SMARTmonNmSpcEnd
