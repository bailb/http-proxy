#ifndef __EVENT_SERVER__
#define __EVENT_SERVER__
#include"event.h"

class CEventServer {
	public:
		CEventServer();
		~CEventServer();
    public:
		bool addEvent(int fd,short event);
		bool removeEvent(int fd, short event);
	private:
		event_base* m_event_handler;
};

#endif

