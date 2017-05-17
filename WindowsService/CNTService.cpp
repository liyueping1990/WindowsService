// Copyright (c) 2000-2009 长峰科技工业集团综合安保系统研发部
//  创建人:付建祖 
//  版 本: 1.0
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 

#include <tchar.h>
#include <stdio.h>
#include "CNTService.h"
#include <crtdbg.h>
#include <io.h>
#include <fcntl.h>

//#include "NTServiceEventLogMsg.h"


//##ModelId=4A2895B801C6
BOOL CNTService :: m_bInstance = FALSE;

static CNTService * gpTheService = 0;		
//extern int				g_nDebugCode;
CNTService * AfxGetService() { return gpTheService; }

static LPCTSTR gszAppRegKey = TEXT("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");

//##ModelId=4A2895B801E4
CNTService::CNTService(LPCTSTR lpServiceName, LPCTSTR lpDisplayName)
: m_lpServiceName(lpServiceName)
, m_lpDisplayName(lpDisplayName ? lpDisplayName : lpServiceName)
, m_dwCheckPoint(0)
, m_dwErr(0)
, m_bDebug(FALSE)
, m_sshStatusHandle(0)
, m_dwControlsAccepted(SERVICE_ACCEPT_STOP)
, m_pUserSID(0)
, m_fConsoleReady(FALSE)
, m_dwDesiredAccess(SERVICE_ALL_ACCESS)
, m_dwServiceType(SERVICE_WIN32_OWN_PROCESS)
, m_dwStartType(SERVICE_AUTO_START)
, m_dwErrorControl(SERVICE_ERROR_NORMAL)
, m_pszLoadOrderGroup(0)
, m_dwTagID(0)
, m_pszDependencies(0)
, m_pszStartName(0)
, m_pszPassword(0)
{
	_ASSERTE( ! m_bInstance );
	m_bInstance = TRUE;
	gpTheService = this;
	m_ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_ssStatus.dwServiceSpecificExitCode = 0;
}

//##ModelId=4A2895B801F6
CNTService::~CNTService()
{
	_ASSERTE( m_bInstance );
	m_bInstance = FALSE;
	gpTheService = 0;
}

//##ModelId=4A2895B80209
void CNTService::Pause()
{
	// TODO: Add your specialized code here.
}

//##ModelId=4A2895B80213
void CNTService::Continue()
{
	// TODO: Add your specialized code here.
}

//##ModelId=4A2895B80215
void CNTService::Shutdown()
{
	// TODO: Add your specialized code here.
}

//##ModelId=4A2895B80222
BOOL CNTService::RegisterService(int argc, char* * argv)
{
	BOOL (CNTService::* fnc)() = &CNTService::StartDispatcher;
    DWORD Argc;
    LPTSTR * Argv;
	
#ifdef UNICODE
    Argv = CommandLineToArgvW(GetCommandLineW(), &Argc );
#else
    Argc = (DWORD) argc;
    Argv = argv;
#endif
	
    while( ++Argv, --Argc ) 
	{
		if (Argv[0][0] == TEXT('-') ) 
		{
			switch( Argv[0][1] ) 
			{
			case TEXT('i'):	// install the service
				fnc = &CNTService::InstallService;
				break;
			case TEXT('u'):	// uninstall the service
				fnc = &CNTService::RemoveService;
				break;
			case TEXT('s'):	// start the service
				fnc = &CNTService::StartupService;
				break;
			case TEXT('e'):	// end the service
				fnc = &CNTService::EndService;
				break;
			case TEXT('d'):	// debug the service

#ifdef UNICODE
				::GlobalFree(HGLOBAL)Argv);
#endif			
				m_bDebug = TRUE;
				//g_nDebugCode = 1;
				
				return DebugService(argc, argv);
			}
		}
	}
	
#ifdef UNICODE
	::GlobalFree(HGLOBAL)Argv);
#endif
	
	return (this->*fnc)();
}

//##ModelId=4A2895B80226
BOOL CNTService::StartDispatcher() 
{
	SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { LPTSTR(m_lpServiceName), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { 0, 0 }
    };
	
	BOOL bRet = StartServiceCtrlDispatcher(dispatchTable);
	if (! bRet ) 
	{
		TCHAR szBuf[256];
        AddToMessageLog(GetLastErrorText(szBuf,255));
	}
	
	return bRet;
}


//##ModelId=4A2895B80265
void WINAPI CNTService :: ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv) 
{
	_ASSERTE( gpTheService != 0 );
	
	gpTheService->m_sshStatusHandle =	RegisterServiceCtrlHandler(
		gpTheService->m_lpServiceName,
		CNTService::ServiceCtrl
		);
	
	if (gpTheService->m_sshStatusHandle )
		if (gpTheService->ReportStatus(SERVICE_START_PENDING) )
		{
			gpTheService->Run( dwArgc, lpszArgv );
		}
		
		if (gpTheService->m_sshStatusHandle )
			gpTheService->ReportStatus(SERVICE_STOPPED);
}

//##ModelId=4A2895B80262
void WINAPI CNTService :: ServiceCtrl(DWORD dwCtrlCode) 
{
	_ASSERTE( gpTheService != 0 );
	
	switch( dwCtrlCode ) 
	{
	case SERVICE_CONTROL_STOP:
		gpTheService->m_ssStatus.dwCurrentState = SERVICE_STOP_PENDING;
		gpTheService->Stop();
		break;
		
	case SERVICE_CONTROL_PAUSE:
		gpTheService->m_ssStatus.dwCurrentState = SERVICE_PAUSE_PENDING;
		gpTheService->Pause();
		break;
		
	case SERVICE_CONTROL_CONTINUE:
		gpTheService->m_ssStatus.dwCurrentState = SERVICE_CONTINUE_PENDING;
		gpTheService->Continue();
		break;
		
	case SERVICE_CONTROL_SHUTDOWN:
		gpTheService->Shutdown();
		break;
		
	case SERVICE_CONTROL_INTERROGATE:
		gpTheService->ReportStatus(gpTheService->m_ssStatus.dwCurrentState);
		break;
		
	default:
		break;
	}
	
}


//##ModelId=4A2895B80271
BOOL WINAPI CNTService :: ControlHandler(DWORD dwCtrlType) 
{
	_ASSERTE(gpTheService != 0);
	switch( dwCtrlType ) 
	{
	case CTRL_BREAK_EVENT:  // Ctrl+C or Ctrl+Break
	case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
		_tprintf(TEXT("Stopping %s.\n"), gpTheService->m_lpDisplayName);
		gpTheService->Stop();
		return TRUE;
	}
	return FALSE;
}

//##ModelId=4A2895B80232
BOOL CNTService::InstallService()
{
    TCHAR szPath[1024];

	SetupConsole();	
	
	if (GetModuleFileName( 0, szPath, 1023 ) == 0 ) 
	{
		TCHAR szErr[256];
		_tprintf(TEXT("Unable to install %s - %s\n"), m_lpDisplayName, GetLastErrorText(szErr, 256));
		return FALSE;
	}
	
	BOOL bRet = FALSE;
	
	
	// Real NT services go here.
	SC_HANDLE schSCManager =	OpenSCManager(
		0,						
		0,						
		SC_MANAGER_ALL_ACCESS	
		);
	if (schSCManager ) 
	{
		SC_HANDLE schService =	CreateService(
			schSCManager,
			m_lpServiceName,
			m_lpDisplayName,
			m_dwDesiredAccess,
			m_dwServiceType,
			m_dwStartType,
			m_dwErrorControl,
			szPath,
			m_pszLoadOrderGroup,
			((m_dwServiceType == SERVICE_KERNEL_DRIVER ||
			m_dwServiceType == SERVICE_FILE_SYSTEM_DRIVER) &&
			(m_dwStartType == SERVICE_BOOT_START ||
			m_dwStartType == SERVICE_SYSTEM_START)) ?
			&m_dwTagID : 0,
			m_pszDependencies,
			m_pszStartName,
			m_pszPassword
			);
		
		if (schService ) 
		{
			_tprintf(TEXT("%s installed.\n"), m_lpDisplayName );
			CloseServiceHandle(schService);
			bRet = TRUE;
		} else 
		{
			TCHAR szErr[256];
			_tprintf(TEXT("CreateService failed - %s\n"), GetLastErrorText(szErr, 256));
		}
		
		CloseServiceHandle(schSCManager);
	} else 
	{
		TCHAR szErr[256];
		_tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
	}
	
	if (bRet ) 
	{
		RegisterApplicationLog(
			szPath,	
			EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE 
			);
		
		AddToMessageLog(TEXT("Service installed"),EVENTLOG_INFORMATION_TYPE);
	}
	
	return bRet;
}

//##ModelId=4A2895B80234
BOOL CNTService::RemoveService()
{
	BOOL bRet = FALSE;

	SetupConsole();	
	
	SC_HANDLE schSCManager = OpenSCManager(
		0,						
		0,						
		SC_MANAGER_ALL_ACCESS	
		);
	if (schSCManager ) 
	{
		SC_HANDLE schService =	OpenService(
			schSCManager,
			m_lpServiceName,
			SERVICE_ALL_ACCESS
			);
		
		if (schService ) 
		{
			if (ControlService(schService, SERVICE_CONTROL_STOP, &m_ssStatus) ) 
			{
				_tprintf(TEXT("Stopping %s."), m_lpDisplayName);
				Sleep(1000);
				
				while( QueryServiceStatus(schService, &m_ssStatus) ) 
				{
					if (m_ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
					{
						_tprintf(TEXT("."));
						Sleep( 1000 );
					} else
						break;
				}
				
				if (m_ssStatus.dwCurrentState == SERVICE_STOPPED )
					_tprintf(TEXT("\n%s stopped.\n"), m_lpDisplayName);
				else
					_tprintf(TEXT("\n%s failed to stop.\n"), m_lpDisplayName);
			}
			
			// now remove the service
			if (DeleteService(schService) ) 
			{
				_tprintf(TEXT("%s removed.\n"), m_lpDisplayName);
				bRet = TRUE;
			} 
			else 
			{
				TCHAR szErr[256];
				_tprintf(TEXT("DeleteService failed - %s\n"), GetLastErrorText(szErr,256));
			}
			
			CloseServiceHandle(schService);
		} 
		else 
		{
			TCHAR szErr[256];
			_tprintf(TEXT("OpenService failed - %s\n"), GetLastErrorText(szErr,256));
		}
		
		CloseServiceHandle(schSCManager);
	} 
	else 
	{
		TCHAR szErr[256];
		_tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
	}
	
	if (bRet )
		DeregisterApplicationLog();	
		
	return bRet;
}

//##ModelId=4A2895B80242
BOOL CNTService::EndService()
{
	BOOL bRet = FALSE;

	SC_HANDLE schSCManager = ::OpenSCManager(
								0,						
								0,						
								SC_MANAGER_ALL_ACCESS	
								);
	if (schSCManager )
	{
		SC_HANDLE schService =	::OpenService(
			schSCManager,
			m_lpServiceName,
			SERVICE_ALL_ACCESS
			);
		
		if (schService ) 
		{
			if (::ControlService(schService, SERVICE_CONTROL_STOP, &m_ssStatus) ) 
			{
				_tprintf(TEXT("Stopping %s."), m_lpDisplayName);
				::Sleep(1000);
				
				while( ::QueryServiceStatus(schService, &m_ssStatus) ) 
				{
					if (m_ssStatus.dwCurrentState == SERVICE_STOP_PENDING ) 
					{
						_tprintf(TEXT("."));
						::Sleep( 1000 );
					} else
						break;
				}
				
				if (m_ssStatus.dwCurrentState == SERVICE_STOPPED )
					bRet = TRUE, _tprintf(TEXT("\n%s stopped.\n"), m_lpDisplayName);
                else
                    _tprintf(TEXT("\n%s failed to stop.\n"), m_lpDisplayName);
			}
			
			::CloseServiceHandle(schService);
		} 
		else 
		{
			TCHAR szErr[256];
			_tprintf(TEXT("OpenService failed - %s\n"), GetLastErrorText(szErr,256));
		}
		
        ::CloseServiceHandle(schSCManager);
    } 
	else 
	{
		TCHAR szErr[256];
		_tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
	}
	
	return bRet;
}

//##ModelId=4A2895B80244
BOOL CNTService::StartupService()
{
	BOOL bRet = FALSE;

	SC_HANDLE schSCManager = ::OpenSCManager(
								0,						
								0,						
								SC_MANAGER_ALL_ACCESS	
								);
	if (schSCManager ) 
	{
		SC_HANDLE schService =	::OpenService(
			schSCManager,
			m_lpServiceName,
			SERVICE_ALL_ACCESS
			);
		
		if (schService ) 
		{
			_tprintf(TEXT("Starting up %s."), m_lpDisplayName);
			if (::StartService(schService, 0, 0) ) 
			{
				Sleep(1000);
				
				while( ::QueryServiceStatus(schService, &m_ssStatus) ) 
				{
					if (m_ssStatus.dwCurrentState == SERVICE_START_PENDING )
					{
						_tprintf(TEXT("."));
						Sleep( 1000 );
					} else
						break;
				}
				
				if (m_ssStatus.dwCurrentState == SERVICE_RUNNING )
					bRet = TRUE, _tprintf(TEXT("\n%s started.\n"), m_lpDisplayName);
                else
                    _tprintf(TEXT("\n%s failed to start.\n"), m_lpDisplayName);
			} 
			else
			{
				TCHAR szErr[256];
				_tprintf(TEXT("\n%s failed to start: %s\n"), m_lpDisplayName, GetLastErrorText(szErr,256));
			}
			
			::CloseServiceHandle(schService);
		} 
		else 
		{
			TCHAR szErr[256];
			_tprintf(TEXT("OpenService failed - %s\n"), GetLastErrorText(szErr,256));
		}
		
        ::CloseServiceHandle(schSCManager);
    } 
	else 
	{
		TCHAR szErr[256];
		_tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
	}
	
	return bRet;
}

//##ModelId=4A2895B80246
BOOL CNTService :: DebugService(int argc, char ** argv) 
{
    DWORD dwArgc;
    LPTSTR *lpszArgv;
	
#ifdef UNICODE
    lpszArgv = CommandLineToArgvW(GetCommandLineW(), &(dwArgc) );
#else
    dwArgc   = (DWORD) argc;
    lpszArgv = argv;
#endif
	
	SetupConsole();	
	_tprintf(TEXT("Debugging %s.\n"), m_lpDisplayName);
	
	SetConsoleCtrlHandler(ControlHandler, TRUE);
		
    Run(dwArgc, lpszArgv);
	
#ifdef UNICODE
	::GlobalFree(HGLOBAL)lpszArgv);
#endif

	
	return TRUE;
}

//##ModelId=4A2895B80274
void CNTService::SetupConsole() 
{
	if (!m_fConsoleReady) 
	{
		AllocConsole();	
		
		DWORD astds[3]={STD_OUTPUT_HANDLE,STD_ERROR_HANDLE,STD_INPUT_HANDLE};
		FILE *atrgs[3]={stdout,stderr,stdin};
		for( register int i=0; i<3; i++ ) 
		{
			long hand=(long)GetStdHandle(astds[i]);
			if (hand!=(long)INVALID_HANDLE_VALUE ) 
			{
				int osf=_open_osfhandle(hand,_O_TEXT);
				if (osf!=-1 ) 
				{
					FILE *fp=_fdopen(osf,(astds[i]==STD_INPUT_HANDLE) ? "r" : "w");
					if (fp!=NULL ) 
					{
						*(atrgs[i])=*fp;
						setvbuf(fp,NULL,_IONBF,0);
					}
				}
			}
		}
		m_fConsoleReady=TRUE;
	}
}

//##ModelId=4A2895B80254
BOOL CNTService :: ReportStatus(
						DWORD dwCurrentState,
						DWORD dwWaitHint,
						DWORD dwErrExit ) {
	BOOL fResult = TRUE;

	if (!m_bDebug ) 
	{
        if (dwCurrentState == SERVICE_START_PENDING)
            m_ssStatus.dwControlsAccepted = 0;
        else
            m_ssStatus.dwControlsAccepted = m_dwControlsAccepted;

        m_ssStatus.dwCurrentState = dwCurrentState;
        m_ssStatus.dwWin32ExitCode = NO_ERROR;
        m_ssStatus.dwWaitHint = dwWaitHint;
		
		m_ssStatus.dwServiceSpecificExitCode = dwErrExit;
		if (dwErrExit!=0)
			m_ssStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
		
        if (dwCurrentState == SERVICE_RUNNING ||
            dwCurrentState == SERVICE_STOPPED )
            m_ssStatus.dwCheckPoint = 0;
        else
            m_ssStatus.dwCheckPoint = ++m_dwCheckPoint;
		
        if (!(fResult = SetServiceStatus( m_sshStatusHandle, &m_ssStatus))) 
		{
            AddToMessageLog(TEXT("SetServiceStatus() failed"));
        }
    }

    return fResult;
}


//##ModelId=4A2895B80258
void CNTService :: AddToMessageLog(LPTSTR lpszMsg, WORD wEventType, DWORD dwEventID) 
{
	m_dwErr = GetLastError();

	if (dwEventID == DWORD(-1) ) 
	{
		switch( wEventType ) 
		{
			case EVENTLOG_ERROR_TYPE:
				dwEventID = MSG_ERROR_1;
				break;
			case EVENTLOG_WARNING_TYPE:
				dwEventID = MSG_WARNING_1;
				break;
			case EVENTLOG_INFORMATION_TYPE:
				dwEventID = MSG_INFO_1;
				break;
			case EVENTLOG_AUDIT_SUCCESS:
				dwEventID = MSG_INFO_1;
				break;
			case EVENTLOG_AUDIT_FAILURE:
				dwEventID = MSG_INFO_1;
				break;
			default:
				dwEventID = MSG_INFO_1;
				break;
		}
	}

	HANDLE hEventSource = RegisterEventSource(0, m_lpServiceName);

	if (hEventSource != 0 ) 
	{
		LPCTSTR lpszMessage = lpszMsg;

		ReportEvent(
			hEventSource,	
			wEventType,		
			0,				
			dwEventID,		
			m_pUserSID,		
			1,				
			0,				
			&lpszMessage,	
			0				
		);

		::DeregisterEventSource(hEventSource);
    }
}


//##ModelId=4A2895B80251
LPTSTR CNTService :: GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize ) 
{
    LPTSTR lpszTemp = 0;

    DWORD dwRet =	::FormatMessage(
						FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
						0,
						GetLastError(),
						LANG_NEUTRAL,
						(LPTSTR)&lpszTemp,
						0,
						0
					);

    if (!dwRet || (dwSize < dwRet+14) )
        lpszBuf[0] = TEXT('\0');
    else 
	{
        lpszTemp[_tcsclen(lpszTemp)-2] = TEXT('\0');  
        _tcscpy(lpszBuf, lpszTemp);
    }

    if (lpszTemp )
        LocalFree(HLOCAL(lpszTemp));

    return lpszBuf;
}

//##ModelId=4A2895B802FD
void CNTService :: RegisterApplicationLog( LPCTSTR lpszFileName, DWORD dwTypes ) 
{
	TCHAR szKey[256];
	_tcscpy(szKey, gszAppRegKey);
	_tcscat(szKey, m_lpServiceName);
	HKEY hKey = 0;
	LONG lRet = ERROR_SUCCESS;
	
	if (::RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) == ERROR_SUCCESS ) 
	{
		lRet =	::RegSetValueEx(
					hKey,						
					TEXT("EventMessageFile"),	
					0,							
					REG_EXPAND_SZ,				
					(CONST BYTE*)lpszFileName,	
					_tcslen(lpszFileName) + 1	
				);

		lRet =	::RegSetValueEx(
					hKey,					
					TEXT("TypesSupported"),	
					0,						
					REG_DWORD,				
					(CONST BYTE*)&dwTypes,	
					sizeof(DWORD)			
				);
		::RegCloseKey(hKey);
	}

	lRet =	::RegOpenKeyEx( 
				HKEY_LOCAL_MACHINE,	
				gszAppRegKey,		
				0,					
				KEY_ALL_ACCESS,		
				&hKey				
			);
	if (lRet == ERROR_SUCCESS ) 
	{
		DWORD dwSize;

		lRet =	::RegQueryValueEx(
					hKey,			
					TEXT("Sources"),
					0,				
					0,				
					0,				
					&dwSize			
				);

 		if (lRet == ERROR_SUCCESS ) 
		{
			DWORD dwType;
			DWORD dwNewSize = dwSize+_tcslen(m_lpServiceName)+1;
			LPBYTE Buffer = LPBYTE(::GlobalAlloc(GPTR, dwNewSize));

			lRet =	::RegQueryValueEx(
						hKey,			
						TEXT("Sources"),
						0,				
						&dwType,		
						Buffer,			
						&dwSize			
					);
			if (lRet == ERROR_SUCCESS ) 
			{
				_ASSERTE(dwType == REG_MULTI_SZ);

				register LPTSTR p = LPTSTR(Buffer);
				for(; *p; p += _tcslen(p)+1 ) 
				{
					if (_tcscmp(p, m_lpServiceName) == 0 )
						break;
				}
				if (! * p ) 
				{
					_tcscpy(p, m_lpServiceName);

					lRet =	::RegSetValueEx(
								hKey,			
								TEXT("Sources"),
								0,				
								dwType,			
								Buffer,			
								dwNewSize		
							);
				}
			}

			::GlobalFree(HGLOBAL(Buffer));
		}

		::RegCloseKey(hKey);
	}
}


//##ModelId=4A2895B80301
void CNTService :: DeregisterApplicationLog() 
{
	TCHAR szKey[256];
	_tcscpy(szKey, gszAppRegKey);
	_tcscat(szKey, m_lpServiceName);
	HKEY hKey = 0;
	LONG lRet = ERROR_SUCCESS;

	lRet = ::RegDeleteKey(HKEY_LOCAL_MACHINE, szKey);

	lRet =	::RegOpenKeyEx( 
				HKEY_LOCAL_MACHINE,	
				gszAppRegKey,		
				0,					
				KEY_ALL_ACCESS,		
				&hKey				
			);
	if (lRet == ERROR_SUCCESS ) 
	{
		DWORD dwSize;

		lRet =	::RegQueryValueEx(
					hKey,			
					TEXT("Sources"),
					0,				
					0,				
					0,				
					&dwSize			
				);

 		if (lRet == ERROR_SUCCESS ) 
		{
			DWORD dwType;
			LPBYTE Buffer = LPBYTE(::GlobalAlloc(GPTR, dwSize));
			LPBYTE NewBuffer = LPBYTE(::GlobalAlloc(GPTR, dwSize));

			lRet =	::RegQueryValueEx(
						hKey,			
						TEXT("Sources"),
						0,				
						&dwType,		
						Buffer,			
						&dwSize			
					);
			if (lRet == ERROR_SUCCESS ) 
			{
				_ASSERTE(dwType == REG_MULTI_SZ);

				register LPTSTR p = LPTSTR(Buffer);
				register LPTSTR pNew = LPTSTR(NewBuffer);
				BOOL bNeedSave = FALSE;	
				for(; *p; p += _tcslen(p)+1) 
				{
					if (_tcscmp(p, m_lpServiceName) != 0 ) 
					{
						_tcscpy(pNew, p);
						pNew += _tcslen(pNew)+1;
					} else 
					{
						bNeedSave = TRUE;			
						dwSize -= _tcslen(p)+1;		
					}
				}
				if (bNeedSave ) 
				{
					lRet =	::RegSetValueEx(
								hKey,			
								TEXT("Sources"),
								0,				
								dwType,			
								NewBuffer,		
								dwSize			
							);
				}
			}

			::GlobalFree(HGLOBAL(Buffer));
			::GlobalFree(HGLOBAL(NewBuffer));
		}

		::RegCloseKey(hKey);
	}
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             