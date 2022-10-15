#include "mainApp.h"
#include <thread>


//Makes main function and implements an instance of MainApp class
wxIMPLEMENT_APP(MainApp); 

MainApp::MainApp()
	:m_timer(this, window::id::TIMER_ID)
{
}

float MainApp::getCpuTemp()
{
	return m_CPUTempGetter->getCPUTemp() - 10.f;
}

bool enumerate_ports(serial::Serial* &a_fanControllerSerialPort)
{
	std::vector<serial::PortInfo> devices_found = serial::list_ports();

	std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

	while (iter != devices_found.end())
	{
		serial::PortInfo device = *iter++;

		std::stringstream ss;
		ss << device.hardware_id << '\n';
		std::string s = ss.str();

		if (s.find("VID_6A61&PID_636B", 0) != std::string::npos)
		{
			for (int i = 0; i < 5; i++)
			{
				try
				{
					serial::Serial* fanControllerSerialPort = new serial::Serial(device.port, 230400, serial::Timeout::simpleTimeout(400));
					a_fanControllerSerialPort = fanControllerSerialPort;
					return true;
				}
				catch (const serial::PortNotOpenedException&)
				{
					assert("Port Not Opened Exception" && false);
				}
				catch (const serial::IOException&)
				{
					assert("PORT ALREDY OPENED" && false);
				}
				catch (const serial::SerialException&)
				{
					assert("Serial Exception" && false);
				}
			}
		}
	}

	return false;

}

//When program is initialized
bool MainApp::OnInit()
{
	if (!doesFileExist())
	{
		createFile();
	}

	m_CPUTempGetter = new CpuTempWrapper();

	//Calls the base class' onInit function
	if (!wxApp::OnInit())
		return false;

	
	if (!enumerate_ports(m_serialPort))
	{
		wxMessageDialog* dialog = new wxMessageDialog(nullptr, "Error connecting to the device. Please restart the program.", "Error", wxOK | wxICON_ERROR);
		std::thread t1([dialog] {dialog->ShowModal(); });
		t1.join();
		Exit();
	}
	

	wxImage::AddHandler(new wxPNGHandler);

	wxBitmap bitmap;
	if (bitmap.LoadFile("BlueFan.png", wxBITMAP_TYPE_PNG))
	{
		m_splashScreen = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 30000, m_frame, window::SPLASH_ID);
	}


	//Creates and displays the first frame

	int temp{ -1 };
	while (temp == -1)
	{
		temp = getNumberOfFans();
	}

	m_frame = new ParentFrame(nullptr, window::id::MAINWINDOW, "2BS Fan Controller", 6, this, wxPoint(100, 100), wxSize(2100,1000), TESTSTYLE);

	delete m_splashScreen;

	m_frame->Show();

	m_timer.Start(2000);

	return true;
}

void MainApp::sendProfile()
{
	if (m_serialPort != nullptr)
	{

		int numOfFans = 6;

		//ss << numOfFans;

		for (int i = 0; i < numOfFans; i++)
		{
			//std::string write;
			std::stringstream ss;

			ss << SEND_PROFILE_REQUEST;

			ss << m_frame->fccVector.at(i)->getFanHeader();
			ss << (m_frame->fccVector.at(i)->m_graph->m_dataPoints.size());

			std::string minTemp = m_frame->fccVector.at(i)->m_graph->m_minTempPoint->getTempAsString(m_frame->fccVector.at(i)->m_graph);
			std::string minFs = m_frame->fccVector.at(i)->m_graph->m_minTempPoint->getFanSpeedAsString(m_frame->fccVector.at(i)->m_graph);

			ss << minTemp << minFs;

			for (int j = 0; j < m_frame->fccVector.at(i)->m_graph->m_dataPoints.size(); j++)
			{

				std::string temp = m_frame->fccVector.at(i)->m_graph->m_dataPoints.at(j)->getTempAsString(m_frame->fccVector.at(i)->m_graph);
				std::string fs = m_frame->fccVector.at(i)->m_graph->m_dataPoints.at(j)->getFanSpeedAsString(m_frame->fccVector.at(i)->m_graph);

				ss << temp << fs;
			}

			std::string maxTemp = m_frame->fccVector.at(i)->m_graph->m_maxTempPoint->getTempAsString(m_frame->fccVector.at(i)->m_graph);
			std::string maxFs = m_frame->fccVector.at(i)->m_graph->m_maxTempPoint->getFanSpeedAsString(m_frame->fccVector.at(i)->m_graph);

			ss << maxTemp << maxFs;

			int tempSelect = m_frame->fccVector.at(i)->getTempChoice();

			ss << tempSelect;

			m_serialPort->flush();

			std::string str = ss.str();

			m_serialPort->write(str);
		}

	}	
}



void MainApp::onTimer(wxTimerEvent& event)
{
	int cpuTemp = static_cast<int>(m_frame->updateCPUTemp());

	std::string portNum = m_serialPort->getPort();

	std::vector<serial::PortInfo> devices_found = serial::list_ports();

	std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

	bool test{ true };

	while (iter != devices_found.end())
	{
		serial::PortInfo device = *iter++;
		if (!device.port.compare(portNum))
		{
			test = false;
			break;
		}
	}

	if (test)
	{
		delete m_frame;
		wxMessageDialog* dialog = new wxMessageDialog(nullptr, "Lost Connection to the Device. Please restart the program.", "Error", wxOK | wxICON_ERROR);
		std::thread t1([dialog] {dialog->ShowModal(); });
		t1.join();
		OnExit();
		return;
	}

	if (m_serialPort != nullptr)
	{
		std::stringstream ss;
		ss << SEND_CPU_TEMP_REQUEST;
		if (cpuTemp >= 100)
		{
			ss << cpuTemp;
		}
		else if (cpuTemp >= 10)
		{
			ss << '0' << cpuTemp;
		}
		else
		{
			ss << "00" << cpuTemp;
		}
		ss << '\n';

		m_serialPort->flushOutput();

		m_serialPort->write(ss.str());

		if (m_serialPort->available())
		{
			std::string read = "";
			try
			{
				read = m_serialPort->readline();
			}
			catch (serial::IOException)
			{
				return;
			}

			// "%%%&&N#nrpmm...

			if (read.substr(0, 5).compare("%%%&&"))
			{
				//Bad data;
				return;
			}

			read = read.substr(6, std::string::npos);

			for (int i = 0; i < m_frame->fccVector.size(); i++)
			{
				this->m_frame->fccVector.at(i)->setFanSpeed(0);
			}

			int i{ 0 };
			while (read[0] == '#')
			{
				int index = read[1] - '0';
				int fanSpeed = std::stoi(read.substr(2, 4), nullptr, 10);
				m_frame->fccVector.at(index)->setFanSpeed(fanSpeed);
				m_frame->updateListRpm(index, fanSpeed);
				i++;
				read = read.substr(6, std::string::npos);
			}

			while (read[0] == 'T')
			{
				int index = read[1] - '0';
				int temp = std::stoi(read.substr(2, 3), nullptr, 10);
				m_frame->updateListTemp(index, temp);
				read = read.substr(5, std::string::npos);
			}

			for (int i = 0; i < m_frame->fccVector.size(); i++)
			{
				this->m_frame->fccVector.at(i)->refreshRpm();
			}

			m_serialPort->flushInput();

			return;
		}
	}
}

bool MainApp::getHardwareInfo()
{
	if (m_serialPort != nullptr)
	{
		m_serialPort->write(INITIALIZE_COMMAND);
		std::string read = m_serialPort->readline();

		if (read.substr(0, 4).compare("%%%&"))
		{
			read = m_serialPort->readline();
			if (read.substr(0, 4).compare("%%%&"))
			{
				return false;
			}
		}

		m_frame->setNumberOfFans(read[5] - '0');

		read = read.substr(5, std::string::npos);

		while (read[0] == '#')
		{
			read = read.substr(10, std::string::npos);
		}

		return true;
	}
	return false;
}

void MainApp::saveProfile()
{
	sendProfile();
	m_serialPort->write(SAVE_PROFILE_REQUEST);
}

int MainApp::getNumberOfFans()
{
	if (m_serialPort != nullptr)
	{
		m_serialPort->write(NUMBER_OF_FANS_REQUEST);
		m_serialPort->flush();
		std::string read = m_serialPort->readline();

		if (read.substr(0, 4).compare("%%%&"))
		{
			read = m_serialPort->readline();
			if (read.substr(0, 4).compare("%%%&"))
			{
				return -1;
			}
		}

		return read[5] - '0';
	}

	return -1;

}

void MainApp::closerSerialPort()
{
	m_serialPort->close();
}

void MainApp::loadFanProfile(int prof)
{
	std::ifstream ifs;
	ifs.open("Profiles.txt");
	for (int i = 0; i < 5; i++)
	{
		char data[340];
		ifs.getline(data, 340, '\n');

		if (i == prof)
		{
			std::string string(data);
			int index = 1;

			for (int j = 0; j < 6; j++)
			{

				int fanHeader = string[index] - '0';
				index++;

				int dataPoints = string[index] - '0';
				index++;

				for (int l = 0; l < m_frame->fccVector.at(j)->m_graph->m_dataPoints.size(); l++)
				{
					delete m_frame->fccVector.at(j)->m_graph->m_dataPoints.at(l);
					m_frame->fccVector.at(j)->m_graph->m_tempDataPoint = nullptr;
					m_frame->fccVector.at(j)->m_graph->m_selectedPoint = nullptr;
				}
				m_frame->fccVector.at(j)->m_graph->m_dataPoints.resize(dataPoints);


				int minTemp = 0;
				minTemp += ((string[index] - '0') * 100);
				index++;
				minTemp += ((string[index] - '0') * 10);
				index++;
				minTemp += ((string[index] - '0'));
				index++;

				int minfs = 0;
				minfs += ((string[index] - '0') * 100);
				index++;
				minfs += ((string[index] - '0') * 10);
				index++;
				minfs += ((string[index] - '0'));
				index++;

				m_frame->fccVector.at(j)->m_graph->setDpPos(minTemp, minfs, m_frame->fccVector.at(j)->m_graph->m_minTempPoint);

				DataPointEvent event1(DATA_POINT_MOVED_EVENT, 1);
				event1.setDP(m_frame->fccVector.at(j)->m_graph->m_minTempPoint);
				wxPostEvent(m_frame->fccVector.at(j)->m_graph->m_minTempPoint->GetParent(), event1);

				for (int k = 0; k < dataPoints; k++)
				{
					int temp = 0;
					temp += ((string[index] - '0') * 100);
					index++;
					temp += ((string[index] - '0') * 10);
					index++;
					temp += ((string[index] - '0'));
					index++;

					int fs = 0;
					fs += ((string[index] - '0') * 100);
					index++;
					fs += ((string[index] - '0') * 10);
					index++;
					fs += ((string[index] - '0'));
					index++;

					m_frame->fccVector.at(j)->m_graph->m_dataPoints.at(k) = new FanCurveDataPoint(m_frame->fccVector.at(j)->m_graph, wxPoint(0,0));

					m_frame->fccVector.at(j)->m_graph->setDpPos(temp, fs, m_frame->fccVector.at(j)->m_graph->m_dataPoints.at(k));

					DataPointEvent event2(DATA_POINT_MOVED_EVENT, 1);
					event2.setDP(m_frame->fccVector.at(j)->m_graph->m_dataPoints.at(k));
					wxPostEvent(m_frame->fccVector.at(j)->m_graph->m_dataPoints.at(k)->GetParent(), event2);

				}

				int maxTemp = 0;
				maxTemp += ((string[index] - '0') * 100);
				index++;
				maxTemp += ((string[index] - '0') * 10);
				index++;
				maxTemp += ((string[index] - '0'));
				index++;

				int maxFs = 0;
				maxFs += ((string[index] - '0') * 100);
				index++;
				maxFs += ((string[index] - '0') * 10);
				index++;
				maxFs += ((string[index] - '0'));
				index++;
				
				DataPointEvent event3(DATA_POINT_MOVED_EVENT, 1);
				event3.setDP(m_frame->fccVector.at(j)->m_graph->m_maxTempPoint);
				wxPostEvent(m_frame->fccVector.at(j)->m_graph->m_maxTempPoint->GetParent(), event3);

				int tempSelect = string[index] - '0';
				m_frame->fccVector.at(j)->setTempSelect(tempSelect);
				index++;


				m_frame->fccVector.at(j)->m_graph->setDpPos(maxTemp, maxFs, m_frame->fccVector.at(j)->m_graph->m_maxTempPoint);

				m_frame->fccVector.at(j)->m_graph->Refresh();
			}

			return;
		}
	}

}

void createFile()
{
	std::ofstream ofs;
	ofs.open("Profiles.txt");
	for (int i = 0; i < 5; i++)
	{
		ofs << i;
		ofs << "000001001001002100001001001002200001001001002300001001001002400001001001002500001001001002\n";
	}
	ofs.close();
	return;
}

bool doesFileExist()
{
	std::ifstream ifs;
	ifs.open("Profiles.txt");
	if (ifs.fail())
	{
		return false;
	}
	return true;
}

void MainApp::editFanProfile(int prof)
{
	std::ifstream ifs;
	ifs.open("Profiles.txt");
	if (ifs.fail())
	{
		createFile();
		ifs.clear();
		ifs.close();
		ifs.open("Profiles.txt");
	}
	std::ofstream ofs;
	ofs.open("ProfilesTemp.txt");


	for (int i = 0; i < 5; i++)
	{
		if (i == prof)
		{
			ofs << i;
			for (int i = 0; i < 6; i++)
			{
				std::stringstream ss;

				ss << m_frame->fccVector.at(i)->getFanHeader();
				ss << (m_frame->fccVector.at(i)->m_graph->m_dataPoints.size());

				std::string minTemp = m_frame->fccVector.at(i)->m_graph->m_minTempPoint->getTempAsString(m_frame->fccVector.at(i)->m_graph);
				std::string minFs = m_frame->fccVector.at(i)->m_graph->m_minTempPoint->getFanSpeedAsString(m_frame->fccVector.at(i)->m_graph);

				ss << minTemp << minFs;

				for (int j = 0; j < m_frame->fccVector.at(i)->m_graph->m_dataPoints.size(); j++)
				{

					std::string temp = m_frame->fccVector.at(i)->m_graph->m_dataPoints.at(j)->getTempAsString(m_frame->fccVector.at(i)->m_graph);
					std::string fs = m_frame->fccVector.at(i)->m_graph->m_dataPoints.at(j)->getFanSpeedAsString(m_frame->fccVector.at(i)->m_graph);

					ss << temp << fs;
				}

				std::string maxTemp = m_frame->fccVector.at(i)->m_graph->m_maxTempPoint->getTempAsString(m_frame->fccVector.at(i)->m_graph);
				std::string maxFs = m_frame->fccVector.at(i)->m_graph->m_maxTempPoint->getFanSpeedAsString(m_frame->fccVector.at(i)->m_graph);

				ss << maxTemp << maxFs;

				int tempSelect = m_frame->fccVector.at(i)->getTempChoice();

				ss << tempSelect;

				std::string str = ss.str();
				ofs << str;
			}
			ofs << '\n';
		}
		else
		{
			char data[500];
			ifs.close();
			ifs.open("Profiles.txt");
			ifs.getline(data, 500, '\n');
			std::string s(data);
			ofs << data << '\n';
		}
	}
	ofs.close();
	ifs.close();

	ofs.open("Profiles.txt");
	ifs.open("ProfilesTemp.txt");

	for (int i = 0; i < 5; i++)
	{
		char data[500];
		ifs.getline(data, 500, '\n');
		std::string s(data);
		ofs << data << '\n';
	}
}




wxBEGIN_EVENT_TABLE(MainApp, wxApp)
	EVT_TIMER(window::id::TIMER_ID, MainApp::onTimer)
wxEND_EVENT_TABLE()
