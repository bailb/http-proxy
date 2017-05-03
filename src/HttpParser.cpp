#include"HttpParser.h"
#include<string>
#include<string.h>

namespace BLB_UTILS {

CHttpParser::CHttpParser()
{


}

CHttpParser::~CHttpParser()
{


}
methodType CHttpParser::getMethod()
{
	return mMethod;
}
std::string& CHttpParser::getBody() 
{
	return mBody;
}

std::string &CHttpParser::getPath()
{
	return mURI;
}
std::string &CHttpParser::getVersion()
{
	return mVersion;
}
inline void CHttpParser::trim(std::string &s)
{
	if (!s.empty())
	{
		s.erase(0,s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}

}
bool CHttpParser::parseRequest(std::string &line)
{
	std::string str = line;
	std::string method;
    	std::string uri;

	size_t pos = str.find(" ");
	if (pos == std::string::npos) {
		return false;
    	}

	method = str.substr(0,pos);
	str = str.substr(pos+1);
	trim(str);

	if (method == "GET") {
		mMethod = HTTP_REQ_GET;
	} else if (method == "POST") {
		mMethod = HTTP_REQ_POST;
	} else if (method == "HEAD") {
		mMethod = HTTP_REQ_HEAD;
	} else if (method == "PUT") {
		mMethod = HTTP_REQ_PUT;
	} else if (method == "DELETE") {
		mMethod = HTTP_REQ_DELETE;
	} else if (method == "OPTIONS") {
		mMethod = HTTP_REQ_OPTIONS;
	} else if (method == "TRACE") {
		mMethod = HTTP_REQ_TRACE;
	} else if (method == "CONNECT") {
		mMethod = HTTP_REQ_CONNECT;
	} else if (method == "PATCH") {
		mMethod = HTTP_REQ_PATCH;
	} else {
		printf("unkown method [%s][%s]\n",method.c_str(),line.c_str());
		return false;
	}

	pos = str.find(" ");
    if (pos == std::string::npos) {
        return false;
    }
    uri = str.substr(0,pos);
	trim(uri);

	mURI = uri;
    str = str.substr(pos+1);

    mVersion = str;

    printf("method[%d],uri[%s],version[%s]\n",mMethod,mURI.c_str(),mVersion.c_str());
    return true;
}

bool CHttpParser::getPair(const std::string &line, std::string &key, std::string &value)
{
	size_t pos = line.find(":");
    if (pos == std::string::npos)
	{
		printf("line[%s]\n",line.c_str());
		return false;
	}

	key = line.substr(0,pos);
	value = line.substr(pos+1);
	trim(key);
	trim(value);

	return true;
}

std::string & CHttpParser::getValueByKey(const char *key)
{
	return this->m_Headers[key];
}

bool CHttpParser::parser(const char *buffer)
{
	std::string str = buffer;
    std::string line;
	size_t pos = 0;

	if (str != "")
	{/* first line , include Method*/
		pos = str.find("\r\n");
		if (pos == std::string::npos)
		{
			printf("can't parser[%s] \r\n",buffer);
			return false;
		}
		line = str.substr(0,pos);
		str = str.substr(pos+strlen("\r\n"));
		if (!parseRequest(line))
		{
			return false;
		}
	}

    while (str != "")
    {
		pos = str.find("\r\n");
		if (pos == std::string::npos)
		{
			break;
		}
		else
		{
			line = str.substr(0,pos);
			std::string key,value;
			if (getPair(line,key,value))
			{
			    m_Headers[key] = value;
				printf("line[%s][%s][%s]\n",line.c_str(),key.c_str(),value.c_str());
			}
			else
			{
				return false;
			}

			str = str.substr(pos+strlen("\r\n"));
		}

		if (0 == str.compare(0,strlen("\r\n"),"\r\n"))
		{ /* BODY */
			if (str.length() > strlen("\r\n"))
			{
				mBody = str.substr(strlen("\r\n"));
				printf("mBody:%s\n",mBody.c_str());
			}

			break;
		}

    }

	return true;
}

}



