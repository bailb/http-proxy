#ifndef __HTTP__PARSER__
#define __HTTP__PARSER__

#include<stdio.h>
#include<string>
#include<map>

namespace BLB_UTILS {
typedef std::map<std::string,std::string> HttpHeader;
typedef HttpHeader::iterator HttpHeaderIter;

enum methodType
{
	HTTP_REQ_GET     = 1 << 0,
	HTTP_REQ_POST    = 1 << 1,
	HTTP_REQ_HEAD    = 1 << 2,
	HTTP_REQ_PUT     = 1 << 3,
	HTTP_REQ_DELETE  = 1 << 4,
	HTTP_REQ_OPTIONS = 1 << 5,
	HTTP_REQ_TRACE   = 1 << 6,
	HTTP_REQ_CONNECT = 1 << 7,
	HTTP_REQ_PATCH   = 1 << 8
};

class CHttpParser
{
	public:
	    CHttpParser();
        ~CHttpParser();
	    bool parser(const char *);
	    methodType getMethod();
	    std::string &getPath();
	    std::string &getVersion();
		std::string& getValueByKey(const char *);
    private:
		bool parseRequest(std::string &);
		bool getPair(const std::string &line, std::string &, std::string &);
		inline void trim(std::string &s);
	private:
		int mPort;
		std::string mProto;
		methodType mMethod;
		std::string mURI;
		std::string mVersion;
	public:
		HttpHeader m_Headers;

};


}

#endif

