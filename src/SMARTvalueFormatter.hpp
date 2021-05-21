/** File:   SMARTvalueFormatter.hpp
 * Author: Stefan Gebauer,M.Sc.Comp.Sc.
 * Created on 2. Januar 2017, 23:13 */

#ifndef SMARTVALUEFORMATTER_HPP
#define SMARTVALUEFORMATTER_HPP

#include <string> //class std::string
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType
#include <stdint.h> //uint64_t

///Forward declaration
class ModelAndFirmware;

/**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes: 
 * Current temperature of SMART ID 194 (Device Temperature) is in the lowmost 2
 * bytes.*/
#define CurrTemp(SMARTrawValue) (SMARTrawValue & 0xFFFF)

class SMARTvalueFormatter {
public:
  SMARTvalueFormatter();
  SMARTvalueFormatter(const SMARTvalueFormatter& orig);
  virtual ~SMARTvalueFormatter();
  static std::string FormatHumanReadable(
    fastestUnsignedDataType SMARTattributeID, 
    const uint64_t & SMARTrawValue,
    const bool unitKnown,
    const ModelAndFirmware *);
  static std::string GetDegCfromCurr(const uint64_t & SMARTrawValue);
  static std::string GetDegCfromCurrMinMax(const uint64_t & SMARTrawValue);
  static std::string GetDegCfrom_mK(const uint64_t & SMARTrawValue);
  static std::string GetTimeFrom_h(const uint64_t & SMARTrawVal);
  static std::string GetTimeFrom_ms(const uint64_t & SMARTrawValue);
  static std::string GetNumberWithSIprefix(const uint64_t & SMARTrawVal);
private:

};

#endif /* SMARTVALUEFORMATTER_HPP */
