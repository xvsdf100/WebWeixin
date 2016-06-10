/************************************************************************/
/* 说明：HTTP Client 常见请求的封装                                     */
/* 时间：2015年12月9日 22:37:07											*/
/* 作者：xjm															*/
/************************************************************************/

/*
默认短连接，长连接要使用对应的重载函数
*/
#ifndef	HTTPCLIENTHELPER_H
#define	HTTPCLIENTHELPER_H

#include <Windows.h>
#include "Wininet.h"
#include <string>
#include <map>
#include <vector>

struct InternalRequestContext ;
class HttpClientHelper
{
public:
	class HttpClientRespond;

	typedef std::map<std::string,std::string> HeadItems;
	typedef HeadItems::const_iterator HeadItemsIt;
	typedef std::vector<char>	ByteArray;
	typedef void (*RequstCallback)(HttpClientRespond& repond,DWORD_PTR dwContext);		//通知外部回调
	
public:
	HttpClientHelper(std::string host,int port = 80,bool bAsyn = false);
	~HttpClientHelper();

	bool Connect();
	void Close();

	//同步接口
	bool Post(std::string requestObj, const HeadItems* headItems = NULL, const char* data = NULL, int len = 0);
	bool Get(std::string requestObj = "", const HeadItems* headItems = NULL, const char* data = NULL, int len = 0);
	bool Head(std::string requestOb = "");

	bool Post(std::string requestObj, HttpClientRespond* repond , const HeadItems* headItems = NULL, const char* data = NULL, int len = 0);
	bool Get(std::string requestObj, HttpClientRespond* repond , const HeadItems* headItems = NULL, const char* data = NULL, int len = 0);

	//异步接口
	bool Get(std::string requestObj, RequstCallback fun, DWORD_PTR dwContext =NULL ,const HeadItems* headItems = NULL, const char* data = NULL, int len = 0);
    bool Post(std::string requestObj, RequstCallback fun, DWORD_PTR dwContext =NULL ,const HeadItems* headItems = NULL, const char* data = NULL, int len = 0);
	//获取跳转url,因为wininet是默认自动跳转，有时候我们需要跳转的URL地址，分析一些东西出来
	std::string GetLocationUrl();

	std::string GetRequestObj();

	//用url是host
	//tip:不能设置在htt heads里面设置，必须要设置flag，不能用系统的COOKIE
	bool SetCookie(const std::string& cookie );

	std::string GetCookie(const std::string& cookieName);

	//返回状态码
	int GetResultCode();

	//默认自动重定向，但有时候需要获取重点向的URL的一些信息
	void SetAutoRedirect(bool bVal);

	static void __stdcall HttpCallBack(HINTERNET hInternet,
		DWORD_PTR dwContext,
		DWORD dwInternetStatus,
		LPVOID lpvStatusInformation,
		DWORD dwStatusInformationLength);


private:
	bool Action(std::string type,std::string requestObj, HttpClientRespond* repond , const HeadItems* headItems = NULL, const char* data = NULL, int len = 0);
	bool Action(std::string type,std::string requestObj, RequstCallback fun, DWORD_PTR dwContext,const HeadItems* headItems = NULL, const char* data = NULL, int len = 0);

public:
	class HttpClientRespond
	{
		
	public:
		HttpClientRespond() { m_statuCode = 0; }
		~HttpClientRespond() {}

		void SetStatuCode(int code);
		void SetHeads(const HeadItems& heads);
		void SetBody(const ByteArray& body);

		void GetBody(ByteArray& body);
		void GetBody(std::string& body);
		int  GetStatuCode() {return m_statuCode;}

		std::string GetHeadItem(std::string name);
		std::string operator[](std::string name);

	private:
		int			m_statuCode;
		HeadItems	m_heads;
		ByteArray	m_bodys;
	};

private:
	void InitInternet();

	std::string	GetHeardItems(const HeadItems* headItems);

	//读取Head
	static void ReadHead(HINTERNET hRequest, HeadItems& heads, int& statuCode);

	//读取BODY数据
	static void ReadData(HINTERNET hRequest,ByteArray& bodyData);

	//释放函数
	void CloseRequestInfo(HINTERNET hRequest);

	//获取请求行
	static std::string GetRequestLine(const std::string& head);

	//获取域名
	std::string GetHostDomain();

private:
	static void ParseHead(const std::string& head, HeadItems& heads);

	static bool ParseLineText(const std::string& line, std::string& name, std::string& value);

	//解析请求行

	static void ParsRequestLine(std::string line,std::string& obj);

	static bool CheckIOPENDING(DWORD dwResult);

public:
	static bool ReadRespond(HINTERNET hSession,HttpClientRespond& respond);

public:
	HINTERNET		m_hInternetRoot;
	HINTERNET		m_hInternetConnect;
	HINTERNET		m_hSession;				//这个结构很重要
	std::string		m_host;
	int				m_port;
	std::string		m_agent;
	bool			m_bAutoRedirect;
	bool			m_bAsyn;
    InternalRequestContext* m_context;
};


struct InternalRequestContext 
{
	DWORD_PTR ptr;
	HttpClientHelper::RequstCallback fn;
	HttpClientHelper* pThis;        //传递自己
	char*           postData;
	bool    bAutoRelese;
	int             State;

	InternalRequestContext()
	{
		ptr = NULL;
		fn = NULL;
		pThis = NULL;
		postData = NULL;
		bAutoRelese = true;
		State = 0;
	}

	~InternalRequestContext()
	{
		if(bAutoRelese)
		{
			if(NULL != pThis)   delete pThis; pThis = NULL;
		}

		if(NULL != postData) delete []postData; postData = NULL;
	}
};


#endif


