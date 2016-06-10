#pragma once

class loginUI:public WindowImplBase
{
public:
	loginUI(void);
	~loginUI(void);
	virtual CDuiString GetSkinFolder() {return L"";}
	virtual CDuiString GetSkinFile() {return L"login/login.xml";}
	virtual LPCTSTR GetWindowClassName(void) const {return L"loginui";}
	virtual void InitWindow();
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	void	ShowQRCodePicture();
	void	StartCheckQRCodeTimer();
	void	StopCheckQRCodeTimer();
private:
	void	OnTimerMsg(WPARAM wParam,LPARAM lParam);
	void	OnTimerQRCodeMsg();

private:
	CControlUI*	GetControlUI(CDuiString name);
	CControlUI*	GetQRCodeUI();

};
