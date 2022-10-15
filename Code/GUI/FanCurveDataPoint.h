#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "FanCurveGraph.h"
#include <wx/utils.h>
//#include <thread>
#include "Id.h"
#include "DataPointMovedEvent.h"
#include <sstream>
#include "wx/dcbuffer.h"

//Defines the style (really just no border but I can always change)
#define FANCURVEDP_STYLE \
            (wxBORDER_NONE)

//Defines the size of the datapoint
#define FANCURVE_DP_SIZE wxSize(static_cast<int>((parent->GetSize().GetHeight()) * 0.02f) * 2 + 1, static_cast<int>((parent->GetSize().GetHeight()) * 0.02f) * 2 + 1)

//Forward declaration of class
class FanCurveGraph;

class FanCurveDataPoint : public wxControl
{
private:

	//used to keep track of the id to be allocated to the next point
	static int m_id;

	//radius of point
	int m_radius;

	//Position of center of the circle
	wxPoint m_position;

	//Horizontal leftmost value
	int m_horiMinValue;

	//Horizontal rightmost value
	int m_horiMaxValue;

	//Horizontal increment
	int m_horiIncrement;

	//Vertical highest value
	int m_vertMinValue;

	//Vertical lowest value
	int m_vertMaxValue;

	//Vertical increment
	int m_vertIncrement;

	//Tooltip (hover over thingy)
	wxToolTip* m_toolTip;

	int m_temp;
	int m_fs;
	FanCurveGraph* m_graphParent;


public:

	//Very Very Long Constructor Im very sorry
	FanCurveDataPoint(
		FanCurveGraph *parent,
		const wxPoint& pos,
		int horiMin = 0,
		int horiMax = 100,
		int horiInc = 5,
		int vertMin = 0,
		int vertMax = 100,
		int vertInc = 1,
		long style = FANCURVEDP_STYLE,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxASCII_STR(wxControlNameStr)
	);


	void resize();

	//Event handling
	void onPaint(wxPaintEvent& event);
	void onClick(wxMouseEvent& event);
	int moveDot();

	//Setters
	wxToolTip* writeToolTip(FanCurveGraph *parent);
	void setCenter(wxPoint center);
	void setVertMax(int vm);
	void setVertMin(int vm);
	void setVertInc(int vi);
	void setHoriMax(int hm);
	void setHoriMin(int hm);
	void setHoriInc(int hi);

	//Getters
	std::string getTempAsString(FanCurveGraph* parent);
	float getTempAsInt(FanCurveGraph* parent);
	std::string getFanSpeedAsString(FanCurveGraph *parent);
	float getFanSpeedAsInt(FanCurveGraph *parent);
	static wxSize getSize(wxWindow* parent);
	wxPoint getCenter();
	int getVertMax();
	int getVertMin();
	int getVertInc();
	int getHoriMax();
	int getHoriMin();
	int getHoriInc();
	int getRadius();
	int getmTemp();
	int getmFs();


	//Declare the event table
	wxDECLARE_EVENT_TABLE();
};

