/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** File:   SMARTvalueFormatter.hpp
 * Author: sg
 * Created on 2. Januar 2017, 23:13 */

#ifndef SMARTVALUEFORMATTER_HPP
#define SMARTVALUEFORMATTER_HPP

#include <string> //class std::string
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType
#include <stdint.h> //uint64_t

class SMARTvalueFormatter {
public:
  SMARTvalueFormatter();
  SMARTvalueFormatter(const SMARTvalueFormatter& orig);
  virtual ~SMARTvalueFormatter();
  static std::string FormatHumanReadable(
    fastestUnsignedDataType SMARTattributeID, 
    const uint64_t & SMARTrawValue);
  static std::string GetDegCfromCurrMinMax(const uint64_t & SMARTrawValue);
  static std::string GetDegCfrom_mK(const uint64_t & SMARTrawValue);
  static std::string GetTimeFrom_h(const uint64_t & SMARTrawVal);
  static std::string GetTimeFrom_ms(const uint64_t & SMARTrawValue);
  static std::string GetNumberWithSIprefix(const uint64_t & SMARTrawVal);
private:

};

#endif /* SMARTVALUEFORMATTER_HPP */

