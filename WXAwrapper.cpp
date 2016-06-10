#include "stdafx.h"
#include "WXApp.h"
#include "WXAwrapper.h"
#include "MainLoopUI.h"
#include "StringUtility.h"

struct SendMsgInfo 
{
	WXAwrapper*		pThis;
	std::string		id;
	std::wstring	msg;
};

HWND g_Hwnd = NULL;     //用于回调

void GSendResultMsg(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    ::SendMessage(g_Hwnd,uMsg,wParam,lParam);
}

void GPostResultMsg(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    ::PostMessage(g_Hwnd,uMsg,wParam,lParam);
}

void CheckAsynCallback(HttpClientHelper::HttpClientRespond& repond,DWORD_PTR dwContext)
{	OutputDebugStringA("RequstCallback\r\n");
    std::string body;
    repond.GetBody(body);
    if(1)
    {
		OutputDebugStringA("CheckAsyn----->Recv\r\n");
      
        GPostResultMsg(UM_MSG_DO_DORECVMSG,0,0);
    }
    else
    {
		OutputDebugStringA("CheckAsyn----->CheckAsynCallback\r\n");

        if(body.find("6") != -1)
        {
            OutputDebugStringA(".....");
        }

        GPostResultMsg(UM_MSG_DO_CHECKASYN,0,0);
    }
    
}

//这一层回调应该重写提供，不应该提高底层回调
bool RecvCallback(const std::vector<MsgInfo>& msgList)
{

   // WXAwrapper::GetInstance().SendResultMsg(UM_MSG_RESULT_QRCODE,UM_MSG_RESULT_RECVMSGLIST)
    //继续发送请求检测，如果没有异常
    GSendResultMsg(UM_MSG_RESULT_RECVMSGLIST,(WPARAM)&msgList,0);		//Internet回调是在DLL会创建线程,里面回调。
    //WXAwrapper::GetInstance().CheckAsyn();
    return true;
}

WXAwrapper::WXAwrapper()
{
	m_WxObj = new WXApp();
}

WXAwrapper::~WXAwrapper()
{
	delete m_WxObj; m_WxObj = NULL;
}

bool WXAwrapper::Init( HWND hw )
{
	m_hWnd = hw;
	m_EventLoop.Init(500);
	m_CheckAsyncLoop.Init(100);
    g_Hwnd = hw;
	return true;
}

void WXAwrapper::Uinit()
{
	m_EventLoop.UnInit();
	m_CheckAsyncLoop.UnInit();
}

bool WXAwrapper::Login()
{
	return m_EventLoop.PostEvent(LoginCallBack,this);
}


bool WXAwrapper::LoginInfo()
{
	return m_EventLoop.PostEvent(LoginInfoCallBack,this);
}

bool WXAwrapper::ContactList()
{
	return m_EventLoop.PostEvent(ContactListCallBack,this);
}

void WXAwrapper::CheckAsyn()
{
	//丢到专门线程
	m_CheckAsyncLoop.PostEvent(CheckAsynCallback,this);
}

//主线程异步下载，并不丢到
bool WXAwrapper::DownloadUserIcon()
{
	return true;
}

void WXAwrapper::Recv()
{
	//丢到专门线程
	m_CheckAsyncLoop.PostEvent(RecvCallback,this);
}

bool WXAwrapper::SendTextMsg( std::string id,std::wstring msg )
{
	SendMsgInfo* Info =new SendMsgInfo();
	Info->id = id;
	Info->pThis = this;
	Info->msg = msg;
	return m_EventLoop.PostEvent(SendTextMsgCallBack,Info);
}

void WXAwrapper::LoginInternal()
{
	bool bRet = false; 
	bRet = m_WxObj->login();
	PostResultMsg(UM_MSG_RESULT_QRCODE,bRet ? 1:0);
}

void WXAwrapper::LoginCallBack( void* lParam )
{
	WXAwrapper* pThis = (WXAwrapper*)lParam;
	pThis->LoginInternal();
}

void WXAwrapper::PostResultMsg( UINT uMsg,WPARAM wParam,LPARAM lParam )
{
	::PostMessage(m_hWnd,uMsg,wParam,lParam);
}


void WXAwrapper::SendResultMsg( UINT uMsg,WPARAM wParam /*= 0*/,LPARAM lParam /*= 0*/ )
{
    ::SendMessage(m_hWnd,uMsg,wParam,lParam);
}

void WXAwrapper::LoginInfoInternal()
{
	//测试代码
	bool bRet = false; 
	std::string url = m_WxObj->requestLoginInfo();

	std::string showInfo = "显示内容URL:";
	showInfo += url;
	::OutputDebugStringA(showInfo.data());

	if(url.length())
	{
		bRet = m_WxObj->requestLoinPage(url);
	}

	if(bRet)
	{
		bRet = m_WxObj->WXInit();
	}

	PostResultMsg(UM_MSG_RESULT_LOGIN,bRet ? 1:0);
}


void WXAwrapper::ContactListInternal()
{
	bool bRet = false; 
	bRet = m_WxObj->requestContactList();
	std::string* pStr = new std::string();
	m_WxObj->GetUserContactList(*pStr);
	PostResultMsg(UM_MSG_RESULT_CONTACKLIST,bRet ? 1:0,(LPARAM)pStr);
}

void WXAwrapper::SendTextMsgInternal(std::string id,std::wstring msg)
{
	bool bRet = false; 
	std::string utf8Msg = StringUtility::WStringToUtf8(msg);
	bRet = m_WxObj->SendMsg(id,utf8Msg);
}

void WXAwrapper::RecvInternal()
{
	Json::Value* msg = new Json::Value();
	m_WxObj->RecvMsg(*msg);
	if(!msg->empty())
	{
		PostResultMsg(UM_MSG_RESULT_RECVMSGLIST,(WPARAM)msg);
	}
}

void WXAwrapper::CheckAsynInternal()
{
	int nResult;
	bool bRet = m_WxObj->RequestAysnCheck(nResult);
	if(nResult)
	{
		RecvInternal();
		CheckAsyn();
	}
	else
	{
		CheckAsyn();	//重新丢到线程读取检测
	}
}


void WXAwrapper::LoginInfoCallBack( void* lParam )
{
	WXAwrapper* pThis = (WXAwrapper*)lParam;
	pThis->LoginInfoInternal();
}

void WXAwrapper::ContactListCallBack( void* lParam )
{
	WXAwrapper* pThis = (WXAwrapper*)lParam;
	pThis->ContactListInternal();
}

void WXAwrapper::SendTextMsgCallBack( void* lParam )
{
	SendMsgInfo* pInfo = (SendMsgInfo*)(lParam);
	pInfo->pThis->SendTextMsgInternal(pInfo->id,pInfo->msg);
}

void WXAwrapper::RecvCallback(void* lParam)
{
	WXAwrapper* pThis = (WXAwrapper*)lParam;
	pThis->RecvInternal();
}

void WXAwrapper::CheckAsynCallback(void* lParam)
{
	WXAwrapper* pThis = (WXAwrapper*)lParam;
	pThis->CheckAsynInternal();
}

Json::Value WXAwrapper::GetSelfUserInfo()
{
	return m_WxObj->GetSelfUserInfo();
}





