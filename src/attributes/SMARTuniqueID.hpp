/** SMARTuniqueID.hpp
 *  Created on: 05.08.2016
 * Author:Stefan Gebauer, M.Sc.Comp.Sc. TU Berlin */

#ifndef ATTRIBUTES_SMARTUNIQUEID_HPP_
#define ATTRIBUTES_SMARTUNIQUEID_HPP_

///standard C/C++ lib
#include <iostream>///class std::ostream
#include <limits>///std::numeric_limits<>::max()
#include <map> //class std::map
#include <set> //class std::set

///common_sourcecode repo of Stefan Gebauer
#include <hardware/CPU/fastest_data_type.h>///fastestUnsignedDataType
///numSMART_SNbytes, numSMART_FWbytes, numSMARTmodelBytes, numDifferentSMART_IDs
#include <hardware/dataCarrier/ATA3Std.h>

#include <SMARTattributeNames.h>///enum SMARTattributeNames
#include "SMARTattributeNameAndID.hpp"///class SMARTattributeNameAndID

/** Same structure as SkIdentifyParsedData in Linux' "atasmart.h" */
struct SMARTuniqueID {
  ///Member vars are set by SMARTaccessBase-derived class, fill(...) method.
  char m_serialNumber[numSMART_SNbytes+1];
  char m_firmWareName[numSMART_FWbytes+1];
  char m_modelName[numSMARTmodelBytes+1];
  SMARTuniqueID & operator = (const SMARTuniqueID & l);
  SMARTuniqueID() { }
#ifdef __linux__
//  SMARTuniqueID(const SkIdentifyParsedData & l);
#endif
  ~SMARTuniqueID();
  ///Fetched in an interval: make access fast
  fastestUnsignedDataType m_SMART_IDsToRd[numDifferentSMART_IDs];
  
  /** \brief Intersection of both SMART IDs supported by data carrier and SMART
   * IDs to read. */
  //TODO also intersect where is a SMART attribute definition?
  void SetSMART_IDsToRead(
    const suppSMART_IDsType & suppSMARTattrNamesAndIDs,
    /**Sorted ascending, last entry has value 0.*/
    const fastestUnsignedDataType SMART_IDsToRead[])
  {
    if(SMART_IDsToRead[0] == 0){///Empty->read all supported S.M.A.R.T. IDs
      ///Prevent stack overflow.
      if(suppSMARTattrNamesAndIDs.size() <= numDifferentSMART_IDs){
      fastestUnsignedDataType SMART_IDsToReadIdx = 0;
      for(suppSMART_IDsType::const_iterator iter = 
        suppSMARTattrNamesAndIDs.begin(); iter !=suppSMARTattrNamesAndIDs.end();
        iter++)
      {
        m_SMART_IDsToRd[SMART_IDsToReadIdx++] = iter->GetID();
      }
      ///Prevent stack overflow.
      if(suppSMARTattrNamesAndIDs.size() < numDifferentSMART_IDs)
        m_SMART_IDsToRd[SMART_IDsToReadIdx] = 0;///Mark last entry
      }
//      else///return error code
    }
    else{
      fastestUnsignedDataType sMARTattrToReadIdx = 0;
      fastestUnsignedDataType sMART_IDtoRead, suppSMART_ID;
      fastestUnsignedDataType SMART_IDsToReadIdx = 0;
      for(suppSMART_IDsType::const_iterator iter =
        suppSMARTattrNamesAndIDs.begin();
        iter !=suppSMARTattrNamesAndIDs.end(); iter++
//        SMART_IDsToObs[SMART_IDsToReadIdx] != 0; SMART_IDsToReadIdx++
        )
      {
//        if(iter !=suppSMARTattrNamesAndIDs.end() )
        suppSMART_ID = iter->GetID();
        do{
        sMART_IDtoRead = SMART_IDsToRead[SMART_IDsToReadIdx];
        if( sMART_IDtoRead == 0)///Last entry reached
          break;
        if(sMART_IDtoRead == suppSMART_ID)
        {
          m_SMART_IDsToRd[sMARTattrToReadIdx++] = sMART_IDtoRead;
          //SMART_IDsToReadIdx++;//TODO why 2nd inc.?
          break;///Get next supported SMART ID
        }
        SMART_IDsToReadIdx++;
        }while(SMART_IDsToReadIdx < numDifferentSMART_IDs &&
           sMART_IDtoRead < suppSMART_ID);
      }
      m_SMART_IDsToRd[sMARTattrToReadIdx] = 0;
    }
  }
  
  //TODO exchange the following by sth. like uint64_t prvVals[numSMART_IDs];
  // to cover making differences of all SMART attributes
  uint64_t m_totDatWrSMARTrawVal = std::numeric_limits<uint64_t>::max();
  uint64_t m_numBwrittenSinceOSstart = std::numeric_limits<
    uint64_t>::max();
  
  /**\param otherVal : Can be called with value directly from device or
  * delivered to client/SMARTvalueProcessorBase(-derived).*/
  void guessUnit(
    const fastestUnsignedDataType SMARTattrID,
    const uint64_t & SMARTrawVal,
    const uint64_t otherVal///E.g. # B written to data carrier since OS start
    )
  {
    /** The unit for "Total Data/LBAs Written/Read" differs among models. For 
     * HFS256G39TND-N210A, firmware:30001P10 (serial:EJ7CN55981080CH09) 
     * Solid State Device (SSD) it seems to be GiB rather than LBAs.*/
    ///For SSDs may be more than written by OS due to.wear levelling.
    if(SMARTattrID == SMARTattributeNames::TotalDataWritten){
    /** Increase was 34 for an HFS256G39TND-N210A firmware 30001P10 SSD using
     * libatasmart's "pretty_value" 
     * Should make a difference of number of bytes written to the data carrier
     * since OS start from the 2nd time the "Total Data/LBAs Written" value
     * increases. Else if doing it the 1st time it increases then the difference
     * may be wrong:
     *       x <- 1st time getting # data written since OS start
     * x     | x <-S.M.A.R.T.'s Total Data/LBAs Written value increases
     * t1   t2 t3 -> time (t1,t2,t3: point in time) */
    if(SMARTrawVal > //TODO may change to prvVals[SMARTattrID]
      m_totDatWrSMARTrawVal)///<=>value set at least once.
    {
//      m_numBwrittenSinceOSstart = numBwrittenSinceOSstart;
      if( otherVal < m_numBwrittenSinceOSstart)
      {
        m_numBwrittenSinceOSstart = otherVal;
        m_totDatWrSMARTrawVal = SMARTrawVal;
      }//http://www.t13.org/Documents/UploadedDocuments/docs2005/e05148r0-ACS-SMARTAttributesAnnex.pdf
      else{/** SMART raw value increased for at least the 2nd time
       * Bytes diff was 118273527808−117363425280=910102528 for an SSD:
       * HFS256G39TND-N210A firmware 30001P10 when S.M.A.R.T. value diff was 34
       * using pretty_value from libATAsmart */
        
      /// /proc/diskstats col. 10: 231002984 - 229225440= 1777544
       uint64_t numBdiff = otherVal - m_numBwrittenSinceOSstart;
       uint64_t SMARTrawValDiff = SMARTrawVal - m_totDatWrSMARTrawVal;
       uint64_t unit = numBdiff / SMARTrawValDiff;
       
      ///The higher the difference  the more accurate?
       /// E.g. 
//      m_numBwrittenSinceOSstart = numBwrittenSinceOSstart;
//      m_totDatWrSMARTrawVal = SMARTrawVal;
//      SMARTrawVal *= unit;///Make value to # bytes 
      }
    }
    else// if(SMARTrawVal < m_totDatWrSMARTrawVal){///<=>value not set yet
      m_totDatWrSMARTrawVal = SMARTrawVal;
    }
  }

  /** For necessary function signature see
   *  https://msdn.microsoft.com/en-us/library/1z2f6c2k.aspx */
  friend std::ostream & operator << ( std::ostream & ostr,
    const SMARTuniqueID & obj)
  {
    ostr << "model:" << obj.m_modelName << " firmware:" << obj.m_firmWareName 
      << " serial #:" << obj.m_serialNumber;
    return ostr;
  }
  std::string str() const;
  void SetModelName(const char *);
  void SetSerialNumber(const char *);
  void SetFirmwareName(const char *);
};
  
  typedef std::set<fastestSignedDataType> supportedSMARTattributeIDs_type;
  typedef std::map<SMARTuniqueID,supportedSMARTattributeIDs_type >
    dataCarrierID2supportedSMARTattrMap_type;

  bool operator < (const SMARTuniqueID & left,
                   const SMARTuniqueID & right);

#endif /* ATTRIBUTES_SMARTUNIQUEID_HPP_ */
