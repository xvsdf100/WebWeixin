#pragma once

class EventCallBack
{
public:
	typedef void (*AsynCallBack)(WPARAM wParam,LPARAM lParam);

public:
	EventCallBack(AsynCallBack fun);
	~EventCallBack(void);

	void	PostIt(HWND hw,UINT uMsg);		//投递对应的消息循环

public:
	void DoWork();
public:
	WPARAM			m_wParam;
	LPARAM			m_lParam;

private:
	AsynCallBack	m_CallBack;
};
