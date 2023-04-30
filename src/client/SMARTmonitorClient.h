/** File:   SMARTmonitorClient.h
 * Author: Stefan Gebauer,M.Sc.Comp.Sc.
 * Created on 20. November 2016, 17:43 */

#ifndef SMARTMONITORCLIENT_H
#define SMARTMONITORCLIENT_H

///Stefan Gebauer's common_sourcecode repository header files:
/** Include at 1st in Windows build to avoid:
 * "#warning Please include winsock2.h before windows.h" */
#ifdef TU_Bln361095useBSDskt
#include <OperatingSystem/BSD/socket/prepCnnctToSrv.h>///prepCnnctToSrv(...)
#endif
#include <OperatingSystem/multithread/nativeThreadType.hpp>///nativeThread_type

///This repository's files:
#include "../SMARTmonitorBase.hpp"
///enum TU_Bln361095::SMARTmon::colIndices::ColIndices
#include <UserInterface/columnIndices.hpp>
#include <SMARTvalueRater.hpp>///class SMARTvalueRater
#include <tinyxml2/ProcessSMARTdata.hpp>///class tinyxml2::SrvDataProcessor

/**Define user interface control character strings here to use for various user
 * interfaces like wxWidgets, ncurses etc.*/
///cnct=connect:
///Btn=button:
#define TU_Bln361095cnctBtnANSIstr "Connect..."
#define TU_Bln361095discnctBtnANSIstr "Disconnect"
#define TU_Bln361095cnctBtnTltpANSIstr "connect to BSD sockets server for "\
  "getting S.M.A.R.T. data"

using namespace TU_Bln361095::SMARTmon;

/** Base class for all BSD socket clients that receive data from a.S.M.A.R.T.
 server */
class SMARTmonitorClient 
  : public SMARTmonitorBase
{
public:
  enum CurrentAction
  {
    cnnctToSrv,
    nonBlckCnnctToSrv,///Non-blocking "connect" (via "select") mode
    readNumBytesForSuppSMART_IDs,
    readSuppSMART_IDsXMLdata,
    readNumBytesForSMARTdata,
    readSMARTvaluesXMLdata,
    hasReadSMARTvaluesXMLdata,
    WaitForSMARTupd8ThreadTerm,
    AfterWaitForSMARTupd8ThreadTerm
  };

  SMARTmonitorClient();
  SMARTmonitorClient(const SMARTmonitorClient& orig);
  virtual ~SMARTmonitorClient();
//#ifdef _DEBUG
  DWORD GetSMARTvalsAndUpd8UIthreadID /*= 0*/;
//#endif
  ///To check whether we need to run UI operations in another thread or not.
  static fastestUnsignedDataType s_UIthreadID;
  static fastestUnsignedDataType s_maxNumCharsNeededForDisplay[];
  static fastestUnsignedDataType s_charPosOAttrNameBegin[colIndices::byndLast];
  static char * s_columnAttriuteNames [];
  
  static enum TU_Bln361095::SMARTmon::SMARTvals::Rating::E
    s_entireSMARTstatus;
//  static nativeThread_type s_updateSMARTparameterValuesThread;
  static fastestUnsignedDataType s_updateUI;
  dataCarrierID2supportedSMARTattrMap_type
    dataCarrierIDandSMARTidsContainer;
  static SMARTvalueRater s_SMARTvalueRater;
  
  enum serverConnectionState {cnnctdToSrv, uncnnctdToSrv, connectToSrv,
    /**to diplay the (local) time of last S.M.A.R.T. value update*/ valUpd8,
    drctSMARTaccss, endedDrctSMART};
#ifdef TU_Bln361095useClntSrv
  enum transmission { successfull = 0, readLessBytesThanIntended, unsetTransmResult };
  enum TransmissionError { numBytesToReceive, SMARTdata, SMARTparameterValues};
#endif
  
  enum serverConnectionState m_srvrCnnctnState = uncnnctdToSrv;
#ifdef TU_Bln361095useBSDskt
  int m_socketFileDesc;
#endif
#ifdef TU_Bln361095useClntSrv
  fastestUnsignedDataType m_srvCnnctnCntDownInSec;
  fastestUnsignedDataType m_serverConnectionState;
  
  void AfterGetSMARTvaluesLoop(int getSMARTvaluesResult);
#endif
  /** E.g. show a dialog that enables cancelling of connection in implementation. */
  virtual void BeforeConnectToServer()/**Implement for not to override by a 
    small test program*/{};
  virtual void GetTextFromUser(const char * label, std::string & ) { };
#ifdef TU_Bln361095useClntSrv
  ///Called after connection attempt to the server.(either from the thread that
  ///prepared the server connection or from another). So UI operations should 
  /// be ensured to run in UI thread via events etc..)
  ///Can be used to hide a "connect to server" window.
  virtual void AfterConnectToServer(int connectResult) { };
  void HandleConnectionError(const char * hostName, const int connectResult);
#endif
#ifdef TU_Bln361095useInterProcComm
  //TODO could use ByteArray datatype here
  void GetSMARTdataViaXML(const uint8_t SMARTvalues[], const
    fastestUnsignedDataType numBytesToRead, SMARTuniqueIDandValues &);
#endif
  /**Called
   * -after a transmission error
   * -after connection to server established
   * -update of (only) S.M.A.R.T. values
   * -if a user canceled the connection via User Interface */
  virtual void ChangeConnectionState(enum serverConnectionState newState){}
#ifdef TU_Bln361095useClntSrv
  fastestUnsignedDataType CnnctToSrvAndGetSMARTvals(const bool asyncCnnctToSvc);
  void ConnectToServer();
  fastestUnsignedDataType ConnectToServer(const char * hostName,
    bool asyncConnect, int * p_errorCode);
#endif
#ifdef TU_Bln361095useInterProcComm
  /*fastestUnsignedDataType*/ void  GetSupportedSMARTattributesViaXML(
    uint8_t * xmlDataByteArray,
    fastestUnsignedDataType numBytesToRead,
    dataCarrierID2supportedSMARTattrMap_type & supportedSMARTattributess
    );
#endif
  void EndUpdateUIthread();
#ifdef TU_Bln361095useClntSrv
  fastestUnsignedDataType GetSupportedSMARTidsFromSrv();
  fastestSignedDataType ReadNumFollowingBytes();
  fastestUnsignedDataType GetSMARTattrValsFromSrv(//std::set<SMARTuniqueIDandValues> & 
    );
#endif
  bool isAsyncCnnct() const {return asynCnnct;}
#if TU_Bln361095useInterProcComm
  ///Should be called in user interface thread? (because it calls functions to 
  /// create the user interface)
  void GetSMARTvaluesAndUpdateUI();
  static DWORD GetSMARTvaluesAndUpdateUIthreadFn(void *);
#endif
#ifdef directSMARTaccess
  bool getsSMARTdataDrctly() const{
    return m_getSMARTvaluesFunctionParams.p_getSMARTvaluesFunction ==
      & SMARTmonitorBase::Upd8SMARTvalsDrctlyThreadSafe;
/** Alternative: execute this function and return false if "directSMARTaccess"
 * not defined.*/
//#else
//    return false;
  }
#endif
#if TU_Bln361095useBSDskt
  void HandleTransmissionError(enum TransmissionError,
    const fastestUnsignedDataType numBread,
    const fastestUnsignedDataType numBtoRead,
    const int rdErrno);
#endif
  virtual void SetCurrentAction(enum CurrentAction) = 0;
#ifdef TU_Bln361095useClntSrv
  void SetServiceAddress(const std::string & str) {
    m_stdstrServerAddress = str;
  }
#endif
  void SetSMARTattribIDandNameLabel();
#ifdef TU_Bln361095useClntSrv
  virtual void ShwCnnctToSrvrDlg(const std::string &){};
  virtual void StartSrvCnnctnAttmptCntDown(
    const fastestUnsignedDataType countDownInSeconds) {}
#endif
  /** Operations that only need to be done once after connection to the service
      is established. 
      "virtual" is needed in order to generate a table of virtual functions. */
  virtual void ReBuildUserInterface() { 
    //SetSMARTdriveID();
    SetSMARTattribIDandNameLabel();
  }
  void setIDandLabel(const SMARTuniqueID &,
    const fastestUnsignedDataType SMARTattrID, void * data);
  virtual void setUI(const enum serverConnectionState) = 0;
  SMARTvalRatngTyp upd8rawAndH_andTime(
    const fastestUnsignedDataType SMARTattrID,
    const SMARTuniqueIDandValues &,
    void * data,
    const ModelAndFirmware *);
  inline void UpdateTimeOfSMARTvalueRetrieval(
    const SMARTuniqueID &,
    const fastestUnsignedDataType SMARTattributeID,
    /** Needs to be uint64_t in order to also work if built as 32 bit program
     *  and a long uptime in ms.*/
    const uint64_t timeStampOfRetrieval, void * data);
  /*virtual*/ void UpdateSMARTvaluesUI();
  virtual void SetAttribute(
    const SMARTuniqueID &,
    fastestUnsignedDataType SMARTattributeID, /**Usually the line (number) */
    //TODO exchange enum with fastestUnsignedDataType for performance?
    const enum colIndices::columnIndices &,/**Usually the column (number) */
    const std::string &,
    const SMARTvalRatngTyp,
    /**e.g. pointer to list ctrl */void * data){}
  virtual void ShowStateAccordingToSMARTvalues(
    const enum TU_Bln361095::SMARTmon::SMARTvals::Rating::E){}
  std::string m_stdstrServerAddress;
protected:
  tinyxml2::SrvDataProcessor srvDataProcessor;
//  nativeThread_type m_GetSMARTvalsAndUpd8UIthread;
private:
};

#endif /* SMARTMONITORCLIENT_H */

