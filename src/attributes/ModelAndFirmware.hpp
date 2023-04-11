#pragma once///Include guard

///Standard C(++) library files:
#include <limits>///std::numeric_limits
#include <string.h>///strcpy(...)

/** Files from Stefan Gebauer's common_sourcecode repository:*/
/**TU_Bln361095hardwareSMARTnumSNbytes, TU_Bln361095hardwareSMARTnumFWbytes,
 * TU_Bln361095hardwareSMARTnumModelBytes */
#include <hardware/dataCarrier/ATA3Std.h>
#include <hardware/CPU/fastest_data_type.h>///fastestUnsignedDataType

/**To avoid compile problem with MicroSoft Visual Studio in
 * "std::numeric_limits<T>::min()".*/
#undef min

class ModelAndFirmware{
protected:
  char m_firmWareName[TU_Bln361095hardwareSMARTnumFWbytes+1];
  char m_modelName[TU_Bln361095hardwareSMARTnumModelBytes+1];
  std::string SMARTparamUnit[numDifferentSMART_IDs];
  typedef fastestUnsignedDataType maxTBytesWrittenType; 
  maxTBytesWrittenType m_maxTeraBytesWritten;

public:
  typedef int64_t remainingTotalDataWrittenType;
  ModelAndFirmware() : m_maxTeraBytesWritten(0) {}
  ModelAndFirmware(
    const char modelName[TU_Bln361095hardwareSMARTnumModelBytes+1],
    const char firmWareName[TU_Bln361095hardwareSMARTnumFWbytes+1])
    : m_maxTeraBytesWritten(0)
  {
    strcpy(m_modelName, modelName);
    strcpy(m_firmWareName, firmWareName);
  }
  maxTBytesWrittenType GetMaxTeraBytesWritten() const{
    return m_maxTeraBytesWritten;}
  double calcTotalDataWrittenPercent(const uint64_t totalBytesWritten) const{
    double percent = -1.0;
    if(m_maxTeraBytesWritten > 0){
      percent = (double) totalBytesWritten / (double)
        m_maxTeraBytesWritten / 10000000000.0 /** 1 tera / 100 */;
    }
    return percent;
  }
  
  static const remainingTotalDataWrittenType maxTeraBytesWrittenNotSet =
    std::numeric_limits<remainingTotalDataWrittenType>::min();
  
  remainingTotalDataWrittenType getRemainingTotalDataWritten(
    const uint64_t numTotalBytesWritten) const
  {
    if(m_maxTeraBytesWritten > 0){
      ///Gets negative if numTotalBytesWritten > maxTeraBytesWritten
      return m_maxTeraBytesWritten * 1000000000000ULL /**1 Tera */ -
        numTotalBytesWritten;
    }
    return maxTeraBytesWrittenNotSet;
  }
  void Set(const char modelName[], const char firmWareName[]){
    strcpy(m_modelName, modelName);
    strcpy(m_firmWareName, firmWareName);
  }
  void setMaxTeraBytesWritten(const maxTBytesWrittenType maxTeraBytesWritten){
    m_maxTeraBytesWritten = maxTeraBytesWritten;
  }
  void setParamUnit(const fastestUnsignedDataType paramID, const char ar_chUnit[]){
    SMARTparamUnit[paramID] = ar_chUnit;
  }
  const char * getParamUnit(const fastestUnsignedDataType paramID) const {
    return SMARTparamUnit[paramID].c_str(); }
  bool operator < (const ModelAndFirmware & rhs) const{
    const int strcmpRslt = strcmp(m_modelName, rhs.m_modelName);
    if(strcmpRslt < 0)
      return true;
    else if(strcmpRslt == 0)
      return strcmp(m_firmWareName, rhs.m_firmWareName) < 0;
    else return false;
  }
};
