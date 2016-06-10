#include "StdAfx.h"
#include "UserICon.h"
#include "MainLoopUI.h"
#include <vector>
#include "Utility.h"
#include "ByteStream.h"
#include "StringUtility.h"
#include "Path.h"


void SavePicture(HttpClientHelper::ByteArray& array)
{
	char Buf[100] = {0};
	sprintf(Buf,"array len:%d\r\n",array.size());
	OutputDebugStringA(Buf);
	FILE* fp = fopen("e:\\test.jpg","wb");
	assert(NULL != fp);
	int dataLen = array.size();
	char *pBuf = (char*)&array.at(0);
	int uWrited = 0;
	int uTotal = 0;

	uWrited = fwrite(pBuf,1,dataLen,fp);

	fclose(fp);
	fp = NULL;
}

void	IncoCallBack(HttpClientHelper::HttpClientRespond& repond,DWORD_PTR dwContext)
{
	//wininert异步是多线程的，所以在这个线程做任何事情，不会就线程竞争
	char* pUserID = (char*)dwContext;
	if(200 == repond.GetStatuCode())
	{
		HttpClientHelper::ByteArray array;
		repond.GetBody(array);
		if(array.size())
		{
			uint32_t UserIDLen = strlen(pUserID);
			assert(UserIDLen < 100);
			uint32_t PicLen = array.size();
			assert(PicLen < 20 * 1024 * 1024);
			uint32_t BufLen = UserIDLen + PicLen + sizeof(uint32_t)*2;
			char* pBuf = new char[BufLen];
			memset(pBuf,0,BufLen);

			ByteStream stream(pBuf,BufLen);
			stream.Write(UserIDLen);
			stream.Write(pUserID,UserIDLen);
			stream.Write(PicLen);
			stream.Write(&array[0],PicLen);
			//太原始的消息投递，如果不注意会导致内存释放或者访问错误的问题
			MainLoopUI::GetInstance().PostMessage(UM_MSG_DO_UPDATEICON,1,(LPARAM)pBuf);
			return;
		}
	}
	
	uint32_t UserIDLen = strlen(pUserID);
	uint32_t BufLen = UserIDLen;
	char* pBuf = new char[BufLen];

	ByteStream stream(pBuf,BufLen);
	stream.Write(UserIDLen);
	stream.Write(pUserID,UserIDLen);
	MainLoopUI::GetInstance().PostMessage(UM_MSG_DO_UPDATEICON,0,(LPARAM)pBuf);

	SAFEDEL(pUserID);
}



UserICon::UserICon(void)
{
}

UserICon::~UserICon(void)
{
}

void UserICon::Init( HWND hw,std::wstring& strPath,const Json::Value& FriendList )
{
	m_hWnd = hw;
	m_SaveIconPath = strPath;
	//解析头像数据解析出来
	InitUserIcons(FriendList);
}

void UserICon::StartDownload()
{
	UserIconInfo info;
	bool bRet = GetCurrentDownloadIcon(info);
	if(!bRet)	return;
	DownloadIcon(info);
}

void UserICon::StopDonload()
{

}

void UserICon::DownloadIcon( const UserIconInfo& info )
{
	std::string host = "wx.qq.com";
	HttpClientHelper* http = new HttpClientHelper(host,80,true);
	HttpClientHelper::HeadItems head;
	head["Connection"] = "keep-alive";
	char* pUserID = new char[info.strUserID.length() + 1];
	memset(pUserID,0,info.strUserID.length() + 1);
	info.strUserID.copy(pUserID,info.strUserID.length());
	HttpClientHelper::RequstCallback CallBack = (HttpClientHelper::RequstCallback)IncoCallBack;
	http->Get(info.strUrl,CallBack,(DWORD_PTR)pUserID,&head,NULL,0);
}

void UserICon::PostUserIcontMsg()
{

}

void UserICon::InitUserIcons(const Json::Value& FriendList)
{
	if(!FriendList["MemberList"].empty())
	{
		Json::Value::const_iterator it;
		for(it = FriendList["MemberList"].begin(); it != FriendList["MemberList"].end(); it++)
		{
			UserIconInfo info;
			info.strUrl = (*it)["HeadImgUrl"].asString();
			info.strUserID = (*it)["UserName"].asString();
			m_UserIcons.push_back(info);
		}
	}
}

bool UserICon::GetCurrentDownloadIcon( UserIconInfo& info )
{
	bool bRet = false;
	UserInconArray::const_iterator it;
	for(it = m_UserIcons.begin(); it != m_UserIcons.end(); it++)
	{
		if(!it->bDownload && it->nDownloadErrorNum < 3)
		{
			info = *it;
			bRet = true;
			break;
		}
	}
	return bRet;
}

bool UserICon::GetUserIconIt( std::string userID,UserIconInfo& Info )
{
	UserInconArray::iterator it;
	bool bRet = false;
	for(it = m_UserIcons.begin(); it != m_UserIcons.end(); it++)
	{
		if(it->strUserID == userID)
		{
			bRet = true;
			Info = *it;
			break;
		}
	}

	return bRet;
}

bool UserICon::SaveIcon( std::string strUserID,char* pBuf,int Len )
{
	std::wstring strFileName = m_SaveIconPath;
	strFileName = Path::Combine(strFileName,StringUtility::AToWString(strUserID));
	strFileName += L".jpg";

	return simpleWrite(strFileName,pBuf,Len);
}

bool UserICon::UpdateIconDownload( std::string strUserID,bool bOK )
{
	UserInconArray::iterator it;
	for(it = m_UserIcons.begin(); it != m_UserIcons.end(); it++)
	{
		if(it->strUserID == strUserID)
		{
			if(bOK)
			{

				it->bDownload = true;
			}
			else
			{
				it->nDownloadErrorNum++;
				assert(it->nDownloadErrorNum < 3);
			}

			return true;
		}
	}
	return false;
}

std::wstring UserICon::GetPicPath( std::string strUserID )
{
	std::wstring strFileName = m_SaveIconPath;
	strFileName = Path::Combine(strFileName,StringUtility::AToWString(strUserID));
	strFileName += L".jpg";
	return strFileName;
}


