#ifndef __HTTP__PROXY__
#define __HTTP__PROXY__

#include<stdio.h>
#include<string>
#include<map>
#include<list>
#include "utils/Thread.h"
#include "EventServer.h"
#include "HttpParser.h"

using namespace BLB_UTILS;

struct CEventPair {
	public:
		struct event *readEv;
		struct event *writeEv;
};

class CHttpConnection : public CThread {
	public:
    	CHttpConnection(int sock);
    	~CHttpConnection();
		virtual void run();
	private:
		bool requestUrl();
	private:
    	int m_sock;
		CHttpParser *m_http_parser;
};

typedef std::map<int, CHttpConnection *> ConMap;

class CHttpProxy : public CEventServer {
	public:
		CHttpProxy();
		~CHttpProxy();
	public:
		static void onAccept(int sock, short event, void* arg);
		void on_Accept(int sock, short event);
		static void onRead(int sock, short event, void* arg);
		void on_Read(int sock, short event);
		void start();
		void stop();
	private:
		ConMap m_con_map;
		int m_port;
};

#endif

