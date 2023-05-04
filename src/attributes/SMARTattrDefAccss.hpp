/** File:   SMARTattrDefAccss.hpp
 * (c) Stefan Gebauer(Computer Science Master from TU Berlin)
 * @author Stefan Gebauer(TU Berlin matriculation number 361095)
 * Created on 31. May 2020, 22:20 (Berlin, Germany time zone) */

/**Bln=BerLiN:http://www.acronymfinder.com/Berlin-(Bln).html
 * Prgm=PRaGMa
 * Incl=INCLude:http://www.abbreviations.com/abbreviation/include
 * Grd=GuaRD:http://www.abbreviations.com/abbreviation/guard */
#ifdef TU_Bln361095usePrgmInclGrd
/**Non-standard include guard:supported by many, but not all industry compilers:
 * see http://en.wikipedia.org/wiki/Pragma_once#Portability */
  #pragma once
#endif
#if defined TU_Bln361095usePrgmInclGrd ||\
/**Include guard supported by (nearly) all industry compilers:*/\
/**Bln=BerLiN:http://www.acronymfinder.com/Berlin-(Bln).html
 * mon=MONitor: http://www.abbreviations.com/abbreviation/monitor 
 * attr=ATTRibute: http://www.abbreviations.com/abbreviation/Attribute
 * Def=DEFinition: http://www.abbreviations.com/abbreviation/definition */\
 !defined TU_Bln361095SMARTmon_SMARTattrDefAccess_hpp
  #define TU_Bln361095SMARTmon_SMARTattrDefAccess_hpp

///_this_ repository's header files:
#include "SMARTattrDef.hpp"///class SMARTattrDef
 ///numDifferentSMART_IDsPlus1,TU_Bln361095dataCarrierNumNVMeSMART_IDsPlus1
#include "SMARTuniqueIDandValues.hpp"

/** @brief Puts all SMARTattrDef accessing functions into a class.
* You may subclass from this to make functions belong to other class.*/
class SMARTattrDefAccss///def=definition: https://en.wiktionary.org/wiki/def
{
///attr=attribute Def=definition Cont=container
//  typedef std::set<SMARTattrDef> SMARTattrDefContType;
//  typedef const SMARTattrDefContType constSMARTattrsContType;
//  typedef SMARTattrDefContType::const_iterator 
//    SMARTattrsContConstIterType;
  typedef SMARTattrDef * SMARTattrDefsType;
protected:
//  SMARTattrDefContType SMARTattrDefs;
  /**If SMART attribute definitions are in an array, access to it is very fast*/
  /**Array for (S)ATA S.M.A.R.T. The first definition is stored at index 1*/
  static SMARTattrDef SMARTattrDefs[numDifferentSMART_IDsPlus1];
  ///NVMe S.M.A.R.T. attribute names differ from ATA S.M.A.R.T. attribute names.
  static SMARTattrDef NVMeSMARTattrDefs[
    TU_Bln361095dataCarrierNumNVMeSMART_IDsPlus1];
  
public:
static void Add(const SMARTattrDef & sMARTattrDef,
  SMARTattrDef SMARTattrDefArr[])
{
  SMARTattrDefArr[sMARTattrDef.GetAttributeID()] = sMARTattrDef;
}
static void Set(const fastestUnsignedDataType attrID, const char attrName [])
{
  SMARTattrDef sMARTattrDef(attrID, attrName);
  SMARTattrDefs[attrID] = sMARTattrDef;
}

static void clearSMARTattrDefs()
{
//  SMARTattrDefs.clear();
  for(fastestUnsignedDataType idx = 0; idx < numDifferentSMART_IDsPlus1; idx++){
    SMARTattrDefs[idx].SetAttributeID(0);
    SMARTattrDefs[idx].SetName("");
    SMARTattrDefs[idx].SetCritical(false);
  }
}

static int GetNumSMARTattrDefs()
{
  fastestUnsignedDataType numSMARTattrDefs = 0;
//  numSMARTattrDefs = SMARTattrDefs.size();
  for(fastestUnsignedDataType idx = 0;idx < numDifferentSMART_IDsPlus1; idx++){
    if(SMARTattrDefs[idx].GetAttributeID() != 0)
      numSMARTattrDefs++;
  }
  return numSMARTattrDefs;
}

static bool getSMARTattrDef(
  const TU_Bln361095::CPU::FaststUint SMARTattrID, 
  SMARTattrDef & sMARTattrDef,
  SMARTattrDef SMARTattrDefArr[])
{
//  bool SMARTattrDefFound = false;
  SMARTattrDef * p_SMARTattrDef = getSMARTattrDef(SMARTattrID, SMARTattrDefArr);
  if(p_SMARTattrDef)
    sMARTattrDef = *p_SMARTattrDef;
    return true;
  return false;
}

inline static SMARTattrDef * getSMARTattrDef(
  const TU_Bln361095::CPU::FaststUint SMARTattrID,
  SMARTattrDef SMARTattrDefArr[])
{
//  SMARTattrContConstIterType SMARTattrDefIter = SMARTattrDefs.begin();
//  SMARTattrIter = r_SMARTattrCont.find( SMARTattrDef(SMART_ID) );
//  if( SMARTattrIter != r_SMARTattrCont.end() )
//  for( ; SMARTattrDefIter->GetAttributeID() < SMARTattrID && 
//     SMARTattrDefIter != SMARTattrDefs.end(); SMARTattrDefIter++)
//  {
//    LOGN_DEBUG("using SMART entry at address " << 
//      & (* SMARTattrDefIter) )
//    if(SMARTattrID == SMARTattrDefIter->GetAttributeID())
//      SMARTattrDefFound = true;
//  }
  return & SMARTattrDefArr[SMARTattrID];
}

inline static SMARTattrDef * getATA_SMARTattrDef(
  const TU_Bln361095::CPU::FaststUint SMARTattrID)
{
  return & SMARTattrDefs[SMARTattrID];
}

inline static SMARTattrDef * getNVMeSMARTattrDef(
  const TU_Bln361095::CPU::FaststUint SMARTattrID)
{
  return & NVMeSMARTattrDefs[SMARTattrID];
}

static SMARTattrDefsType /*&*/ getSMARTattrDefs(){ return SMARTattrDefs;}
private:

};

#endif///include guard
