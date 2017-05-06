#include<stdio.h>
#include"HttpProxy.h"

int main()
{
	CHttpProxy *hpServer = new CHttpProxy(9000);
	hpServer->start();
	while(1)
	{
	}	
}
