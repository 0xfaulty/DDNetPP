#include "fifoconsole.h"

#include <engine/shared/config.h>

#include <fstream>

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

FifoConsole::FifoConsole(IConsole *pConsole)
{
	void *m_pFifoThread = thread_create(ListenFifoThread, pConsole);
#if defined(CONF_FAMILY_UNIX)
	pthread_detach((pthread_t)m_pFifoThread);
#endif
}

FifoConsole::~FifoConsole()
{
	thread_destroy(m_pFifoThread);
}

void FifoConsole::ListenFifoThread(void *pUser)
{
	IConsole *pConsole = (IConsole *)pUser;

	if (str_comp(g_Config.m_SvInputFifo, "") == 0)
		return;

	mkfifo(g_Config.m_SvInputFifo, 0600);

	std::ifstream f;
	char aBuf[256];

	while (true)
	{
		f.open(g_Config.m_SvInputFifo);
		while (f.getline(aBuf, 256))
		{
			pConsole->ExecuteLineFlag(aBuf, CFGFLAG_SERVER, -1);
		}
		f.close();
	}
}