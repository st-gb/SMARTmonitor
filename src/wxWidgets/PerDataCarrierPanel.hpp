#pragma once///Include guard

/** Include at 1st in Windows build to avoid:
 * "#warning Please include winsock2.h before windows.h" */
#include "SMARTtableListCtrl.hpp"///class wxWidgets::SMARTtableListCtrl

///wxWidgets header files:
#include <wx/button.h>///class wxButton
#include <wx/listctrl.h>///class wxListCtrl
#include <wx/panel.h>///class wxPanel
#include <wx/sizer.h>///class wxSizer


///from Stefan Gebauer's "common_sourcecode" repository :
///wxWidgets::GetwxString_Inline(...)
#include <wxWidgets/charStr/wxStringHelper.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h>///LOGN_DEBUG(...)

using namespace wxWidgets;

class PerDataCarrierPanel
  : public wxPanel
{
  wxTextCtrl * m_p_wxDataCarrierIDtextCtrl;
  wxButton * m_p_showSupportedSMART_IDs;
  TU_Bln361095::hardware::bus::Type m_busType;
public:
  SMARTtableListCtrl * m_pwxlistctrl;
  PerDataCarrierPanel(
    wxWindow * parent,
    const wxWindowID windowID,
    const SMARTuniqueID & sMARTuniqueID)
    : wxPanel(parent)
  {
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

    m_p_wxDataCarrierIDtextCtrl = new wxTextCtrl(this, wxID_ANY, wxT("") );
    m_p_wxDataCarrierIDtextCtrl->SetEditable(false);
    m_p_wxDataCarrierIDtextCtrl->SetToolTip(wxT("data carrier ID"));

    wxSizer * const diskIDsizer = new wxBoxSizer(wxHORIZONTAL);

    //TODO button for: bytewise read whole data carrier _or_ :
    // 1. long S.M.A.R.T. self-test, 2. if fails read whole data carrier
  //  m_p_getDISKhealth() = new wxBitmapButton(this);
    //TODO: retrieve unit all the time until the unit is determined (see class
    // SMARTuniqueID)
  //  m_p_determineUnits() = new wxBitmapButton(this);
    m_p_showSupportedSMART_IDs = new wxButton(this, windowID,
      wxT("supported SMART IDs"));

    diskIDsizer->Add( m_p_wxDataCarrierIDtextCtrl,
      /** "in the main orientation of the wxBoxSizer - where 0 stands for not
       * changeable" */
      1,
      wxEXPAND, 0);
    diskIDsizer->Add( m_p_showSupportedSMART_IDs, 0, wxSTRETCH_NOT, 0);
    sizerTop->Add( diskIDsizer, 0, wxEXPAND, 0);

    m_pwxlistctrl = new SMARTtableListCtrl(this,
      sMARTuniqueID
      //, wxID_ANY, wxDefaultPosition,
      //wxDefaultSize, wxLC_REPORT
      );

    sizerTop->Add( m_pwxlistctrl,
      /** "in the main orientation of the wxBoxSizer - where 0 stands for not changeable" */
      1 //proportion
      , wxEXPAND //int flag=0
      , /*int border*/ 0);
    SetSizerAndFit(sizerTop);
  }

  wxButton * getSupportedSMART_IDsBtn() const{
    return m_p_showSupportedSMART_IDs;}
  
  void ReCreate(const SMARTuniqueID & sMARTuniqueID){
    /** TODO: avoid creating of double lines if connecting to a server more than
     * once*/
    const bool allItemsDeleted = m_pwxlistctrl->DeleteAllItems();
    LOGN_DEBUG("all list control items deleted:" << (allItemsDeleted ? "yes" :
      "no" ))
    m_pwxlistctrl->CreateLines(sMARTuniqueID);
  }

  void setIDandLabel(const fastestUnsignedDataType SMARTattrID, void * data)
  {
    wxString label = wxString::Format(wxT("%u"), SMARTattrID);
    m_pwxlistctrl->SetSMARTattribValue(
      SMARTattrID,
      TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::ID,
      label,
      TU_Bln361095::SMARTmon::SMARTattrVal::Rtg::NoCriticalVal
      );

    /** Now get the attribute name belonging to SMART ID */
    SMARTattrDef * p_sMARTattrDef;
    ///Use S.M.A.R.T. attribute name according to bus type (ATA, NVMe, ...):
    switch(m_busType)
    {
     case TU_Bln361095::hardware::bus::NVMe:
      p_sMARTattrDef = SMARTattrDefAccss::getNVMeSMARTattrDef(
        SMARTattrID);
        break;
     default:
      p_sMARTattrDef = SMARTattrDefAccss::getATA_SMARTattrDef(
        SMARTattrID);
        break;
    }
    std::string stdstrSMARTattrName;
    if( p_sMARTattrDef != NULL)
    {
      const SMARTattrDef & sMARTattrDef = *p_sMARTattrDef;
      stdstrSMARTattrName = sMARTattrDef.GetName();
    }

    label = TU_Bln361095::wxWidgets::GetwxString_inln(stdstrSMARTattrName),
    m_pwxlistctrl->SetSMARTattribValue(
      SMARTattrID,
      TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::Name,
      label,
      TU_Bln361095::SMARTmon::SMARTattrVal::Rtg::NoCriticalVal
      );
    m_pwxlistctrl->setMaxSMARTattrNmStrWdthInPx(label);
  }

  void SetSMARTattribIDandNameLabel(const SMARTuniqueID & sMARTuniqueID){
    fastestUnsignedDataType SMARTattributeID;
    /** Traverse intersection of all SMART attribute IDs to read and supported
     *  SMART IDs either got from server or read from config file.*/
    for(fastestUnsignedDataType SMART_IDsToReadIdx = 0;
      sMARTuniqueID.SMART_IDsToReadNotEnd(SMART_IDsToReadIdx);
      SMART_IDsToReadIdx++)
    {
      SMARTattributeID = sMARTuniqueID.m_SMART_IDsToRd[SMART_IDsToReadIdx];
      setIDandLabel(SMARTattributeID, NULL);
    }
  }

  void SetSMARTdriveID(const SMARTuniqueID & sMARTuniqueID)
  {
    std::ostringstream stdoss;
    stdoss << "model:" << std::string(sMARTuniqueID.m_modelName,
      /**The length of the character string must be limited if it does not 
       * end with a terminating \0 character.*/
      TU_Bln361095hardwareSMARTnumModelBytes);
    stdoss << " firmware:" << std::string(sMARTuniqueID.m_firmWareName,
      /**The length of the character string must be limited if it does not
       * end with a terminating \0 character.*/
      TU_Bln361095hardwareSMARTnumFWbytes);
    stdoss << " serial:" << std::string(sMARTuniqueID.m_serialNumber,
      /**The length of the character string must be limited if it does not
       * end with a terminating \0 character.*/
      TU_Bln361095hardwareSMARTnumSNbytes);
    const std::string mediaInfo = stdoss.str();
    const wxString & label = TU_Bln361095::wxWidgets::GetwxString_inln(mediaInfo);
    SetDataCarrierLabel(label);
    m_busType = sMARTuniqueID.getBusType();
  }

  void EnableShowSupportedSMART_IDs() const {m_p_showSupportedSMART_IDs->Enable(); }
  void SetDataCarrierLabel(const wxString & label){
    m_p_wxDataCarrierIDtextCtrl->SetValue(label); }
};
