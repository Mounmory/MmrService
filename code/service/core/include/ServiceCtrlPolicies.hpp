/**
 * @file ServiceCtrlPolicies.hpp
 * @brief 几个服务管理策略类，作为CCompFramework模板，用于处理服务接口的注册及获取
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 2024 12 12
 *
 * 
 */

#ifndef SERVICECTRLPOLICES_HPP
#define SERVICECTRLPOLICES_HPP
#include <common/include/Common_def.h>
#include <common/include/general/TypeInfo.hpp>

#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <unordered_map>



BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrCore)


/*
	使用map管理服务接口，typeid作为服务添加或获取的key，对数时间复杂度
*/
class  MapServKeyByTypeID
{
public:
	MapServKeyByTypeID() = default;
	MapServKeyByTypeID(const MapServKeyByTypeID& rhs) = delete;
	~MapServKeyByTypeID() = default;

	template<typename _T>
	void registService(std::shared_ptr<_T>&& pSer)
	{
		m_mapServices.insert(std::make_pair(mmrComm::TypeInfo(typeid(_T)), std::forward<std::shared_ptr<_T>>(pSer)));
	}

	template<typename _T>
	std::shared_ptr<_T> getService()
	{
		auto iterSer = m_mapServices.find(mmrComm::TypeInfo(typeid(_T)));
		if (iterSer == m_mapServices.end()) 
		{
			std::cerr << "error! unregister service type " << typeid(_T).name() << std::endl;
			return nullptr;
		}
		return std::static_pointer_cast<_T>(iterSer->second);
	}

	void clear() { m_mapServices.clear(); }

	size_t size() { return m_mapServices.size(); }
private:
	std::map<mmrComm::TypeInfo, std::shared_ptr<void>> m_mapServices;

};


/*
	使用vector管理服务接口，通过索引添加或获取服务，常数时间复杂度
*/

//使用这个策略的类，必须添加这个宏
#define IMPLEMENT_INDEXABLE_CLASS \
public:\
	static int32_t& GetClassIndexStatic() \
	{\
		static int32_t index = -1;\
		return index;\
	}

////必须定义动态库的导出
//#if defined(OS_MMR_WIN)
//#ifdef MMR_COMPONENT_EXPORT
//#define SERVICE_API_EXPORT __declspec(dllexport)
//#else
//#define SERVICE_API_EXPORT __declspec(dllimport)
//#endif // MMR_COMPONENT_EXPORT
//#else
//#define SERVICE_API_EXPORT 
//#endif

class VecServByIndex //在window系统中，存在类导出问题待解决，不能使用这个策略
{
public:
	VecServByIndex() = default;
	VecServByIndex(const VecServByIndex& rhs) = delete;
	~VecServByIndex() = default;

	template<typename _T>
	void registService(std::shared_ptr<_T>&& pSer)
	{
		int32_t& index = _T::GetClassIndexStatic();
		m_vecServices.emplace_back(std::forward<std::shared_ptr<_T>>(pSer));
		index = m_vecServices.size() - 1;
		//std::cout << "register service index " << index <<" static index " << _T::GetClassIndexStatic() << " vector size " << m_vecServices.size() << std::endl;
	}

	template<typename _T>
	std::shared_ptr<_T> getService()
	{
		const int32_t& index = _T::GetClassIndexStatic();
		if (index < 0 || index >= m_vecServices.size())
		{
			std::cerr << "error! unregister service type " << typeid(_T).name() << " index " << index << std::endl;
			return nullptr;
		}
		return std::static_pointer_cast<_T>(m_vecServices[index]);
	}

	void clear() { m_vecServices.clear(); }

	size_t size() { return m_vecServices.size(); }
private:
	std::vector<std::shared_ptr<void>> m_vecServices;
};


/*
使用unordered_map管理服务接口，GUID作为服务添加或获取的key，对数时间复杂度
*/
#define IMPLEMENT_GUID_CLASS(strGUID) \
public:\
	static const std::string& GetClassGUIDStatic() \
	{\
		static const std::string guid = strGUID;\
		return guid;\
	}

class UnMapServKeyByGUID
{
public:
	UnMapServKeyByGUID() = default;
	UnMapServKeyByGUID(const UnMapServKeyByGUID& rhs) = delete;
	~UnMapServKeyByGUID() = default;

	template<typename _T>
	void registService(std::shared_ptr<_T>&& pSer)
	{
		const std::string& strGUID = _T::GetClassGUIDStatic();
		if (m_unMapServices.count(strGUID))
		{
			std::cerr << "error! service "<< typeid(_T).name()<< " GUID " << strGUID << " repeated!" << std::endl;
		}
		m_unMapServices[_T::GetClassGUIDStatic()] = std::forward<std::shared_ptr<_T>>(pSer);
	}

	template<typename _T>
	std::shared_ptr<_T> getService()
	{
		auto iterSer = m_unMapServices.find(_T::GetClassGUIDStatic());
		if (iterSer == m_unMapServices.end())
		{
			std::cerr << "error! unregister service type " << typeid(_T).name() << std::endl;
			return nullptr;
		}
		return std::static_pointer_cast<_T>(iterSer->second);
	}
	
	void clear() { m_unMapServices.clear(); }

	size_t size() { return m_unMapServices.size(); }
private:
	std::unordered_map<std::string, std::shared_ptr<void>> m_unMapServices;

};

END_NAMESPACE(mmrCore)
END_NAMESPACE(mmrService)
#endif
