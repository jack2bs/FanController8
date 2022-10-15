#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Id.h"
#include "ParentFrame.h"

class ParentFrame;

class ControlPanel : public wxControl
{
private:
	wxButton* m_sendProfileButton;
	wxButton* m_saveProfileButton;
	wxButton* m_loadProfileButton;
	wxComboBox* m_profileSelect;
	wxButton* m_setStartupProfile;
	ParentFrame* m_frameParent;

public:
	ControlPanel(ParentFrame *parent, 
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0 | wxBORDER_NONE,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxASCII_STR(wxControlNameStr));

	void resize();

	void onCBText(wxCommandEvent& event);
	void onSaveButton(wxCommandEvent& event);
	void onLoadButton(wxCommandEvent& event);
	void onSendButton(wxCommandEvent& event);
	void onUseButton(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

