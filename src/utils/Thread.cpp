#include "Thread.h"
#include <sys/types.h>
#include <unistd.h>


void* CThread::run1()
{
    m_threadStatus = THREAD_STATUS_RUNNING;
    m_tid = pthread_self();
    run();
    m_threadStatus = THREAD_STATUS_EXIT;
    m_tid = 0;
    pthread_exit(NULL);
}

CThread::CThread()
{
       m_tid = 0;
       m_threadStatus = THREAD_STATUS_NEW;
}

bool CThread::start()
{
    int iRet = 0;
    pthread_create(&m_tid, NULL, thread_proxy_func, this) == 0;
}

pthread_t CThread::getThreadID()
{
    return m_tid;
}

int CThread::getState()
{
    return m_threadStatus;
}

void CThread::join()
{
    if (m_tid > 0)
    {
        pthread_join(m_tid, NULL);
    }
}
void * CThread::thread_proxy_func(void * args)
{
 	CThread * pThread = static_cast<CThread *>(args); 
 
	pThread->run(); 
 		
 	return NULL; 
}

void CThread::join(unsigned long millisTime)
{
    if (m_tid == 0)
    {
        return;
    }
    if (millisTime == 0)
    {
        join();
    }
	else
    {
        unsigned long k = 0;
        while (m_threadStatus != THREAD_STATUS_EXIT && k <= millisTime)
        {
            usleep(100);
            k++;
        }
    }
}
