#include <stdio.h>
#include"HttpProxy.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <event.h>
#include <curl/curl.h>
#include <errno.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

#define BACKLOG 25
#define MEM_SIZE 1024*10

CHttpProxy::CHttpProxy()
{
    printf("CHttpProxy\r\n");
}

CHttpProxy::~CHttpProxy()
{
    printf("CHttpProxy\r\n");
}

void CHttpProxy::onRead(int sock, short event, void* arg)
{
    CHttpProxy *hp = (CHttpProxy*)arg;
    hp->on_Read(sock,event);
}

void CHttpProxy::onAccept(int sock, short event, void* arg)
{
    CHttpProxy *hp = (CHttpProxy*)arg;
    hp->on_Accept(sock,event);
}

void CHttpProxy::on_Read(int sock, short event)
{
    CHttpConnection *hCon = m_con_map[sock];
    if (!hCon)
    {
        printf("onRead error,There is no this connection[%d]\n",sock);
    }
    else
    {
        hCon->start();
    }
}

void CHttpProxy::on_Accept(int sock, short event)
{
    
    struct sockaddr_in cli_addr;
    int newfd, sin_size;

    struct event* ConEv = NULL;

    sin_size = sizeof(struct sockaddr_in);
    newfd = accept(sock, (struct sockaddr*)&cli_addr, (socklen_t*)&sin_size);
    
    CHttpConnection *hCon = new CHttpConnection(newfd);
    event_set(ConEv, newfd, EV_READ, onRead, (void*)this);
    if (!addEvent(ConEv))
    {
        printf("addEvent error\n");
    }

    m_con_map[newfd]=hCon;
}

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *user)
{
    size_t sock = (int64_t)user;
    size_t length = size * nmemb;
    size_t ret = 0;
    ret = send(sock, buffer, length, 0);

    if (ret != length)
    {
        printf("write_data failed[%d][%d][%d]\n",(int)sock,(int)errno,(int)length);
    }
    else
    {
        printf("ok[%d]\n",(int)length);
    }

    return length;
}

bool CHttpConnection::requestUrl()
{
    CURL *curl;
    CURLcode res;
    CHttpParser *h = m_http_parser;

    std::string url = h->getPath();
    struct curl_slist *headers = NULL;
    curl = curl_easy_init();
    if(curl) 
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER,0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST,0L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)m_sock);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTP_TRANSFER_DECODING, 0L);
        std::string key = "";
        std::string value = "";
           
        for (HttpHeaderIter it = h->m_Headers.begin(); it != h->m_Headers.end();it ++)
        {
            if (it->first != "Proxy-Connection")
            {
               key = it->first;
               value = it->second;
               headers = curl_slist_append(headers,(key+":"+value).c_str());
            }
        }
            
        if (headers)
        {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
        if (h->getMethod() == HTTP_REQ_GET)
        {
        // do nothing
        }
        else if (h->getMethod()== HTTP_REQ_POST)
        {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            if (!h->getBody().empty())
            {
                 curl_easy_setopt(curl, CURLOPT_POSTFIELDS, h->getBody().c_str());
            }
        }

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        /* always cleanup */
        if (headers)
            curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return true;
}

void CHttpConnection::run()
{
    int size;
    pthread_detach(pthread_self());
    bool keepAlive = false;
    do{

        char *buffer = (char*)malloc(MEM_SIZE);
        bzero(buffer, MEM_SIZE);
        size = recv(m_sock, buffer, MEM_SIZE, 0);
        if (size == 0)
        {
            printf("recv error[%d]\n",errno);
            return;
        }
        else
        {
            printf("recv size[%d]\n",size);
        }

        m_http_parser = new CHttpParser();
        m_http_parser->parser(buffer);
		std::string alive = "";
		if ( m_http_parser->getValueByKey("Proxy-Connection",alive))
		{
			if (alive == "Keep-Alive")
			{
				keepAlive = true;
			}
		}

        if (!requestUrl())
        {
            printf("request error\n");
        }

   }while(keepAlive);

}

void CHttpProxy::start()
{
    struct sockaddr_in my_addr;
    int sock = 0;
    int yes = 1;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(m_port);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
    listen(sock, BACKLOG);
    struct event *listen_ev = NULL;
    event_set(listen_ev, sock, EV_READ|EV_PERSIST, onAccept, (void*)this);
    
    if (!addEvent(listen_ev))
    {
        printf("addEvent error\n");
    }

    CEventServer::start();

}

CHttpConnection::CHttpConnection(int sock)
{
    m_sock = sock;
}
CHttpConnection::~CHttpConnection()
{

}
