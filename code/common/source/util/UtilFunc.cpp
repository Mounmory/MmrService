#include "common/include/util/UtilFunc.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>


#ifdef OS_MMR_WIN//windows
#include <Windows.h>
#include <codecvt>
#elif defined OS_MMR_LINUX//linux
#include <sys/stat.h>
#include <unistd.h>
#include <iconv.h>
#endif

#define MAX_STR_LEN 1024

COMMON_FUN_API std::string mmrUtil::getComputerID()
{
	return "undefined function";
}

bool mmrUtil::utf8ToLocalString(const std::string& strIn, std::string& strOut)
{
#ifdef OS_MMR_WIN
	try
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::wstring_convert<std::codecvt_byname<wchar_t, char, std::mbstate_t>> localConverter(new std::codecvt_byname<wchar_t, char, std::mbstate_t >(""));
		std::wstring wstr = converter.from_bytes(strIn);
		strOut = localConverter.to_bytes(wstr);
		return true;
	}
	catch (...)
	{
		strOut.clear();
		return false;
	}
#elif defined OS_MMR_LINUX
	//linux下转换
	//https://www.cnblogs.com/huojing/articles/16291647.html

	iconv_t iconvDescriptor = iconv_open("gb2312", "UTF-8");
	if (iconvDescriptor == (iconv_t)-1) 
	{
		STD_CERROR << "iconv_open failed!" << std::endl;
		return false;
	}

	const char* inputBuffer = strIn.c_str();
	size_t inputSize = strIn.size();
	size_t outputSize = inputSize * 2; // 估计输出缓冲区的大小

	strOut.resize(outputSize);
	char* outputPointer = const_cast<char*>(strOut.c_str());

	if (iconv(iconvDescriptor, (char**)&inputBuffer, &inputSize, &outputPointer, &outputSize) == (size_t)-1) 
	{
		STD_CERROR << "convert utf8 to local failed" << std::endl;
		iconv_close(iconvDescriptor);
		return false;
	}
	iconv_close(iconvDescriptor);
	return true;
#endif //OS_MMR_WIN
}

bool mmrUtil::localStringToUtf8(const std::string& strIn, std::string& strOut)
{
#ifdef OS_MMR_WIN
	try
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::wstring_convert<std::codecvt_byname<wchar_t, char, std::mbstate_t>> localConverter(new std::codecvt_byname<wchar_t, char, std::mbstate_t >(""));
		std::wstring wstr = localConverter.from_bytes(strIn);
		strOut = converter.to_bytes(wstr);
		return true;
	}
	catch (...)
	{
		strOut.clear();
		return false;
	}
#elif defined OS_MMR_LINUX
	iconv_t iconvDescriptor = iconv_open("UTF-8", "gb2312");
	if (iconvDescriptor == (iconv_t)-1) 
	{
		STD_CERROR << "iconv_open failed!" << std::endl;
		return false;
	}

	const char* inputBuffer = strIn.c_str();
	size_t inputSize = strIn.size();
	size_t outputSize = inputSize * 4; // 估计输出缓冲区的大小

	strOut.resize(outputSize);
	char* outputPointer = const_cast<char*>(strOut.c_str());

	if (iconv(iconvDescriptor, (char**)&inputBuffer, &inputSize, &outputPointer, &outputSize) == (size_t)-1) 
	{
		STD_CERROR << "convert local to utf8 failed" << std::endl;
		iconv_close(iconvDescriptor);
		return false;
	}
	iconv_close(iconvDescriptor);
	return true;
#endif //OS_MMR_WIN
}

COMMON_FUN_API bool mmrUtil::getAppPathAndName(std::string& filePath, std::string& exeName)
{
#ifdef OS_MMR_WIN
	char path[MAX_STR_LEN];
	auto pathLen = GetModuleFileName(NULL, path, MAX_STR_LEN);
	if (pathLen > MAX_STR_LEN)
	{
		STD_CERROR << "funciton mmrUtil::getAppPathAndName path len[" << pathLen << "] is longer than max string leng " << std::endl;
	}
	filePath = path;

	auto pos = filePath.rfind('.');
	if (pos != std::string::npos) 
	{
		filePath.erase(filePath.begin() + pos, filePath.end());
	}
	pos = filePath.rfind('\\');
	if (pos != std::string::npos)
	{
		exeName = &filePath.c_str()[pos + 1];
		filePath.erase(filePath.begin() + pos + 1, filePath.end());
	}
	else
	{
		return false;
	}
	return true;
#else
	pid_t pid = getpid();
	char tmpPath[MAX_STR_LEN];//路径
	char tmpName[MAX_STR_LEN];//exe名称
	ssize_t len = readlink(std::string("/proc/").append(std::to_string(pid)).append("/exe").c_str(), tmpPath, MAX_STR_LEN - 1);
	if (len <= 0)
		return false;
	tmpPath[len] = '\0';
	char *path_end = strrchr(tmpPath, '/');
	if (path_end == NULL)
		return false;
	++path_end;
	strcpy(tmpName, path_end);
	*path_end = '\0';
	filePath = tmpPath;
	exeName = tmpName;
	return true;
#endif
}

COMMON_FUN_API std::string mmrUtil::generateGUID()
{
	std::string retStr;
#ifdef OS_MMR_WIN
	//GUID guid;
	//if (CoCreateGuid(&guid) == RPC_S_OK) {
	//	// 将GUID转换为字符串形式
	//	char guidStr[39];
	//	StringFromGUID2(guid, guidStr, sizeof(guidStr) / sizeof(guidStr[0]));

	//	// 输出GUID
	//	std::cout << "Generated GUID: " << guidStr << std::endl;
	//}

#else



#endif

	return retStr;
}

COMMON_FUN_API std::string mmrUtil::getComplieTime()
{
	return std::string(__DATE__) + " " + std::string(__TIME__);
}

#define TIME_STR_LEN 19

COMMON_FUN_API std::string mmrUtil::timeInt64ToString(int64_t llTime)
{
	std::string stRet(TIME_STR_LEN,0);
	std::tm* time_info = std::localtime(&llTime);
	snprintf(&stRet[0], stRet.size() + 1, "%04d-%02d-%02d %02d:%02d:%02d",
		time_info->tm_year + 1900, time_info->tm_mon + 1, time_info->tm_mday,
		time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

	return stRet;
}


COMMON_FUN_API const char* mmrUtil::getFileName(const char* szFullPath)
{
	char* p = (char*)szFullPath;
	while (*p) ++p;
	while (--p >= szFullPath) {
#ifdef OS_MMR_WIN
		if (*p == '/' || *p == '\\')
#else
		if (*p == '/')
#endif
			return ++p;
	}
	return szFullPath;
}

