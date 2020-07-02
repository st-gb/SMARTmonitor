#include <tinyxml2.h> //tinyxml2::XMLElement etc.
#include <client/SMARTmonitorClient.h>
#include <preprocessor_macros/logging_preprocessor_macros.h>
#include "hardware/CPU/atomic/AtomicExchange.h"
#include "Controller/character_string/ConvertStdStringToTypename.hpp"

bool GetSMARTuniqueID(
  tinyxml2::XMLElement * p_tinyxml2XMLelement, 
  SMARTuniqueID & sMARTuniqueID)
{
  //tinyxml2::XMLAttribute * p_tinyxml2XMLattribute = 
  const char * modelString = p_tinyxml2XMLelement->Attribute("model", 
    /** Value: specify '0' in order to retrieve the value */ NULL);
  const char * firmwareString = p_tinyxml2XMLelement->Attribute("firmware", 
    /** Value: specify '0' in order to retrieve the value */ NULL);
  const char * serialNumberString = p_tinyxml2XMLelement->Attribute(
    "serial_number", 
    /** Value: specify '0' in order to retrieve the value */ NULL);
//      if( ! p_tinyxml2XMLattribute)
//      {
//        LOGN_ERROR("Failed to get XML model attribute")
//        return;
//      }
  if( ! modelString ) {
    LOGN_ERROR("failed to get model atribute value")
    return false;
  }
  if( ! firmwareString ) {
    LOGN_ERROR("failed to get firmware atribute value")
    return false;
  }
  if( ! serialNumberString ) {
    LOGN_ERROR("failed to get serial number atribute value")
    return false;
  }
  sMARTuniqueID.SetModelName(modelString);
  sMARTuniqueID.SetFirmwareName(firmwareString);
  sMARTuniqueID.SetSerialNumber(serialNumberString);
  LOGN("data carrier ID:" << sMARTuniqueID)
  return true;
}

int CheckSMARTidRange(const int SMARTattributeID)
{
  if( SMARTattributeID >= NUM_DIFFERENT_SMART_ENTRIES)
  {
    LOGN_ERROR("SMART ID too high:" << SMARTattributeID)
    return 1;
  }
  if( SMARTattributeID < 0)
  {
    LOGN_ERROR("SMART ID is negative:" << SMARTattributeID)
    return -1;
  }
  return 0;
}

void HandleSingleSMARTentry(
  tinyxml2::XMLElement * p_SMARTelement,
  SMARTuniqueIDandValues & sMARTuniqueIDandValues)
{
  const int SMARTattributeID = p_SMARTelement->IntAttribute("ID", 0);
  if( CheckSMARTidRange(SMARTattributeID) != 0)
  {
    LOGN_ERROR("SMART ID is not in range 0..255->not processing this SMART entry")
    return;
  }
  const int64_t SMARTrawVal = p_SMARTelement->Int64Attribute("raw_value", 0);
  SMARTvalue & sMARTvalue = sMARTuniqueIDandValues.m_SMARTvalues[SMARTattributeID];
  if( SMARTrawVal < 0)
  {
    LOGN_ERROR("SMART raw value for attrib ID " << SMARTattributeID 
      << " is negative:" << SMARTrawVal)
    //sMARTuniqueIDandValues.m_successfullyReadSMARTrawValue[SMARTattributeID] = 0;
    AtomicExchange(& sMARTvalue.m_successfullyReadSMARTrawValue, 0);
    return;
  }
  AtomicExchange(& sMARTvalue.m_successfullyReadSMARTrawValue, 1);
  const float timeInS = p_SMARTelement->FloatAttribute("time_in_s", 0.0f);
  
  AtomicExchange(& sMARTvalue.m_timeStampOfRetrieval, timeInS * 1000.0f );
  
  LOGN("adding SMART raw value " << SMARTrawVal << " (time:" << timeInS 
    << ") to SMART ID " << SMARTattributeID)
  sMARTvalue.SetRawValue(SMARTrawVal);
}

void GetSMARTrawValues(
  tinyxml2::XMLElement * p_tinyxml2XMLelement, 
  SMARTuniqueIDandValues & sMARTuniqueIDandValues)
{
  tinyxml2::XMLElement * p_SMARTelement = p_tinyxml2XMLelement->
    FirstChildElement("SMART");
  if( ! p_SMARTelement)
  {
    LOGN_ERROR("no SMART XML element name below the root element")
    return;
  }
  /** from http://stackoverflow.com/questions/13919817/sscanf-and-locales-how-does-one-really-parse-things-like-3-14 :
   *  For "FloatAttribute" which uses "sscanf" to use "." as a decimal point. */
  setlocale(LC_NUMERIC, "C");
  HandleSingleSMARTentry(p_SMARTelement, sMARTuniqueIDandValues);
  
  p_SMARTelement = p_SMARTelement->NextSiblingElement("SMART");
  while(p_SMARTelement)
  {
    HandleSingleSMARTentry(p_SMARTelement, sMARTuniqueIDandValues);
    p_SMARTelement = p_SMARTelement->NextSiblingElement("SMART");
  }
  LOGN("end")
}

void SMARTmonitorClient::GetSMARTdataViaXML(
  uint8_t * SMARTvalues, unsigned numBytesToRead,
  //std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainter
  SMARTuniqueIDandValues & sMARTuniqueIDandValues)
{
  std::string xmlData( (const char *) SMARTvalues);
  LOGN("XML data:" << xmlData)
  tinyxml2::XMLDocument tinyXML2Doc;
  const tinyxml2::XMLError xmlErr = tinyXML2Doc.Parse(xmlData.c_str(), numBytesToRead);
  LOGN("result of parsing XML data:" << xmlErr)
  if( xmlErr != tinyxml2::XML_SUCCESS)
  {
    switch(xmlErr)
    {
      case tinyxml2::XML_ERROR_PARSING_ELEMENT :
        LOGN_ERROR("XML parsing error")
        break;
    }
    return;
  }
  tinyxml2::XMLElement * p_tinyxml2XMLelement = tinyXML2Doc.RootElement();
  if( ! p_tinyxml2XMLelement )
  {
    LOGN_ERROR("Failed to get XML root element")
    return;
  }
  LOGN("got XML root element")
  SMARTuniqueID sMARTuniqueID;
  GetSMARTuniqueID(p_tinyxml2XMLelement,sMARTuniqueID);
  //SMARTuniqueIDandValues sMARTuniqueIDandValues(sMARTuniqueID);
  sMARTuniqueIDandValues.SetDataCarrierID(sMARTuniqueID);
  //TODO
  GetSMARTrawValues(p_tinyxml2XMLelement, sMARTuniqueIDandValues);
//  SMARTuniqueIDandValues.m_SMARTrawValues;
  //sMARTuniqueIDandValuesContainter.insert(sMARTuniqueIDandValues);
}

void HandleXMLresult(const tinyxml2::XMLError XMLparsingResult)
{
  LOGN("result of parsing XML data:" << XMLparsingResult)
  if( XMLparsingResult != tinyxml2::XML_SUCCESS)
  {
    LOGN_ERROR("XML error occured")
    switch(XMLparsingResult)
    {
      case tinyxml2::XML_ERROR_PARSING_ELEMENT :
        LOGN_ERROR("XML parsing error")
        break;
    }
    return;
  }
}

void ConvertStringToInt(
  const char * const p_BeginOfNumber, 
  supportedSMARTattributeIDs_type & supportedSMARTattributeIDs )
{
  fastestUnsignedDataType number;
  //std::string std_strConvertToNumber(p_BeginOfNumber);
  
  if( ConvertCharStringToTypename(number, p_BeginOfNumber) )
  {
    if( CheckSMARTidRange(number) == 0 )
    {
      LOGN_DEBUG( "string:" << p_BeginOfNumber << " as int:" << number )
      supportedSMARTattributeIDs.insert( number );
    }
    else
      LOGN_ERROR("SMART ID is not in range 0..255->not processing this SMART entry")
  }
  else
    LOGN_ERROR("error converting " << p_BeginOfNumber << " to a number")
}

void SMARTmonitorClient::GetSupportedSMARTattributesViaXML(
  uint8_t * xmlDataByteArray,
  fastestUnsignedDataType numBytesToRead,
  //std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainter
  dataCarrierID2supportedSMARTattrMap_type & 
    dataCarrierID2supportedSMARTattributess)
{
  std::string std_strXMLdata( (const char *) xmlDataByteArray);
  LOGN("XML data:" << std_strXMLdata)
  //TODO source out into tinyxml2.cpp because this code is specific to the XML lib
  tinyxml2::XMLDocument tinyXML2Doc;
  const tinyxml2::XMLError XMLparsingResult = tinyXML2Doc.Parse(
    std_strXMLdata.c_str(), 
    numBytesToRead);
  HandleXMLresult(XMLparsingResult);
  if( XMLparsingResult != tinyxml2::XML_SUCCESS )
  {
    ShowMessage("error parsing the XML data for supported SMART attributes", MessageType::error);
    LOGN_ERROR("error parsing the XML data->no further processing->exit")
    return;
  }
  tinyxml2::XMLElement * p_tinyxml2XMLelement = tinyXML2Doc.RootElement();
  if( ! p_tinyxml2XMLelement )
  {
    LOGN_ERROR("Failed to get XML root element")
    return;
  }
  LOGN_DEBUG("got XML root element")
  SMARTuniqueID sMARTuniqueID;
  if( GetSMARTuniqueID(p_tinyxml2XMLelement, sMARTuniqueID) )
  {
    p_tinyxml2XMLelement = p_tinyxml2XMLelement->FirstChildElement("supportedSMART_IDs");
    if( ! p_tinyxml2XMLelement )
    {
      LOGN_ERROR("no \"supportedSMART_IDs\" child element in XML data??")
      return;
    }
    const char * supportedSMARTattributeIDsString = p_tinyxml2XMLelement->GetText();
//      "supportedSMART_IDs", NULL);
    if( supportedSMARTattributeIDsString == NULL )
    {
      LOGN_ERROR("no \"supportedSMART_IDs\" attribute in XML data??")
      return;
    }
    LOGN_DEBUG( "text inside \"supportedSMART_IDs\":" << 
      supportedSMARTattributeIDsString )
    supportedSMARTattributeIDs_type supportedSMARTattributeIDs;
    
    const char * p_lastComma = (char * ) supportedSMARTattributeIDsString;
    char * p_currentChar=(char * )supportedSMARTattributeIDsString;
    for( ; *p_currentChar != '\0' ; ++ p_currentChar)
    {
      if( * p_currentChar == ',')
      {
        * p_currentChar = '\0';
        ConvertStringToInt(p_lastComma, supportedSMARTattributeIDs);
        p_lastComma = p_currentChar + 1;
      }
      //TODO
//      GetSupportedSMARTattributes(p_tinyxml2XMLelement, supportedSMARTattributes);
    }
    if( p_lastComma < p_currentChar ) /** Process last attribute ID */
      ConvertStringToInt(p_lastComma, supportedSMARTattributeIDs);
      
    dataCarrierID2supportedSMARTattributess.insert( std::make_pair(sMARTuniqueID, 
      supportedSMARTattributeIDs) );
  }
}
