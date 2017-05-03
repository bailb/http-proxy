#include <stdio.h>
#include"HttpProxy.h"

CHttpProxy::CHttpProxy()
{
	printf("CHttpProxy\r\n");
}

CHttpProxy::~CHttpProxy()
{
    printf("CHttpProxy\r\n");
}

CHttpConnection::CHttpConnection()
{

}
CHttpConnection::~CHttpConnection()
{

}

void CHttpConnection::run()
{
/* 
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
			printf("recv error[%d]\n",errno);
			return NULL;
		}
		else
		{   
			printf("recv size[%d]\n",size);
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
*/  
   return NULL;

}
