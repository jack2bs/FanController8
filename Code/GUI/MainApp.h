#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "serial/serial.h"
#include "ParentFrame.h"
#include "Id.h"
#include "Miscellaneous.h"
#include <iostream>
#include "SerialCommands.h"
#include "Wrapper.h"
#include <fstream>

void createFile();
bool doesFileExist();

class ParentFrame;

class MainApp :	public wxApp
{
private:
	serial::Serial* m_serialPort;
	ParentFrame* m_frame;
	wxTimer m_timer;
	wxSplashScreen* m_splashScreen;
	CpuTempWrapper* m_CPUTempGetter;

public:

	MainApp();
	virtual bool OnInit();
	void onTimer(wxTimerEvent& event);
	bool getHardwareInfo();
	int getNumberOfFans();
	void sendProfile();
	void saveProfile();
	void closerSerialPort();
	float getCpuTemp();
	void editFanProfile(int prof);
	void loadFanProfile(int prof);

	wxDECLARE_EVENT_TABLE();

};




DECLARE_APP(MainApp);	//wxGetApp() function available, which returns an instance of the mainApp class which was created by the implement app macro
