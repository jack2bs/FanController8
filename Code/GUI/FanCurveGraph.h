#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "FanCurveDataPoint.h"
#include "Id.h"
#include <vector>
#include "DataPointMovedEvent.h"
#include "FanCurveContainer.h"
#include "wx/dcbuffer.h"

class FanCurveContainer;

class FanCurveGraph : public wxControl
{
private:
	static int m_id;
	FanCurveContainer *m_FccParent;
	int m_maxTemp;
	int m_minTemp;
	int m_incTemp;
	int m_maxFanSpeed;
	int m_minFanSpeed;
	int m_incFanSpeed;



public:

	FanCurveDataPoint *m_tempDataPoint;
	FanCurveDataPoint *m_selectedPoint;
	std::vector<FanCurveDataPoint *> m_dataPoints;
	FanCurveDataPoint *m_minTempPoint;
	FanCurveDataPoint *m_maxTempPoint;

	FanCurveGraph(FanCurveContainer *parent,
		int minTemp,
		int maxTemp,
		int incTemp,
		int minFanSpeed,
		int maxFanSpeed,
		int incFanSpeed,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0 | wxBORDER_NONE,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxASCII_STR(wxControlNameStr));
	
	void init();

	void onPaint(wxPaintEvent& event);
	void onDataPointMoved(DataPointEvent& event);
	void newDataPoint(); //When a new temperature point is created
	static wxSize DoGetBestSize(wxWindow* parent);
	int getTempRange();
	int getFanSpeedRange();
	int getBottom();
	int getRight();
	void findToolTip(FanCurveDataPoint* dp);
	void deletePoint();
	void resize();

	int getMaxTemp();
	int getMinTemp();
	int getIncTemp();
	int getMinFanSpeed();
	int getMaxFanSpeed();
	int getIncFanSpeed();
	FanCurveContainer* getFccParent();

	void setMaxTemp(int mt);
	void setMinTemp(int mt);
	void setIncTemp(int it);
	void setMinFanSpeed(int mfs);
	void setMaxFanSpeed(int mfs);
	void setIncFanSpeed(int ifs);
	void setDpPos(int temp, int fs, FanCurveDataPoint* dp);
	void setDpPos(float temp, float fs, FanCurveDataPoint* dp);

	wxDECLARE_EVENT_TABLE();
};

