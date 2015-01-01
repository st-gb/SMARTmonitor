/*
 * SMARTreaderTest.cpp
 *
 *  Created on: 25.11.2013
 *      Author: mr.sys
 */
#include "SMARTreaderTest.hpp" //class SMARTreaderTest

BOOL SMARTreaderTest::ReadSMARTindexAndValueAndWorstAndRaw(
    HANDLE hDevice,
    BYTE * szAttributes,
    UCHAR ucDriveIndex,
    SENDCMDINPARAMS & stCIP
    )
{
  UCHAR currentSMARTattributeIndex;
  ST_SMART_INFO * pCurrentSMARTparameter;
  for (currentSMARTattributeIndex = 0; currentSMARTattributeIndex < 30;
      ++ currentSMARTattributeIndex)
  {
    pCurrentSMARTparameter =
      & m_stDrivesInfo[ucDriveIndex].m_stSmartInfo[m_stDrivesInfo[ucDriveIndex]
        .m_ucSmartValues];
    ST_SMART_DETAILS * p_st_smart_details = GetSMARTDetails(
        pCurrentSMARTparameter->m_ucAttribIndex);
//    if( p_st_smart_details && p_st_smart_details->m_bCritical )
//    {
      DWORD dwRAWvalue = pCurrentSMARTparameter->m_dwAttribValue + 1;
      TestForChangedCriticalParameters(dwRAWvalue, pCurrentSMARTparameter,
          m_stDrivesInfo[ucDriveIndex]);
//    }
  }
  return TRUE;
}
