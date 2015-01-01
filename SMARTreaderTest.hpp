/*
 * SMARTreaderTest.hpp
 *
 *  Created on: 25.11.2013
 *      Author: mr.sys
 */

#ifndef SMARTREADERTEST_HPP_
#define SMARTREADERTEST_HPP_

#include <windef.h> //ULONG
#include <CString.hpp>
//http://msdn.microsoft.com/en-us/library/windows/desktop/aa364972%28v=vs.85%29.aspx:
// "FileAPI.h (include Windows.h);"
#include <Windows.h>
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>

// class MFC_Compatibilit::CString -> CString
using namespace MFC_Compatibility;

#include "SmartReader.h" //base class

#include <C:\devel\unit++\unit++.h>

using namespace unitpp;

// use anonymous namespace so all test classes can be named Test
//namespace {
class SMARTreaderTest
  : public CSmartReader//, public suite
{
//public:
//  SMARTreaderTest()
//    : suite("appropriate name for test suite") {}
  BOOL ReadSMARTindexAndValueAndWorstAndRaw(
    HANDLE hDevice,
    BYTE * szAttributes,
    UCHAR ucDriveIndex,
    SENDCMDINPARAMS & stCIP
    );
};
//}

#endif /* SMARTREADERTEST_HPP_ */
