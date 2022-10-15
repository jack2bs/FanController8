#include "ControlPanel.h"



ControlPanel::ControlPanel(ParentFrame *parent,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxValidator& validator,
	const wxString& name)
	:
	wxControl{ parent, window::id::CONTROL_PANEL_ID, pos, size, style, validator, name },
	m_frameParent{ parent }
{

	wxSize buttonSize(GetSize().GetWidth() - (2 * parent->GetSize().GetWidth() / 100), (GetSize().GetHeight() - (2 * GetSize().GetWidth() / 100))/10);
	int buttonPosx = parent->GetSize().GetWidth() / 100;
	m_sendProfileButton = new wxButton(this, window::id::CONTROL_PANEL_SEND_BUTTON, "Send Profile To Fan Controller",wxPoint(buttonPosx, parent->GetSize().GetWidth() / 100),buttonSize);
	m_setStartupProfile = new wxButton(this, window::id::CONTROL_PANEL_USE_BUTTON, "Use Profile On Startup", wxPoint(buttonPosx, buttonSize.y + 2 * parent->GetSize().GetWidth() / 100), buttonSize);
	m_loadProfileButton = new wxButton(this, window::id::CONTROL_PANEL_LOAD_BUTTON, "Load Profile From Computer", wxPoint(buttonPosx, buttonSize.y * 2 + 3 * parent->GetSize().GetWidth() / 100), buttonSize);
	m_saveProfileButton = new wxButton(this, window::id::CONTROL_PANEL_SAVE_BUTTON, "Save Profile To Computer", wxPoint(buttonPosx, buttonSize.y * 3 + 4 * parent->GetSize().GetWidth() / 100), buttonSize);
	m_loadProfileButton->Enable(false);
	m_saveProfileButton->Enable(false);


	wxString choices[]{ "Profile 1", "Profile 2", "Profile 3", "Profile 4", "Profile 5" };
	m_profileSelect = new wxComboBox(this, window::id::CONTROL_PANEL_SELECT, "Profile Select", wxPoint(buttonPosx, buttonSize.y * 4 + 5 * parent->GetSize().GetWidth() / 100), buttonSize, 5, choices, wxCB_READONLY);
}

void ControlPanel::resize()
{
	int buttonPosx = m_parent->GetSize().GetWidth() / 100;
	wxSize buttonSize(GetSize().GetWidth() - (2 * m_parent->GetSize().GetWidth() / 100), (GetSize().GetHeight() - (2 * GetSize().GetWidth() / 100)) / 10);

	SetSize(m_parent->GetSize().GetWidth() * 25 / 136 - m_parent->GetSize().GetWidth() / 100, m_parent->GetSize().GetHeight() * 83 / 233 - m_parent->GetSize().GetWidth() / 100);
	m_sendProfileButton->SetPosition(wxPoint(buttonPosx, m_parent->GetSize().GetWidth() / 100));
	m_sendProfileButton->SetSize(buttonSize);
	m_setStartupProfile->SetPosition(wxPoint(buttonPosx, buttonSize.y + 2 * m_parent->GetSize().GetWidth() / 100));
	m_setStartupProfile->SetSize(buttonSize);
	m_loadProfileButton->SetPosition(wxPoint(buttonPosx, buttonSize.y * 2 + 3 * m_parent->GetSize().GetWidth() / 100));
	m_loadProfileButton->SetSize(buttonSize);
	m_saveProfileButton->SetPosition(wxPoint(buttonPosx, buttonSize.y * 3 + 4 * m_parent->GetSize().GetWidth() / 100));
	m_saveProfileButton->SetSize(buttonSize);
	m_profileSelect->SetPosition(wxPoint(buttonPosx, buttonSize.y * 4 + 5 * m_parent->GetSize().GetWidth() / 100));
	m_profileSelect->SetSize(buttonSize);


}

void ControlPanel::onCBText(wxCommandEvent& event)
{
	if (m_profileSelect != nullptr)
	{
		if (m_profileSelect->GetValue() != "Profile Select")
		{
			m_loadProfileButton->Enable();
			m_saveProfileButton->Enable();
		}
		else
		{
			m_loadProfileButton->Enable(false);
			m_saveProfileButton->Enable(false);
		}
	}
}

void ControlPanel::onSaveButton(wxCommandEvent& event)
{
	int profileNum = m_profileSelect->GetValue().at(8) - '1';

	m_frameParent->getAppParent()->editFanProfile(profileNum);
}

void ControlPanel::onLoadButton(wxCommandEvent& event)
{
	int profileNum = m_profileSelect->GetValue().at(8) - '1';

	m_frameParent->getAppParent()->loadFanProfile(profileNum);
}

void ControlPanel::onSendButton(wxCommandEvent& event)
{
	m_frameParent->getAppParent()->sendProfile();
}

void ControlPanel::onUseButton(wxCommandEvent& event)
{
	m_frameParent->getAppParent()->saveProfile();
}

wxBEGIN_EVENT_TABLE(ControlPanel, wxControl)
EVT_TEXT(window::id::CONTROL_PANEL_SELECT, ControlPanel::onCBText)
EVT_BUTTON(window::id::CONTROL_PANEL_SAVE_BUTTON, ControlPanel::onSaveButton)
EVT_BUTTON(window::id::CONTROL_PANEL_LOAD_BUTTON, ControlPanel::onLoadButton)
EVT_BUTTON(window::id::CONTROL_PANEL_SEND_BUTTON, ControlPanel::onSendButton)
EVT_BUTTON(window::id::CONTROL_PANEL_USE_BUTTON, ControlPanel::onUseButton)
wxEND_EVENT_TABLE()