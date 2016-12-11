/*
 * ParseConfigFileException.hpp
 *
 *  Created on: 20.02.2015
 *      Author: mr.sys
 */

#ifndef CONFIGLOADER_PARSECONFIGFILEEXCEPTION_HPP_
#define CONFIGLOADER_PARSECONFIGFILEEXCEPTION_HPP_

#include <fastest_data_type.h> //fastestSignedDataType
#include <FileSystem/File/FileException.hpp>

class ParseConfigFileException
  : public FileException
{
  std::wstring m_filePath;
public:
  ParseConfigFileException(const wchar_t * filePath, fastestSignedDataType lineNumber)
    : m_filePath(filePath)
  {
//    m_filePath = filePath;
  }
};

#endif /* CONFIGLOADER_PARSECONFIGFILEEXCEPTION_HPP_ */
