#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//Ids for classes derived from wxWindow
namespace window
{
	enum id
	{
		//You start at wxID_HIGHEST which is the highest reserved ID from the wxAPI
		MAINWINDOW = wxID_HIGHEST + 1,

		//Separates frames from widgets
		FRAME_MAX_ID,

		//Test widgets
		CUSTOM_1,
		TIMER_ID,
		SPLASH_ID,
		LIST_ID,

		CONTROL_PANEL_ID,
		CONTROL_PANEL_SEND_BUTTON,
		CONTROL_PANEL_SAVE_BUTTON,
		CONTROL_PANEL_LOAD_BUTTON,
		CONTROL_PANEL_USE_BUTTON,
		CONTROL_PANEL_SELECT,


		//Reserve 200 Datapoint ids
		DATAPOINT_MIN_ID,
		DATAPOINT_MAX_ID = DATAPOINT_MIN_ID + 200,

		//Reserve 20 Graph ids
		FANCURVE_GRAPH_MIN_ID,
		FANCURVE_GRAPH_MAX_ID = FANCURVE_GRAPH_MIN_ID + 20,

		//Reserve 100 FanContainer ids (20 for the container, 40 for buttons, 20 for the comboboxes, and 20 for the fan speed readouts)
		FANCURVE_CONTAINER_MIN_ID,
		FANCURVE_CONTAINER_MAX_ID = FANCURVE_CONTAINER_MIN_ID + 100,

		//Max id for classes derived from wxWindow
		WIDGET_MAX_ID
	};
}