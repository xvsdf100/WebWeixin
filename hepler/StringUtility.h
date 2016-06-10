#ifndef STRING_UTILITY_H
#define STRING_UTILITY_H

/************************************************************************/
/* 说明：增加std字符串处理功能                                          */
/* 作者：xjm															*/
/* 时间：2015年12月19日 21:16:55										*/
/************************************************************************/
#include <string>
#include <Windows.h>

class StringUtility
{
public:
	static std::wstring		AToWString(std::string str);
	static std::string		WToAString(std::wstring str);
	static std::wstring		Utf8ToWString(std::string str);
	static std::string		Utf8ToString(std::string str);
	static std::string		WStringToUtf8(std::wstring str);
	static std::string		StringToUtf8(std::string str);

	static int				WSTringToInt(std::wstring str);
	static int				StringToInt(std::string str);
	static long				StringToLong(std::wstring str);
	static long				StringToLong(std::string str);
	static INT64			WStringToInt64(std::wstring str);
	static INT64			StringToInt64(std::string str);
	static std::string		IToString(int n);
	static std::wstring		IToWString(int n);
	static std::string		LToString(long n);
	static std::wstring		LToWString(long n);			
	static std::string		I64ToString(INT64 n);
	static std::wstring		I64ToWString(INT64 n);
	
	static void				ReplaceAll(std::string& src,std::string oldValue,std::string newValue);
	static void				ReplaceAll(std::wstring& src,std::wstring oldValue,std::wstring newValue);
};
#endif