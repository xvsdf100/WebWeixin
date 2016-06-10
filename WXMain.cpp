// WX.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WXMain.h"
#include "MainLoopUI.h"
#include "WXAwrapper.h"
#include "MainPanel.h"
#include "HttpClientHelper.h"
#include "UserICon.h"

void Test()
{
	MainPannel ui;
	ui.CreateDuiWindow(NULL,L"test");
	ui.CenterWindow();
	ui.ShowWindow();
	CPaintManagerUI::MessageLoop();
	//WXAwrapper::GetInstance().Uinit();线程有BUG

	::CoUninitialize();
}


void TestAsynHttp();
void bingHttp(HttpClientHelper::HttpClientRespond& repond,DWORD_PTR dwContext)
{
	std::string str = (char*)dwContext;
	OutputDebugString(L"请求ok:\r\n");
    TestAsynHttp();
}

void TestAsynHttp()
{
    HttpClientHelper* http = new HttpClientHelper("192.168.12.103",6000,true);
    HttpClientHelper::RequstCallback CallBack = bingHttp;
    std::string str = "RequestBing";
    http->Get("/",CallBack,(DWORD_PTR)http,NULL,NULL,0);
}

#include <time.h>
#include <sstream>

void TestHttp()
{
    HttpClientHelper http("wx.qq.com",80);
    http.Get("/");
}
void TestAysnCheck()
{
	//内部
    TestHttp();
    TestHttp();

	static UINT Req = 1000;
	HttpClientHelper http("192.168.12.103",60000,true);
	HttpClientHelper::RequstCallback CallBack = bingHttp;
	std::string str = "/cgi-bin/mmwebwx-bin/synccheck?r=";
	time_t tm;
	tm = ::time(0);
	std::stringstream strStream;
	strStream << tm;
	std::string r;
	strStream >> r;
	str += r;
	str += "&skey=";
	str += "@crypt_50631e16_39151f05f931c4906030f7b3387b612";
	str += "&sid=";
	str += "2326085821";
	str += "&uin=";
	str += "2326085821";
	str += "&deviceid=e599146929270706";
	str += "&synckey=";
	str += "1_637187967|2_6371880213_6371878571000_1455530999";
	strStream.clear();
	strStream << Req;
	str += "&_=";
	str += strStream.get();
	http.Get(str,CallBack,(DWORD_PTR)NULL);
	CPaintManagerUI::MessageLoop();	//进行消息循环
}

void AsynDownloadPic()
{
	UserICon::GetInstance().StartDownload();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);



	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("../res"));

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;


    //快速测试
#ifdef LOCAL_TEST
	Test();
	return 0;
#endif

	/*TestAsynHttp();
    CPaintManagerUI::MessageLoop();
	//return 0;*/

	/*AsynDownloadPic();
	CPaintManagerUI::MessageLoop();
	return 0;*/

	//TestAysnCheck();
	//return 0;
	/*MainLoopUI::GetInstance().CreateDuiWindow(NULL,L"MsgLoop",WS_POPUP,WS_EX_TOOLWINDOW);
	MainLoopUI::GetInstance().ShowWindow();
	CPaintManagerUI::MessageLoop();
	return 0;*/

	MainLoopUI::GetInstance().CreateDuiWindow(NULL,L"MsgLoop",WS_POPUP,WS_EX_TOOLWINDOW);
	MainLoopUI::GetInstance().ShowWindow();

	CPaintManagerUI::MessageLoop();
	WXAwrapper::GetInstance().Uinit();

	::CoUninitialize();
	return 0;
}
