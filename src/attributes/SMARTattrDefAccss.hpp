/** File:   SMARTattrDefAccss.hpp
 * Author: S.Gebauer
 * Created on 31. Mai 2020, 22:20 */

#ifndef SMARTATTRDEFACCSS_HPP
#define SMARTATTRDEFACCSS_HPP

#include "SMARTattrDef.hpp"
#include "SMARTuniqueIDandValues.hpp"///NUM_DIFFERENT_SMART_ENTRIES

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
  /** The first definition is stored at index 1*/
  static SMARTattrDef SMARTattrDefs[numDifferentSMART_IDsPlus1];
  
public:
static void Add(const SMARTattrDef & sMARTattrDef)
{
  SMARTattrDefs[sMARTattrDef.GetAttributeID()] = sMARTattrDef;
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

static bool getSMARTattrDef(const fastestUnsignedDataType SMARTattrID, 
  SMARTattrDef & sMARTattrDef){
//  bool SMARTattrDefFound = false;
  SMARTattrDef * p_SMARTattrDef = getSMARTattrDef(SMARTattrID);
  if(p_SMARTattrDef)
    sMARTattrDef = *p_SMARTattrDef;
    return true;
  return false;
}

static SMARTattrDef * getSMARTattrDef(const fastestUnsignedDataType SMARTattrID)
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
  return & SMARTattrDefs[SMARTattrID];
}

static SMARTattrDefsType /*&*/ getSMARTattrDefs(){ return SMARTattrDefs;}
private:

};

#endif /* SMARTATTRDEFACCSS_HPP */
