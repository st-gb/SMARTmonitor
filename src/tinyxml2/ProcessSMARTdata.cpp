#include <tinyxml2.h> //tinyxml2::XMLElement etc.

#include <client/SMARTmonitorClient.h>

///from Stefan Gebauer's common_sourcecode repository:
#include <Controller/character_string/ConvertStdStringToTypename.hpp>
#include <hardware/CPU/atomic/AtomicExchange.h>///AtomicExchange(...)
#include <hardware/dataCarrier/ATA3Std.h>///numDifferentSMART_IDsPlus1
#include <preprocessor_macros/logging_preprocessor_macros.h>
//#include <data_structures/set.hpp>///std::set that uses emplace if >= C++11

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
  LOGN_DEBUG("data carrier ID:" << sMARTuniqueID)
  return true;
}

int CheckSMARTidRange(const int SMARTattributeID)
{
  if(SMARTattributeID > numDifferentSMART_IDsPlus1)
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
  
  const int64_t lowerUnitBound = p_SMARTelement->Int64Attribute(
    "lower_unit_bound", 0);
  const char * unit = p_SMARTelement->Attribute("unit", NULL);
  if(lowerUnitBound && strlen(unit) > 0){
    if(unit[0] == '>')
      ;
  }
  
  const float timeInS = p_SMARTelement->FloatAttribute("time_in_s", 0.0f);
  //TODO inaccuracy because of floating point: 764717.375 * 1000.0f =764717376
  sMARTvalue.SetRetrievalTime(timeInS);
  
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

tinyxml2::XMLElement * getXMLdoc(
  const uint8_t SMARTdataXML[],
  const fastestSignedDataType numBytesToRead,
  tinyxml2::XMLDocument & tinyXML2Doc)
{
  std::string xmlData( (const char *) SMARTdataXML);
  LOGN_DEBUG("XML data:" << xmlData)
  const tinyxml2::XMLError xmlErr = tinyXML2Doc.Parse(xmlData.c_str(),
    numBytesToRead);
  LOGN("result of parsing XML data:" << xmlErr)
  if( xmlErr != tinyxml2::XML_SUCCESS)
  {
    switch(xmlErr)
    {
      case tinyxml2::XML_ERROR_PARSING_ELEMENT :
        LOGN_ERROR("XML parsing error")
        break;
    }
    return (tinyxml2::XMLElement *)xmlErr;
  }
  tinyxml2::XMLElement * p_tinyxml2XMLelement = tinyXML2Doc.RootElement();
  if( ! p_tinyxml2XMLelement )
  {
    LOGN_ERROR("Failed to get XML root element")
  }
  LOGN_DEBUG("got XML root element")
  return p_tinyxml2XMLelement;
}

void GetSMARTuniqueID(
  tinyxml2::XMLElement * p_tinyxml2XMLelement,
  SMARTuniqueIDandValues & sMARTuniqueIDandValues){
  SMARTuniqueID sMARTuniqueID;
  GetSMARTuniqueID(p_tinyxml2XMLelement,sMARTuniqueID);
  //SMARTuniqueIDandValues sMARTuniqueIDandValues(sMARTuniqueID);
  sMARTuniqueIDandValues.SetDataCarrierID(sMARTuniqueID);
}

namespace tinyxml2{
bool SrvDataProcessor::Begin(
  const uint8_t SMARTdataXML[],
  fastestSignedDataType numBytesToRead){
  p_tinyxml2XMLele = getXMLdoc(
    SMARTdataXML,
    numBytesToRead,
    tinyXML2Doc);
  if(p_tinyxml2XMLele > (tinyxml2::XMLElement *)tinyxml2::XML_ERROR_COUNT)
    return true;
  return false;
}
void SrvDataProcessor::GetSMARTuniqueID(
  SMARTuniqueIDandValues & sMARTuniqueIDandValues){
  ::GetSMARTuniqueID(p_tinyxml2XMLele, sMARTuniqueIDandValues);
}
void SrvDataProcessor::GetSMARTrawValues(
  SMARTuniqueIDandValues & sMARTuniqueIDandValues){
  ::GetSMARTrawValues(p_tinyxml2XMLele, sMARTuniqueIDandValues);
}
}///End namespace tinyxml2

void SMARTmonitorClient::GetSMARTdataViaXML(
  const uint8_t * SMARTdataXML, const unsigned numBytesToRead,
  SMARTuniqueIDandValues & sMARTuniqueIDandValues
  )
{
  tinyxml2::XMLDocument tinyXML2Doc;
  tinyxml2::XMLElement * p_tinyxml2XMLele = ::getXMLdoc(
    SMARTdataXML,
    numBytesToRead,
    tinyXML2Doc);
  if(p_tinyxml2XMLele > (tinyxml2::XMLElement *) tinyxml2::XML_ERROR_COUNT){
    GetSMARTuniqueID(p_tinyxml2XMLele, sMARTuniqueIDandValues);
    GetSMARTrawValues(p_tinyxml2XMLele, sMARTuniqueIDandValues);
  }
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

int ConvertStringToInt(const char * const p_BeginOfNumber)
{
  fastestUnsignedDataType number;
  //std::string std_strConvertToNumber(p_BeginOfNumber);
  
  if( ConvertCharStringToTypename(number, p_BeginOfNumber) )
  {
    if( CheckSMARTidRange(number) == 0 )
    {
      LOGN_DEBUG( "string:" << p_BeginOfNumber << " as int:" << number )
      return number;
    }
    else
      LOGN_ERROR("SMART ID is not in range 0..255->not processing this SMART entry")
  }
  else
    LOGN_ERROR("error converting " << p_BeginOfNumber << " to a number")
  return 0;
}

void SMARTmonitorClient::GetSupportedSMARTattributesViaXML(
  uint8_t * xmlDataByteArray,
  fastestUnsignedDataType numBytesToRead,
  //std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainter
  dataCarrierID2supportedSMARTattrMap_type & 
    dataCarrierID2supportedSMARTattrs)
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

    /** It saves copy constructor operations if SMARTuniqueIDandValues is
     * inserted here and the iterator used for changing. */
//    pair<std::set<SMARTuniqueIDandValues>::iterator,bool> insert =
//      SMARTuniqueIDsAndValues.insert(SMARTuniqueIDandValues());

    const char * p_lastComma = (char * ) supportedSMARTattributeIDsString;
    char * p_currChar=(char *)supportedSMARTattributeIDsString;
    fastestUnsignedDataType arrIdx = 0;
    suppSMART_IDsType suppSMARTattrNamesAndIDs;
    supportedSMARTattrIDsType supportedSMARTattrIDs;
    for(;*p_currChar != '\0'; ++ p_currChar, arrIdx++)
    {
      if(* p_currChar == ',')
      {
        * p_currChar = '\0';
        const int number = ConvertStringToInt(p_lastComma);
//      supportedSMARTattrIDs.insert(number);
        sMARTuniqueID.supportedSMART_IDs[arrIdx] = number;
        supportedSMARTattrIDs.insert(number);
        suppSMARTattrNamesAndIDs.insert(SMARTattributeNameAndID("", number) );
        p_lastComma = p_currChar + 1;
      }
      //TODO
//      GetSupportedSMARTattributes(p_tinyxml2XMLelement, supportedSMARTattributes);
    }
    if(p_lastComma < p_currChar){/**Process last attribute ID*/
      const int number = ConvertStringToInt(p_lastComma);
      supportedSMARTattrIDs.insert(number);
      suppSMARTattrNamesAndIDs.insert(SMARTattributeNameAndID("", number) );
      sMARTuniqueID.supportedSMART_IDs[arrIdx] = number;
    }
    if(arrIdx < numDifferentSMART_IDs)
      sMARTuniqueID.supportedSMART_IDs[arrIdx] = 0;///Mark last item

    //sMARTuniqueID.setSupportedSMART_IDs(suppSMARTattrNamesAndIDs);
    sMARTuniqueID.SetSMART_IDsToRead(suppSMARTattrNamesAndIDs,
      m_SMARTattrIDsToObs);
    //TODO Uncomment. Causes not to display the current SMART data at least if
    // data from service.
    SMARTuniqueIDsAndValues.insert/*emplace insOrEmpl*/(SMARTuniqueIDandValues(
      sMARTuniqueID) );

    dataCarrierID2supportedSMARTattrs.insert( std::make_pair(sMARTuniqueID, 
      supportedSMARTattrIDs) );
  }
}
