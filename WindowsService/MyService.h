#pragma once
#include "CNTService.h"
class MyService :
	public CNTService
{
public:
	MyService();
	virtual ~MyService();

public:
	void ExitCleanup();
	void Run(DWORD dwArgc, LPTSTR* ppszArgv);
	void Stop();


private:
	HANDLE m_hStop;
};

