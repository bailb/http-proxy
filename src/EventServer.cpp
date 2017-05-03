#include"EventServer.h"
CEventServer::CEventServer():m_event_handler(NULL)
{
	if (!m_event_handler)
	{
		m_event_handler = event_base_new();
	}
}

CEventServer::~CEventServer()
{
    if (m_event_handler)
    {
        event_base_free(m_event_handler);
    }

}

bool CEventServer::addEvent(struct event* care)
{
    return event_add(care, NULL)?false:true;
}

bool CEventServer::delEvent(struct event* care)
{
    return event_del(care)?false:true;
}

void CEventServer::run()
{
    event_base_dispatch(m_event_handler);
}
