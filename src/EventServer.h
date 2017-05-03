#ifndef __EVENT_SERVER__
#define __EVENT_SERVER__
#include"event.h"
#include"utils/Thread.h"

class CEventServer:public CThread {
	public:
		CEventServer();
		~CEventServer();
    public:
		bool addEvent(struct event *listen_ev);
		bool delEvent(struct event *listen_ev);
		virtual void run();
	private:
		event_base* m_event_handler;
};

#endif

