#include "MyService.h"



MyService::MyService():CNTService(TEXT("MyService"), TEXT("Win_MyService")), m_hStop(0)
{
	m_dwControlsAccepted = 0;
	m_dwControlsAccepted |= SERVICE_ACCEPT_STOP;
}


MyService::~MyService()
{
}

void MyService::ExitCleanup()
{
	if (m_hStop)
	{
		::CloseHandle(m_hStop);
	}
}

void MyService::Run(DWORD dwArgc, LPTSTR * ppszArgv)
{
	ReportStatus(SERVICE_START_PENDING);
	m_hStop = ::CreateEvent(0, TRUE, FALSE, 0);

	//startYourFunction();

	ReportStatus(SERVICE_RUNNING);

	while (::WaitForSingleObject(m_hStop, 1500) != WAIT_OBJECT_0)
	{
	}
	ExitCleanup();
}

void MyService::Stop()
{
	ReportStatus(SERVICE_STOP_PENDING, 5000);

	if (m_hStop)
	{
		::SetEvent(m_hStop);
	}
}
