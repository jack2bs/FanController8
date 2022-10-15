#include "FanCurveDataPoint.h"
#include <thread>

wxDEFINE_EVENT(DATA_POINT_MOVED_EVENT, DataPointEvent);

int FanCurveDataPoint::m_id{ window::id::DATAPOINT_MIN_ID };

FanCurveDataPoint::FanCurveDataPoint(
	FanCurveGraph *parent,
	const wxPoint& pos,
	int horiMin,
	int horiMax,
	int horiInc,
	int vertMin,
	int vertMax,
	int vertInc,
	long style,
	const wxValidator& validator,
	const wxString& name
)
	: wxControl{ parent, m_id, pos, FANCURVE_DP_SIZE, style, validator, name },
	m_position{ FANCURVE_DP_SIZE.GetWidth() / 2, FANCURVE_DP_SIZE.GetHeight() / 2 },
	m_horiMinValue{ horiMin },
	m_horiMaxValue{ horiMax },
	m_horiIncrement{ horiInc },
	m_vertMinValue{ vertMin },
	m_vertMaxValue{ vertMax },
	m_vertIncrement{ vertInc },
	m_radius{ FANCURVE_DP_SIZE.GetWidth() / 2 },
	m_graphParent{ parent }
{
	m_temp = getTempAsInt(parent);
	m_fs = getFanSpeedAsInt(parent);
	DataPointEvent event(DATA_POINT_MOVED_EVENT, 1);
	event.setDP(this);
	wxPostEvent(this->GetParent(), event);
	//SetBackgroundColour(*wxGREEN);
	m_id++;
	wxWindow::SetBackgroundStyle(static_cast<wxBackgroundStyle>(wxBG_STYLE_PAINT | this->GetBackgroundStyle()));
}

void FanCurveDataPoint::onPaint(wxPaintEvent& event)
{

	//You must create a wxPaintDC object whenever you do a wxPaintEvent function
	wxBufferedPaintDC dc(this);

	dc.Clear();

	//Make the brush yellow for the circles
	dc.SetBrush(wxBrush(*wxBLUE, wxBRUSHSTYLE_SOLID));

	//Draw the point on the graph
	dc.DrawCircle(m_position, m_radius);
}

void FanCurveDataPoint::resize()
{
	wxWindow* parent = m_parent;
	SetSize(FANCURVE_DP_SIZE);
}

int FanCurveDataPoint::moveDot()
{
	//This is in its own thread so it can go forever and not interrupt the prorgam
	//While the left mouse button is clicked
	while (wxGetMouseState().ButtonIsDown(wxMouseButton::wxMOUSE_BTN_LEFT))
	{	
		//Holds the eventual position of the window itself
		wxPoint widTempPos{};

		//Holds the eventual position of the circle
		wxPoint cirTempPos{};

		//The coordinates of the mouse which is used for positioning the Datapoint
		wxPoint mousCoords{ GetParent()->ScreenToClient(wxGetMouseState().GetPosition()) };

		//If it is further right than its maximum value (a datapoint it is left of)
		if (mousCoords.x > m_horiMaxValue)
		{
			//Set the center to the maximum horizontal value, and the widget to align with that
			widTempPos.x = m_horiMaxValue - GetSize().GetWidth() / 2;
			cirTempPos.x = m_horiMaxValue;
		}
		//If it is further left than its minimum value (a datapoint it is right of)
		else if (mousCoords.x < m_horiMinValue)
		{
			//Set the center to the minimum horizontal value, and the widget to align with that
			widTempPos.x = m_horiMinValue - GetSize().GetWidth() / 2;
			cirTempPos.x = m_horiMinValue;
		}
		//It is somewhere in between
		else
		{
			//Set it to the mouse coords
			widTempPos.x = mousCoords.x - GetSize().GetWidth() / 2;
			cirTempPos.x = mousCoords.x;
		}
		//If it is further down than its maximum value (a datapoint it is above)
		if (mousCoords.y > m_vertMaxValue)
		{
			//Set it to its max vertical distance
			widTempPos.y = m_vertMaxValue - GetSize().GetHeight() / 2;
			cirTempPos.y = m_vertMaxValue;
		}
		//If it is further up than its minimum value (a datapoint above it)
		else if (mousCoords.y < m_vertMinValue)
		{
			//Set it to its min vertical distance
			widTempPos.y = m_vertMinValue - GetSize().GetHeight() / 2;
			cirTempPos.y = m_vertMinValue;
		}
		//It is somewhere in between
		else
		{
			//Use the mouse coords
			widTempPos.y = mousCoords.y - GetSize().GetHeight() / 2;
			cirTempPos.y = mousCoords.y;
		}
	
		//Set the widget and center to their correct positions
		SetPosition(widTempPos);
		m_position = ScreenToClient(GetParent()->ClientToScreen(cirTempPos));

		//Redraw it
		Refresh();
	}

	m_temp = getTempAsInt(m_graphParent);
	m_fs = getFanSpeedAsInt(m_graphParent);

	//Redraw the graph (lines and stuff) and the point one more time
	GetParent()->Refresh();
	Refresh();

	//Create a DP moved event and point it to the datapoint we moved
	DataPointEvent event(DATA_POINT_MOVED_EVENT, 1);
	event.setDP(this);

	// Post the event. pDest is a pointer to the destination
	wxPostEvent(this->GetParent(), event);

	//Should change to void
	return 1;
}

int FanCurveDataPoint::getmTemp()
{
	return m_temp;
}

int FanCurveDataPoint::getmFs()
{
	return m_fs;
}

//Handles click events
void FanCurveDataPoint::onClick(wxMouseEvent& event)
{
	//Creates a new thread and assigns it to moveDot(), then detaches the thread
	std::thread t1([this] {this->moveDot(); });
	t1.detach();
}

//Sets the tool tip (hover over thingy)
wxToolTip* FanCurveDataPoint::writeToolTip(FanCurveGraph* parent)
{
	//Get the center to use to calculate the tooltip values
	wxPoint center = parent->ScreenToClient(this->getCenter());

	//Calculates the temp (x axis) and fanspeed (y axis) values of the datapoint
	int temp = (static_cast<float>(center.x) / parent->GetSize().GetWidth()) * (parent->getTempRange()) + parent->getMinTemp();
	int fanSpeed = (static_cast<float>(center.y) / parent->GetSize().GetHeight()) * (-parent->getFanSpeedRange()) + parent->getMaxFanSpeed();

	//Puts the temp and fanspeed into a string
	std::stringstream ss;
	ss << temp << ", " << fanSpeed;
	std::string toolTipString = ss.str();

	//Sets the string of the tooltip and returns the tooltip to the caller
	this->SetToolTip(toolTipString);
	return this->GetToolTip();
}

//getters and setters

std::string FanCurveDataPoint::getTempAsString(FanCurveGraph* parent)
{
	//Get the center to use to calculate the tooltip values
	wxPoint center = parent->ScreenToClient(this->getCenter());

	//Calculates the temp (x axis) values of the datapoint
	int temp = ((static_cast<float>(center.x) / parent->GetSize().GetWidth()) * (parent->getTempRange()) + parent->getMinTemp());

	std::stringstream ss;

	if (temp < 10)
	{
		ss << "00" << temp;
	}
	else if (temp < 100)
	{
		ss << "0" << temp;
	}
	else
	{
		ss << temp;
	}

	return ss.str();
}

float FanCurveDataPoint::getTempAsInt(FanCurveGraph* parent)
{
	//Get the center to use to calculate the tooltip values
	wxPoint center = parent->ScreenToClient(this->getCenter());

	//Calculates the temp (x axis) values of the datapoint
	return ((static_cast<float>(center.x) / parent->GetSize().GetWidth()) * (parent->getTempRange()) + parent->getMinTemp());
}



std::string FanCurveDataPoint::getFanSpeedAsString(FanCurveGraph* parent)
{
	//Get the center to use to calculate the tooltip values
	wxPoint center = parent->ScreenToClient(this->getCenter());

	//Calculates the fanspeed (y axis) values of the datapoint
	int fs = ((static_cast<float>(center.y) / parent->GetSize().GetHeight()) * (-parent->getFanSpeedRange()) + parent->getMaxFanSpeed());

	std::stringstream ss;

	if (fs < 10)
	{
		ss << "00" << fs;
	}
	else if (fs < 100)
	{
		ss << "0" << fs;
	}
	else
	{
		ss << fs;
	}

	return ss.str();
}


float FanCurveDataPoint::getFanSpeedAsInt(FanCurveGraph *parent)
{
	//Get the center to use to calculate the tooltip values
	wxPoint center = parent->ScreenToClient(this->getCenter());

	//Calculates the fanspeed (y axis) values of the datapoint
	return ((static_cast<float>(center.y) / parent->GetSize().GetHeight()) * (-parent->getFanSpeedRange()) + parent->getMaxFanSpeed());
}

wxSize FanCurveDataPoint::getSize(wxWindow* parent)
{
	return FANCURVE_DP_SIZE;
}
wxPoint FanCurveDataPoint::getCenter()
{
	return ClientToScreen(m_position);
}
void FanCurveDataPoint::setCenter(wxPoint center)
{
	m_position = center;
}
void FanCurveDataPoint::setVertMax(int vm)
{
	m_vertMaxValue = vm;
}
void FanCurveDataPoint::setVertMin(int vm)
{
	m_vertMinValue = vm;
}
void FanCurveDataPoint::setVertInc(int vi)
{
	m_vertIncrement = vi;
}
void FanCurveDataPoint::setHoriMax(int hm)
{
	m_horiMaxValue = hm;
}
void FanCurveDataPoint::setHoriMin(int hm)
{
	m_horiMinValue = hm;
}
void FanCurveDataPoint::setHoriInc(int hi)
{
	m_horiIncrement = hi;
}
int FanCurveDataPoint::getVertMax()
{
	return m_vertMaxValue;
}
int FanCurveDataPoint::getVertMin()
{
	return m_vertMinValue;
}
int FanCurveDataPoint::getVertInc()
{
	return m_vertIncrement;
}
int FanCurveDataPoint::getHoriMax()
{
	return m_horiMaxValue;
}
int FanCurveDataPoint::getHoriMin()
{
	return m_horiMinValue;
}
int FanCurveDataPoint::getHoriInc()
{
	return m_horiIncrement;
}
int FanCurveDataPoint::getRadius()
{
	return m_radius;
}

//Events
wxBEGIN_EVENT_TABLE(FanCurveDataPoint, wxControl)
	EVT_LEFT_DOWN(FanCurveDataPoint::onClick)
	EVT_PAINT(FanCurveDataPoint::onPaint)
wxEND_EVENT_TABLE()