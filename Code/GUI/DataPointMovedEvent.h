#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//Forward declare the classes required for the class declaration
class DataPointEvent;
class FanCurveDataPoint;

//Declares the DP Moved events
wxDECLARE_EVENT(DATA_POINT_MOVED_EVENT, DataPointEvent);


class DataPointEvent : public wxCommandEvent
{
private:
	//The only member variable is a pointer to a FanCurveDataPoint
	FanCurveDataPoint* m_dp;

public:

	//Default Constructor
	DataPointEvent(wxEventType commandType = DATA_POINT_MOVED_EVENT, int id = 0)
		: wxCommandEvent(commandType, id) {	}

	//Copy constructor (REQUIRED FOR THE WX API TO WORK)
	DataPointEvent(const DataPointEvent& event)
		: wxCommandEvent(event) {
		this->setDP(event.getDP());
	}

	// Required for sending with wxPostEvent()
	wxEvent* Clone() const { return new DataPointEvent(*this); }

	//Getter and Setter for m_dp
	FanCurveDataPoint* getDP() const { return m_dp; }
	void setDP(FanCurveDataPoint* dp) { m_dp = dp; }

};

//Convenience typdef
typedef void (wxEvtHandler::*DataPointEventFunction)(DataPointEvent &);

//Creates/defines/idk how this works... the DP Moved handler
#define DataPointMovedEventHandler(func) wxEVENT_HANDLER_CAST(DataPointEventFunction, func)                    

//Required if you want to use in an event table
#define EVT_DATA_POINT_MOVED(id, func) \
	wx__DECLARE_EVT1(DATA_POINT_MOVED_EVENT, id, DataPointMovedEventHandler(func))