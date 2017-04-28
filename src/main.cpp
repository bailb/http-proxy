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

#define PORT        25341
#define BACKLOG     100 
#define MEM_SIZE    1024*10
#include "HttpHeader.h"

using namespace BLB_UTILS;

struct event_base* base;
struct sock_ev
{
    struct event* read_ev;
    struct event* write_ev;
    char* buffer;
};

char *get_ip(char *host){  
    struct hostent *hent;  
    int iplen=15;  
    char *ip=(char *)malloc(iplen+1);  
    memset(ip,0,iplen+1);  
    if((hent=gethostbyname(host))==NULL){  
        perror("Can't get ip");  
        exit(1);  
    }  
    if(inet_ntop(AF_INET,(void *)hent->h_addr_list[0],ip,iplen)==NULL){  
        perror("Can't resolve host!\n");  
        exit(1);  
    }  
    return ip;  
}

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *user)
{
	size_t sock = (int64_t)user;
	size_t length = size * nmemb;
	char *buf = (char*)buffer;
    	size_t ret = 0;
	ret = send(sock, buffer, length, 0);
	if (ret != length)
	{
	    printf("write_data failed[%d][%d][%d]\n",sock,errno,length);
	}
	else
	{
	    printf("ok[%d]\n",length);
	}
	return length;
}
static size_t save_header(void *buffer,size_t size,size_t nmemb, void *user)
{
	return size*nmemb;
}

bool requestUrl(CHttpParser *h, int sock)
{

	CURL *curl;
	CURLcode res;
	std::string url = h->getPath();
	struct curl_slist *headers = NULL;
	printf("sock[%d]\n",sock);
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER,0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST,0L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)sock);
		curl_easy_setopt(curl, CURLOPT_HEADER,(void*)sock);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION,save_header);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
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


		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
		/* always cleanup */
		if (headers)
		    curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

    return true;

}

void *aConn(void *arg)
{
    int size;
    int sock = int64_t(static_cast <int*>(arg));

    pthread_detach(pthread_self());
    bool keepAlive = false;
    do{

	char *buffer = (char*)malloc(MEM_SIZE);
	bzero(buffer, MEM_SIZE);
	size = recv(sock, buffer, MEM_SIZE, 0);
	if (size == 0)
	{
		printf("---------------------\n");
		return NULL;
	}
	else
	{
		printf("recv---------------------size[%d]\n",size);
	}

	CHttpParser *httpParser = new CHttpParser();
	httpParser->parser(buffer);
	printf("Proxy-conn[%s]\n",httpParser->getValueByKey("Proxy-Connection").c_str());
	if (httpParser->getValueByKey("Proxy-Connection") == "Keep-Alive")
	{
		keepAlive = true;
	}
	if (!requestUrl(httpParser, sock))
	{
		printf("request error\n");
	}

   }while(keepAlive);

   return NULL;
}


void release_sock_event(struct sock_ev* ev)
{
    event_del(ev->read_ev);
    free(ev->read_ev);
    free(ev->write_ev);
    free(ev->buffer);
    free(ev);
}

void on_write(int sock, short event, void* arg)
{
    char* buffer = (char*)arg;
    send(sock, buffer, strlen(buffer), 0);
    free(buffer);
}

void on_read(int sock, short event, void* arg)
{
    pthread_t tid;
    int error = pthread_create(&tid, NULL,aConn, (void*)sock);
    if(0 != error)
	fprintf(stderr, "Couldn't run thread %d error %d\n",sock,error);
    else
	fprintf(stderr, "Thread %d, gets %d\n", tid, (int)sock);


}

void on_accept(int sock, short event, void* arg)
{
    struct sockaddr_in cli_addr;
    int newfd, sin_size;
    struct sock_ev* ev = (struct sock_ev*)malloc(sizeof(struct sock_ev));

    sin_size = sizeof(struct sockaddr_in);
    newfd = accept(sock, (struct sockaddr*)&cli_addr, (socklen_t*)&sin_size);

    ev->read_ev = event_new(base, newfd, EV_READ, on_read, ev);
    event_add(ev->read_ev, NULL);
}

int main(int argc, char* argv[])
{
    struct sockaddr_in my_addr;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
    listen(sock, BACKLOG);
    struct event *listen_ev;
    base = event_base_new();

    listen_ev = event_new(base, sock, EV_READ|EV_PERSIST, on_accept, NULL);
    event_add(listen_ev, NULL);
    event_base_dispatch(base);

    return 0;
}
