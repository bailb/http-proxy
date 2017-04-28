#ifndef __HTTP__PROXY__
#define __HTTP__PROXY__
#include<stdio.h>
#include<string>

class HttpConnection {//: public CThread {
public:
    HttpConnection();
    ~HttpConnection();
private:
    virtual void run();
private:
    int fd;
};

class CHttpProxy {
	public:
		CHttpProxy();
		~CHttpProxy();
	public:
		void start();
		void stop();
};
#endif
