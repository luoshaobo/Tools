#ifndef SERVICE_H__82384012042387503895478239492304235845892394293492309
#define SERVICE_H__82384012042387503895478239492304235845892394293492309

#include "StdAfx.h"
#include "TK_Tools.h"

namespace TK_Tools {
class DebugServer;
} //namespace TK_Tools {

class Launcher;
class Service;

class ServiceFactory
{
public:
	enum {
		SERVICE_DEF = 0,
		SERVICE_ID_0 = SERVICE_DEF,
		SERVICE_ID_1,
		SERVICE_ID_2,
		SERVICE_ID_3,
		SERVICE_ID_4,
		SERVICE_ID_5,
		SERVICE_ID_6,
		SERVICE_ID_7,
		SERVICE_ID_8,
		SERVICE_ID_9,
		SERVICE_ID_COUNT
	};

private:
	ServiceFactory();
	
public:
	virtual ~ServiceFactory();
	static ServiceFactory &GetInstance();

public:
	Service *GetService(unsigned int nServiceId);
	void DestroyService(unsigned int nServiceId);
	void DestroyAllServices();

public:
	void SetLastServiceId(unsigned int nServiceId) { m_nLastServiceId = nServiceId; }
	unsigned int GetLastServiceId() { return m_nLastServiceId; }

public:
	static bool ProcessCmdLine_static(TK_Tools::DebugServerInstanceId id, const char *pCmdLine);
	
private:
	bool ProcessCmdLine(TK_Tools::DebugServerInstanceId id, const char *pCmdLine);

private:
	void Lock();
	void Unlock();

private:
	Service *m_pServices[SERVICE_ID_COUNT];
	HANDLE m_hLockMutex;
	unsigned int m_nLastServiceId;
};

class Service
{
public:
	Service(unsigned int nServiceId);
	virtual ~Service();

public:
	bool ProcessCmdLine(const char *pCmdLine);
	DWORD Read(PUCHAR pBuf, ULONG nCount);
	DWORD Write(PUCHAR pBuf, ULONG nCount);

private:
	void Lock();
	void Unlock();

	DWORD WaitReadBufNonempty(DWORD dwMilliseconds);
	void SetReadBufNonempty();

private:
	unsigned int m_nServiceId;
	TK_Tools::DebugServer &m_rDebugServer;
	Launcher *m_pLauncher;
	std::list<std::string> m_arrCmdLines;
	bool m_bExeExited;
	HANDLE m_hLockMutex;
	HANDLE m_hReadBufNonemptyEvent;
};

#endif // #ifndef SERVICE_H__82384012042387503895478239492304235845892394293492309

