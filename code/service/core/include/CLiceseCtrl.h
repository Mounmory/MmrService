/**
 * @file CLiceseCtrl.h
 * @brief 
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */
 
#ifndef CLICENSECTRL_H
#define CLICENSECTRL_H
#include <common/include/util/CLicenseObj.h>

#include <memory>
#include <mutex>
#include <future>
#include <atomic>
#include <condition_variable>

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrCore)

//权限控制类
class CLicenseCtrl
{
public:
	CLicenseCtrl(std::string strFilePath, std::string strModule);
	~CLicenseCtrl();

	//权限校验
	void initLicense();

	//权限控制循环
	void loop(std::atomic_bool& bRunFlag);

private:
	bool reloadLicense(std::string strFilePath);//更新权限

	void licenseThread();//权限等待线程

	void releaseFuture();//结束现场

private:
	std::unique_ptr<mmrUtil::CLicenseObj> m_licObjPtr;

	std::mutex	m_mutex;
	std::condition_variable m_cv;
	std::future<void>	m_futureThread;//
};

END_NAMESPACE(mmrCore)
END_NAMESPACE(mmrService)
#endif
