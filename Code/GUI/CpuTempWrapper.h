#pragma once

class CpuTempWrapperPrivate;

class CpuTempWrapper
{
private:
	CpuTempWrapperPrivate* m_tempReceiver;

public:
	CpuTempWrapper();

	float getCPUTemp();
};
