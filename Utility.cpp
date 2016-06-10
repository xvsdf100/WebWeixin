#include "Utility.h"

std::string GetTagValue( const std::string& str, const std::string& TagName )
{
	std::string ret;
	std::string StartTag = "<" + TagName + ">";
	std::string EndTag = "</" + TagName + ">";

	size_t nPos = str.find(StartTag);
	if(nPos != std::string::npos)
	{
		size_t nEndPos = str.find(EndTag,nPos);
		nPos += StartTag.length();
		ret = str.substr(nPos,nEndPos - nPos);
	}

	return ret;
}

bool simpleWrite( std::wstring strFilePath,char* pBuf,int Len )
{
	FILE* fp = _wfopen(strFilePath.c_str(),L"wb");
	if(NULL == fp)	return false;

	int nWrite = fwrite(pBuf,sizeof(char),Len,fp);
	fclose(fp);
	return nWrite ? true : false;
}
