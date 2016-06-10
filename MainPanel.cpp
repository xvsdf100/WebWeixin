#include "stdafx.h"
#include "MainPanel.h"
#include "StringUtility.h"
#include "WXAwrapper.h"
#include "UserICon.h"
#include "MainLoopUI.h"
#include "Path.h"

#define CHECK_MSG(SRCNAME,DSTNAME,FUNC)	\
	if(SRCNAME == DSTNAME)	\
	{\
		FUNC(msg);\
		return;\
	}

#define UM_TIMER_ASYN	WM_USER + 1

std::string ReadJsonFile(CDuiString strJsonFile)
{
	CDuiString strPath = CPaintManagerUI::GetInstancePath() + strJsonFile;
	std::string strNewPath = StringUtility::WToAString(strPath.GetData());
	FILE* fp = fopen(strNewPath.data(),"r");
	assert(NULL != fp);
	char buf[1024*8] = {0};
	size_t num = 0;
	std::string strUtf8;
	do 
	{
		num = fread(buf,1,1024*8,fp);
		strUtf8.append(buf,num);
	} while (num);
	fclose(fp);
	fp = NULL;

	return strUtf8;
}
MainPannel::MainPannel()
{
	m_bInitFiendList = false;
}

MainPannel::~MainPannel()
{

}

DUI_BEGIN_MESSAGE_MAP(MainPannel,WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnListItemSelect)
DUI_ON_MSGTYPE(DUI_MSGTYPE_RETURN,OnReturn)
DUI_END_MESSAGE_MAP()


void MainPannel::InitWindow()
{


	/*
	本地测试用
	*/

#ifdef LOCAL_TEST
	std::string friendList = ReadJsonFile(L"ContactList.txt");
	UpdateFriendList(friendList);
#endif

#ifndef LOCAL_TEST
	//更新用户名

	//自动检测
	StartAsyn();
#endif


}

LRESULT MainPannel::OnClose( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	PostQuitMessage(0);
	return TRUE;
}

LRESULT MainPannel::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	switch(uMsg)
	{
	case WM_TIMER:
		OnTimer(wParam,lParam);
		break;
	}
	return TRUE;
}

void MainPannel::OnClick( TNotifyUI& msg )
{
	CDuiString name = msg.pSender->GetName();
	CHECK_MSG(name,L"sendBtn",OnSendMsg);
	CHECK_MSG(name,L"close_right_btn",OnCloseBtn);
	return __super::OnClick(msg);
}

CControlUI* MainPannel::GetControlUI( CDuiString name )
{
	CControlUI* pUI = m_PaintManager.FindControl(name);
	ASSERT(NULL != pUI);
	return pUI;
}

CListUI* MainPannel::GetFriendListUI()
{
	return dynamic_cast<CListUI*>(GetControlUI(L"FriendList"));
}

CListUI* MainPannel::GetChatViewListUI( std::string id )
{
	CContainerUI* container = GetChatView(id);
	CListUI* ui = NULL;
	if(NULL != container)
	{
		ui = dynamic_cast<CListUI*>(container->FindSubControl(L"chatlist"));
	}
	return ui;
}


CContainerUI* MainPannel::GetChatContainerUI()
{
	return dynamic_cast<CContainerUI*>(GetControlUI(L"ChatContainer"));
}

CControlUI* MainPannel::GetNoSessionLaoutUI()
{
	return GetControlUI(L"noSession_layout");
}

CLabelUI* MainPannel::GetChatViewTalkerUI(CContainerUI* container)
{
	CLabelUI* ui = dynamic_cast<CLabelUI*>(container->FindSubControl(L"talkerName"));
	ASSERT(NULL != ui);
	return ui;
}


CRichEditUI* MainPannel::GetChatViewInput( std::string id )
{
	CContainerUI* container = GetChatView(id);
	CRichEditUI* ui = dynamic_cast<CRichEditUI*>(container->FindSubControl(L"input_richedit"));
	ASSERT(NULL != ui);
	return ui;
}

void MainPannel::UpdateFriendList( std::string& friendList )
{
   ToJsonObj(friendList,m_FriendList);
   UpdateFriendListUI();
   StartDownloadIcon();
}

void MainPannel::UpdateFriendICon( std::string strUserID,std::wstring strPath )
{
	CControlUI* pIt = GetFriendListItemIconUI(strUserID);
	if(NULL != pIt)
	{
		pIt->SetBkImage(strPath.c_str());
	}
}

void MainPannel::UpdateChatViewItemICon( std::string strUserID,std::wstring strPath )
{
	CListUI* pChatList = GetChatViewListUI(strUserID);
	if(NULL != pChatList)
	{
		for(int n = 0; n < pChatList->GetCount(); n++)
		{
			CContainerUI* pIt = (CContainerUI*)pChatList->GetItemAt(n);
			CDuiString strName = pIt->GetName();
			CControlUI* HeadIcon = pIt->FindSubControl(L"HeadIcon");
			HeadIcon->SetBkImage(strPath.c_str());
		}
	}
}



void MainPannel::ToJsonObj( std::string& friendList,Json::Value& jsonObj )
{
    Json::Reader reader;
    reader.parse(friendList,jsonObj);
}

Json::Value MainPannel::GetFriendListJson( std::string id )
{
	Json::Value it;
	for(size_t n = 0; n < m_FriendList["MemberList"].size(); n++)
	{
		it = m_FriendList["MemberList"][n];
		if(it["UserName"] == id)
		{
			break;
		}
	}

	return it;
}

void MainPannel::UpdateFriendListUI()
{
    if(!m_FriendList.empty())
    {
        CListUI* plist = GetFriendListUI();
		if(!m_bInitFiendList)
		{
			Json::Value memberListJson = m_FriendList["MemberList"];
			if(!memberListJson.empty())
			{
				for(size_t n = 0; n < memberListJson.size(); n++)
				{
					Json::Value memberInfo = memberListJson[n];
					InsertByJsonInfo(memberInfo);
				}
			}
		}
        
    }
}

void MainPannel::InsertByJsonInfo( Json::Value& jsonObj )
{
	ASSERT(!jsonObj.empty());
	std::wstring wName = StringUtility::Utf8ToWString(jsonObj["NickName"].asString());
	std::wstring uiName = FriendListItemUIClass(jsonObj["UserName"].asString());

	CListUI* list = this->GetFriendListUI();
	CListContainerElementUI* pItem = new CListContainerElementUI();
	pItem->SetAttribute(L"height",L"32");
	pItem->SetName(uiName.data());
	list->Add(pItem);
	CChildLayoutUI* ChildLayout = new CChildLayoutUI();
	ChildLayout->SetChildLayoutXML(L"Main\\UserListItem.xml");
	pItem->Add(ChildLayout);
	CControlUI* contactListFriend = ChildLayout->FindSubControl(L"name");
	contactListFriend->SetText(wName.data());
	
}

std::wstring MainPannel::FriendListItemUIClass( std::string id )
{
	std::string className = "FriendItem_" + id;
	return StringUtility::AToWString(className);
}


std::wstring MainPannel::ChatViewUIClass( std::string id )
{
	std::string className = "ChatView_" + id;
	return StringUtility::AToWString(className);
}


std::string MainPannel::GetIDFromUIClass( std::wstring name )
{
	std::string ascllName = StringUtility::WToAString(name);
	size_t nPos = ascllName.find("_");
	ASSERT(std::string::npos != nPos);
	nPos++;
	return ascllName.substr(nPos);
}

void MainPannel::OnListItemSelect( TNotifyUI& msg )
{
	CDuiString name = msg.pSender->GetName();
	int npos = name.Find(L"FriendItem");
	if(name.Find(L"FriendItem") != -1)
	{
		OnFriendListSelect(msg);
	}
	
}

void MainPannel::OnFriendListSelect( TNotifyUI& msg )
{
	//判断类名做响应
	CDuiString strName = msg.pSender->GetName();
	if(msg.pSender->GetName())
	{
		std::string id = GetIDFromUIClass(strName.GetData());
		UpdateChatView(id);
	}
}


void MainPannel::OnSendMsg( TNotifyUI& msg )
{
	CListUI* pList = GetFriendListUI();
	int nIndex = pList->GetCurSel();
	CControlUI* pFriendList = pList->GetItemAt(nIndex);
	CDuiString name = pFriendList->GetName();
	std::string id = GetIDFromUIClass(name.GetData());
	CRichEditUI* inputUI = GetChatViewInput(id);
	CDuiString strInputMsg = inputUI->GetText();
	CreateNormaTextMsg(id,strInputMsg.GetData());
	inputUI->Clear();
	inputUI->SetText(L"");
	SendNormalTextMsg(id,strInputMsg.GetData());
}

void MainPannel::OnCloseBtn( TNotifyUI& msg )
{
	PostQuitMessage(0);
}

void MainPannel::UpdateChatView( std::string id,bool bShow )
{
	GetNoSessionLaoutUI()->SetVisible(false);
	CChildLayoutUI* pChatView = GetChatView(id);
	if(NULL == pChatView)	
	{
		pChatView = CreateChatView(id);
	}
	else
	{
		CContainerUI* pContainer = GetChatContainerUI();
		SetChildContainerVisit(pContainer,false);
		pChatView->SetVisible(true);
	}
	pChatView->SetVisible(bShow);
}

CChildLayoutUI* MainPannel::GetChatView( std::string id )
{
	CChildLayoutUI* pChatView = NULL;
	CContainerUI* pContainer = GetChatContainerUI();
	std::wstring name = ChatViewUIClass(id);
	CControlUI* item = pContainer->FindSubControl(name.data());
	if(NULL != item)
	{
		pChatView = dynamic_cast<CChildLayoutUI*>(item);
	}
	return pChatView;
}

CChildLayoutUI* MainPannel::CreateChatView( std::string id)
{
	CChildLayoutUI* pChatView = new CChildLayoutUI();
	pChatView->SetChildLayoutXML(L"Main\\Chat.xml");
	std::wstring wName = ChatViewUIClass(id);
	pChatView->SetName(wName.data());
	CContainerUI* pContainer = GetChatContainerUI();
	pContainer->Add(pChatView);

	InitChateView(id,pChatView);
	return pChatView;
}

void MainPannel::SetChildContainerVisit( CContainerUI* ui,bool bVisit )
{
	for(int n = 0; n < ui->GetCount(); n++)
	{
		CControlUI * pIem = ui->GetItemAt(n);
		pIem->SetVisible(false);
	}
}

void MainPannel::InitChateView( std::string id,CChildLayoutUI* pChatView )
{
	ASSERT(NULL != pChatView);
	
	Json::Value it = GetFriendListJson(id);
	if(!it.empty())
	{
		CLabelUI* labelUI = GetChatViewTalkerUI(pChatView);
		std::wstring strText = StringUtility::Utf8ToWString(it["NickName"].asString());
		labelUI->SetText(strText.data());
	}
}


void MainPannel::UpdateUserBar()
{
	Json::Value UserInfo = WXAwrapper::GetInstance().GetSelfUserInfo();
	std::string NickName = UserInfo["NickName"].asString();
	std::wstring wNickName = StringUtility::Utf8ToWString(NickName);
	GetMainNameUI()->SetText(wNickName.c_str());
}


CChildLayoutUI* MainPannel::CreateNormaTextMsg(std::string id, std::wstring msg,bool bLeft /*= true*/ )
{
	//msgid要生成，对消息进行删除，撤回都可以
	const std::wstring xmlFiles[] = {L"Chat\\ChatBubbleLeft.xml",L"Chat\\ChatBubbleRight.xml"};
	std::wstring xmlFile = bLeft ? xmlFiles[0] : xmlFiles[1];
	CChildLayoutUI* chatItemLaout = new CChildLayoutUI();

    //简单计算高度
    int n = msg.length();
    int nLine = ( (n % 15) ? 0 : 1 )*1 + n / 15;
    int height = nLine*34;
    int width = 0;
    GetBubblePos(msg,height,width);
    height=25;
    nLine = width / 98 + (width % 98 ? 1 : 0)*1;
    nLine = nLine ? nLine : 1;
    height = height * nLine;

    


	CListContainerElementUI* pItem = new CListContainerElementUI();
    pItem->SetFixedHeight(height + 35);
	CListUI* list = this->GetChatViewListUI(id);
	n = list->GetCount();
	list->Add(pItem);

    chatItemLaout->SetManager(&m_PaintManager,pItem,false);
    chatItemLaout->SetChildLayoutXML(xmlFile.data());
    //chatItemLaout->Init();
	pItem->Add(chatItemLaout);
	
	//CControlUI* pEdit = chatItemLaout->FindSubControl(L"chatContent");
     //int height = pEdit->GetFixedHeight();
    CRichEditUI* pEdit = ( CRichEditUI*)chatItemLaout->FindSubControl(L"chatContent");
    CControlUI* pLayout = chatItemLaout->FindSubControl(L"chatContentLayout");
	CControlUI* pHeadIcon = chatItemLaout->FindSubControl(L"HeadIcon");
	//图标的最好的解决办法是动态实现网络图像

	//找到自己的照片
	std::string rightID;
	if(!bLeft)
	{
		rightID = id;
	}
	else
	{
		Json::Value UserInfo = WXAwrapper::GetInstance().GetSelfUserInfo();
		if(!UserInfo.empty())
		{
			rightID = UserInfo["UserName"].asString();
		}
	}

	std::wstring strHeadPicPath = UserICon::GetInstance().GetPicPath(rightID);
	if(strHeadPicPath.length())
	{
		pHeadIcon->SetBkImage(strHeadPicPath.c_str());
	}
	

	pEdit->SetText(msg.data());

    pLayout->SetFixedHeight(height + 25);    //简单实现自动适应大小，对宽度没有跳转。暂时先这样以后再桌修正
    width = pLayout->GetFixedWidth();
    width = pLayout->GetWidth();
        
	//添加消息，滚动到下面
    RECT rc = list->GetPos();   //duilib bug 对于自定义item height大小如果直接调用EndDown会导致不会滚动到底部。
    list->SetPos(rc,false);     //因为没有list->Add 没有更新滚动条的信息，所以我直接调用setPost来更新位置
	list->EndDown();

	return chatItemLaout;
}

void MainPannel::SendNormalTextMsg(std::string id, std::wstring msg )
{
#ifndef LOCAL_TEST
	WXAwrapper::GetInstance().SendTextMsg(id,msg);
#endif
	
}

void MainPannel::StartAsyn()
{
#ifndef LOCAL_TEST
	WXAwrapper::GetInstance().CheckAsyn();
#endif
}


void MainPannel::StartDownloadIcon()
{
	std::wstring wPath = Path::GetCurrentPath();
	wPath += L"ICon";

	if(!CreateDirectory(wPath.c_str(),NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		::MessageBox(m_hWnd,L"创建ICON文件失败",0,0);
		return;
	}

	HWND hWnd = MainLoopUI::GetInstance().GetHWND();
	UserICon::GetInstance().Init(hWnd,wPath,m_FriendList);
	UserICon::GetInstance().StartDownload();
}


void MainPannel::OnTimer(WPARAM wParam,LPARAM lParam)
{
	/*static UINT uTestNum = 0;
	UINT uId = (UINT)wParam;
	if(uId == UM_TIMER_ASYN && uTestNum <= 20)
	{
		::OutputDebugString(L"接收到消息");
		CheckAsyn();
		uTestNum++;
	}*/
}

void MainPannel::CheckAsyn()
{

}

bool MainPannel::IsChatView(std::string id)
{
	CChildLayoutUI* pChatView = GetChatView(id);
	bool bShow = false;
	if(NULL != pChatView)
	{
		bShow = pChatView->IsVisible();
	}
	return bShow;
}

//根据内容获取气泡的大小
void MainPannel::GetBubblePos( const std::wstring& str,int& height,int& width,bool bLeft /*= true*/ )
{
    HDC dc = GetDC(m_hWnd);//暂时用这个窗口获取。
    DWORD dw = GetTabbedTextExtent(dc,str.data(),str.length(),0,NULL);
    short h = HIWORD(dw);//(short)(dw >> 16);
    short w = LOWORD(dw);//(short) (dw << 16);
    height = h;
    width = w;
    ReleaseDC(m_hWnd,dc);
}


LRESULT MainPannel::ResponseDefaultKeyEvent(WPARAM wParam)
{
	return __super::ResponseDefaultKeyEvent(wParam);
}

void MainPannel::OnReturnKey()
{
	TNotifyUI msg;
	OnSendMsg(msg);

    /*
       测试代码用来触发获取消息
    */

    //WXAwrapper::GetInstance().Recv();
}

void MainPannel::InsertNewMsg( std::string id,std::wstring msg,bool bLeft /*= true*/ )
{
    //获取到指定消息

    //这里要过滤掉
    //刷新到指定界面
    //创建对应view
    CControlUI* pLastFocus = m_PaintManager.GetFocus();
    //保留最后focus,新加入的control 会获取新的focus
    bool bShow = IsChatView(id);
    UpdateChatView(id,bShow);	//这里和点击接收不一样
    CreateNormaTextMsg(id,msg,bLeft);
    if(NULL != pLastFocus)
    {
          pLastFocus->SetFocus();
    }
  
}

void MainPannel::OnReturn( TNotifyUI& msg )
{
	CDuiString strName = msg.pSender->GetName();
	if(strName == L"input_richedit")
	{
		OnReturnKey();
	}
	
}

CContainerUI* MainPannel::GetFriendListItemUI( std::string id )
{
	CListUI* pFriendList = GetFriendListUI();
	std::wstring wRealID = FriendListItemUIClass(id);
	return  (CContainerUI*)pFriendList->FindSubControl(wRealID.c_str());
}

CControlUI* MainPannel::GetFriendListItemIconUI( std::string id )
{
	CControlUI* pUI = NULL;
	CContainerUI* pItemContainer = GetFriendListItemUI(id);
	if(NULL != pItemContainer)
	{
		pUI =  pItemContainer->FindSubControl(L"ico");
	}

	return pUI;
}

CControlUI* MainPannel::GetMainNameUI()
{
	return GetControlUI(L"main_name");
}



