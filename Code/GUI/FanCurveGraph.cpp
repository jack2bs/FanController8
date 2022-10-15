#include "FanCurveGraph.h"

int FanCurveGraph::m_id{window::id::FANCURVE_GRAPH_MIN_ID};

int count;

FanCurveGraph::FanCurveGraph(FanCurveContainer *parent,
	int minTemp,
	int maxTemp,
	int incTemp,
	int minFanSpeed,
	int maxFanSpeed,
	int incFanSpeed,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxValidator& validator,
	const wxString& name)
	:
	wxControl{ parent, m_id, pos, size, style, validator, name },
	m_maxTemp{ maxTemp },
	m_minTemp{ minTemp },
	m_incTemp{ incTemp },
	m_minFanSpeed{ minFanSpeed },
	m_maxFanSpeed{ maxFanSpeed },
	m_incFanSpeed{ incFanSpeed },
	m_FccParent{ parent }
{
	m_id++;
	SetBackgroundColour(*wxWHITE);
	int maxDataPoints = (maxTemp - minTemp) / incTemp;
	m_dataPoints.reserve(maxDataPoints);
	m_minTempPoint = new FanCurveDataPoint{ this, wxPoint(-FanCurveDataPoint::getSize(this).GetWidth() / 2, GetSize().GetHeight() / 2), 0, 0, (GetSize().GetWidth()) / (((maxTemp + minTemp) / 2) / incTemp), 0, GetSize().GetHeight(), (GetSize().GetHeight()) / (((maxFanSpeed + minFanSpeed) / 2) / incFanSpeed) };
	m_maxTempPoint = new FanCurveDataPoint{ this, wxPoint(GetSize().GetWidth() - FanCurveDataPoint::getSize(this).GetWidth() / 2, GetSize().GetHeight() / 2), GetSize().GetWidth(), GetSize().GetWidth(), (GetSize().GetWidth()) / (((maxTemp + minTemp) / 2) / incTemp), 0, GetSize().GetHeight(), (GetSize().GetHeight()) / (((maxFanSpeed + minFanSpeed) / 2) / incFanSpeed) };
	
	m_minTempPoint->moveDot();
	m_maxTempPoint->moveDot();
	
	m_dataPoints.push_back(new FanCurveDataPoint { this,  wxPoint(GetSize().GetWidth()/2, GetSize().GetHeight()/2), 0, GetSize().GetWidth(), (GetSize().GetWidth()) / (((maxTemp + minTemp) / 2) / incTemp), 0, GetSize().GetHeight(), (GetSize().GetHeight()) / (((maxFanSpeed + minFanSpeed) / 2) / incFanSpeed) });
	wxWindow::SetBackgroundStyle(static_cast<wxBackgroundStyle>(wxBG_STYLE_PAINT | this->GetBackgroundStyle()));
}

void FanCurveGraph::resize()
{
	SetPosition(wxPoint(m_parent->GetParent()->GetSize().GetWidth() / 100, m_parent->GetParent()->GetSize().GetWidth() / 100));
	int *temps{ new int[m_dataPoints.size() + 2] };
	int *fs{ new int[m_dataPoints.size() + 2] };

	temps[0] = m_minTempPoint->getmTemp();
	fs[0] = m_minTempPoint->getmFs();

	temps[1] = m_maxTempPoint->getmTemp();
	fs[1] = m_maxTempPoint->getmFs();

	for (int i = 2; i < m_dataPoints.size()+2; i++)
	{
		temps[i] = (m_dataPoints.at(i-2)->getmTemp());
		fs[i] = (m_dataPoints.at(i-2)->getmFs());
	}
	SetSize(DoGetBestSize(m_parent));


	setDpPos(temps[0], fs[0], m_minTempPoint);
	m_minTempPoint->resize();
	setDpPos(temps[1], fs[1], m_maxTempPoint);
	m_maxTempPoint->resize();
	for (int i = 2; i < m_dataPoints.size() + 2; i++)
	{
		setDpPos(temps[i], fs[i], m_dataPoints.at(i-2));
		m_dataPoints.at(i-2)->resize();
	}

	if (m_dataPoints.size() > 0)
	{
		m_minTempPoint->setVertMin(ScreenToClient((m_dataPoints.at(0)->getCenter())).y);
		m_minTempPoint->setVertMax(GetSize().GetHeight());
		m_minTempPoint->setHoriMin(0);
		m_minTempPoint->setHoriMax(0);

		m_maxTempPoint->setVertMax(ScreenToClient(m_dataPoints.at(m_dataPoints.size() - 1)->getCenter()).y);
		m_minTempPoint->setVertMin(0);
		m_maxTempPoint->setHoriMin(GetSize().GetWidth());
		m_maxTempPoint->setHoriMax(GetSize().GetWidth());

		for (int i = 0; i < m_dataPoints.size(); i++)
		{
			if (i == 0)
			{
				m_dataPoints.at(i)->setVertMax(ScreenToClient(m_minTempPoint->getCenter()).y);
				m_dataPoints.at(i)->setHoriMin(ScreenToClient(m_minTempPoint->getCenter()).x);
			}
			else
			{
				m_dataPoints.at(i)->setVertMin(ScreenToClient(m_dataPoints.at(i - 1)->getCenter()).y);
				m_dataPoints.at(i)->setHoriMax(ScreenToClient(m_dataPoints.at(i - 1)->getCenter()).x);
			}
			if (i == m_dataPoints.size() - 1)
			{
				m_dataPoints.at(i)->setVertMin(ScreenToClient(m_maxTempPoint->getCenter()).y);
				m_dataPoints.at(i)->setHoriMax(ScreenToClient(m_maxTempPoint->getCenter()).x);
			}
			else
			{
				m_dataPoints.at(i)->setVertMin(ScreenToClient(m_dataPoints.at(i + 1)->getCenter()).y);
				m_dataPoints.at(i)->setHoriMax(ScreenToClient(m_dataPoints.at(i + 1)->getCenter()).x);
			}
		}
	}

}

void FanCurveGraph::onPaint(wxPaintEvent& event)
{
	ClearBackground();

	wxBufferedPaintDC dc(this);

	dc.Clear();

	dc.SetPen(*wxBLACK_PEN);
	int bottom = this->GetSize().GetHeight();
	for (float j{ 0 }; j < this->GetSize().GetWidth(); j += static_cast<float>(this->GetSize().GetWidth()) / 5)
	{
		dc.DrawLine(wxPoint(j, 0), wxPoint(j, bottom));
	}
	int right = this->GetSize().GetWidth();
	for (float k{ 0 }; k < this->GetSize().GetHeight(); k += static_cast<float>(this->GetSize().GetHeight()) / 5)
	{
		dc.DrawLine(wxPoint(0, k), wxPoint(right, k));
	}

	dc.SetPen(*wxRED_PEN);

	if (m_dataPoints.size() > 0)
	{
		dc.DrawLine(ScreenToClient(m_minTempPoint->getCenter()), ScreenToClient(m_dataPoints[0]->getCenter()));

		int i{ 0 };
		for (; i < m_dataPoints.size() - 1; i++)
		{
			dc.DrawLine(ScreenToClient(m_dataPoints[i]->getCenter()), ScreenToClient(m_dataPoints[i + 1]->getCenter()));
		}

		dc.DrawLine(ScreenToClient(m_dataPoints[i]->getCenter()), ScreenToClient(m_maxTempPoint->getCenter()));
	}
	else
	{
		dc.DrawLine(ScreenToClient(m_minTempPoint->getCenter()), ScreenToClient(m_maxTempPoint->getCenter()));
	}


}

void FanCurveGraph::findToolTip(FanCurveDataPoint* dp)
{
	wxPoint center = ScreenToClient(dp->getCenter());
	int temp = (static_cast<float>(center.x) / this->GetSize().GetWidth()) * getTempRange() + getMinTemp();
	int fanSpeed = (static_cast<float>(center.y) / this->GetSize().GetHeight()) * -getFanSpeedRange() + getMaxFanSpeed();

	std::stringstream ss;
	ss << temp << ", " << fanSpeed;
	std::string toolTipString = ss.str();
	dp->SetToolTip(toolTipString);
}

void FanCurveGraph::setDpPos(int temp, int fs, FanCurveDataPoint* dp)
{
	float tempf = static_cast<float>(temp) + 0.2;
	float fsf = static_cast<float>(fs);
	wxPoint center(((tempf - getMinTemp()) / getTempRange()) * this->GetSize().GetWidth(), ((getMaxFanSpeed() - fsf) / getFanSpeedRange()) * this->GetSize().GetHeight());
	//wxPoint realCenter(ClientToScreen(center));
	wxPoint pos(center.x - FanCurveDataPoint::getSize(this).GetWidth() / 2, center.y - FanCurveDataPoint::getSize(this).GetHeight() / 2);

	dp->SetPosition(pos);
}

void FanCurveGraph::setDpPos(float temp, float fs, FanCurveDataPoint* dp)
{
	wxPoint center(((temp - getMinTemp()) / getTempRange()) * this->GetSize().GetWidth(), ((getMaxFanSpeed() - fs) / getFanSpeedRange()) * this->GetSize().GetHeight());
	//wxPoint realCenter(ClientToScreen(center));
	wxPoint pos(center.x - FanCurveDataPoint::getSize(this).GetWidth() / 2, center.y - FanCurveDataPoint::getSize(this).GetHeight() / 2);

	dp->SetPosition(pos);
}

void FanCurveGraph::onDataPointMoved(DataPointEvent& event)
{
	count++;

	FanCurveDataPoint* dp = event.getDP();

	if (m_selectedPoint != nullptr)
	{
		m_selectedPoint->SetBackgroundColour(m_selectedPoint->GetParent()->GetBackgroundColour());
	}

	m_selectedPoint = dp;
	m_selectedPoint->SetBackgroundColour(*wxBLUE);
	m_selectedPoint->Refresh();

	dp->writeToolTip(this);

	if (dp == m_tempDataPoint)
	{
		if (m_tempDataPoint->GetPosition() == wxPoint(this->GetSize().GetWidth() / 2, this->GetSize().GetHeight() / 2))
		{
			m_tempDataPoint->SetBackgroundColour(*wxGREEN);
			m_FccParent->setDeleteButton(true);
			return;
		}

		int i = 0;

		for (; i < m_dataPoints.size(); i++)
		{
			if (dp->getCenter().x < m_dataPoints.at(i)->getCenter().x)
			{
				auto it = m_dataPoints.begin() + i;
				m_dataPoints.emplace(it, dp);

				m_tempDataPoint = nullptr;
				break;
			}
		}

		if (m_dataPoints.size() >= 8)
		{
			m_FccParent->setNewPointButton(false);
		}

		if (m_tempDataPoint != nullptr)
		{
			m_dataPoints.emplace_back(dp);
			m_tempDataPoint = nullptr;
		}

		if (i == 0)
		{
			m_dataPoints.at(i)->setVertMax(ScreenToClient(m_minTempPoint->getCenter()).y);
		}
		else
		{
			m_dataPoints.at(i)->setVertMax(ScreenToClient(m_dataPoints.at(i-1)->getCenter()).y);
			m_dataPoints.at(i)->setHoriMin(ScreenToClient(m_dataPoints.at(i-1)->getCenter()).x);
		}
		if (i == m_dataPoints.size() - 1)
		{
			m_dataPoints.at(i)->setVertMin(ScreenToClient(m_maxTempPoint->getCenter()).y);
		}
		else
		{
			m_dataPoints.at(i)->setVertMin(ScreenToClient(m_dataPoints.at(i + 1)->getCenter()).y);
			m_dataPoints.at(i)->setHoriMax(ScreenToClient(m_dataPoints.at(i + 1)->getCenter()).x);
		}

		if (ScreenToClient(m_dataPoints.at(i)->getCenter()).y < m_dataPoints.at(i)->getVertMin())
		{
			m_dataPoints.at(i)->SetPosition(wxPoint(ScreenToClient(m_dataPoints.at(i)->getCenter()).x - m_dataPoints.at(i)->getSize(this).GetWidth() / 2, m_dataPoints.at(i)->getVertMin() - m_dataPoints.at(i)->getSize(this).GetHeight() / 2));
		}
		else if (ScreenToClient(m_dataPoints.at(i)->getCenter()).y > m_dataPoints.at(i)->getVertMax())
		{
			m_dataPoints.at(i)->SetPosition(wxPoint(ScreenToClient(m_dataPoints.at(i)->getCenter()).x - m_dataPoints.at(i)->getSize(this).GetWidth()/2, m_dataPoints.at(i)->getVertMax() - m_dataPoints.at(i)->getSize(this).GetHeight() / 2));
		}

		wxWindow::Refresh();
	}


	if (dp == m_minTempPoint)
	{
		m_FccParent->setDeleteButton(false);
		if (m_dataPoints.size() > 0)
		{
			m_dataPoints.at(0)->setVertMax(ScreenToClient(dp->getCenter()).y);
			m_dataPoints.at(0)->setHoriMin(ScreenToClient(dp->getCenter()).x);
		}
		else
		{
			m_maxTempPoint->setVertMax(ScreenToClient(dp->getCenter()).y);
		}
	}
	else if (dp == m_maxTempPoint)
	{
		m_FccParent->setDeleteButton(false);
		if (m_dataPoints.size() > 0)
		{
			m_dataPoints.at(m_dataPoints.size() - 1)->setVertMin(ScreenToClient(dp->getCenter()).y);
			m_dataPoints.at(m_dataPoints.size() - 1)->setHoriMax(ScreenToClient(dp->getCenter()).x);
		}		
		else
		{
			m_minTempPoint->setVertMin(ScreenToClient(dp->getCenter()).y);
		}
	}
	else
	{
		m_FccParent->setDeleteButton(true);
		for (int i = 0; i < m_dataPoints.size(); i++)
		{

			if (dp == m_dataPoints.at(i))
			{
				if (i == 0)
				{
					m_minTempPoint->setVertMin(ScreenToClient(dp->getCenter()).y);
				}
				else
				{
					m_dataPoints.at(i - 1)->setVertMin(ScreenToClient(dp->getCenter()).y);
					m_dataPoints.at(i - 1)->setHoriMax(ScreenToClient(dp->getCenter()).x);
				}
				if (i == m_dataPoints.size() - 1)
				{
					m_maxTempPoint->setVertMax(ScreenToClient(dp->getCenter()).y);
				}
				else
				{
					m_dataPoints.at(i + 1)->setVertMax(ScreenToClient(dp->getCenter()).y);
					m_dataPoints.at(i + 1)->setHoriMin(ScreenToClient(dp->getCenter()).x);
				}
			}
		}
	}
}

void FanCurveGraph::newDataPoint()
{
	if (m_tempDataPoint == nullptr)
	{
		m_tempDataPoint = new FanCurveDataPoint(this, wxPoint(this->GetSize().GetWidth() / 2, this->GetSize().GetHeight() / 2), 0, this->GetSize().GetWidth(), this->m_incTemp, 0, this->GetSize().GetHeight(), this->m_incFanSpeed);
		m_tempDataPoint->SetBackgroundColour(*wxGREEN);
		Refresh();
	}
}

wxSize FanCurveGraph::DoGetBestSize(wxWindow* parent)
{
	wxSize bestSize;
	int width = parent->GetSize().GetWidth() - (2 * parent->GetParent()->GetSize().GetWidth() / 100);
	bestSize.SetWidth(width);
	bestSize.SetHeight(width * 0.7);
	return bestSize;
}

void FanCurveGraph::deletePoint()
{
	FanCurveDataPoint* tempPoint = m_selectedPoint;

	if (m_selectedPoint == m_maxTempPoint)
		return;
	if (m_selectedPoint == m_minTempPoint)
		return;
	if (m_selectedPoint == m_tempDataPoint)
	{
		m_tempDataPoint = nullptr;
		delete tempPoint;
		Refresh();
	}
	for (int i = 0; i < m_dataPoints.size(); i++)
	{
		if (m_dataPoints.at(i) == m_selectedPoint)
		{
			m_dataPoints.at(i) = nullptr;
			delete (tempPoint);
			m_dataPoints.erase(m_dataPoints.begin() + i);
			Refresh();

			DataPointEvent event1(DATA_POINT_MOVED_EVENT, 2);
			DataPointEvent event2(DATA_POINT_MOVED_EVENT, 3);
			if (i == 0)
			{
				event1.setDP(m_minTempPoint);
			}
			else
			{
				event1.setDP(m_dataPoints.at(0));
			}

			if (i == m_dataPoints.size())
			{
				event2.setDP(m_maxTempPoint);
			}
			else
			{
				event2.setDP(m_dataPoints.at(i));
			}

			wxPostEvent(this, event1);
			wxPostEvent(this, event2);
			break;

		}
	}

	m_selectedPoint = nullptr;
}

int FanCurveGraph::getBottom()
{
	return (this->GetPosition().y + this->GetSize().GetHeight());
}
int FanCurveGraph::getRight()
{
	return (this->GetPosition().x + this->GetSize().GetWidth());
}

int FanCurveGraph::getTempRange()
{
	return (m_maxTemp - m_minTemp);
}
int FanCurveGraph::getFanSpeedRange()
{
	return (m_maxFanSpeed - m_minFanSpeed);
}

int FanCurveGraph::getMaxTemp()
{
	return m_maxTemp;
}
int FanCurveGraph::getMinTemp()
{
	return m_minTemp;
}
int FanCurveGraph::getIncTemp()
{
	return m_incTemp;
}
int FanCurveGraph::getMinFanSpeed()
{
	return m_minFanSpeed;
}
int FanCurveGraph::getMaxFanSpeed()
{
	return m_maxFanSpeed;
}
int FanCurveGraph::getIncFanSpeed()
{
	return m_incFanSpeed;
}

FanCurveContainer* FanCurveGraph::getFccParent()
{
	return m_FccParent;
}

void FanCurveGraph::setMaxTemp(int mt)
{
	m_maxTemp = mt;
}
void FanCurveGraph::setMinTemp(int mt)
{
	m_minTemp = mt;
}
void FanCurveGraph::setIncTemp(int it)
{
	m_incTemp = it;
}
void FanCurveGraph::setMinFanSpeed(int mfs)
{
	m_minFanSpeed = mfs;
}
void FanCurveGraph::setMaxFanSpeed(int mfs)
{
	m_maxFanSpeed = mfs;
}
void FanCurveGraph::setIncFanSpeed(int ifs)
{
	m_incFanSpeed = ifs;
}

wxBEGIN_EVENT_TABLE(FanCurveGraph, wxControl)
	EVT_PAINT(FanCurveGraph::onPaint)
	EVT_DATA_POINT_MOVED(wxID_ANY, FanCurveGraph::onDataPointMoved)
wxEND_EVENT_TABLE()