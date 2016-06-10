#include "StdAfx.h"
#include "EventCallBack.h"
#include <assert.h>

EventCallBack::EventCallBack(AsynCallBack fun)
{
	assert(NULL != fun);
	m_CallBack = fun;
	m_lParam = NULL;
	m_wParam = NULL;
}

EventCallBack::~EventCallBack(void)
{
}

void EventCallBack::DoWork()
{
	assert(NULL != m_CallBack);
	m_CallBack(m_wParam,m_lParam);
	delete this;		//必须自我释放，这个一定要通过new ,后面把构造函数变成私有，只要另外一个友元函数访问
}

void EventCallBack::PostIt( HWND hw,UINT uMsg )
{
	PostMessage(hw,uMsg,0,(LPARAM)this);
}
