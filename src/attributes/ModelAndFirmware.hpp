#pragma once///Include guard

///Standard C(++) library files:
 #include <limits>///std::numeric_limits
 #include <string.h>///strcpy(...)
 #include <string>///class std::string

/** Files from Stefan Gebauer's common_sourcecode repository:*/
 /**TU_Bln361095disableUseSecC_RunTimeStrFnWarn,
  * TU_Bln361095enableUseSecC_RunTimeStrFnWarn */
 #include <compiler/C,C++/useSecureC_RunTimeStrFuncsWarn.h>
 /**TU_Bln361095hardwareSMARTnumSNbytes, TU_Bln361095hardwareSMARTnumFWbytes,
  * TU_Bln361095hardwareSMARTnumModelBytes */
 #include <hardware/dataCarrier/ATA3Std.h>
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint

///_This_ project's repository files:
 ///TU_Bln361095SMARTmonNmSpcBgn, TU_Bln361095SMARTmonNmSpcEnd
 #include <SMARTmon_ID_prefix.h>

/**To avoid compile problem with MicroSoft Visual Studio in
 * "std::numeric_limits<T>::min()".*/
#undef min

#define TU_Bln361095hardwareSMARTnumFWbytesPlus1 \
  (TU_Bln361095hardwareSMARTnumFWbytes + 1)
#define TU_Bln361095hardwareSMARTnumModelBytesPlus1 \
  (TU_Bln361095hardwareSMARTnumModelBytes + 1)

TU_Bln361095SMARTmonNmSpcBgn

/**This class represents a certain data carrier model, not a distinct/unique
 * data carrier.*/
class ModelAndFirmware{
protected:
  char m_firmWareName[TU_Bln361095hardwareSMARTnumFWbytesPlus1];
  char m_modelName[TU_Bln361095hardwareSMARTnumModelBytesPlus1];
  std::string SMARTparamUnit[
    /**1 unit for every S.M.A.R.T. attribute. Use the largest possible array
     * size(the one for (S)ATA, NVMe has fewer) to cover all hardware cases.*/
    TU_Bln361095dataCarrierNumSATA_SMARTattrIDs];
  typedef TU_Bln361095::CPU::faststUint maxTBytesWrittenType;
  ///Typ=TYPe
  typedef TU_Bln361095::CPU::faststUint maxTeraBytesWrittenTyp;
  maxTBytesWrittenType m_maxTeraBytesWritten;

public:
  typedef int64_t remainingTotalDataWrittenType;
  ModelAndFirmware() : m_maxTeraBytesWritten(0) {}
  ModelAndFirmware(
    const char modelName[TU_Bln361095hardwareSMARTnumModelBytesPlus1],
    const char firmWareName[TU_Bln361095hardwareSMARTnumFWbytesPlus1])
    : m_maxTeraBytesWritten(0)
  {
    /**TODO validate:
     * Both source and destination character string have the same array size.
     * If the source character string has a terminating 0('\0') character then
     * using strcpy(...) without array size checks instead of strcpy_s(...) is
     * safe. */
    TU_Bln361095disableUseSecC_RunTimeStrFnWarn
    strcpy(m_modelName, modelName);
    strcpy(m_firmWareName, firmWareName);
    TU_Bln361095enableUseSecC_RunTimeStrFnWarn
  }
  maxTBytesWrittenType GetMaxTeraBytesWritten() const{
    return m_maxTeraBytesWritten;}
  double calcTotalDataWrittenPercent(const uint64_t totalBytesWritten) const
  {
    double percent = -1.0;
    if(m_maxTeraBytesWritten > 0){
      percent = (double) totalBytesWritten / (double)
        m_maxTeraBytesWritten / 10000000000.0 /** 1 tera / 100 */;
    }
    return percent;
  }
  
  static const remainingTotalDataWrittenType maxTeraBytesWrittenNotSet =
    std::numeric_limits<remainingTotalDataWrittenType>::min();
  
  template<typename SMARTrealCircaValTyp> /*remainingTotalDataWrittenType*/
    SMARTrealCircaValTyp
   getRemainingTotalDataWritten(
    const /*uint64_t*/SMARTrealCircaValTyp & numTotalBytesWritten) const
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
  void setParamUnit(
    const TU_Bln361095::CPU::faststUint paramID,
    const char ar_chUnit[])
  {
    SMARTparamUnit[paramID] = ar_chUnit;
  }
  const char * getParamUnit(const TU_Bln361095::CPU::faststUint paramID) const{
    return SMARTparamUnit[paramID].c_str(); }
  bool operator < (const ModelAndFirmware & rightOperand) const
  {
    const int strcmpRslt = strcmp(m_modelName, rightOperand.m_modelName);
    if(strcmpRslt < 0)
      return true;
    else if(strcmpRslt == 0)
      return strcmp(m_firmWareName, rightOperand.m_firmWareName) < 0;
    else return false;
  }
};

TU_Bln361095SMARTmonNmSpcEnd
