#include "common/include/util/UtilFunc.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <fstream>
#include <cctype>

#ifdef OS_MMR_WIN	//windows
#include <Windows.h>
#include <codecvt>	//字符转换头文件
#include <intrin.h>	//cpu id头文件
#include <objbase.h> //生成GUID
#elif defined OS_MMR_LINUX	//linux
#include <sys/stat.h>
#include <unistd.h>
#include <iconv.h>
#include <cpuid.h>//cpu id 头文件
#endif


#define MAX_STR_LEN 1024

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

std::string mmrUtil::getComputerID()
{
	char buffer[32] = { 0 };

#ifdef OS_MMR_WIN
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 1); // 使用 CPUID 指令获取 CPU 信息
	snprintf(buffer, sizeof(buffer), "%08X%08X", CPUInfo[3], CPUInfo[0]);
#else
	unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(1, &eax, &ebx, &ecx, &edx); // 使用 __get_cpuid 函数获取 CPU 信息
	snprintf(buffer, sizeof(buffer), "%08X%08X", edx, eax);
#endif
	return std::string(buffer);
}

std::string mmrUtil::getAppPath() 
{
	std::string strRet;
#ifdef OS_MMR_WIN
	char path[MAX_STR_LEN];
	auto pathLen = GetModuleFileName(NULL, path, MAX_STR_LEN);
	if (pathLen > MAX_STR_LEN)
	{
		STD_CERROR << "funciton mmrUtil::getAppPathAndName path len[" << pathLen << "] is longer than max string leng " << std::endl;
	}
	strRet = path;
	auto pos = strRet.rfind('\\');
	if (pos != std::string::npos)
	{
		strRet.erase(strRet.begin() + pos + 1, strRet.end());
	}
#else
	do 
	{
		pid_t pid = getpid();
		char tmpPath[MAX_STR_LEN];//路径
		char tmpName[MAX_STR_LEN];//exe名称
		ssize_t len = readlink(std::string("/proc/").append(std::to_string(pid)).append("/exe").c_str(), tmpPath, MAX_STR_LEN - 1);
		if (len <= 0)
			break;
		tmpPath[len] = '\0';
		char* path_end = strrchr(tmpPath, '/');
		if (path_end == NULL)
			break;
		++path_end;
		strcpy(tmpName, path_end);
		*path_end = '\0';
		strRet = tmpPath;
	} while (false);
#endif
	return strRet;
}

bool mmrUtil::getAppPathAndName(std::string& filePath, std::string& exeName)
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
	char* path_end = strrchr(tmpPath, '/');
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

std::string mmrUtil::generateGUID()
{
	std::string strRet;
#ifdef OS_MMR_WIN
	std::stringstream guidStream;
	// Windows 实现
	GUID guid;
	if (CoCreateGuid(&guid) == S_OK) {
		guidStream << std::hex << std::setfill('0')
			<< std::setw(8) << guid.Data1 << "-"
			<< std::setw(4) << guid.Data2 << "-"
			<< std::setw(4) << guid.Data3 << "-"
			<< std::setw(2) << static_cast<int>(guid.Data4[0]) << std::setw(2) << static_cast<int>(guid.Data4[1]) << "-"
			<< std::setw(2) << static_cast<int>(guid.Data4[2]) << std::setw(2) << static_cast<int>(guid.Data4[3])
			<< std::setw(2) << static_cast<int>(guid.Data4[4]) << std::setw(2) << static_cast<int>(guid.Data4[5])
			<< std::setw(2) << static_cast<int>(guid.Data4[6]) << std::setw(2) << static_cast<int>(guid.Data4[7]);
	}
	else
	{
		std::cout << "Failed to generate GUID on Windows." << std::endl;
	}
	strRet = guidStream.str();
#else
	// Linux 实现
	std::ifstream uuidFile("/proc/sys/kernel/random/uuid");
	if (uuidFile.is_open()) 
	{
		std::getline(uuidFile, strRet);
		uuidFile.close();
	}
	else 
	{
		std::cout << "Failed to open /proc/sys/kernel/random/uuid." << std::endl;
	}
#endif
	for (char& c : strRet)
	{    // 遍历每个字符
		c = std::toupper(c);    // 将小写字母转换为大写
	}
	return strRet;
}

std::string mmrUtil::getComplieTime()
{
	return std::string(__DATE__) + " " + std::string(__TIME__);
}

std::string mmrUtil::getComplierInfo()
{
#if defined(OS_MMR_WIN)
	// Windows platform
#if defined(_MSC_VER)
	// Microsoft Visual C++
	std::string version = "_MSC_VER=";
	version += std::to_string(_MSC_VER);
#if defined(_MSC_FULL_VER)
	version += " (_MSC_FULL_VER=" + std::to_string(_MSC_FULL_VER) + ")";
#endif
#if defined(_MSC_BUILD)
	version += " (_MSC_BUILD=" + std::to_string(_MSC_BUILD) + ")";
#endif
	return version;
#elif defined(__MINGW32__) || defined(__MINGW64__)
	// MinGW (GCC for Windows)
	const char* version = __VERSION__;
	return std::string("MinGW version: ") + version;
#else
	// Unknown compiler on Windows
	return "Unknown compiler on Windows";
#endif
#elif defined(OS_MMR_LINUX)
	// Linux platform
#if defined(__GNUC__)
	// GCC or Clang in GCC compatibility mode
	std::string version = "__GNUC__=" + std::to_string(__GNUC__) +
		", __GNUC_MINOR__=" + std::to_string(__GNUC_MINOR__) +
		", __GNUC_PATCHLEVEL__=" + std::to_string(__GNUC_PATCHLEVEL__);
#if defined(__clang__)
	// Clang
	version += ", Clang version: " + __clang_version__;
#else
	// GCC
	version += ", GCC version: " + std::string(__VERSION__);
#endif
	return version;
#else
	// Unknown compiler on Linux
	return "Unknown compiler on Linux";
#endif
#else
	// Unknown platform
	return "Unknown platform";
#endif
}

#define TIME_STR_LEN 19

std::string mmrUtil::timeInt64ToString(int64_t llTime)
{
	std::string stRet(TIME_STR_LEN, 0);
	std::tm* time_info = std::localtime(&llTime);
	snprintf(&stRet[0], stRet.size() + 1, "%04d-%02d-%02d %02d:%02d:%02d",
		time_info->tm_year + 1900, time_info->tm_mon + 1, time_info->tm_mday,
		time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

	return stRet;
}

const char* mmrUtil::getFileName(const char* szFullPath)
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

std::string mmrUtil::formatMemorySize(int64_t bytes) 
{
	constexpr int64_t KB = 1024;
	constexpr int64_t MB = KB * 1024;
	constexpr int64_t GB = MB * 1024;

	std::ostringstream oss;
	oss << std::fixed << std::setprecision(3);

	if (bytes >= GB) {
		double gb = static_cast<double>(bytes) / GB;
		oss << gb << " GB";
	}
	else if (bytes >= MB) {
		double mb = static_cast<double>(bytes) / MB;
		oss << mb << " MB";
	}
	else if (bytes >= KB) {
		double kb = static_cast<double>(bytes) / KB;
		oss << kb << " KB";
	}
	else {
		oss << bytes << " B";
	}

	// 移除末尾无意义的零和小数点（如 1.000 → 1）
	std::string result = oss.str();
	size_t dot_pos = result.find('.');
	if (dot_pos != std::string::npos) {
		// 从末尾开始删除连续的零
		while (result.back() == '0') {
			result.pop_back();
		}
		// 如果小数点后没有数字了，删除小数点
		if (result.back() == '.') {
			result.pop_back();
		}
	}

	return result;
}
