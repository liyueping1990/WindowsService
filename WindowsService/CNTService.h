// Copyright (c) 2000-2009 长峰科技工业集团综合安保系统研发部
//  创建人:付建祖 
//  版 本: 1.0
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _INC_CNTSERVICE_49F8EE4903AD_INCLUDED
#define _INC_CNTSERVICE_49F8EE4903AD_INCLUDED

#include <Windows.h>
#include "NTServiceEventLogMsg.h"

//##ModelId=4A2895B801C5
class CNTService 
{
public:
	//只运行一个进程
	//##ModelId=4A2895B801C6
	static BOOL	m_bInstance;
public:
	//##ModelId=4A2895B801E4
	CNTService(LPCTSTR lpServiceName, LPCTSTR lpDisplayName = 0);

	//##ModelId=4A2895B801F6
	~CNTService();

	//##ModelId=4A2895B80203
	virtual void Run(DWORD argc, LPTSTR* argv) = 0;

	//##ModelId=4A2895B80207
	virtual void Stop() = 0;

	//##ModelId=4A2895B80209
	virtual void Pause();

	//##ModelId=4A2895B80213
	virtual void Continue();

	//##ModelId=4A2895B80215
	virtual void Shutdown();

	//##ModelId=4A2895B80222
	virtual BOOL RegisterService(int argc, char* * argv);

	//##ModelId=4A2895B80226
	virtual BOOL StartDispatcher();

	//##ModelId=4A2895B80232
	virtual BOOL InstallService();

	//##ModelId=4A2895B80234
	virtual BOOL RemoveService();

	//##ModelId=4A2895B80242
	virtual BOOL EndService();

	//##ModelId=4A2895B80244
	virtual BOOL StartupService();

	//##ModelId=4A2895B80246
	virtual BOOL DebugService(int argc, char **argv);

	//##ModelId=4A2895B80251
	LPTSTR GetLastErrorText(LPTSTR Buf, DWORD Size);
	
	//##ModelId=4A2895B80254
	BOOL ReportStatus(
		DWORD CurState,				
		DWORD WaitHint = 3000,	
		DWORD ErrExit = 0		
		);

	//##ModelId=4A2895B80258
	virtual void	AddToMessageLog(
		LPTSTR	Message,
		WORD	EventType = EVENTLOG_ERROR_TYPE,
		DWORD	dwEventID = DWORD(-1)
		);

	//##ModelId=4A2895B80262
	static void WINAPI	ServiceCtrl(DWORD CtrlCode);
	//##ModelId=4A2895B80265
	static void WINAPI	ServiceMain(DWORD argc, LPTSTR * argv);
	//##ModelId=4A2895B80271
	static BOOL WINAPI	ControlHandler(DWORD CtrlType);

	//##ModelId=4A2895B80274
	void SetupConsole();
protected:
	//##ModelId=4A2895B80280
	LPCTSTR m_lpServiceName;

	//##ModelId=4A2895B80281
	LPCTSTR m_lpDisplayName;

	//##ModelId=4A2895B80283
	SERVICE_STATUS			m_ssStatus;	
	//##ModelId=4A2895B80290
	SERVICE_STATUS_HANDLE	m_sshStatusHandle;

	//##ModelId=4A2895B80291
	DWORD					m_dwCheckPoint;
	//##ModelId=4A2895B8029F
	DWORD					m_dwErr;
	//##ModelId=4A2895B802A0
	BOOL					m_bDebug;
	//##ModelId=4A2895B802A1
	BOOL					m_fConsoleReady;
	
	//##ModelId=4A2895B802AF
	DWORD					m_dwControlsAccepted;
	//##ModelId=4A2895B802B1
	PSID					m_pUserSID;	
	
	//##ModelId=4A2895B802BF
	DWORD					m_dwDesiredAccess;		
	//##ModelId=4A2895B802C0
	DWORD					m_dwServiceType;		
	//##ModelId=4A2895B802CE
	DWORD					m_dwStartType;			
	//##ModelId=4A2895B802CF
	DWORD					m_dwErrorControl;		
	//##ModelId=4A2895B802DE
	LPCTSTR					m_pszLoadOrderGroup;	
	//##ModelId=4A2895B802DF
	DWORD					m_dwTagID;				
	//##ModelId=4A2895B802E0
	LPCTSTR					m_pszDependencies;		
	//##ModelId=4A2895B802EE
	LPCTSTR					m_pszStartName;			
	//##ModelId=4A2895B802EF
	LPCTSTR					m_pszPassword;	
	
protected: 
	//##ModelId=4A2895B802FD
	virtual void	RegisterApplicationLog(
		LPCTSTR lpszProposedMessageFile,
		DWORD dwProposedTypes
		);

	//##ModelId=4A2895B80301
	virtual void	DeregisterApplicationLog();

};

CNTService * AfxGetService();
#endif /* _INC_CNTSERVICE_49F8EE4903AD_INCLUDED */
