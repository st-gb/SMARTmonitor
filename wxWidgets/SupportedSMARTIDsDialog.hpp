/*
 * SupportedSMARTIDs.hpp
 *
 *  Created on: 29.10.2016
 *      Author: root
 */

#ifndef WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_
#define WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_

#include <wx/dialog.h> //base class wxDialog
#include <attributes/SMARTattributeNameAndID.hpp>
#include <vector> //class std::vector

/** Forward declarations: */
class wxListCtrl;

class SupportedSMART_IDsDialog
  : public wxDialog
{
  wxListCtrl * m_pwxlistctrl;
public:
  SupportedSMART_IDsDialog (std::vector<SMARTattributeNameAndID> &);
  virtual
  ~SupportedSMART_IDsDialog ();
  enum columnIndices { COL_IDX_SMART_ID = 0 , COL_IDX_SMARTparameterName};
};

#endif /* WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_ */
