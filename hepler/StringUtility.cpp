#include "StringUtility.h"
#include <Windows.h>

std::wstring StringUtility::AToWString( std::string str )
{
	std::wstring strOut;
	int num = MultiByteToWideChar(CP_ACP,0,str.data(),-1,NULL,0);  
	num -=1;	//std 不需要终止符号
	strOut.resize(num);

	MultiByteToWideChar(CP_ACP,0,str.data(),-1,(LPWSTR)strOut.data(),num);  
	return strOut;
}

std::string StringUtility::WToAString( std::wstring str )
{
	std::string strOut;
	int num = WideCharToMultiByte(CP_ACP,0,str.data(),-1,NULL,0,NULL,0);
	num -=1;	//std 不需要终止符号
	strOut.resize(num);

	WideCharToMultiByte(CP_ACP,0,str.data(),-1,(LPSTR)strOut.data(),num,NULL,0);  
	return strOut;
}

std::wstring StringUtility::Utf8ToWString( std::string str )
{
	std::wstring strOut;
	int num = MultiByteToWideChar(CP_UTF8,0,str.data(),-1,NULL,0); 
	num -=1;	//std 不需要终止符号
	strOut.resize(num);
	MultiByteToWideChar(CP_UTF8,0,str.data(),-1,(LPWSTR)strOut.data(),num);
	return strOut;
}

std::string StringUtility::Utf8ToString( std::string str )
{
	std::wstring strWide = Utf8ToWString(str);
	return WToAString(strWide);
}

std::string StringUtility::WStringToUtf8( std::wstring str )
{
	std::string strOut;
	int num = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, 0);
	num -=1;	//std 不需要终止符号
	strOut.resize(num);
	::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, (LPSTR)strOut.data(), num, NULL, 0);
	return strOut;
}


std::string StringUtility::StringToUtf8( std::string str )
{
	return WStringToUtf8(AToWString(str));
}

int StringUtility::WSTringToInt( std::wstring str )
{
	int n = 0;
	n = _wtoi(str.data());
	return n;
}

int StringUtility::StringToInt( std::string str )
{
	int n = 0;
	n = atoi(str.data());
	return n;
}

std::string StringUtility::IToString( int n )
{
	char buf[256] = {0};
	itoa(n,buf,10);
	return std::string(buf);
}

std::wstring StringUtility::IToWString( int n )
{
	wchar_t buf[256] = {0};
	_itow(n,buf,10);
	return std::wstring(buf);
}

INT64 StringUtility::WStringToInt64( std::wstring str )
{
	INT64 i;
	i = _wtoi64(str.data());
	return i;
}

INT64 StringUtility::StringToInt64( std::string str )
{
	INT64 i;
	i = _atoi64(str.data());
	return i;
}

std::string StringUtility::I64ToString( INT64 n )
{
	char buf[256];
	return std::string(_i64toa(n,buf,256));
}

std::wstring StringUtility::I64ToWString( INT64 n )
{
	wchar_t buf[256];
	return std::wstring(_i64tow(n,buf,256));
}

long StringUtility::StringToLong( std::wstring str )
{
	long n = 0;
	n = _wtol(str.data());
	return n;
}

long StringUtility::StringToLong( std::string str )
{
	int n = 0;
	n = atol(str.data());
	return n;
}

std::string StringUtility::LToString( long n )
{
	char buf[256] = {0};
	ltoa(n,buf,10);
	return std::string(buf);
}

std::wstring StringUtility::LToWString( long n )
{
	wchar_t buf[256] = {0};
	_ltow(n,buf,10);
	return std::wstring(buf);
}

void StringUtility::ReplaceAll( std::string& src,std::string oldValue,std::string newValue )
{
	size_t beginPos = 0;
	size_t findPos = -1;
	while(true)
	{
		findPos = src.find(oldValue,beginPos);
		if(findPos != std::string::npos)
		{
			src.replace(findPos,oldValue.length(),newValue);
			beginPos = findPos + newValue.length();	//排除对原来进行检测
			continue;
		}

		break;
	}
}

void StringUtility::ReplaceAll( std::wstring& src,std::wstring oldValue,std::wstring newValue )
{
	size_t beginPos = 0;
	size_t findPos = -1;
	while(true)
	{
		findPos = src.find(oldValue,beginPos);
		if(findPos != std::wstring::npos)
		{
			src.replace(findPos,oldValue.length(),newValue);
			beginPos = findPos + newValue.length();	//排除对原来进行检测
			continue;
		}

		break;
	}
}

