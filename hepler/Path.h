#pragma once

#include <string>
class Path
{
public:
	Path(const std::wstring& wPath);
	~Path(void);

public:
	static std::wstring GetCurrentPath();
	static std::wstring Combine(std::wstring wPath, std::wstring wName);

private:
	std::wstring	m_wPath;

};



