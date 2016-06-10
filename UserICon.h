#pragma once
/************************************************************************/
/* 说明：用于管理用户图像                                               */
/************************************************************************/
#include <string>
#include "json/json.h"
#include "HttpClientHelper.h"

class UserICon
{
	friend void	IncoCallBack(HttpClientHelper::HttpClientRespond& repond,DWORD_PTR dwContext);
public:
	UserICon(void);
	~UserICon(void);

	struct UserIconInfo
	{
		bool bDownload;
		std::string strUrl;
		std::string	strUserID;
		int nDownloadErrorNum;

		UserIconInfo()
		{
			bDownload = false;
			nDownloadErrorNum = 0;
		}
	};

	

	typedef std::vector<UserIconInfo> UserInconArray;

	static UserICon& GetInstance()
	{
		static UserICon ins;
		return ins;
	}

	void	Init(HWND hw,std::wstring& strPath,const Json::Value& FriendList);
	void	StartDownload();
	void	StopDonload();
	bool	SaveIcon(std::string strUserID,char* pBuf,int Len);
	bool	UpdateIconDownload(std::string strUserID,bool bOK);
	std::wstring GetPicPath(std::string strUserID);

private:
	void	DownloadIcon(const UserIconInfo& info);
	void	PostUserIcontMsg();
	void	InitUserIcons(const Json::Value& FriendList);
	bool	GetCurrentDownloadIcon(UserIconInfo& info);
	bool	GetUserIconIt(std::string userID,UserIconInfo& Info);

private:
	UserInconArray	m_UserIcons;
	bool			m_Download;
	HWND			m_hWnd;
	std::wstring	m_SaveIconPath;
};

void	IncoCallBack(HttpClientHelper::HttpClientRespond& repond,DWORD_PTR dwContext);