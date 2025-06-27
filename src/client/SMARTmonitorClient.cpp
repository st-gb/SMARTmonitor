/**Author:Stefan Gebauer,Computer Science Master(TU Berlin matric.number 361095)
 * Created on 20. November 2016, 17:43 UTC+1*/

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
 /**TU_Bln361095TU_Bln361095GCCdisableWarn(...),
  * TU_Bln361095TU_Bln361095GCCdisableWarn(...) */
 //#include <compiler/C,C++/enableAndDisableWarn.h>
 ///TU_Bln361095::CPU::atomicXchg(...)
 #include <hardware/CPU/atomic/AtomicExchange.h>
#include <hardware/CPU/atomic/memory_barrier.h>///memory_barrier(...)
///OperatingSystem::GetLastErrorCode()
#include <OperatingSystem/GetLastErrorCode.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h>///LOGN(...)

///This repository's header files:
#include "SMARTmonitorClient.h"///this class' header file
///SMARTvalueFormatter::FormatHumanReadable(...)
#include <SMARTvalueFormatter.hpp>
#include <UserInterface/UserInterface.hpp>///class UserInterface

///Standard C(++) header files:
#include <stdint.h> //uint8_t

using namespace TU_Bln361095SMARTmonNmSpc;///for SMARTuniqueID
/** Static/class variable defintion: */
fastestUnsignedDataType SMARTmonitorClient::s_UIthreadID;
fastestUnsignedDataType SMARTmonitorClient::s_updateUI = 1;
#ifdef TU_Bln361095SMARTmonMultithread
//nativeThread_type SMARTmonitorClient::s_updateSMARTparameterValuesThread;
#endif
enum SMARTvals::Rating::E SMARTmonitorClient::
  s_entireSMARTstatus =TU_Bln361095SMARTmonNmSpc::SMARTvals::Rating::unknown;
fastestUnsignedDataType SMARTmonitorClient::s_maxNumCharsNeededForDisplay [] =
 { 3, 30, 15, 20, 40};
fastestUnsignedDataType SMARTmonitorClient::s_charPosOAttrNameBegin [] =
 { 0, 3, 33, 48, 68};
//TU_Bln361095GCCdisableWarn(write-strings)
const char * const SMARTmonitorClient::s_columnAttributeNames[] =
 { "ID", "name", "raw", "human readable", "last update"};
//TU_Bln361095GCCenableWarn(write-strings)
SMARTvalueRater SMARTmonitorClient::s_SMARTvalueRater;

SMARTmonitorClient::SMARTmonitorClient()
#if defined TU_Bln361095useClntSrv || defined _DEBUG
  :
#endif
#if defined TU_Bln361095useClntSrv
  m_srvCnnctnCntDownInSec(60)
#endif
#if defined TU_Bln361095useClntSrv && defined _DEBUG
  ,
#endif
#ifdef _DEBUG
  GetSMARTvalsAndUpd8UIthreadID(0)
#endif
{
  m_getSMARTvaluesFunctionParams.p_SMARTmonitorBase = this;
  /** Setting seconds to an invalid number (100) indicates that the time
   *   hasn't been retrieved yet. */
  m_timeOfLastSMARTvaluesUpdate.tm_sec = 100;
//  for( int i = 0; i < COL_IDX_beyondLast ; i++ )
//  {
//    s_charPosOAttrNameBegin[i] += 
//  }
}

SMARTmonitorClient::SMARTmonitorClient(const SMARTmonitorClient& orig) {
}

SMARTmonitorClient::~SMARTmonitorClient() {
}

#ifdef TU_Bln361095SMARTmonMultithread
void SMARTmonitorClient::EndUpdateUIthread()
{
  if(m_updateSMARTparameterValuesThread.IsRunning() )
  {
    /** Inform the SMART values update thread about we're going to exit,*/
    //m_wxCloseMutex.TryLock();
    LOGN("disabling update UI (ends update UI thread)");
    waitOrSignalEvt.Broadcast();
    TU_Bln361095::CPU::atomicXchg( (long *) & s_updateSMARTvalues, 0);
    
    LOGN("waiting for close event");
    /** http://docs.wxwidgets.org/trunk/classwx_condition.html : 
      "Wait() atomically unlocks the mutex which allows the thread to continue "
      "and starts waiting */
    //m_p_wxCloseCondition->Wait();
    
    SetCurrentAction(WaitForSMARTupd8ThreadTerm);
    m_updateSMARTparameterValuesThread.WaitForTermination();
    SetCurrentAction(AfterWaitForSMARTupd8ThreadTerm);
    ///Enable get S.M.A.R.T. values loop.
    TU_Bln361095::CPU::atomicXchg( (long *) & s_updateSMARTvalues, 1);
    TU_Bln361095::CPU::atomicXchg( (long *) & GetSMARTvalsAndUpd8UIthreadID, 0);
#if TU_Bln361095SMARTmonDrctSMARTaccss
    if(getsSMARTdataDrctly() )
      ChangeConnectionState(endedDrctSMART);
    else
#endif
      ChangeConnectionState(uncnnctdToSrv);
  }
}
#endif

/** These values are fixed and so need to be shown only once in the user
  * interface (and not at/for every SMART values update -> saves resources) */
void SMARTmonitorClient::SetSMARTattribIDandNameLabel()
{
  LOGN_DEBUG("begin")
  fastestUnsignedDataType SMARTattributeID;
  
  SMARTuniqueIDandValsContType & SMARTuniqueIDsAndValues =
    GetSMARTuniqueIDsAndVals();
  for(SMARTuniqueIDandValsContType::const_iterator sMARTuniqueIDandValsIter =
    SMARTuniqueIDsAndValues.begin(); sMARTuniqueIDandValsIter !=
    SMARTuniqueIDsAndValues.end(); sMARTuniqueIDandValsIter ++)
  {
    const SMARTuniqueID & sMARTuniqueID = sMARTuniqueIDandValsIter->
      getSMARTuniqueID();
    /** Traverse intersection of all SMART attribute IDs to read and supported
     *  SMART IDs either got from server or read from config file.*/
    for(fastestUnsignedDataType SMART_IDsToReadIdx = 0;
      sMARTuniqueID.SMART_IDsToReadNotEnd(SMART_IDsToReadIdx);
      SMART_IDsToReadIdx++)
    {
      SMARTattributeID = sMARTuniqueID.m_SMART_IDsToRd[SMART_IDsToReadIdx];
      setIDandLabel(sMARTuniqueID, SMARTattributeID, NULL);
    }
    //TODO regard multiple data carriers
    break;///Only set ID and label for 1 data carrier
  }
  LOGN_DEBUG("end")
}

///\param data: For supported S.M.A.R.T. IDs dialog list control
void SMARTmonitorClient::setIDandLabel(
  const SMARTuniqueID & sMARTuniqueID,
  const fastestUnsignedDataType SMARTattrID,
  void * data)
{
  std::ostringstream std_oss;
  std_oss << SMARTattrID;
  SetAttribute(
    sMARTuniqueID,
    SMARTattrID,
    colIndices::SMART_ID,
    std_oss.str(),
    TU_Bln361095SMARTmonNmSpc::SMARTvals::Rating::noCriticalVal,
    data
    );

  /** Now get the attribute name belonging to SMART ID */
  SMARTattrDef * p_sMARTattrDef;
  switch(sMARTuniqueID.getBusType() )
  {
  case TU_Bln361095::hardware::bus::NVMe:
   p_sMARTattrDef = SMARTattrDefAccss::getSMARTattrDef(
    SMARTattrID, SMARTattrDefAccss::NVMeSMARTattrDefs);
   break;
  default:
   p_sMARTattrDef = SMARTattrDefAccss::getSMARTattrDef(
    SMARTattrID, SMARTattrDefAccss::SMARTattrDefs);
  }
  std::string stdstrSMARTattrName;
  if( p_sMARTattrDef != NULL)
  {
    const SMARTattrDef & sMARTattrDef = *p_sMARTattrDef;
    stdstrSMARTattrName = sMARTattrDef.GetName();
  }
    //SMARTattributeToObserve.name
  SetAttribute(
    sMARTuniqueID,
    SMARTattrID,
    colIndices::SMARTparameterName,
    stdstrSMARTattrName,
    TU_Bln361095SMARTmonNmSpc::SMARTvals::Rating::noCriticalVal
    ,data
    );
}

void SMARTmonitorClient::UpdateTimeOfSMARTvalueRetrieval(
  const SMARTuniqueID & sMARTuniqueID,
  const fastestUnsignedDataType SMARTattributeID,
  const uint64_t timeStampOfRetrievalInMs, void * data)
{  
  std::string timeFormatString;
  UserInterface::FormatTime(
    timeStampOfRetrievalInMs, 
    timeFormatString);
  SetAttribute(
    sMARTuniqueID,
    SMARTattributeID,
    colIndices::lastUpdate /** column #/ index */,
    timeFormatString,
    TU_Bln361095SMARTmonNmSpc::SMARTvals::Rating::noCriticalVal,
    data
    );
}

///Updates the user interface (UI), e.g. for GUIs/TUIs it updates the cells
/// of the S.M.A.R.T. values table.
void SMARTmonitorClient::UpdateSMARTvaluesUI()
{
  LOGN_DEBUG("begin")
  bool atLeast1CriticalNonNullValue = false;

  const fastestUnsignedDataType numberOfDifferentDrives = 
    SMARTuniqueIDsAndValues.size();

  //memory_barrier(); //TODO necessary at all??
  LOGN("SMART unique ID and values container:" << & SMARTuniqueIDsAndValues )
  std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
    SMARTuniqueIDsAndValues.begin();
  fastestUnsignedDataType SMARTattrID;
//  const numSMARTattributeIDbits = sizeof(SMARTattributeID) * 8;
#ifdef DEBUG
#endif
  //memory_barrier(); //TODO: not really necessary??
  
  SMARTvalRatngTyp SMARTvalRatng;
  ///Set to unknown at begin because no single SMART value may be read.
  enum SMARTvals::Rating::E entireSMARTvalRating =
    TU_Bln361095SMARTmonNmSpc::SMARTvals::Rating::unknown;
  /** Loop over data carriers. */
  //TODO (only) 1 item here when wxGUI started as root?
  for(fastestUnsignedDataType currentDriveIndex = 0;
    SMARTuniqueIDandValuesIter != SMARTuniqueIDsAndValues.end() ;
    SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueID & sMARTuniqueID = SMARTuniqueIDandValuesIter->
      getSMARTuniqueID();
    LOGN("SMART unique ID and values object " << &(*SMARTuniqueIDandValuesIter) )
    
    const ModelAndFirmware * p_currModelAndFirmware = getDataCarrierAttrDefs(
      sMARTuniqueID);

    const fastestUnsignedDataType * SMART_IDsToRd = sMARTuniqueID.
      m_SMART_IDsToRd;
    /** Loop over attribute IDs to observe */ //TODO if list is empty nothing is updated
    for(fastestUnsignedDataType SMARTattrIDtoReadIdx = 0;
      sMARTuniqueID.SMART_IDsToReadNotEnd(SMARTattrIDtoReadIdx);
      SMARTattrIDtoReadIdx++)
    {
      SMARTattrID = SMART_IDsToRd[SMARTattrIDtoReadIdx];
#ifdef _DEBUG///For debugging
      const SMARTuniqueIDandValues & sMARTuniqueIDandVals = 
        *SMARTuniqueIDandValuesIter;
#endif
      SMARTvalRatng = upd8rawAndH_andTime(SMARTattrID,
        *SMARTuniqueIDandValuesIter, NULL, p_currModelAndFirmware);
      if(///Negative if normalized current value < normalized threshold
        SMARTvalRatng < SMARTvalAbs1RngWarnThresh)
        entireSMARTvalRating = TU_Bln361095SMARTmonNmSpc::SMARTvals::Rating::
          atLeast1Warn;
    }
  }
  /** After hitting "Disconnect" some more values updates may arrive here.
   * In this case don't change the state to let the state at "disconnected" to
   * be able to connect again. */
  if(m_updateSMARTparameterValuesThread.IsRunning() )///Only if not cancelled.
    ChangeConnectionState(valUpd8);
  /** ^= state changed. */
  if(s_entireSMARTstatus != entireSMARTvalRating)
  {
    ShowStateAccordingToSMARTvalues(entireSMARTvalRating);
  }
  s_entireSMARTstatus = entireSMARTvalRating;
  LOGN_DEBUG("end")
}

template<typename SMARTattrRawValTyp, typename realCircaSMARTattrRawValTyp> bool
  getRealValue(
  const std::string & stdstrUnit,
  const SMARTattrRawValTyp & SMARTattrRawVal,
  realCircaSMARTattrRawValTyp & realCircaSMARTattrRawVal)
{
  bool error = false;
  uint64_t currNum = 0;
  const fastestUnsignedDataType numChars = stdstrUnit.size();
  const char * ar_chUnit = stdstrUnit.c_str();
//  const char * ar_chUnitBegin = NULL, * ar_chUnitEnd = NULL;
  bool isNumber = false;
  char ch;
  realCircaSMARTattrRawVal = 0;
  for(TU_Bln361095::CPU::faststUint unitCharStrIdx = 0; unitCharStrIdx <
    numChars; unitCharStrIdx++)
  {
    ch = ar_chUnit[unitCharStrIdx];
    switch(ar_chUnit[unitCharStrIdx])
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if(! isNumber){
//          ar_chUnitBegin = ar_chUnit + unitCharStrIdx;
          isNumber = true;
          currNum = 0;
        }
        else
//        i = atoi();
          currNum *= 10;
        currNum += (ch - '0');
        break;
      default:
        if(isNumber){
//          ar_chUnitEnd = ar_chUnit + unitCharStrIdx;
          isNumber = false;
        }
        switch(ch){
          case 'B':///Needs to be contained for not to be in "default" branch.
            break;
          case 'G':///giga=10^9
            currNum *= 1000000000;
            break;
          case 'h':///1 hour=60min/h*60s/min=3600 s 
            currNum *= 3600000;
            break;
          case 's':///1 second=1000 ms
            currNum *= 1000;
            break;
         case -62:/// '°' in UTF-8 = -61 -80
          if(unitCharStrIdx != 0)
            error = true;
          break;
         case -80:/// '°' in UTF-8 = -61 -80
          if(unitCharStrIdx != 1)
            error = true;
          break;
         case 'C':/// '°' in UTF-8 = -61 -80
          if(unitCharStrIdx != 2)
            error = true;
          else
            currNum = 1;
          break;
          default:
            error = true;
        }
        //if(! error)
        realCircaSMARTattrRawVal += SMARTattrRawVal * currNum;
    }
  }
  return error;
}


/**@tparam SMARTattrRawValTyp may be boost::multiprecision for NVMe 16 byte
 *  S.M.A.R.T. raw value
 * @brief gets the real circa S.M.A.R.T. attribute raw value by using the
 *   determined S.M.A.R.T. attribute raw value unit */
template<typename SMARTattrRawValTyp, typename realCircaSMARTattrRawValTyp>
 inline void
 useDeterminedUnits(
  const SMARTuniqueID & sMARTuniqueID,
  const fastestUnsignedDataType SMARTattrID,
  const SMARTattrRawValTyp & SMARTattrRawVal,
  long & unit,
  realCircaSMARTattrRawValTyp & realCircaSMARTattrRawVal,
  //TODO data types should be realCircaSMARTattrRawValTyp/SMARTattrRawValTyp?!
  double & accuracy,
  double & lowerUnitLimit,
  double & upperLimit,
  std::ostringstream & std_ossUnit,
  std::ostringstream & std_ossUnitAccuracy,/// unit accuracy as "from - to"
  std::string & stdstrHumanReadableRawVal,
  const ModelAndFirmware * p_currModelAndFirmware
  )
{
  const SMARTuniqueID::unitDataType bitMaskForHighestBit =
    GetBitMaskForMostSignificantBit(unit);
  const SMARTuniqueID::unitDataType unMaskedUnit = SMARTuniqueID::
    GetUnMaskedValue(unit, bitMaskForHighestBit);
  if(unMaskedUnit)///If unit is determined <=> > 0 after bit removed
  {
    lowerUnitLimit = (double) sMARTuniqueID.lowerUnitBound[SMARTattrID];
    if(unit & bitMaskForHighestBit){///If highmost bit set / ">="
      std_ossUnit << ">=~";
      unit = unMaskedUnit;///Without highmost bit
      stdstrHumanReadableRawVal = ">=~";
      if(lowerUnitLimit != 0.0)/** Lower unit bound set*/{
        std::string humanReadableAccuracy = SMARTvalueFormatter::
          //TODO also show percentage of TBW if upper unit bound is unknown?
          FmtHumanReadable(sMARTuniqueID, SMARTattrID, 
            (TU_Bln361095::SMARTmon::SMARTattr::rawValTyp) lowerUnitLimit, true,
            NULL);
        std_ossUnitAccuracy << ">=~" << humanReadableAccuracy;
        ///Value can get high: 16230×3618000 = 58720140000
        realCircaSMARTattrRawVal = SMARTattrRawVal * (uint64_t) lowerUnitLimit;
      }
      else
        ///Value can get high: 16230×3618000 = 58720140000
        realCircaSMARTattrRawVal = SMARTattrRawVal * (uint64_t) unit;
    }
    else{
      std_ossUnit << "~";
      stdstrHumanReadableRawVal = "~";
      ///Value can get high: 16230×3618000 = 58720140000
      realCircaSMARTattrRawVal = SMARTattrRawVal * (uint64_t) unit;
      upperLimit = (double) sMARTuniqueID.upperUnitBound[SMARTattrID];
      if(upperLimit != 0.0)///Prevent division by 0.
        accuracy = lowerUnitLimit / upperLimit;
    }
    std_ossUnit << SMARTvalueFormatter::FmtHumanReadable(sMARTuniqueID,
      SMARTattrID, unit, true, NULL);

    stdstrHumanReadableRawVal += SMARTvalueFormatter::
      FmtHumanReadable(sMARTuniqueID, SMARTattrID, realCircaSMARTattrRawVal,
        true, p_currModelAndFirmware);
    if(accuracy != 0.0){
      std::string humanReadableAccuracy = SMARTvalueFormatter::
        FmtHumanReadable(sMARTuniqueID, SMARTattrID,
          (TU_Bln361095::SMARTmon::SMARTattr::rawValTyp) lowerUnitLimit, true,
          NULL);
      std_ossUnitAccuracy << " " << std::fixed << humanReadableAccuracy <<
        "-";/** "..." */
      humanReadableAccuracy = SMARTvalueFormatter::FmtHumanReadable(
        sMARTuniqueID, SMARTattrID,
        (TU_Bln361095::SMARTmon::SMARTattr::rawValTyp) upperLimit, true, NULL);
      std_ossUnitAccuracy << std::fixed << /* "]" */ humanReadableAccuracy;
    }
  }
  else{///Unknown unit/use default unit
    /*uint64_t*/ SMARTattrRawValTyp numForHumanReadableFormat;
    switch(sMARTuniqueID.getBusType() )
    {
    case TU_Bln361095::hardware::bus::NVMe:
      TU_Bln361095::SMARTmon::NVMe::SMART::AttrVal::useDefaultUnit(SMARTattrID,
        realCircaSMARTattrRawVal, SMARTattrRawVal, std_ossUnit,
        numForHumanReadableFormat);
      stdstrHumanReadableRawVal = TU_Bln361095::SMARTmon::NVMe::SMART::AttrVal::
        FmtHumanReadable(
          SMARTattrID,
          //(uint8_t*) &numForHumanReadableFormat
          /*SMARTattrRawVal*/ numForHumanReadableFormat, false, NULL);
      break;
    default:
      TU_Bln361095::SMARTmon::ATA::SMART::AttrVal::useDefaultUnit(SMARTattrID,
        realCircaSMARTattrRawVal, SMARTattrRawVal, std_ossUnit,
        numForHumanReadableFormat);
      stdstrHumanReadableRawVal = TU_Bln361095::SMARTmon::ATA::SMART::AttrVal::
        FmtHumanReadable(
          SMARTattrID, numForHumanReadableFormat, false, NULL);
      break;
    }
  }
}

/** \param data e.g. a list control. A "void" pointer (alternative: subclassung)
 * to enable different UI control classes .*/
SMARTvalRatngTyp SMARTmonitorClient::upd8rawAndH_andTime(
  const fastestUnsignedDataType SMARTattrID,
  const SMARTuniqueIDandValues & SMARTuniqueIDandVals,
  void * data,
  const ModelAndFirmware * p_modelAndFirmware)
{
  LOGN_DEBUG("begin--S.M.A.R.T. attribute ID:" << SMARTattrID)
  SMARTvalRatngTyp sMARTvalueRating;
  uint64_t SMARTrawVal;
  //TODO attribute IDs of SMART values to observe may not be a subset of
  // SMART attributes in config file!
//  SMARTattrDef * p_sMARTattrDef = SMARTattrDefAccss::getSMARTattrDef(
//    SMARTattrID);
//  if(p_sMARTattrDef){
  const TU_Bln361095::SMARTmon::SMARTattr & sMARTattr = SMARTuniqueIDandVals.
    m_SMARTattrs[SMARTattrID];
  const SMARTuniqueID & sMARTuniqueID = SMARTuniqueIDandVals.
    getSMARTuniqueID();
  bool isConsistent = sMARTvalue.IsConsistent(SMARTrawVal);
  if(sMARTattr.m_successfullyReadSMARTrawValue)
  {
  /**Make a copy of the raw value because it may be changed by the "get 
   * S.M.A.R.T. values" thread*/
    const TU_Bln361095::CPU::faststUint numSMARTattrRawValB = sMARTuniqueID.
      getNumSMARTattrRawValB(SMARTattrID);

#ifdef TU_Bln361095useBoostMultiprecisionCppInt
#endif
    SMARTattr::rawValTyp SMARTattrRawVal;
    sMARTattr.getRawVal(/*(uint8_t*)&SMARTattrRawVal, numSMARTattrRawValB*/
      SMARTattrRawVal);

//      memory_barrier(); //TODO: not really necessary??
  int successfullyUpdatedSMART = sMARTattr.m_successfullyReadSMARTrawValue;

  uint64_t upTimeOfRetrievalInMs;
  /** Also fails if client wants attribute ID but service did not send raw
   * S.M.A.R.T. values. */
  if(! sMARTattr.GetRetrievalTime(upTimeOfRetrievalInMs) )
    upTimeOfRetrievalInMs = 0;
  //memory_barrier(); //TODO: not really necessary??
  if( /*successfullyUpdatedSMART*/ isConsistent && upTimeOfRetrievalInMs)
  {
//    SMARTattrDef & sMARTattrDef = *p_sMARTattrDef;
    std::string stdstrHumanReadableRawVal;
    
  //TODO move following code so it can be used in another place when not
  // SMARTmonitorClient-derived
    std::ostringstream std_ossUnit, std_ossUnitAccuracy;
    long unit = sMARTuniqueID.units[SMARTattrID];
    /**The real value can get bigger than the raw value because if the unit 
     * prefix is greater than 1 then the raw value is multiplied by it.
     * So use a data type that can be bigger than uint64_t */
#ifdef TU_Bln361095useBoostMultiprecisionCppInt
    /*uint64_t*/boost::multiprecision::cpp_int realCircaSMARTattrRawVal;
#endif
    double accuracy = 0.0;
    double lowerUnitLimit = 0.0, upperLimit;
    std::string stdstrUnit;
    bool useDeterminedUnit = true;
    if(p_modelAndFirmware)
    {
      stdstrUnit = p_modelAndFirmware->getParamUnit(SMARTattrID);
      if( stdstrUnit != "" )
      {
        if(! getRealValue(stdstrUnit, SMARTattrRawVal, realCircaSMARTattrRawVal)
          )
        {
          std_ossUnit << p_modelAndFirmware->getParamUnit(SMARTattrID);
          useDeterminedUnit = false;
          stdstrHumanReadableRawVal += SMARTvalueFormatter::
            FmtHumanReadable(sMARTuniqueID, SMARTattrID,
              realCircaSMARTattrRawVal, true, p_modelAndFirmware);
        }
      }
    }
    if(useDeterminedUnit)
      useDeterminedUnits(sMARTuniqueID, SMARTattrID, SMARTattrRawVal,
        unit, realCircaSMARTattrRawVal,
        accuracy,
        lowerUnitLimit, upperLimit,
        std_ossUnit, std_ossUnitAccuracy,
        stdstrHumanReadableRawVal,
        p_modelAndFirmware);
    std::ostringstream std_ossRawSMARTval;
    switch(sMARTuniqueID.getBusType() )
    {
    case TU_Bln361095::hardware::bus::NVMe:
      TU_Bln361095::SMARTmon::NVMe::SMART::AttrVal::fmtRawVal(SMARTattrID,
        SMARTattrRawVal, std_ossRawSMARTval);
      break;
    default:
      TU_Bln361095::SMARTmon::ATA::SMART::AttrVal::fmtRawVal(SMARTattrID,
        SMARTattrRawVal, std_ossRawSMARTval);
    }
    stdstrUnit = std_ossUnit.str();
    std::ostringstream std_ossNrmlzdCurrSMARTval, std_ossNrmlzdThreshVal;
    /**According to struct "NVME_HEALTH_INFO_LOG" in file "nvme.h" (see
http://learn.microsoft.com/en-us/windows/win32/api/nvme/ns-nvme-nvme_health_info_log
     * ) S.M.A.R.T. via NVMe does not have normalized current, worst and
     * threshold values per attribute ID.*/
    if(sMARTuniqueID.getBusType() != TU_Bln361095::hardware::bus::NVMe){
      std_ossNrmlzdCurrSMARTval << sMARTattr.GetNrmlzdCurrVal();
      std_ossNrmlzdThreshVal << sMARTattr.GetNrmlzdThresh();
    }
    //TODO pass warning or OK fpr critical SMART IDs to function
    //e.g. use highmost bits of SMARTattributeID for that purpose
      //std::numeric_limits<>::min();
//          SMARTattributeID &= 2 << (numSMARTattributeIDbits - 1)
    sMARTvalueRating = s_SMARTvalueRater.GetSMARTvalueRating(SMARTattrID,
      SMARTuniqueIDandVals, realCircaSMARTattrRawVal, p_modelAndFirmware);
    /**According to struct "NVME_HEALTH_INFO_LOG" in file "nvme.h" (see
http://learn.microsoft.com/en-us/windows/win32/api/nvme/ns-nvme-nvme_health_info_log
     * ) S.M.A.R.T. via NVMe does not have normalized current, worst and
     * threshold values per attribute ID.*/
    if(sMARTuniqueID.getBusType() != TU_Bln361095::hardware::bus::NVMe)
    {
      SetAttribute(
        sMARTuniqueID,
        SMARTattrID,
        colIndices::nrmlzdCurrVal/**column #/index*/,
        std_ossNrmlzdCurrSMARTval.str(),
        sMARTvalueRating,
        data);
      SetAttribute(
        sMARTuniqueID,
        SMARTattrID,
        colIndices::nrmlzdThresh/**column #/index*/,
        std_ossNrmlzdThreshVal.str(),
        sMARTvalueRating,
        data);
    }
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      colIndices::rawValue /** column #/ index */,
      std_ossRawSMARTval.str(),
      sMARTvalueRating,
      data);
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      colIndices::humanReadableRawVal,
      stdstrHumanReadableRawVal,
      sMARTvalueRating,
      data);
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      colIndices::unit,
      stdstrUnit,
      sMARTvalueRating,
      data);
    if(! std_ossUnitAccuracy.str().empty() )
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      colIndices::unitRange,
      std_ossUnitAccuracy.str(),
      sMARTvalueRating,
      data);

        /** https://cboard.cprogramming.com/c-programming/115586-64-bit-integers-printf.html
        *   : "%llu": Linux %llu, "%I64u": Windows */
          //TODO wxString::Format(...) causes "smallbin double linked list corrupted"
//          wxString::Format( , ) expands to : 
//        template < typename T1 > static wxString Format ( const wxFormatString & f1 , T1 a1 ) {
//          typedef const wxFormatString & TF1 ;
//          const wxFormatString * fmt = ( ( wxFormatStringArgumentFinder < TF1 > :: find ( f1 ) ) ) ;
//          return DoFormatWchar ( f1 , wxArgNormalizerWchar < T1 > ( a1 , fmt , 1 ) . get ( ) ) ;
//        }
//          wxString::Format( wxT("%llu"), SMARTrawValue)
//        if( critical && SMARTrawValue > 0)
//        {
//          atLeast1CriticalNonNullValue = true;
//          m_pwxlistctrl->SetItemBackgroundColour(lineNumber, * wxRED);
//        }
//        else
//          m_pwxlistctrl->SetItemBackgroundColour(lineNumber, * wxGREEN);
    UpdateTimeOfSMARTvalueRetrieval(
      sMARTuniqueID,
      SMARTattrID,
      upTimeOfRetrievalInMs,
      data);
  }
  }///
//  if(upTimeOfRetrievalInMs == 0)///0 means: "not read" / "not send by server"
  else///sMARTattr.m_successfullyReadSMARTrawValue
  {
    sMARTvalueRating = TU_Bln361095SMARTmonNmSpc::SMARTvals::Rating::unknown;
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      colIndices::lastUpdate,
      /*"error:uptime is 0"*/"not read yet",
      sMARTvalueRating,
      data);
  }
//  }
//  else
//  {//TODO show message that no S.M.A.R.T. attribute definition found
//  }
  LOGN_DEBUG("return " << sMARTvalueRating)
  return sMARTvalueRating;
}
