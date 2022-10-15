#include "ParentFrame.h"
#include <iostream>
#include <sstream>


ParentFrame::ParentFrame(wxWindow *parent, wxWindowID id, const wxString& title, int numberOfFans, MainApp* par, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	:	wxFrame(parent, id, title, pos, size, style, name),
	m_numberOfFans{ numberOfFans },
	m_AppParent{par}
{
	if (!wxPersistentRegisterAndRestore(this, GetName()))
	{
		SetSize(wxSystemSettings::GetMetric(wxSYS_SCREEN_X) / 2, wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) / 2);
	}


	fccVector.reserve(6);
	for (int i = 0; i < 6; i++)
	{
		FanCurveContainer *fccTemp = new FanCurveContainer(this, 0, 100, 1, 0, 100, 1, i, wxPoint(0,0), FanCurveContainer::DoGetBestSize(this));
		fccVector.push_back(fccTemp);
	}
	SetBackgroundColour(FRAME_BACKGROUND_COLOUR);	

	wxPoint controlPanelPos(GetSize().GetWidth() / 100, GetSize().GetWidth() / 100);
	wxSize controlPanelSize(GetSize().GetWidth() * 25 / 136 - GetSize().GetWidth() / 100, GetSize().GetHeight() * 83 / 233 - GetSize().GetWidth() / 100);
	m_controlPanel = new ControlPanel(this, controlPanelPos, controlPanelSize);
	m_controlPanel->Show();
	m_controlPanel->SetBackgroundColour(FCC_BACKGROUND_COLOUR);

	listInit();
}

void ParentFrame::listInit()
{
	m_listView = new wxListView(this, window::id::LIST_ID, wxPoint(GetSize().GetWidth() / 100, GetSize().GetHeight() * 83/233 + GetSize().GetWidth() / 100), wxSize(GetSize().GetWidth() * 25 / 136 - GetSize().GetWidth() / 100, fccVector.at(4)->getBottom() - GetSize().GetHeight() * 83 / 233));

	m_listView->DeleteAllColumns();
	m_listView->AppendColumn("Name");
	m_listView->AppendColumn("Value");
	m_listView->AppendColumn("Unit");

	m_listView->InsertItem(0, "CPU Temperature");
	float f = m_AppParent->getCpuTemp();
	std::stringstream ss;
	ss << f;
	m_listView->SetItem(0, 1, ss.str());
	m_listView->SetItem(0, 2, "°C");

	m_listView->InsertItem(1, "Case Temperature 1");
	//float holding case temp 1
	m_listView->SetItem(1, 2, "°C");

	m_listView->InsertItem(2, "Case Temperature 2");
	//float holding case temp 1
	m_listView->SetItem(2, 2, "°C");

	m_listView->InsertItem(3, "Fan 0 Speed");
	//float holding case temp 1
	m_listView->SetItem(3, 2, "rpm");

	m_listView->InsertItem(4, "Fan 1 Speed");
	//float holding case temp 1
	m_listView->SetItem(4, 2, "rpm");

	m_listView->InsertItem(5, "Fan 2 Speed");
	//float holding case temp 1
	m_listView->SetItem(5, 2, "rpm");

	m_listView->InsertItem(6, "Fan 3 Speed");
	//float holding case temp 1
	m_listView->SetItem(6, 2, "rpm");

	m_listView->InsertItem(7, "Fan 4 Speed");
	//float holding case temp 1
	m_listView->SetItem(7, 2, "rpm");

	m_listView->InsertItem(8, "Fan 5 Speed");
	//float holding case temp 1
	m_listView->SetItem(8, 2, "rpm");

}


//Closes the frame when the X is clicked
void ParentFrame::onExit(wxCommandEvent& event)
{
	m_AppParent->closerSerialPort();
	Close(true);
}

void ParentFrame::setNumberOfFans(int numberOfFans)
{
	m_numberOfFans = numberOfFans;
	fccVector.reserve(numberOfFans);
}

int ParentFrame::getNumberOfFans()
{
	return 6;
}

void ParentFrame::onResize(wxSizeEvent& event)
{
	if (m_listView != nullptr)
	{
		m_controlPanel->resize();
	}
	for (int i = 0; i < fccVector.size(); i++)
	{
		fccVector.at(i)->resize();
	}
	if (m_listView != nullptr)
	{
		m_listView->SetPosition(wxPoint(GetSize().GetWidth() / 100, GetSize().GetHeight() * 83 / 233 + GetSize().GetWidth() / 100));
		m_listView->SetSize(wxSize(GetSize().GetWidth() * 25 / 136 - GetSize().GetWidth() / 100, fccVector.at(4)->getBottom() - GetSize().GetHeight() * 83 / 233));
		m_listView->Refresh();
	}
}

MainApp* ParentFrame::getAppParent()
{
	return m_AppParent;
}

float ParentFrame::updateCPUTemp()
{
	float f = m_AppParent->getCpuTemp();
	std::stringstream ss;
	ss << f;
	m_listView->SetItem(0, 1, ss.str());
	return f;
}

void ParentFrame::updateListRpm(int fanNum, int RPM)
{
	std::stringstream ss;
	if (RPM == 9999)
	{
		ss << "3PIN";
	}
	else
	{
		ss << RPM;
	}
	m_listView->SetItem(3 + fanNum, 1, ss.str());
}

void ParentFrame::updateListTemp(int tempHeader, int temp)
{
	std::stringstream ss;
	ss << temp;
	m_listView->SetItem(1 + tempHeader, 1, ss.str());
}

BEGIN_EVENT_TABLE(ParentFrame, wxFrame)
	EVT_SIZE(ParentFrame::onResize)
END_EVENT_TABLE()

