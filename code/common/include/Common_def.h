#ifndef COMMON_H
#define COMMON_H

//#if defined(_WIN64) || defined(WIN32)
//#define OS_MMR_WIN
//#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
//#define OS_MMR_LINUX
////#elif defined(unix) || defined(__unix) || defined(__unix__)
////#define MMR_UNIX
//#endif

#define BEGINE_NAMESPACE(ns) namespace ns {
#define END_NAMESPACE(ns) }

#define IN
#define OUT

#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>

#ifdef OS_MMR_WIN
	//#ifndef WIN32_LEAN_AND_MEAN//WINDOWS API��������һЩ�����õ�API���Ż�Ӧ�ó��򣩲��õġ�
	//	#define WIN32_LEAN_AND_MEAN
	//#endif
	//#ifndef _CRT_NONSTDC_NO_DEPRECATE
	//	#define _CRT_NONSTDC_NO_DEPRECATE
	//#endif
	//#ifndef _CRT_SECURE_NO_WARNINGS
	//	#define _CRT_SECURE_NO_WARNINGS
	//#endif
	//#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
	//	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	//#endif
#include <winsock2.h>
//#include <ws2tcpip.h>   // for inet_pton,inet_ntop
#include <windows.h>

#define DIR_SEPARATOR       '\\'
#define DIR_SEPARATOR_STR   "\\"

#define Thread_ID GetCurrentThreadId()		//��ȡ�߳�ID
#define Process_ID GetCurrentProcessId()	//��ȡ����ID

#define OS_TYPE "Windows"
#else
#include <pthread.h>  
#include <unistd.h>  
#include <sys/syscall.h>  
#include <string>

#define DIR_SEPARATOR       '/'
#define DIR_SEPARATOR_STR   "/"

#define Thread_ID syscall(SYS_gettid)	//��ȡ�߳�ID
#define Process_ID getpid()				//��ȡ����ID

static std::string _CutParenthesesNTail(std::string&& prettyFuncon)
{
	auto pos = prettyFuncon.find('(');
	if (pos != std::string::npos)
		prettyFuncon.erase(prettyFuncon.begin() + pos, prettyFuncon.end());
	pos = prettyFuncon.find(' ');
	if (pos != std::string::npos)//ɾ����������
		prettyFuncon.erase(prettyFuncon.begin(), prettyFuncon.begin() + pos + 1);
	return std::move(prettyFuncon);
}
#define __FUNCTION__ _CutParenthesesNTail(__PRETTY_FUNCTION__).c_str()//ϵͳ�Դ��ĺ겻��ʾ��������Ϣ

#define OS_TYPE "Linux"
#endif

#ifndef __FILENAME__
// #define __FILENAME__  (strrchr(__FILE__, DIR_SEPARATOR) ? strrchr(__FILE__, DIR_SEPARATOR) + 1 : __FILE__)
#define __FILENAME__  (strrchr(DIR_SEPARATOR_STR __FILE__, DIR_SEPARATOR) + 1)
#endif

#ifdef NDEBUG
#define BUILD_TYPE "Release"
#else
#define BUILD_TYPE "Debug"
#endif

#define STD_CERROR \
std::cerr << "[" << __FUNCTION__ << "][" << __LINE__ << "]"

#endif

