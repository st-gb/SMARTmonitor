/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMARTvalueFormatter.hpp
 * Author: sg
 *
 * Created on 2. Januar 2017, 23:13
 */

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
  static std::string OutputTemperatureInCelsius(const uint64_t & SMARTrawValue);
  static std::string OutputPowerOnTimeAssumingMilliS(const uint64_t & SMARTrawValue);
private:

};

#endif /* SMARTVALUEFORMATTER_HPP */
