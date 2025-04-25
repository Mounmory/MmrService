#include "common/include/util/Clogger.h"
#include "common/include/util/UtilFunc.h"
#include <stdarg.h>

#include <iomanip> //std::setfill头文件


#ifdef OS_MMR_WIN
#include <corecrt_io.h>	//_access头文件
#include <windows.h>
#elif defined OS_MMR_LINUX
#include <sys/stat.h>  //新建文件夹头文件
#include <sys/types.h> //新建文件夹头文件
#include <unistd.h>
#endif

#define MAX_FILE_SIZE 64*1024*1024
#define MIN_AVAILI_SIZE 1024
//#define MAX_FILE_SIZE 100

#define LOG_BY_LEVEL(logLevel)\
if (m_LogLevel < logLevel)\
return;\
auto now = std::chrono::system_clock::now();\
auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;\
std::time_t current_time = std::chrono::system_clock::to_time_t(now);\
std::tm* time_info = std::localtime(&current_time);\
std::unique_lock<std::mutex> guard(m_mutWrite); \
if (time_info->tm_sec != m_lastTime.tm_sec\
	|| time_info->tm_min != m_lastTime.tm_min\
	|| time_info->tm_hour != m_lastTime.tm_hour\
	|| time_info->tm_mday != m_lastTime.tm_mday\
	|| time_info->tm_mon != m_lastTime.tm_mon\
	|| time_info->tm_year != m_lastTime.tm_year)\
{\
	m_lastTime.tm_sec = time_info->tm_sec;\
	m_lastTime.tm_min = time_info->tm_min;\
	m_lastTime.tm_hour = time_info->tm_hour;\
	m_lastTime.tm_mday = time_info->tm_mday;\
	m_lastTime.tm_mon = time_info->tm_mon;\
	m_lastTime.tm_year = time_info->tm_year;\
	snprintf(m_szLastTime, sizeof(m_szLastTime), "%04d-%02d-%02d %02d:%02d:%02d",\
		m_lastTime.tm_year + 1900, m_lastTime.tm_mon + 1, m_lastTime.tm_mday,\
		m_lastTime.tm_hour, m_lastTime.tm_min, m_lastTime.tm_sec);\
}\
if (m_bAsynLog && m_pBufWrite->getTryAvailid() <= MIN_AVAILI_SIZE)/*保留1024个字节，小于这个字节就保存了*/\
{\
	updateBufWrite();\
	m_cv.notify_all();\
}\
snprintf(m_pBufWrite->getTryCurrent(), 25, "%s.%03d ", m_szLastTime, now_ms);\
m_pBufWrite->addTryIncrease(24);\
va_list arglist;\
va_start(arglist, format);\
int strLen = vsnprintf(m_pBufWrite->getTryCurrent(), m_pBufWrite->getTryAvailid(), format, arglist);\
if (strLen > m_lMaxStrLen)\
	strLen = m_lMaxStrLen;\
m_pBufWrite->addTryIncrease(strLen);\
m_pBufWrite->doneTry();\
va_end(arglist);\
if (!m_bAsynLog)\
{\
	m_logStream << m_pBufWrite->getBuf();\
	m_logStream.flush();\
	m_pBufWrite->clear();\
}

mmrUtil::CLogger::CLogger(uint32_t ulFileNum, uint64_t ullFileSize, bool bAsyn)
	: m_LogLevel(emLogLevel::Log_Debug)
	, m_fileNum(ulFileNum)
	, m_fileSize(ullFileSize * 1024 *1024)
	, m_lMaxStrLen(2048)//
	, m_bAsynLog(bAsyn)
{
	std::cout << "Logger instance construct: file number[" << m_fileNum << "] file size [" << ullFileSize << " Mb]" << std::endl;
	start();
}

mmrUtil::CLogger::~CLogger()
{
	stop();
	std::cout << "Logger instance destruct." << std::endl;
}

bool mmrUtil::CLogger::setFileMaxNum(uint32_t fileNum)
{
	if (m_logStream.is_open()) 
		return false;

	m_fileNum = fileNum;
	return true;
}

bool mmrUtil::CLogger::setFileMaxSize(uint64_t fileSize)
{
	if (m_logStream.is_open())
		return false;

	m_fileSize = fileSize;
	return true;
}

bool mmrUtil::CLogger::setLogLevel(emLogLevel logLevel)
{
	if (m_logStream.is_open())
		return false;

	m_LogLevel = logLevel;
	return true;
}

bool mmrUtil::CLogger::setAsynLog(bool bAsyn)
{
	if (bAsyn != m_bAsynLog) 
	{
		//如果异步转同步

	}
	return false;
}

void mmrUtil::CLogger::LogForce(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Forece);

	//if (m_LogLevel < emLogLevel::Log_Forece)
	//	return; 
	//va_list args;
	//va_start(args, format);
	//logWrite(format, args);
	//va_end(args);
}

void mmrUtil::CLogger::LogFatal(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Fatal);
}

void mmrUtil::CLogger::LogError(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Error);
}

void mmrUtil::CLogger::LogWarn(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Warn);
}

void mmrUtil::CLogger::LogInfo(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Info);
}

void mmrUtil::CLogger::LogDebug(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Debug);
}

void mmrUtil::CLogger::logWrite(const char *format, ...)
{
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
	std::time_t current_time = std::chrono::system_clock::to_time_t(now);
	std::tm* time_info = std::localtime(&current_time);

	std::unique_lock<std::mutex> lock(m_mutWrite);
	if (time_info->tm_sec != m_lastTime.tm_sec //由低到高比较
		|| time_info->tm_min != m_lastTime.tm_min
		|| time_info->tm_hour != m_lastTime.tm_hour
		|| time_info->tm_mday != m_lastTime.tm_mday
		|| time_info->tm_mon != m_lastTime.tm_mon
		|| time_info->tm_year != m_lastTime.tm_year)
	{
		m_lastTime.tm_sec = time_info->tm_sec;
		m_lastTime.tm_min = time_info->tm_min;
		m_lastTime.tm_hour = time_info->tm_hour;
		m_lastTime.tm_mday = time_info->tm_mday;
		m_lastTime.tm_mon = time_info->tm_mon;
		m_lastTime.tm_year = time_info->tm_year;

		snprintf(m_szLastTime, sizeof(m_szLastTime), "%04d-%02d-%02d %02d:%02d:%02d",
			m_lastTime.tm_year + 1900, m_lastTime.tm_mon + 1, m_lastTime.tm_mday,
			m_lastTime.tm_hour, m_lastTime.tm_min, m_lastTime.tm_sec);
	}


	if (m_bAsynLog && m_pBufWrite->getTryAvailid() <= MIN_AVAILI_SIZE)//保留1024个字节，小于这个字节就保存了
	{
		updateBufWrite();
		m_cv.notify_all();
	}

	//写入时间
	snprintf(m_pBufWrite->getTryCurrent(), 25, "%s.%03d ", m_szLastTime, now_ms);
	m_pBufWrite->addTryIncrease(24);

	va_list arglist;
	va_start(arglist, format);
	int strLen = vsnprintf(m_pBufWrite->getTryCurrent(), m_pBufWrite->getTryAvailid(), format, arglist);
	//if (strLen > weakAvilid)//linux系统下二次调用vsnprintf会产生段错误，因此即使长度不重新写了
	//{
	//	m_pBufWrite->clearTry();
	//	updateBufWrite();
	//	m_cv.notify_all();
	//	snprintf(m_pBufWrite->getTryCurrent(), 25, "%s.%03d ", m_szLastTime, now_ms);
	//	m_pBufWrite->addTryIncrease(24);
	//	//若新数据的availid长度小于strLen，将导致后面addTryIncrease长度错误，但新建缓冲长度往往较大，几乎不可能出现，因此暂且不做长度判断
	//	vsnprintf(m_pBufWrite->getTryCurrent(), m_pBufWrite->getTryAvailid(), format, arglist);
	//}
	if (strLen > m_lMaxStrLen)
	{
		strLen = m_lMaxStrLen;
	}
	m_pBufWrite->addTryIncrease(strLen);
	m_pBufWrite->doneTry();
	va_end(arglist);

	if (!m_bAsynLog)
	{
		m_logStream.write(m_pBufWrite->getBuf(), m_pBufWrite->getSize());
		m_logStream.flush();
		m_pBufWrite->clear();
	}

}

bool mmrUtil::CLogger::init(const std::string& strPath, const std::string& strName)
{
	m_strLogDir = strPath;
	m_strLogName = strName;
	m_strFilePath = m_strLogDir + m_strLogName + ".log";
#ifdef OS_MMR_WIN
	if (_access(m_strLogDir.c_str(), 0) == -1)//如果文件夹不存在，则创建
	{
		if (CreateDirectory(m_strLogDir.c_str(), 0) == false)
		{
			m_LogLevel = emLogLevel::Log_Off;//创建文件夹失败，不输出日志
			STD_CERROR << "Create directory " << m_strLogDir.c_str() << "failed!" << std::endl;
			return false;
		}
	}
#elif defined OS_MMR_LINUX
	if (access(m_strLogDir.c_str(), 0) != F_OK) //检查文件夹是否存在，不存在则创建
	{
		mkdir(m_strLogDir.c_str(), S_IRWXO); //所有人都有权限读写

		if (access(m_strLogDir.c_str(), 0) != F_OK)
		{
			m_LogLevel = emLogLevel::Log_Off;//创建文件夹失败，不输出日志
			std::cerr << __LINE__ << "Create directory " << m_strLogDir.c_str() << "failed!" << std::endl;
			return false;
		}
	}
#endif
	//读配置文件设置参数

	return true;
}

bool mmrUtil::CLogger::start()
{
	//m_pBufDeal = std::make_unique<CBigBuff>(m_ulBigBufSize);

	m_pBufWrite = std::make_unique<CBigBuff>(m_ulBigBufSize);

	for (uint16_t i = 0; i < m_usBufEmptySize; ++i)
	{
		m_queBufsEmpty.push(std::make_unique<CBigBuff>(m_ulBigBufSize));
	}


	if (m_strLogDir.empty() || m_strLogName.empty())
	{
		std::string logDir, logName;
		getAppPathAndName(logDir, logName);
		logDir += "log/";
		if (!init(logDir, logName))
		{
			return false;
		}
	}

	if (!m_logStream.is_open())
	{
		//m_logStream.open(m_strFilePath.c_str(), std::ios::app);
		m_logStream.open(m_strFilePath.c_str(), std::ios::app | std::ios::binary);
		if (m_logStream.fail())
		{
			STD_CERROR << "open file " << m_strFilePath.c_str() << "failed!" << std::endl;
			return false;
		}
		m_logStream.seekp(0, std::ios::end);
	}

	//启动处理线程
	if (m_bAsynLog)
	{
		m_bRunning.store(true, std::memory_order_relaxed);
		m_threadDeal = std::make_unique<std::thread>(&CLogger::dealThread, this);
	}

	logWrite("[%d][A][%s][%d]----------------- start -----------------", Thread_ID, __FUNCTION__, __LINE__);
	return true;
}

void mmrUtil::CLogger::stop()
{
	//停掉线程
	if (true == m_bRunning)
	{
		logWrite("[%d][A][%s][%d]----------------- stop -----------------", Thread_ID, __FUNCTION__, __LINE__);
		m_bRunning.store(false, std::memory_order_relaxed);//退出线程
		m_cv.notify_all();
		if (m_threadDeal && m_threadDeal->joinable())
		{
			m_threadDeal->join();//等待线程结束
		}
	}

	if (m_logStream.is_open())
	{
		m_logStream.close();
	}
}


void mmrUtil::CLogger::dealThread()
{
	while (m_bRunning.load(std::memory_order_relaxed) || m_pBufWrite->getSize() || m_queBufsWrite.size())
	{
		{
			std::unique_lock<std::mutex> lock(m_mutWrite);
			m_cv.wait_for(lock, std::chrono::milliseconds(5000));//异步日志5秒写一次
		}

		if (m_pBufWrite->getSize() > 0 || m_queBufsWrite.size() > 0)
		{
			{
				std::unique_lock<std::mutex> lock(m_mutWrite);
				updateBufWrite();
				m_queBufsDeal = std::move(m_queBufsWrite);
			}

			while (m_queBufsDeal.size())
			{
				m_pBufDeal = std::move(m_queBufsDeal.front());
				m_queBufsDeal.pop();
				//std::cout << "log write!" << std::endl;
				m_logStream.write(m_pBufDeal->getBuf(), m_pBufDeal->getSize());
				m_logStream.flush();//异步日志写入到文件
				m_pBufDeal->clear();

				fileSizeCheck();

				{
					std::unique_lock<std::mutex> lock(m_mutWrite);
					m_queBufsEmpty.push(std::move(m_pBufDeal));
				}
			}
		}
	}
}

void mmrUtil::CLogger::updateBufWrite()
{
	m_pBufWrite->zeroEnd();
	m_queBufsWrite.push(std::move(m_pBufWrite));
	if (m_queBufsEmpty.size())
	{
		m_pBufWrite = std::move(m_queBufsEmpty.front());
		m_queBufsEmpty.pop();
	}
	else
	{
		//是否新增缓冲区标记
		std::cout << " warning! log buf empty queue is empty!" << std::endl;
		m_pBufWrite = std::make_unique<CBigBuff>(m_ulBigBufSize);
	}
}

void mmrUtil::CLogger::fileSizeCheck()
{
	if (m_logStream.tellp() > m_fileSize)//文件大于最大文件大小
	{
		//修改文件名称
		m_logStream.close();

		std::string strOldName; //旧名称
		std::string strNewName; //新名称
		int32_t nLogNumIndex = m_fileNum;
		strNewName = m_strFilePath + "." + std::to_string(nLogNumIndex); //最大序号的日志
#ifdef OS_MMR_WIN
		if (_access(strNewName.c_str(), 0) != -1)
		{
			remove(strNewName.c_str()); //删除
		}
		while (--nLogNumIndex > 0) //减小序号，以此更改日志名称
		{
			strOldName = m_strFilePath + "." + std::to_string(nLogNumIndex);
			if (_access(strOldName.c_str(), 0) != -1) //检查文件夹是否存在，不存在则创建
			{
				rename(strOldName.c_str(), strNewName.c_str()); //删除
			}
			strNewName = m_strFilePath + "." + std::to_string(nLogNumIndex);
		}

#elif defined OS_MMR_LINUX
		if (access(strNewName.c_str(), 0) == F_OK)                                                 //检查文件夹是否存在，不存在则创建
		{
			remove(strNewName.c_str()); //删除
		}
		while (--nLogNumIndex > 0) //减小序号，以此更改日志名称
		{
			strOldName = m_strFilePath + "." + std::to_string(nLogNumIndex);
			if (access(strOldName.c_str(), 0) == F_OK) //检查文件夹是否存在，不存在则创建
			{
				rename(strOldName.c_str(), strNewName.c_str()); //删除
			}
			strNewName = m_strFilePath + "." + std::to_string(nLogNumIndex);
		}
#endif
		rename(m_strFilePath.c_str(), strNewName.c_str()); //重命名
		m_logStream.open(m_strFilePath, std::fstream::app);
	}
}

