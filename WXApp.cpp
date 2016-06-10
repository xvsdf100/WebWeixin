#include "StdAfx.h"
#include "WXApp.h"
#include "HttpClientHelper.h"
#include "StringUtility.h"
#include <time.h>
#include "Utility.h"
#include "Path.h"
#include <sstream>
using namespace std;

void SaveCodeJpg(WXApp::ByteArray array)
{
	std::wstring wCodePath = Path::GetCurrentPath();
	wCodePath += L"wxcode.jpg";

	FILE* fp = _wfopen(wCodePath.c_str(),L"wb");
	assert(NULL != fp);
	fwrite(&array.at(0),1,array.size(),fp);
	fclose(fp);
	fp = NULL;
}

struct HookCallBack
{
    UINT        type; 
    DWORD_PTR   old;       //方便上层提供任意
    DWORD_PTR   dwContext;
    DWORD_PTR   hooker;
};


WXApp::WXApp(void)
{
	m_MsgID = 0;
}

WXApp::~WXApp(void)
{
}

bool WXApp::login()
{
	bool bRet = false;
	std::string strUUID = requestUUID();

	if(strUUID.length())
	{
		ByteArray array;
		requestQRCode(strUUID,array);
		SaveCodeJpg(array);
		m_UUID = strUUID;
		bRet = true;
	}
	
	return bRet;
}

std::string WXApp::requestUUID()
{
	HttpClientHelper http("login.weixin.qq.com");
	std::string requestObj = "/jslogin?appid=wx782c26e4c19acffb&redirect_uri=https%3A%2F%2Fwx.qq.com%2Fcgi-bin%2Fmmwebwx-bin%2Fwebwxnewloginpage&fun=new&lang=zh_CN&_=";
	//获取时间

	std::string strUUID;

	time_t tm;
	tm = ::time(0);
	requestObj += StringUtility::IToString((int)tm);

	HttpClientHelper::HttpClientRespond respond;
	http.Get(requestObj,&respond);

	int statuCode = respond.GetStatuCode();

	if(200 == statuCode)
	{
		std::string strBody;
		respond.GetBody(strBody);

		if(strBody.length())
		{
			std::string it = "uuid = \"";
			int nPos = strBody.find(it);
			if(-1 != nPos)
			{
				strUUID = strBody.substr(nPos + it.length(),strBody.length() - nPos - it.length() - 2);
			}
		}
	}
	
	return strUUID;
}

bool WXApp::requestQRCode( std::string strUUID,ByteArray& array )
{
	bool bRet = false;
	HttpClientHelper http("login.weixin.qq.com");
	std::string requestObj = "/qrcode/";
	requestObj += strUUID;
	HttpClientHelper::HttpClientRespond respond;
	http.Get(requestObj,&respond);

	int statuCode = respond.GetStatuCode();
	if(200 == statuCode)
	{
		respond.GetBody(array);
		bRet = true;
	}

	return bRet;
}


bool WXApp::requestContactList()
{
	bool bRet = false;
	HttpClientHelper http("wx.qq.com");
	std::string requestObj = "/cgi-bin/mmwebwx-bin/webwxgetcontact?pass_ticket=";
	requestObj += m_PassTicket;
	requestObj += "&seq=0&skey=";
	requestObj += m_EncryKey;

	HttpClientHelper::HttpClientRespond respond;
	http.Get(requestObj,&respond);

	int statuCode = respond.GetStatuCode();
	if(200 == statuCode)
	{
		std::string body;
		respond.GetBody(body);
		bRet = ParseContactInfo(body);
	}

	return bRet;
}


std::string WXApp::requestLoginInfo()
{
	std::string strLoginUrl;
	std::string strLoginInfo;
	HttpClientHelper http("login.weixin.qq.com");
	std::string requestObj = "/cgi-bin/mmwebwx-bin/login?loginicon=true&uuid=";
	requestObj += m_UUID;
	requestObj += "&tip=0&r=-707963359&_=";

	time_t tm;
	tm = ::time(0);
	requestObj += StringUtility::IToString((int)tm);

	HttpClientHelper::HttpClientRespond respond;
	http.Get(requestObj,&respond);

	int statuCode = respond.GetStatuCode();
	if(200 == statuCode)
	{
		std::string body;
		respond.GetBody(body);
		std::string it = "redirect_uri=\"";
		int nPos = body.find(it);
		if(-1 != nPos)
		{
			strLoginUrl = body.substr(nPos + it.length(), body.length() - it.length() - nPos - 1 -1);
		}
	}

	return strLoginUrl;

}

bool WXApp::requestLoinPage(std::string strLoginUrl)
{
	bool bRet = false;
	HttpClientHelper http("wx.qq.com");
	std::string requestObj = "/cgi-bin/mmwebwx-bin/webwxnewloginpage";
	std::string strParam = GetLoginPageRequestObj(strLoginUrl);
	requestObj += strParam;

	std::string body;
	HttpClientHelper::HttpClientRespond respond;
	http.SetAutoRedirect(false);	//获取自己需要的，不能直接跳转
	http.Get(requestObj,&respond);

	int statuCode = respond.GetStatuCode();
	if(301 == statuCode)
	{
		respond.GetBody(body);
		bRet = true;

		m_EncryKey = GetTagValue(body,"skey");
		m_Sid = GetTagValue(body,"wxsid");
		m_PassTicket = GetTagValue(body,"pass_ticket");

		std::string lpWxUin = GetTagValue(body,"wxuin");
		m_WxUin = (UINT)StringUtility::StringToInt64(lpWxUin);
	}

	return bRet;
}

std::string WXApp::GetLoginPageRequestObj( const std::string& url )
{
	std::string strLoginPageObj;
	int nPos = url.find("?ticket");
	if(-1 != nPos)
	{
		strLoginPageObj = url.substr(nPos);
		strLoginPageObj = strLoginPageObj.substr(0,strLoginPageObj.length() - 2);
	}
	return strLoginPageObj;
}

bool WXApp::SendMsg( std::string userName, std::string body )
{
	bool bRet = false;
	std::string msg;
	msg = CreateTextMsg(userName,body);
	
	HttpClientHelper http("wx.qq.com");
	HttpClientHelper::HttpClientRespond respond;
	std::string obj = "/cgi-bin/mmwebwx-bin/webwxsendmsg?pass_ticket=";
	obj += m_PassTicket;

	bRet = http.Post(obj,&respond,NULL,msg.data(),msg.length());
	if(bRet && http.GetResultCode() == 200)
	{
		bRet = true;
	}

	return bRet;
}

std::string WXApp::CreateTextMsg( std::string userName,std::string body )
{
	std::string formatString;
	std::string LocalID = "14526093633040712";

	time_t tm;
	tm = ::time(0);

	std::stringstream stream;
	stream << tm;
	stream >> LocalID;
	stream.clear();
	stream << m_MsgID;
	m_MsgID++;

	std::string strNum;
	stream >> strNum;
	LocalID += strNum;

	if(!userName.empty()&&!body.empty())
	{
		
		Json::Value TextMsg;
		Json::Value BaseRequest;
		Json::Value Msg;

		BaseRequest["DeviceID"] = "e273337146912900";
		BaseRequest["Sid"] = m_Sid;
		BaseRequest["Skey"] = m_EncryKey;
		BaseRequest["Uin"] = m_WxUin;

		Msg["ClientMsgId"] = LocalID;
		Msg["LocalID"] = LocalID;
		Msg["Content"] = body;
		Msg["FromUserName"] = m_User["User"]["UserName"];
		Msg["ToUserName"] = userName;
		Msg["Type"] = 1;

		TextMsg["BaseRequest"] = BaseRequest;
		TextMsg["Msg"] = Msg;

		formatString = TextMsg.toStyledString();

	}

	return formatString;
}

std::string WXApp::GetRequestCheckAysnc()
{
	size_t n = m_CheckAysn["List"].size();
	std::string strCheckAsync;
	std::stringstream str;

	for(size_t i = 0; i < m_CheckAysn["List"].size(); i++)
	{
		str << m_CheckAysn["List"][i]["Key"].asUInt();
		str << "_";
		str << m_CheckAysn["List"][i]["Val"].asUInt();

		if(i != n -1 )
		str << "|";

	}
	str >> strCheckAsync;
	return strCheckAsync;
}


bool WXApp::WXInit()
{
	bool bRet = false;

	HttpClientHelper http("wx.qq.com");
	HttpClientHelper::HttpClientRespond respond;
	std::string obj = "/cgi-bin/mmwebwx-bin/webwxinit?r=-910243318&pass_ticket=";
	obj += m_PassTicket;

	std::string msg = GetInitJsonBody();

	bRet = http.Post(obj,&respond,NULL,msg.data(),msg.length());
	if(bRet && http.GetResultCode() == 200)
	{
		bRet = true;
		std::string body;
		respond.GetBody(body);
		ParseInitInfo(body);
		m_CheckAysn = m_User["SyncKey"];
		msg = m_CheckAysn.toStyledString();
	}

	return bRet;
}

Json::Value WXApp::GetBaseRequest()
{
	Json::Value BaseRequest;
	BaseRequest["DeviceID"] = "e273337146912900";
	BaseRequest["Sid"] = m_Sid;
	BaseRequest["Skey"] = m_EncryKey;
	BaseRequest["Uin"] = m_WxUin;
	return BaseRequest;
}

std::string WXApp::GetInitJsonBody()
{
	Json::Value body;
	body["BaseRequest"] = GetBaseRequest();
	return body.toStyledString();
	
}


std::string WXApp::GetWxSyncBody()
{
    Json::Value body;
    Json::Value baseReqeust = GetBaseRequest();
    body["BaseRequest"] = baseReqeust;
    body["SyncKey"] = m_CheckAysn;
    body["rr"] = 0;
    return body.toStyledString();
}

bool WXApp::ParseInitInfo(const std::string& str )
{
	bool bRet = false;
	Json::Reader reader;
	if(reader.parse(str,m_User))
	{
		bRet = true;
	}

	return bRet;
}

bool WXApp::ParseContactInfo( const std::string& str )
{
	bool bRet = false;
	Json::Reader reader;
	if(reader.parse(str,m_ContactList))
	{
		bRet = true;
	}

	return bRet;
}

bool WXApp::ParseNewCheckAsyncResult( const std::string& str,Json::Value& obj)
{
    bool bRet = false;
    Json::Reader reader;
    if(reader.parse(str,obj))
    {
        m_AysnCheckResult = obj;
        if(!obj["SyncKey"].empty())
        {
            bRet = true;
            m_CheckAysn.clear();
            m_CheckAysn = obj["SyncKey"];
        }

       /* MsgInfo info;
        for(size_t i = 0; i < result["AddMsgList"].size(); i++)
        {
            info.id = result["AddMsgList"][i]["MsgId"].asString();
            info.fromeUser = result["AddMsgList"][i]["FromUserName"].asString();
            info.toUser = result["AddMsgList"][i]["ToUserName"].asString();
            info.type = result["AddMsgList"][i]["MsgType"].asInt();
            info.msg = result["AddMsgList"][i]["Content"].asString();
            msgList.push_back(info);
        }*/
    }

    return bRet;
}


Json::Value WXApp::GetUserInfo( std::string Name,UserInfoIndex Key /*= eAlias*/ )
{
	const std::string IndexString[2] = {"Alias","NickName"};
	std::string	keyName = IndexString[Key];
	Json::Value UserInfo;
	std::string ssss = m_ContactList.toStyledString();
	size_t total = m_ContactList.size();

	if(!m_ContactList.empty())
	{
		for(size_t i = 0; i < m_ContactList["MemberList"].size(); i++)
		{
			std::string It = m_ContactList["MemberList"][i][keyName].asString();
			It = StringUtility::Utf8ToString(It);
			if(It == Name)
			{
				UserInfo = m_ContactList["MemberList"][i];
			}
		}
	}


	return UserInfo;
}

void WXApp::GetUserContactList( std::string& contactList )
{
	contactList =  m_ContactList.toStyledString();
}
/************************************************************************/
/* 临时代码处理                                                         */
/************************************************************************/

char dec2hexChar(short int n) {
    if ( 0 <= n && n <= 9 ) {
        return char( short('0') + n );
    } else if ( 10 <= n && n <= 15 ) {
        return char( short('A') + n - 10 );
    } else {
        return char(0);
    }
}

short int hexChar2dec(char c) {
    if ( '0'<=c && c<='9' ) {
        return short(c-'0');
    } else if ( 'a'<=c && c<='f' ) {
        return ( short(c-'a') + 10 );
    } else if ( 'A'<=c && c<='F' ) {
        return ( short(c-'A') + 10 );
    } else {
        return -1;
    }
}

string escapeURL(const string &URL)
{
    string result = "";
    for ( unsigned int i=0; i<URL.size(); i++ ) {
        char c = URL[i];
        if (
            ( '0'<=c && c<='9' ) ||
            ( 'a'<=c && c<='z' ) ||
            ( 'A'<=c && c<='Z' ) ||
            c=='/' || c=='.'
            ) {
                result += c;
        } else {
            int j = (short int)c;
            if ( j < 0 ) {
                j += 256;
            }
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1*16;
            result += '%';
            result += dec2hexChar(i1);
            result += dec2hexChar(i0);
        }
    }
    return result;
}

string deescapeURL(const string &URL) {
    string result = "";
    for ( unsigned int i=0; i<URL.size(); i++ ) {
        char c = URL[i];
        if ( c != '%' ) {
            result += c;
        } else {
            char c1 = URL[++i];
            char c0 = URL[++i];
            int num = 0;
            num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
            result += char(num);
        }
    }
    return result;
}

bool WXApp::RequestAysnCheck(int& nResult)
{
    //内部
    HttpClientHelper* http = new HttpClientHelper("webpush.weixin.qq.com",80);
	HttpClientHelper::HttpClientRespond repond;
    std::string str = "/cgi-bin/mmwebwx-bin/synccheck?r=";
    std::string UrlCode;
    time_t tm;
    tm = ::time(0);
    std::stringstream strStream;
    strStream << tm;
    std::string r;
    strStream >> r;
    str += r;
    str += "&skey=";
    str += m_EncryKey;
    str += "&sid=";
    UrlCode = escapeURL(m_Sid);
    str += UrlCode;
    str += "&uin=";
    str += StringUtility::IToString(m_WxUin);
    str += "&deviceid=e599146929270706";
    str += "&synckey=";
    str += GetRequestCheckAysnc();
    bool bVal = http->Get(str,&repond);
	if(!bVal)
	{
		std::string body;
		repond.GetBody(body);
		nResult = GetCheckAsyncResult(body);
	}

    return true;
}

//代码需要修改
void RecvCallbackHook(HttpClientHelper::HttpClientRespond& repond,DWORD_PTR dwContext)
{
    HookCallBack* pHookInfo = (HookCallBack*)dwContext;
    WXApp* pThis = (WXApp*)pHookInfo->hooker;
    std::string strBody;
    repond.GetBody(strBody);
    std::vector<MsgInfo> msgLis;
   // pThis->ParseNewCheckAsyncResult(strBody,msgLis);
	OutputDebugStringA("Debug1\r\n");
    ((RecvMsgWrapperCallBack)(pHookInfo->old))(msgLis);
	OutputDebugStringA("Debug2\r\n");
    
}

bool WXApp::RecvMsg(Json::Value& msg)
{
	HttpClientHelper* http = new HttpClientHelper("wx.qq.com",80);
	HttpClientHelper::HttpClientRespond repond;
	std::string str = "/cgi-bin/mmwebwx-bin/webwxsync?sid=";
	str += m_Sid;
	str += "&skey=";
	str += m_EncryKey;
    str += "&pass_ticket=";
    str += m_PassTicket;
    std::string body = GetWxSyncBody();
	bool bRet = http->Post(str,&repond,NULL,body.data(),body.length());
	repond.GetBody(body);
	bRet = ParseNewCheckAsyncResult(body,msg);
	return bRet;
}

int WXApp::GetCheckAsyncResult(std::string& str)
{
	//这里需要优化解析
	int bRet = 0;
	if(str.find("2") != -1)
	{
		bRet = 1;
	}

	return bRet;
}

bool WXApp::ParseJsonStr(const std::string& str,Json::Value& obj)
{
	bool bRet = false;
	Json::Reader reader;
	if(reader.parse(str,obj))
	{
		bRet = true;
	}

	return bRet;
}

Json::Value WXApp::GetSelfUserInfo()
{
	return m_User["User"];
}



