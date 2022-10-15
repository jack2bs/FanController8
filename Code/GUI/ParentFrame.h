#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <serial/serial.h>
#include <vector>
#include "Id.h"
#include "Miscellaneous.h"
#include "FanCurveDataPoint.h"
#include "FanCurveGraph.h"
#include "FanCurveContainer.h"
#include "MainApp.h"
#include "Color.h"
#include "wx/listctrl.h"
#include "wx/persist.h"
#include "wx/persist/toplevel.h"
#include "ControlPanel.h"

//Declares the ParentFrameClass

class ControlPanel;
class MainApp;

class ParentFrame : public wxFrame
{
private:

	MainApp* m_AppParent;
	FanCurveDataPoint* dp1;
	FanCurveContainer* fcc1;
	wxListView* m_listView;
	int m_numberOfFans;
	ControlPanel* m_controlPanel;


public:

	//These are public because it is soooo much more convenient
	std::vector<FanCurveContainer *> fccVector;

	ParentFrame(wxWindow *parent,
		wxWindowID id,
		const wxString& title,
		int numberOfFans,
		MainApp* par,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxASCII_STR(wxFrameNameStr));

	void onExit(wxCommandEvent& event);
	void listInit();
	void onResize(wxSizeEvent& event);
	void setNumberOfFans(int numberOfFans);
	int getNumberOfFans();
	MainApp* getAppParent();
	float updateCPUTemp();
	void updateListRpm(int fanNum, int RPM);
	void updateListTemp(int tempHeader, int temp);

	DECLARE_EVENT_TABLE()
};

