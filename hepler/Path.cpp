#include "Path.h"
#include <Windows.h>

Path::Path(const std::wstring& wPath)
{
	m_wPath = wPath;
}

Path::~Path(void)
{
}

std::wstring Path::GetCurrentPath()
{
	std::wstring wPath;
	wPath.resize(MAX_PATH);
	::GetModuleFileName(NULL,(wchar_t*)wPath.c_str(),MAX_PATH);
	std::wstring::size_type nPos = wPath.rfind(L"\\");
	if(nPos != std::wstring::npos)
	{
		wPath = wPath.substr(0,nPos + 1);
	}

	return wPath;
}

std::wstring Path::Combine(std::wstring wPath, std::wstring wName )
{
	if(!(wPath.length() && wName.length()))	return L"";

	int nLastPos = wPath.length() - 1;
	if(wPath.at(nLastPos) != L'\\')
	{
		wPath += L"\\";
	}
	
	return wPath + wName;
}
