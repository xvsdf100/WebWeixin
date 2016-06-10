#include "StdAfx.h"
#include "loginUI.h"
#include "WXAwrapper.h"
#include "Path.h"
#define TIMER_ID_CHECKQRCODE	1

loginUI::loginUI(void)
{
}

loginUI::~loginUI(void)
{
}

void loginUI::InitWindow()
{
	WXAwrapper::GetInstance().Login();
}

CControlUI* loginUI::GetControlUI(CDuiString name)
{
	CControlUI* pUI = m_PaintManager.FindControl(name);
	ASSERT(NULL != pUI);
	return pUI;
}

CControlUI* loginUI::GetQRCodeUI()
{
	return GetControlUI(L"qrCode");
}

void loginUI::ShowQRCodePicture()
{
	CControlUI* pUI = GetQRCodeUI();
	std::wstring wCodePath = Path::GetCurrentPath();
	wCodePath += L"wxCode.jpg";
	pUI->SetBkImage(wCodePath.c_str());
}


void loginUI::StartCheckQRCodeTimer()
{
	::SetTimer(m_hWnd,TIMER_ID_CHECKQRCODE,1000,NULL);
}

void loginUI::StopCheckQRCodeTimer()
{
	::KillTimer(m_hWnd,TIMER_ID_CHECKQRCODE);
}

LRESULT loginUI::OnClose( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	PostQuitMessage(0);
	return TRUE;
}

LRESULT loginUI::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	switch(uMsg)
	{
	case WM_TIMER:
		OnTimerMsg(wParam,lParam);
		break;
	}

	return TRUE;
}

void loginUI::OnTimerMsg( WPARAM wParam,LPARAM lParam )
{
	UINT TimerID = (UINT)wParam;
	if(TIMER_ID_CHECKQRCODE == TimerID)
	{
		OnTimerQRCodeMsg();
		return;
	}
}

void loginUI::OnTimerQRCodeMsg()
{
	WXAwrapper::GetInstance().LoginInfo();
	::OutputDebugString(L"定时器调用显示检测是否成功");
}
