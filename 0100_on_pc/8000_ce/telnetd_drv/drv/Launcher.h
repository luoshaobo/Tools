#ifndef LAUNCHER_H__83895092300423590358928959349589345782374237832
#define LAUNCHER_H__83895092300423590358928959349589345782374237832

#include "StdAfx.h"

class Launcher 
{
public:
	Launcher(unsigned int nServiceId);
	virtual ~Launcher();

public:
	bool LaunchExe(const std::string &sExePath, const std::string &sArgs);

private:
	static DWORD LauncherThreadProc_static(LPVOID lpParameter);
	DWORD LauncherThreadProc();

private:
	unsigned int m_nServiceId;
	std::string m_sExePath;
	std::string m_sArgs;
};

#endif // #ifndef LAUNCHER_H__83895092300423590358928959349589345782374237832

