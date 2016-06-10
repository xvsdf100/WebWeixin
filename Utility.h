#ifndef UTILITY_H
#define UTILITY_H

#include <string>

std::string GetTagValue(const std::string& str,const std::string& TagName);
#define SAFEDEL(X) if(NULL != X) delete X; X = NULL;
#define SAFADELARRAY(X) if(NULL != X) delete[] X; X = NULL;

bool simpleWrite(std::wstring strFilePath,char* pBuf,int Len);

#endif