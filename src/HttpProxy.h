#ifndef __HTTP__PROXY__
#define __HTTP__PROXY__

#include<stdio.h>
#include<string>
#include<map>

#include "utils/Thread.h"
#include "EventServer.h"

class CHttpConnection : public CThread {
public:
    CHttpConnection();
    ~CHttpConnection();
private:
    virtual void run();
private:
    int fd;
};

class CHttpProxy : public CEventServer {
	public:
		CHttpProxy();
		~CHttpProxy();
	public:
		void start();
		void stop();
};

#endif
