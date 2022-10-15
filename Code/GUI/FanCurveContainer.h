#pragma once

#include "Id.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Color.h"
#include <sstream>
#include "FanCurveGraph.h"
#include "Miscellaneous.h"
#include <thread>

class FanCurveGraph;

class FanCurveContainer	:	public wxControl
{
private:
	static int m_id;
	wxButton* m_newDataPointButton;
	wxButton* m_deleteDataPointButton;
	int m_fanSpeed;
	wxTextCtrl* m_fanSpeedReadout;
	int m_fanheader;
	wxComboBox* m_tempSelect;

public:
	FanCurveGraph* m_graph;

	FanCurveContainer(
		wxWindow *parent,
		int minTemp,
		int maxTemp,
		int incTemp,
		int minFanSpeed,
		int maxFanSpeed,
		int incFanSpeed,
		int fanHeader,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0 | wxBORDER_NONE,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxASCII_STR(wxControlNameStr));

	int getButtonId();
	void onNewPointPress(wxCommandEvent& event);
	void onDeletePointPress(wxCommandEvent& event);
	void testFunc();
	static wxSize DoGetBestSize(wxFrame* parent);
	static wxSize DoGetBestSize(wxWindow* parent);
	void onPaint(wxPaintEvent& event);
	int getBottom();
	int getRight();
	void setDeleteButton(bool onOrOff);
	int getFanHeader();
	int getTempChoice();
	wxPoint DoGetBestPosition();
	void resize();

	void setTempSelect(int ts);

	void setNewPointButton(bool onOrOff);

	void setFanSpeed(int fanSpeed);
	int getFanSpeed();

	void refreshRpm();





	wxDECLARE_EVENT_TABLE();
};

