/** Author: Stefan gebauer, M.Sc. Comp.Sc.
 * Created on 20. November 2016, 17:43 */

///from Stefan Gebauer's common_sourcecode git repository:
/** Include 1st to avoid MinGW GCC (9.2.0) "warning: #warning Please include
 *  winsock2.h before windows.h [-Wcpp]" */
#include <OperatingSystem/BSD/socket/socket.h>///readFromSocket(...)
#include <compiler/GCC/enable_disable_warning.h>///GCC_DIAG_OFF(...)
#include <hardware/CPU/atomic/AtomicExchange.h>///AtomicExchange(...)
///OperatingSystem::GetLastErrorCode()
#include <OperatingSystem/GetLastErrorCode.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h>///LOGN(...)

///This repository's header files:
#include "SMARTmonitorClient.h"///this class' header file
///SMARTvalueFormatter::FormatHumanReadable(...)
#include <SMARTvalueFormatter.hpp>
#include <UserInterface/UserInterface.hpp>///class UserInterface

///Stefan Gebauer's common_sourcecode repository header files:
///Standard C(++) header files:
#include <stdint.h> //uint8_t
///http://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/close?view=vs-2019
#include <unistd.h>///close(...), read(...)

/** Static/class variable defintion: */
fastestUnsignedDataType SMARTmonitorClient::s_updateUI = 1;
#ifdef multithread
//nativeThread_type SMARTmonitorClient::s_updateSMARTparameterValuesThread;
#endif
enum SMARTvalueRating SMARTmonitorClient::s_atLeast1CriticalNonNullValue =
  unknown;
fastestUnsignedDataType SMARTmonitorClient::s_maxNumCharsNeededForDisplay [] =
 { 3, 30, 15, 20, 40};
fastestUnsignedDataType SMARTmonitorClient::s_charPosOAttrNameBegin [] =
 { 0, 3, 33, 48, 68};
GCC_DIAG_OFF(write-strings)
char * SMARTmonitorClient::s_columnAttriuteNames [] =
 { "ID", "name", "raw", "human readable", "last update"};
GCC_DIAG_ON(write-strings)
SMARTvalueRater SMARTmonitorClient::s_SMARTvalueRater;

SMARTmonitorClient::SMARTmonitorClient() 
  : m_serviceConnectionCountDownInSeconds(60)
#ifdef _DEBUG
  , GetSMARTvalsAndUpd8UIthreadID(0)
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

#ifdef multithread
void SMARTmonitorClient::EndUpdateUIthread()
{
  if(m_updateSMARTparameterValuesThread.IsRunning() )
  {
    /** Inform the SMART values update thread about we're going to exit,*/
    //m_wxCloseMutex.TryLock();
    LOGN("disabling update UI (ends update UI thread)");
    AtomicExchange( (long *) & s_updateSMARTvalues, 0);
    
    LOGN("waiting for close event");
    /** http://docs.wxwidgets.org/trunk/classwx_condition.html : 
      "Wait() atomically unlocks the mutex which allows the thread to continue "
      "and starts waiting */
    //m_p_wxCloseCondition->Wait();
    
    m_updateSMARTparameterValuesThread.WaitForTermination();
    GetSMARTvalsAndUpd8UIthreadID = 0;
    ChangeState(unconnectedFromService);
  }
}
#endif

/** @brief Gets S.M.A.R.T. values and starts "get S.M.A.R.T. values" loop (in a
 *  dedicated thread if multithreaded) */
void SMARTmonitorClient::GetSMARTvaluesAndUpdateUI()
{
#ifdef _DEBUG
  /**Prevent the update thread from running more than once (this brings socket
   * problems--read the wrong size of bytes to receive)*/
  if( GetSMARTvalsAndUpd8UIthreadID == 0)
    GetSMARTvalsAndUpd8UIthreadID = OperatingSystem::GetCurrentThreadNumber();
  else
  {
//  std::string fnName = compiler::GetCurrFnName();
    ShowMessage("a thread already started GetSMARTvaluesAndUpdateUI",
      UserInterface::MessageType::error);
    return;
  }
#endif
  /** Avoid using old values for unit etc. after reconnecting (to another
   *  server) */
  SMARTuniqueIDsAndValues.clear();
  /** Before calling this function the connection should be established. */
  //TODO possibly move this line to after successfull connection.
  m_serverConnectionState = connectedToService;
  ChangeState(connectedToService);
  //SMARTaccess_type & sMARTaccess = m_SMARTaccess.;
  std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValues = 
    SMARTuniqueIDsAndValues;
  int result = GetSupportedSMARTidsFromServer();
  if( result > 0 )
  {
    /** If not closing then the socket file descriptor number increases on 
     *  next socket(...) call?! */
    /** http://man7.org/linux/man-pages/man2/close.2.html :
     * "close() returns zero on success." */
    result = close(m_socketFileDesc);
    //TODO use variable m_serviceConnectionCountDownInSeconds as parameter
    StartServiceConnectionCountDown(60);
    return;
  }
  LOGN("SMART unique ID and values container:" << &sMARTuniqueIDandValues)
  /** Get # of attributes to in order build the user interface (write 
   *  attribute ID an name into the table--creating the UI needs to be done 
   *  only once because the attribute IDs received usually do not change).*/
  const int getSMARTvaluesResult = GetSMARTattrValsFromSrv(
    /*sMARTuniqueIDandValues*/);
  if(getSMARTvaluesResult == 0)
  {
    //TODO the following code could go into a function of a new
    // "class SMARTuniqueIDsAndValues
    // : public SMARTuniqueIDandValsContType"
    ///Important if no S.M.A.R.T. attributes to read exist yet.
    for(SMARTuniqueIDandValsContType::iterator iter = 
      SMARTuniqueIDsAndValues.begin(); iter != SMARTuniqueIDsAndValues.end();
      iter++)
    {
      SMARTuniqueID & sMARTuniqueID =(SMARTuniqueID &) iter->getSMARTuniqueID();
      if(sMARTuniqueID.noSMARTattrsToRead() )
        ( (SMARTuniqueIDandValues &) *iter).
          setSMART_IDsToReadFromSuccSMARTrawValUpd8();
    }
    SetSMARTattributesToObserve(sMARTuniqueIDandValues);
//      m_p_ConnectAndDisconnectButton->SetLabel(wxT("disconnect"));
    ReBuildUserInterface();
    UpdateSMARTvaluesUI();
    /** Show the state (SMART OK or warning) for the 1st time 
     *  (in the user interface). E.g. show the icon and message belonging. 
     *  Afterwards this method is only called if "s_atLeast1CriticalNonNullValue" changes.*/
    ShowStateAccordingToSMARTvalues(s_atLeast1CriticalNonNullValue);
    m_getSMARTvaluesFunctionParams.p_getSMARTvaluesFunction =
      & SMARTmonitorBase::GetSMARTattrValsFromSrv;
#ifdef multithread
    StartAsyncUpdateThread(
      //UpdateSMARTvaluesThreadSafe 
      m_getSMARTvaluesFunctionParams
      );
#else
    UpdateSMARTparameterValuesThreadFunc(&m_getSMARTvaluesFunctionParams);
#endif
//      ((SMARTmonitorBase *)this)->StartAsyncUpdateThread(SMARTmonitorBase::UpdateSMARTvaluesThreadSafe);
  }
  else
  {
    /** If not closing then the socket file descriptor number increases on 
     *  next socket(...) call?! */
    close(m_socketFileDesc);
    //TODO use variable m_serviceConnectionCountDownInSeconds as parameter
    StartServiceConnectionCountDown(60);
  }
}

/** Called e.g. when server address was given via program options/
 *  as command line argument */
void SMARTmonitorClient::ConnectToServerAndGetSMARTvalues(const bool asyncCnnctToSvc)
{
  LOGN_DEBUG("begin")
#ifdef directSMARTaccess
  if(m_SMARTaccess.GetNumSMARTattrDefs() == 0)
  {
    ShowMessage("no SMART attribute definition from config file. ->Don't know "
      //"whether a SMART attribute is critical"
      "the SMART attribute names", UserInterface::MessageType::warning);
//    return;
  }
#endif
//  BeforeConnectToServer();
  const fastestUnsignedDataType connectToServerResult = ConnectToServer(
    m_stdstrServiceHostName.c_str(), asyncCnnctToSvc);
  if(! asyncCnnctToSvc)
  {
    AfterConnectToServer(connectToServerResult);
  }
}

void SMARTmonitorClient::ConnectToServer() {
#ifdef multithread
  /** Terminate a possibly running update UI thread (e.g.. already connected
   *   to a service). */
  EndUpdateUIthread();
#endif
//  std::string stdstrServerAddress;
  //GetTextFromUser("input SMART values server address", m_stdstrServiceHostName);
  ShwCnnctToSrvrDlg(m_stdstrServiceHostName);

//  SetServiceAddress(m_stdstrServerAddress);
//  BeforeConnectToServer();
//  ConnectToServerAndGetSMARTvalues();
}

void SMARTmonitorClient::HandleTransmissionError( 
  enum SMARTmonitorClient::TransmissionError transmissionError,
  const fastestUnsignedDataType numBread,
  const fastestUnsignedDataType numBtoRead)
{
  std::ostringstream stdoss;
  char * errorMessageForErrno = NULL;
#ifdef _WIN32
  int lastWSAerror = WSAGetLastError();
  //http://stackoverflow.com/questions/3400922/how-do-i-retrieve-an-error-string-from-wsagetlasterror
#endif
  const int lastErrorNumber = /*OperatingSystem::GetLastErrorCode();*/ errno;
  if( lastErrorNumber != 0)
  {
    switch(lastErrorNumber)
    {
      //TODO Unix-specific value
      case EPIPE :
        //see https://linux.die.net/man/2/write
        errorMessageForErrno = (char *) "The reading end of socket is closed.";
        break;
      default:
        errorMessageForErrno = strerror(lastErrorNumber);
//        errorMessageForErrno  = OperatingSystem::GetErrorMessageFromErrorCodeA(
//          lastErrorNumber);
        break;
    }
  }
  stdoss << "OS error #:" << errno << "\n";
  switch(transmissionError)
  {
    case numBytesToReceive :
      stdoss << "ERROR reading the number of following bytes from socket (read "
        << numBread << "B instead of " << numBtoRead << "B)";
      break;
    case SMARTparameterValues :
      stdoss << "ERROR reading SMART parameter values from socket";
      break;
    case SMARTdata :
      stdoss << "ERROR reading SMART data from socket";
      break;
  }
  if( errorMessageForErrno )
    stdoss << " for socket file descriptor #" << m_socketFileDesc << ":\n" << errorMessageForErrno;
  LOGN_ERROR(stdoss.str() );
  ShowMessage(stdoss.str().c_str(), MessageType::error);
  //TODO set connection status of the user interface to "network errors"/"unconnected"
  m_serverConnectionState = connectedToService;
  ChangeState(unconnectedFromService);
  //TODO close socket, set status (also in UI) to unconnected
}

//TODO Is socket-specific -> move to "socket" folder?!
fastestUnsignedDataType SMARTmonitorClient::GetSupportedSMARTidsFromServer()
{
  dataCarrierIDandSMARTidsContainer.clear();
  fastestSignedDataType numBytesToRead = ReadNumFollowingBytes();
  if( numBytesToRead < 1 )
    return readLessBytesThanIntended;
  LOGN_DEBUG("# of following bytes: " << numBytesToRead );
  std::ostringstream std_oss;
  std_oss << "# of bytes for supported SMART IDs: " << numBytesToRead;
  UserInterface::MessageType::messageTypes msgType = UserInterface::MessageType::error;
  if( numBytesToRead > 0 )
    msgType = UserInterface::MessageType::success;
  ShowMessage(std_oss.str().c_str(), msgType);
  const fastestUnsignedDataType numBytesToAllocate = numBytesToRead + 1;
  uint8_t * XMLdata = new uint8_t[numBytesToAllocate];
  if( XMLdata)
  {
    int numBytesRead = OperatingSystem::BSD::sockets::readFromSocket(
      m_socketFileDesc, XMLdata, numBytesToRead);
    if(numBytesRead < numBytesToRead) {
      HandleTransmissionError(SMARTdata, numBytesRead, numBytesToRead);
      LOGN_ERROR("read less bytes (" << numBytesRead << ") than expected (" 
        << numBytesToRead << ")");
      return 2; //TODO provide error handling (show message to user etc.)
    }
    LOGN_DEBUG("successfully read " << numBytesRead << "bytes")
    XMLdata[numBytesToRead] = '\0';

    GetSupportedSMARTattributesViaXML(XMLdata, numBytesToRead, 
      dataCarrierIDandSMARTidsContainer);
    delete [] XMLdata;
  }
  return 0;
}

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

///param data: For supported S.M.A.R.T. IDs dialog list cotrol
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
    ColumnIndices::SMART_ID,
    std_oss.str(),
    noCriticalValue,
    data
    );

  /** Now get the attribute name belonging to SMART ID */
  SMARTattrDef * p_sMARTattrDef = SMARTattrDefAccss::getSMARTattrDef(
    SMARTattrID);
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
    ColumnIndices::SMARTparameterName,
    stdstrSMARTattrName,
    noCriticalValue
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
    ColumnIndices::lastUpdate /** column #/ index */,
    timeFormatString,
    noCriticalValue,
    data
    );
}

///Updates the user interface (UI), e.g. for GUIs/TUIs it updates the cells
/// of the S.M.A.R.T. values table.
void SMARTmonitorClient::UpdateSMARTvaluesUI()
{
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
  
  enum SMARTvalueRating sMARTvalueRating, entireSMARTvalRating = SMARTvalueOK;
  /** Loop over data carriers. */
  //TODO (only) 1 item here when wxGUI started as root?
  for(fastestUnsignedDataType currentDriveIndex = 0;
    SMARTuniqueIDandValuesIter != SMARTuniqueIDsAndValues.end() ;
    SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueID & sMARTuniqueID = SMARTuniqueIDandValuesIter->
      getSMARTuniqueID();
    LOGN("SMART unique ID and values object " << &(*SMARTuniqueIDandValuesIter) )
    
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
      sMARTvalueRating = upd8rawAndH_andTime(SMARTattrID,
        *SMARTuniqueIDandValuesIter, NULL);
      if(sMARTvalueRating == SMARTvalueWarning)
        entireSMARTvalRating = SMARTvalueWarning;
    }
  }
  /** ^= state changed. */
  if(s_atLeast1CriticalNonNullValue != entireSMARTvalRating)
  {
    ShowStateAccordingToSMARTvalues(entireSMARTvalRating == SMARTvalueWarning);
  }
  s_atLeast1CriticalNonNullValue = entireSMARTvalRating;
}

bool getRealValue(const std::string & stdstrUnit, const uint64_t SMARTrawVal,
  uint64_t & realCircaValue)
{
  bool error = false;
  uint64_t currNum = 0;
  const fastestUnsignedDataType numChars = stdstrUnit.size();
  const char * ar_chUnit = stdstrUnit.c_str();
//  const char * ar_chUnitBegin = NULL, * ar_chUnitEnd = NULL;
  bool isNumber = false;
  char ch;
  realCircaValue = 0;
  for(fastestUnsignedDataType idx = 0; idx < numChars; idx++){
    ch = ar_chUnit[idx];
    switch(ar_chUnit[idx]){
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
//          ar_chUnitBegin = ar_chUnit + idx;
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
//          ar_chUnitEnd = ar_chUnit + idx;
          isNumber = false;
        }
        switch(ch){
          case 'B':///Needs to be contained for not to be in "default" branch.
            break;
          case 'G':
            currNum *= 1000000000;
            break;
          case 'h':
            currNum *= 3600000;
            break;
          case 's':
            currNum *= 1000;
            break;
          default:
            error = true;
        }
        realCircaValue += SMARTrawVal * currNum;
    }
  }
  return error;
}

inline void useDeterminedUnits(
  const SMARTuniqueID & sMARTuniqueID,
  const fastestUnsignedDataType SMARTattrID,
  const uint64_t SMARTrawVal,
  long & unit,
  uint64_t & realCircaValue,
  double & accuracy,
  double & lowerUnitLimit,
  double & upperLimit,
  std::ostringstream & std_ossUnit,
  std::ostringstream & std_ossUnitAccuracy,
  std::string & stdstrHumanReadableRawVal
  )
{
  const long int highestBit = mostSignificantBit(unit);
  if(unit & ~highestBit)///If unit is determined <=> > 0 after bit removed
  {
    lowerUnitLimit = (double) sMARTuniqueID.lowerUnitBound[SMARTattrID];
    if(unit & highestBit ){///If highmost bit set/>=
      std_ossUnit << ">=~";
      unit &= ~highestBit;///Without highmost bit
      stdstrHumanReadableRawVal = ">=~";
      if(lowerUnitLimit != 0.0)/** Lower unit bound set*/{
        std::string humanReadableAccuracy = SMARTvalueFormatter::
          FormatHumanReadable(SMARTattrID, lowerUnitLimit, true);
        std_ossUnitAccuracy << ">=~" << humanReadableAccuracy;
        ///Value can get high: 16230×3618000 = 58720140000
        realCircaValue = SMARTrawVal * (uint64_t) lowerUnitLimit;
      }
      else
        ///Value can get high: 16230×3618000 = 58720140000
        realCircaValue = SMARTrawVal * (uint64_t) unit;
    }
    else{
      std_ossUnit << "~";
      stdstrHumanReadableRawVal = "~";
      ///Value can get high: 16230×3618000 = 58720140000
      realCircaValue = SMARTrawVal * (uint64_t) unit;
      upperLimit = (double) sMARTuniqueID.upperUnitBound[SMARTattrID];
      if(upperLimit != 0.0)///Prevent division by 0.
        accuracy = lowerUnitLimit / upperLimit;
    }
    std_ossUnit << SMARTvalueFormatter::FormatHumanReadable(SMARTattrID,unit,
      true);

    stdstrHumanReadableRawVal += SMARTvalueFormatter::
      FormatHumanReadable(SMARTattrID, realCircaValue, true);
    if(accuracy != 0.0){
      std::string humanReadableAccuracy = SMARTvalueFormatter::
        FormatHumanReadable(SMARTattrID, lowerUnitLimit, true);
      std_ossUnitAccuracy << " " << std::fixed << humanReadableAccuracy <<
        "-";/** "..." */
      humanReadableAccuracy = SMARTvalueFormatter::FormatHumanReadable(
        SMARTattrID, upperLimit, true);
      std_ossUnitAccuracy << std::fixed << /* "]" */ humanReadableAccuracy;
    }
  }
  else{///Unknown unit/use default unit
    uint64_t numForHumanReadableFormat;
    switch(SMARTattrID)
    {
     case SMARTattributeNames::DevTemp:
      numForHumanReadableFormat = SMARTrawVal;
       std_ossUnit << "°C?";
      realCircaValue = CurrTemp(SMARTrawVal);
      break;
     case SMARTattributeNames::PowerOnTime:
       numForHumanReadableFormat = SMARTrawVal * /**h to ms*/3600000ULL;
     case SMARTattributeNames::HeadFlyingHours:
       numForHumanReadableFormat = (SMARTrawVal & 0xFFFFFF) * /**h to ms*/
         3600000ULL;
       std_ossUnit << "~h?";
       break;
     default:
      numForHumanReadableFormat = SMARTrawVal;
      switch(SMARTattrID)
      {
      case SMARTattributeNames::ReallocSectorsCnt:
      case SMARTattributeNames::PwrCycleCnt:
      case SMARTattributeNames::ReallocEvtCnt:
      case SMARTattributeNames::UncorrSecCnt:
      case SMARTattributeNames::UDMA_CRCerrorCnt:
      case SMARTattributeNames::FreeFallEvtCnt:
        std_ossUnit << "#?";
        break;
      case SMARTattributeNames::TotalDataWritten:
      case SMARTattributeNames::TotalDataRead:
        std_ossUnit << ">=1sector?";
        break;
  default:
        std_ossUnit << "?";
      }
      realCircaValue = SMARTrawVal;
    }
    stdstrHumanReadableRawVal = SMARTvalueFormatter::
      FormatHumanReadable(SMARTattrID, numForHumanReadableFormat, false);
  }
}

enum SMARTvalueRating SMARTmonitorClient::upd8rawAndH_andTime(
  const fastestUnsignedDataType SMARTattrID,
  const SMARTuniqueIDandValues & SMARTuniqueIDandVals,
  void * data)
{
  enum SMARTvalueRating sMARTvalueRating;
  uint64_t SMARTrawVal;
  //TODO attribute IDs of SMART values to observe may not be a subset of
  // SMART attributes in config file!
//  SMARTattrDef * p_sMARTattrDef = SMARTattrDefAccss::getSMARTattrDef(
//    SMARTattrID);
//  if(p_sMARTattrDef){
    const SMARTvalue & sMARTvalue = SMARTuniqueIDandVals.m_SMARTvalues[
      SMARTattrID];
  bool isConsistent = sMARTvalue.IsConsistent(SMARTrawVal);
//      memory_barrier(); //TODO: not really necessary??
  int successfullyUpdatedSMART = sMARTvalue.m_successfullyReadSMARTrawValue;

  uint64_t upTimeOfRetrievalInMs;
  /** Also fails if client wants attribute ID but service did not send raw
   * S.M.A.R.T. values. */
  if(! sMARTvalue.GetRetrievalTime(upTimeOfRetrievalInMs) )
    upTimeOfRetrievalInMs = 0;
  const SMARTuniqueID & sMARTuniqueID = SMARTuniqueIDandVals.
    getSMARTuniqueID();
  //memory_barrier(); //TODO: not really necessary??
  if( /*successfullyUpdatedSMART*/ isConsistent && upTimeOfRetrievalInMs)
  {
//    SMARTattrDef & sMARTattrDef = *p_sMARTattrDef;
    std::string stdstrHumanReadableRawVal;
    
  //TODO move following code so it can be used in another place when not
  // SMARTmonitorClient-derived
    std::ostringstream std_ossUnit, std_ossUnitAccuracy;
    long unit = sMARTuniqueID.units[SMARTattrID];
    uint64_t realCircaValue;
    double accuracy = 0.0;
    double lowerUnitLimit = 0.0, upperLimit;
    std::string stdstrUnit;
    //TODO Derive SMARTuniqueID from ModelAndFirmware then this is not needed.
    ModelAndFirmware thisModelAndFirmware(sMARTuniqueID.m_modelName,
      sMARTuniqueID.m_firmWareName);
    ModelAndFirmwareTuplesType::iterator iter = m_modelAndFirmwareTuples.find(
      thisModelAndFirmware);
#ifdef _DEBUG
    int num = m_modelAndFirmwareTuples.count(thisModelAndFirmware);
#endif
    const bool entryFound = iter != m_modelAndFirmwareTuples.end();
    bool useDeterminedUnit = true;
    if(entryFound){
      const ModelAndFirmware & thisModelAndFirmware = *iter;
      stdstrUnit = thisModelAndFirmware.getParamUnit(SMARTattrID);
      if( stdstrUnit != "" )
      {
        if(! getRealValue(stdstrUnit, SMARTrawVal, realCircaValue) ){
          std_ossUnit << iter->getParamUnit(SMARTattrID);
          useDeterminedUnit = false;
          stdstrHumanReadableRawVal += SMARTvalueFormatter::
            FormatHumanReadable(SMARTattrID, realCircaValue, true);
        }
      }
    }
    if(useDeterminedUnit)
      useDeterminedUnits(sMARTuniqueID, SMARTattrID, SMARTrawVal,
        unit, realCircaValue,
        accuracy,
        lowerUnitLimit, upperLimit,
        std_ossUnit, std_ossUnitAccuracy,
        stdstrHumanReadableRawVal);
    std::ostringstream std_ossRawSMARTval;
    switch(SMARTattrID)
    {
     case SMARTattributeNames::GiB_Erased:
    /** https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
     *  : "Value is equal to (100-temp. °C)"*/
     case SMARTattributeNames::TempDiffOrAirflowTemp:
    /**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
     * 9 Jul 2020: "Lowest byte of the raw value contains the exact temperature
     * value (Celsius degrees)"*/
     case SMARTattributeNames::DevTemp:
     case SMARTattributeNames::HW_ECC_Recovered:
    /**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
     * 9 Jul 2020: "Decoded as: byte 0-1-2 = average erase count (big endian)
     * and byte 3-4-5 = max erase count (big endian).*/
     case SMARTattributeNames::AvgEraseCntAndMaxEraseCnt:
     case SMARTattributeNames::HeadFlyingHours:
       std_ossRawSMARTval << std::hex << /**To better differentiate between number and
       * base*/std::uppercase << SMARTrawVal << "h";
       break;
     default:
       std_ossRawSMARTval << SMARTrawVal;
     }
    stdstrUnit = std_ossUnit.str();
    //TODO pass warning or OK fpr critical SMART IDs to function
    //e.g. use highmost bits of SMARTattributeID for that purpose
      //std::numeric_limits<>::min();
//          SMARTattributeID &= 2 << (numSMARTattributeIDbits - 1)
    sMARTvalueRating = s_SMARTvalueRater.GetSMARTvalueRating(SMARTattrID,
      SMARTuniqueIDandVals, realCircaValue);
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      ColumnIndices::rawValue /** column #/ index */,
      std_ossRawSMARTval.str(),
      sMARTvalueRating,
      data);
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      ColumnIndices::humanReadableRawValue,
      stdstrHumanReadableRawVal,
      sMARTvalueRating,
      data);
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      ColumnIndices::unit,
      stdstrUnit,
      sMARTvalueRating,
      data);
    if(! std_ossUnitAccuracy.str().empty() )
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      ColumnIndices::unitRange,
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
  if(upTimeOfRetrievalInMs == 0)
    SetAttribute(
      sMARTuniqueID,
      SMARTattrID,
      ColumnIndices::lastUpdate,
      "error:uptime is 0",
      sMARTvalueRating,
      data);
  	;
//  }
//  else
//  {//TODO show message that no S.M.A.R.T. attribute definition found
//  }
  return sMARTvalueRating;
}
