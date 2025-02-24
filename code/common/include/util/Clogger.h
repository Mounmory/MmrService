#ifndef CLOGGER_H
#define CLOGGER_H
#include "common/include/Common_def.h"
#include "common/include/util/UtilExport.h"
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <fstream>
#include <string.h>
#include <iostream>
#include <queue>
#include <thread>

BEGINE_NAMESPACE(mmrUtil)

enum class emLogLevel
{
	Log_Off = -1,
	Log_Forece = 0,
	Log_Fatal,
	Log_Error,
	Log_Warn,
	Log_Info,
	Log_Debug
};



class COMMON_CLASS_API CLogger
{
private:
	CLogger();
	~CLogger();

	class CBigBuff
	{
	public:
		CBigBuff() = delete;
		CBigBuff(CBigBuff&) = delete;
		CBigBuff(CBigBuff&&) = delete;

		CBigBuff(uint32_t ulLen)
			: m_buf(new char[ulLen])
			, m_ulLen(ulLen - 1)//长度-1，避免添最后一位置空越界
			, m_ulPos(0)
			, m_usTryIncrease(0)
		{
		};

		~CBigBuff() { delete[] m_buf; }

		void tryWrite(char* buf, uint16_t len)
		{
			memcpy(m_buf + m_ulPos, buf, len);
			m_usTryIncrease += len;
		}

		void doneTry() {
			m_ulPos += m_usTryIncrease;
			m_buf[m_ulPos++] = '\n';
			m_usTryIncrease = 0;
		}

		void zeroEnd() 
		{
			m_buf[m_ulPos] = 0x00;
		}

		void clearTry()
		{
			m_usTryIncrease = 0;
			m_buf[m_ulPos] = 0x00;
		}

		uint32_t getTryAvailid() { return (m_ulLen - m_ulPos - m_usTryIncrease); }

		char* getTryCurrent() { return (m_buf + m_ulPos + m_usTryIncrease); }

		void addTryIncrease(uint16_t weakLen) { m_usTryIncrease += weakLen; }

		void clear() {
			m_ulPos = 0;
			m_usTryIncrease = 0;
		}

		char* getBuf() { return m_buf; }

		uint32_t getMaxLen() { return m_ulLen; }

		uint32_t getSize() { return m_ulPos; }

	private:
		char* m_buf;
		uint32_t m_ulLen;//buf长度
		uint32_t m_ulPos;//当前buf位置
		uint32_t m_usTryIncrease;
	};

public:
	static CLogger* getLogger();

	bool setFileMaxNum(uint32_t fileNum);
	bool setFileMaxSize(uint64_t fileSize);
	bool setLogLevel(emLogLevel logLevel);
	bool setAsynLog(bool bAsyn);

	const uint32_t getFileMaxNum() const { return m_fileNum; }
	const uint64_t getFileMaxSize()const { return m_fileSize; }
	const emLogLevel getLogLevel() const { return m_LogLevel; }

	bool init(const std::string& strPath, const std::string& strName);
	bool start(bool bAsynLog = true);
	void stop();

	void LogForce(const char *format, ...);
	void LogFatal(const char *format, ...);
	void LogError(const char *format, ...);
	void LogWarn(const char *format, ...);
	void LogInfo(const char *format, ...);
	void LogDebug(const char *format, ...);

	emLogLevel getLevel() { return m_LogLevel; }

	//使用双缓冲队列写日志接口
	void logWrite(const char *format, ...);
private:
	void dealThread();

	void updateBufWrite();

	void fileSizeCheck();//检查文件大小
private:
	emLogLevel m_LogLevel;

	uint32_t m_fileNum;
	uint64_t m_fileSize;
	uint32_t m_lMaxStrLen;//每一条日志的最大长度

	std::string m_strLogDir; //当前路径
	std::string m_strLogName; //文件路径
	std::string m_strFilePath; //输出文件全路径

	std::fstream m_logStream;   //写文件流,后续考虑对比C标准库中FILE文件接口

	std::unique_ptr<CBigBuff> m_pBufWrite;//写
	std::unique_ptr<CBigBuff> m_pBufDeal;//写
	std::queue<std::unique_ptr<CBigBuff>> m_queBufsWrite;
	std::queue<std::unique_ptr<CBigBuff>> m_queBufsDeal;
	std::queue<std::unique_ptr<CBigBuff>> m_queBufsEmpty;
	uint16_t m_usBufEmptySize = 3;
	uint32_t m_ulBigBufSize = 1024 * 1024;
	bool m_bAsynLog;//是否为异步日志

	std::mutex	m_mutWrite;  //进行客户端句柄存储修改时，线程锁
	std::condition_variable m_cv;
	std::unique_ptr<std::thread> m_threadDeal;
	std::atomic_bool m_bRunning;

	std::tm m_lastTime;//上一次日志时间
	char m_szLastTime[32];//上一次时间字符串
};


struct LogWrapper 
{
	emLogLevel logLevel = emLogLevel::Log_Debug;
	CLogger* loger = CLogger::getLogger();
};



END_NAMESPACE(mmrUtil)


#if MMR_LOGGER_WRAP//使用日志封装，每个模块单独控制日志等级
#define logInstancePtr mmrUtil::CLogger::getLogger()
extern std::shared_ptr<mmrUtil::LogWrapper> g_LoggerPtr;

#define LOG_FORCE(format, ...) \
if(g_LoggerPtr->logLevel >= mmrUtil::emLogLevel::Log_Forece)\
   g_LoggerPtr->loger->logWrite("[%ld][A][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_FATAL(format, ...) \
if(g_LoggerPtr->logLevel >= mmrUtil::emLogLevel::Log_Fatal)\
   g_LoggerPtr->loger->logWrite("[%ld][F][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
if(g_LoggerPtr->logLevel >= mmrUtil::emLogLevel::Log_Error)\
   g_LoggerPtr->loger->logWrite("[%ld][E][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_ERROR_PRINT(format, ...) \
if(g_LoggerPtr->logLevel >= mmrUtil::emLogLevel::Log_Error)\
   g_LoggerPtr->loger->logWrite("[%ld][A][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)\
	,printf("[%ld][A][%s][%d]" format "\n", Thread_ID, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define LOG_WARN(format, ...) \
if(g_LoggerPtr->logLevel >= mmrUtil::emLogLevel::Log_Warn)\
   g_LoggerPtr->loger->logWrite("[%ld][W][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_INFO(format, ...) \
if(g_LoggerPtr->logLevel >= mmrUtil::emLogLevel::Log_Info)\
   g_LoggerPtr->loger->logWrite("[%ld][I][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...) \
if(g_LoggerPtr->logLevel >= mmrUtil::emLogLevel::Log_Debug)\
   g_LoggerPtr->loger->logWrite("[%ld][D][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#else
#define logInstancePtr mmrUtil::CLogger::getLogger()

#define LOG_FORCE(format, ...) \
   logInstancePtr->LogForce("[%ld][A][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_FATAL(format, ...) \
   logInstancePtr->LogFatal("[%ld][F][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
   logInstancePtr->LogError("[%ld][E][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_ERROR_PRINT(format, ...) \
	logInstancePtr->LogError("[%ld][E][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__);\
   printf("[%ld][E][%s][%d]" format "\n",Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_WARN(format, ...) \
   logInstancePtr->LogWarn("[%ld][W][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_WARN_PRINT(format, ...) \
	logInstancePtr->LogWarn("[%ld][W][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__);\
   printf("[%ld][W][%s][%d]" format "\n",Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_INFO(format, ...) \
   logInstancePtr->LogInfo("[%ld][I][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...) \
   logInstancePtr->LogDebug("[%ld][D][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#endif


#endif