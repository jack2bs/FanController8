#include "FanCurveContainer.h"

int FanCurveContainer::m_id = window::id::FANCURVE_CONTAINER_MIN_ID;

FanCurveContainer::FanCurveContainer(
	wxWindow *parent,
	int minTemp,
	int maxTemp,
	int incTemp,
	int minFanSpeed,
	int maxFanSpeed,
	int incFanSpeed,
	int fanHeader,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxValidator& validator,
	const wxString& name)
	: wxControl{ parent, m_id, pos, size, style, validator, name },
	m_fanheader{ fanHeader }
{
	SetPosition(DoGetBestPosition());


	SetBackgroundColour(FCC_BACKGROUND_COLOUR);
	m_graph = new FanCurveGraph(this, minTemp, maxTemp, incTemp, minFanSpeed, maxFanSpeed, incFanSpeed, wxPoint(parent->GetSize().GetWidth() / 100, parent->GetSize().GetWidth() / 100), FanCurveGraph::DoGetBestSize(this));

	m_id++;
	wxPoint newButtonPos(parent->GetSize().GetWidth() / 100, m_graph->getBottom() + parent->GetSize().GetWidth() / 100);
	m_newDataPointButton = new wxButton(this, m_id, "New Point", newButtonPos, wxSize(0,0), wxBORDER_NONE);
	m_newDataPointButton->SetSize(m_newDataPointButton->GetBestSize());

	m_id++;
	wxPoint deleteButtonPos(parent->GetSize().GetWidth() / 100 * 2 + m_newDataPointButton->GetSize().GetWidth(), m_graph->getBottom() + parent->GetSize().GetWidth() / 100);
	m_deleteDataPointButton = new wxButton(this, m_id, "Delete Point", deleteButtonPos, wxSize(0, 0), wxBORDER_NONE);
	m_deleteDataPointButton->SetSize(m_deleteDataPointButton->GetBestSize());

	m_id++;
	wxPoint tempSelectPos(parent->GetSize().GetWidth() / 100 + m_deleteDataPointButton->GetPosition().x + m_deleteDataPointButton->GetSize().GetWidth(), m_graph->getBottom() + parent->GetSize().GetWidth() / 100);
	wxString choices[]{ "Temp header 1", "Temp header 2", "CPU Temp" };
	m_tempSelect = new  wxComboBox(this, m_id, "Temp Select", tempSelectPos, wxSize(0, 0), 3, choices, wxCB_DROPDOWN | wxCB_READONLY);
	m_tempSelect->SetSize(m_tempSelect->GetBestSize());
	setTempSelect(2);

	m_id++;
	wxPoint fanSpeedReadoutPos(parent->GetSize().GetWidth() / 100 + tempSelectPos.x + m_tempSelect->GetSize().GetWidth(), m_graph->getBottom() + parent->GetSize().GetWidth() / 100);
	std::stringstream ss;
	ss << "Fan Speed: " << m_fanSpeed << " RPM";
	m_fanSpeedReadout = new wxTextCtrl(this, m_id, ss.str(), fanSpeedReadoutPos, wxSize(0, 0), wxTE_READONLY);
	m_fanSpeedReadout->SetSize(GetSize().GetWidth() - parent->GetSize().GetWidth() / 100 - m_fanSpeedReadout->GetPosition().x, m_fanSpeedReadout->GetBestSize().y);

	if (m_fanSpeedReadout->GetPosition().x + m_fanSpeedReadout->GetBestSize().GetWidth() > GetSize().GetWidth())
	{
		m_tempSelect->SetSize(GetSize().GetWidth() - parent->GetSize().GetWidth() / 100 - m_tempSelect->GetPosition().x, m_fanSpeedReadout->GetBestSize().y);
		m_fanSpeedReadout->Show(false);
	}


	//I only do this here because it won't work statically without the id of the button (I think)
	m_newDataPointButton->Bind(wxEVT_BUTTON, &FanCurveContainer::onNewPointPress, this);
	m_deleteDataPointButton->Bind(wxEVT_BUTTON, &FanCurveContainer::onDeletePointPress, this);
}

void FanCurveContainer::resize()
{
	SetPosition(DoGetBestPosition());
	SetSize(DoGetBestSize(m_parent));

	m_graph->resize();

	Refresh();

	m_newDataPointButton->SetPosition(wxPoint(m_parent->GetSize().GetWidth() / 100, m_graph->getBottom() + m_parent->GetSize().GetWidth() / 100));
	m_newDataPointButton->SetSize(m_newDataPointButton->GetBestSize());

	m_deleteDataPointButton->SetPosition(wxPoint(m_parent->GetSize().GetWidth() / 100 * 2 + m_newDataPointButton->GetSize().GetWidth(), m_graph->getBottom() + m_parent->GetSize().GetWidth() / 100));
	m_deleteDataPointButton->SetSize(m_deleteDataPointButton->GetBestSize());

	m_tempSelect->SetPosition(wxPoint(m_parent->GetSize().GetWidth() / 100 + m_deleteDataPointButton->GetPosition().x + m_deleteDataPointButton->GetSize().GetWidth(), m_graph->getBottom() + m_parent->GetSize().GetWidth() / 100));
	m_tempSelect->SetSize(m_tempSelect->GetBestSize());

	m_fanSpeedReadout->SetPosition(wxPoint(m_parent->GetSize().GetWidth() / 100 + m_tempSelect->GetPosition().x + m_tempSelect->GetSize().GetWidth(), m_graph->getBottom() + m_parent->GetSize().GetWidth() / 100));
	m_fanSpeedReadout->SetSize(GetSize().GetWidth() - m_parent->GetSize().GetWidth() / 100 - m_fanSpeedReadout->GetPosition().x, m_fanSpeedReadout->GetBestSize().y);

	if (m_fanSpeedReadout->GetPosition().x + m_fanSpeedReadout->GetBestSize().GetWidth() > GetSize().GetWidth())
	{
		m_tempSelect->SetSize(GetSize().GetWidth() - m_parent->GetSize().GetWidth() / 100 - m_tempSelect->GetPosition().x, m_fanSpeedReadout->GetBestSize().y);
		m_fanSpeedReadout->Show(false);
	}
	else
	{
		m_fanSpeedReadout->Show(true);
	}

	Refresh();
}

void FanCurveContainer::setTempSelect(int ts)
{
	switch (ts)
	{
	case 0:
		m_tempSelect->SetValue("CPU Temp");
		break;
	case 1:
		m_tempSelect->SetValue("Temp header 1");
		break;
	case 2:
		m_tempSelect->SetValue("Temp header 2");
		break;
	}
	return;
}

int FanCurveContainer::getButtonId()
{
	return m_newDataPointButton->GetId();
}

void FanCurveContainer::onNewPointPress(wxCommandEvent& event)
{

	m_graph->newDataPoint();
}

void FanCurveContainer::onDeletePointPress(wxCommandEvent& event)
{
	m_graph->deletePoint();
	if (m_graph->m_dataPoints.size() < 8)
	{
		m_newDataPointButton->Enable(true);
	}
}

wxSize FanCurveContainer::DoGetBestSize(wxFrame* parent)
{
	wxSize bestSize;
	int width = (parent->GetSize().GetWidth() * 139) / 544;
	bestSize.SetWidth(width);
	int margin = (parent->GetSize().GetWidth() / 100) / 2;
	int height = (((parent->GetSize().GetHeight() - margin) / 2.2) - margin);
	bestSize.SetHeight(height);
	return bestSize;
}

wxSize FanCurveContainer::DoGetBestSize(wxWindow* parent)
{
	wxSize bestSize;
	int width = (parent->GetSize().GetWidth() * 139) / 544;
	bestSize.SetWidth(width);
	int margin = (parent->GetSize().GetWidth() / 100) / 2;
	int height = (((parent->GetSize().GetHeight() - margin)/2.2) - margin);
	bestSize.SetHeight(height);
	return bestSize;
}

wxPoint FanCurveContainer::DoGetBestPosition()
{
	wxPoint bestPosition;
	int x = ((GetParent()->GetSize().GetWidth() * 83) / 408) - (GetParent()->GetSize().GetWidth() / 100) + ((m_fanheader % 3) * (DoGetBestSize(GetParent()).GetWidth() + (GetParent()->GetSize().GetWidth() / 100)));
	bestPosition.x = x;
	int y = ((GetParent()->GetSize().GetWidth() / 100) + ((m_fanheader / 3) * (DoGetBestSize(GetParent()).GetHeight() + (GetParent()->GetSize().GetWidth() / 100))));
	bestPosition.y = y;
	return bestPosition;
}

void FanCurveContainer::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc{ this };

	dc.SetPen(*wxBLACK_PEN);

	for (float j{ 0 }; j <=  m_graph->GetSize().GetWidth(); j += static_cast<float>(m_graph->GetSize().GetWidth()) / 5)
	{
		int posX = j + m_graph->GetPosition().x;

		int labelNum = ((j / m_graph->GetSize().GetWidth()) * (m_graph->getTempRange()) + m_graph->getMinTemp());
		std::stringstream ss;
		ss << labelNum;
		std::string label;
		ss >> label;

		dc.SetTextForeground(*wxWHITE);

		if (j == 0)
		{
			wxRect rectangle(wxPoint(posX, m_graph->getBottom()), wxPoint(posX + 50, m_graph->getBottom() + 30));
			dc.DrawLabel(label, rectangle, wxALIGN_LEFT);
		}
		else if (j > m_graph->GetSize().GetWidth() / 5 * 4)
		{
			wxRect rectangle(wxPoint(posX - 50, m_graph->getBottom()), wxPoint(posX, m_graph->getBottom() + 30));
			dc.DrawLabel(label, rectangle, wxALIGN_RIGHT);
		}
		else
		{
			wxRect rectangle(wxPoint(posX - 25, m_graph->getBottom()), wxPoint(posX + 25, m_graph->getBottom() + 30));
			dc.DrawLabel(label, rectangle, wxALIGN_CENTER_HORIZONTAL);
		}
	}
	for (float j{ 0 }; j <= m_graph->GetSize().GetHeight(); j += (static_cast<float>(m_graph->GetSize().GetHeight()) / 5))
	{
		int posY = j + m_graph->GetPosition().y;

		int labelNum = ((j / m_graph->GetSize().GetHeight()) * -(m_graph->getFanSpeedRange()) + m_graph->getMaxFanSpeed());

		std::stringstream ss;
		ss << labelNum;
		std::string label;
		ss >> label;

		if (j == 0)
		{
			wxRect rectangle(wxPoint(m_graph->getRight() + 2, posY), wxPoint(m_graph->getRight() + 52, posY + 30));
			dc.DrawLabel(label, rectangle, wxALIGN_LEFT | wxALIGN_TOP);
		}
		else if (j > (m_graph->GetSize().GetHeight() / 5 * 4) + 2)
		{
			wxRect rectangle(wxPoint(m_graph->getRight() + 2, posY - 30), wxPoint(m_graph->getRight() + 52, posY));
			dc.DrawLabel(label, rectangle, wxALIGN_LEFT | wxALIGN_BOTTOM);
		}
		else
		{
			wxRect rectangle(wxPoint(m_graph->getRight() + 2, posY - 15), wxPoint(m_graph->getRight() + 52, posY + 15));
			dc.DrawLabel(label, rectangle, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
	}
}

int FanCurveContainer::getTempChoice()
{
	wxString tempChoice = m_tempSelect->GetValue();

	if (!tempChoice.Cmp("Temp header 2"))
	{
		return 2;
	}
	else if (!tempChoice.Cmp("CPU Temp"))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void FanCurveContainer::setDeleteButton(bool onOrOff)
{
	m_deleteDataPointButton->Enable(onOrOff);
}

void FanCurveContainer::setNewPointButton(bool onOrOff)
{
	m_newDataPointButton->Enable(onOrOff);
}

int FanCurveContainer::getBottom()
{
	return (this->GetPosition().y + this->GetSize().GetHeight());
}
int FanCurveContainer::getRight()
{
	return (this->GetPosition().x + this->GetSize().GetWidth());
}

void FanCurveContainer::setFanSpeed(int fanSpeed)
{
	m_fanSpeed = fanSpeed;

	std::stringstream ss;
	if (fanSpeed == 9999)
	{
		ss << "3 Pin fan; Can't measure RPM";
	}
	else
	{
		ss << "Fan Speed: " << fanSpeed << " RPM";
	}

	m_fanSpeedReadout->SetLabelText(ss.str());
}
int FanCurveContainer::getFanSpeed()
{
	return m_fanSpeed;
}

int FanCurveContainer::getFanHeader()
{
	return m_fanheader;
}

void FanCurveContainer::refreshRpm()
{
	m_fanSpeedReadout->Refresh();
}



wxBEGIN_EVENT_TABLE(FanCurveContainer, wxControl)
	EVT_PAINT(FanCurveContainer::onPaint)
wxEND_EVENT_TABLE()