#pragma once
#include "ThreadHelper.h"
#include "WXApp.h"
#include <vector>

class WXAwrapper
{
public:
	WXAwrapper();
	~WXAwrapper();

public:
	static WXAwrapper& GetInstance() 
	{
		static WXAwrapper ins;
		return ins;
	}

	typedef std::vector<char>	ByteArray;
	
	bool	Init(HWND hw);	//这里传入一个回调对象效果还好一些，更加通用一点
	void	Uinit();

	bool	Login();
	bool	LoginInfo();
	bool	ContactList();
	bool	SendTextMsg(std::string id,std::wstring msg);
	void	Recv();
	void	CheckAsyn();
	bool	DownloadUserIcon();
	Json::Value GetSelfUserInfo();

private:
	void	LoginInternal();
	void	LoginInfoInternal();
	void	ContactListInternal();
	void	SendTextMsgInternal(std::string id,std::wstring msg);
	void	RecvInternal();
	void	CheckAsynInternal();



	static void	LoginCallBack(void* lParam);
	static void	LoginInfoCallBack(void* lParam);
	static void	ContactListCallBack(void* lParam);
	static void	SendTextMsgCallBack(void* lParam);
	static void RecvCallback(void* lParam);
	static void CheckAsynCallback(void* lParam);

private:
	void	PostResultMsg(UINT uMsg,WPARAM wParam = 0,LPARAM lParam = 0);
    void    SendResultMsg(UINT uMsg,WPARAM wParam = 0,LPARAM lParam = 0);


private:
	HWND				m_hWnd;
	ThreadLoopHelper	m_EventLoop;
	ThreadLoopHelper	m_CheckAsyncLoop;
	WXApp*				m_WxObj;

};