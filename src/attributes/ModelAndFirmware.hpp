#pragma once///Include guard

#include <string.h>///strcpy(...)

///numSMART_SNbytes, numSMART_FWbytes, numSMARTmodelBytes, 
#include <hardware/dataCarrier/ATA3Std.h>

class ModelAndFirmware{
protected:
  char m_firmWareName[numSMART_FWbytes+1];
  char m_modelName[numSMARTmodelBytes+1];
  std::string SMARTparamUnit[numDifferentSMART_IDs];

public:
  ModelAndFirmware(){}
  ModelAndFirmware(
    const char modelName[numSMARTmodelBytes+1],
    const char firmWareName[numSMART_FWbytes+1])
  {
    strcpy(m_modelName, modelName);
    strcpy(m_firmWareName, firmWareName);
  }
  void Set(const char modelName[], const char firmWareName[]){
    strcpy(m_modelName, modelName);
    strcpy(m_firmWareName, firmWareName);
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
