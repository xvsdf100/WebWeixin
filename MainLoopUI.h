/************************************************************************/
/* 说明：整个程序的消息推送	                                            */
/************************************************************************/

#pragma once

#define UM_MSG_LOGIN	WM_USER + 1
#define UM_MSG_LOGOUT	WM_USER + 2


//WX异步消息通知
//WX异步消息通知
#define UM_MSG_EVENT_CORE				WM_USER + 999
#define UM_MSG_RESULT_QRCODE			WM_USER + 1000
#define UM_MSG_RESULT_LOGIN				WM_USER + 1001
#define UM_MSG_RESULT_CONTACKLIST		WM_USER + 1002
#define UM_MSG_RESULT_RECVMSGLIST       WM_USER + 1003
#define UM_MSG_DO_CHECKASYN             WM_USER + 1004
#define UM_MSG_DO_DORECVMSG             WM_USER + 1005
#define UM_MSG_DO_UPDATEICON			WM_USER + 1006

#define UM_MSG_UI_SHOWMAINPANNEL		WM_USER + 2000

class loginUI;
class MainPannel;
class MainLoopUI:public WindowImplBase
{
public:
	MainLoopUI(void);
	~MainLoopUI(void);

	static MainLoopUI& GetInstance()
	{
		static MainLoopUI loop;
		return loop;
	}


	virtual CDuiString GetSkinFolder() {return L"";}
	virtual CDuiString GetSkinFile() {return L"loop.xml";}
	virtual LPCTSTR GetWindowClassName(void) const {return L"MsgRun";}
	virtual void InitWindow();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);		//消息处理
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);


private:
	void	OnLoginMsg(WPARAM wParam,LPARAM lParam);
	void	OnLogoutMsg(WPARAM wParam,LPARAM lParam);

private:
	void	OnEventMsg(WPARAM wParam,LPARAM lParam);
	void	OnQRCodeMsg(WPARAM wParam,LPARAM lParam);
	void	OnLoginInfoMsg(WPARAM wParam,LPARAM lParam);
	void	OnContactListMsg(WPARAM wParam,LPARAM lParam);
    void    OnRecvMsg(WPARAM wParam,LPARAM lParam);
    void    OnDoCheckAsynMsg(WPARAM wParam,LPARAM lParam);
    void    OnDoRecvMsg(WPARAM wParam,LPARAM lParam);
	void	OnDoUpdateIconMsg(WPARAM wParam,LPARAM lParam);
private:
	void	ShowMainPannel(bool bShow = true);
	

private:
	loginUI*	m_Login;
	MainPannel*	m_MainPannel;
	std::string	m_ContactList;
	
};
