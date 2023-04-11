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
public:
  SMARTtableListCtrl * m_pwxlistctrl;
  PerDataCarrierPanel(wxWindow * parent, const wxWindowID windowID)
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

    m_pwxlistctrl = new SMARTtableListCtrl(this//, wxID_ANY, wxDefaultPosition,
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
      colIndices::SMART_ID,
      label,
      noCriticalValue
      );

    /** Now get the attribute name belonging to SMART ID */
    SMARTattrDef * p_sMARTattrDef = SMARTattrDefAccss::getSMARTattrDef(
      SMARTattrID);
    std::string stdstrSMARTattrName;
    if( p_sMARTattrDef != NULL)
    {
      const SMARTattrDef & sMARTattrDef = *p_sMARTattrDef;
      stdstrSMARTattrName = sMARTattrDef.GetName();
    }

    label = TU_Bln361095::wxWidgets::GetwxString_inln(stdstrSMARTattrName),
    m_pwxlistctrl->SetSMARTattribValue(
      SMARTattrID,
      colIndices::SMARTparameterName,
      label,
      noCriticalValue
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
    std::ostringstream oss;
    oss << "model:" << sMARTuniqueID.m_modelName;
    oss << " firmware:" << sMARTuniqueID.m_firmWareName;
    oss << " serial:" << sMARTuniqueID.m_serialNumber;
    const std::string mediaInfo = oss.str();
    const wxString & label = TU_Bln361095::wxWidgets::GetwxString_inln(mediaInfo);
    SetDataCarrierLabel(label);
  }

  void EnableShowSupportedSMART_IDs() const {m_p_showSupportedSMART_IDs->Enable(); }
  void SetDataCarrierLabel(const wxString & label){
    m_p_wxDataCarrierIDtextCtrl->SetValue(label); }
};
