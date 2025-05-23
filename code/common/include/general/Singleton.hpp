/**
 * @file Singleton.hpp
 * @brief 
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_COMMON_SINGLETON_HPP
#define MMR_COMMON_SINGLETON_HPP
#include <common/include/Common_def.h>

#include <memory>
#include <mutex>


BEGINE_NAMESPACE(mmrComm)

//template <typename _Ty, typename... Args>
//class Singleton {
//public:
//	static _Ty* getInstance(Args&&... args) {
//		std::call_once(initFlag, [&] {
//			ptrInstance.reset(new _Ty(std::forward<Args>(args)...));
//		});
//		return ptrInstance.get();
//	}
//
//	// 禁止拷贝和赋值
//	Singleton(const Singleton&) = delete;
//	Singleton& operator=(const Singleton&) = delete;
//
//protected:
//	Singleton() = default;
//	virtual ~Singleton() = default;
//
//private:
//	static std::unique_ptr<_Ty> ptrInstance;
//	static std::once_flag initFlag;
//};
//
//template <typename _Ty, typename... Args>
//std::unique_ptr<_Ty> Singleton<_Ty, Args...>::ptrInstance;
//
//template <typename _Ty, typename... Args>
//std::once_flag Singleton<_Ty, Args...>::initFlag;




// 处理非模板类型
template <typename _Ty, typename _Tv = void>
class Singleton {
public:
	template<typename... Args>
	static _Ty* initInstance(Args&&... args) 
	{
		if(nullptr == m_pInstance)
		{
			m_pInstance = new _Ty(std::forward<Args>(args)...);
		}
		return m_pInstance;
	}

	static _Ty* getInstance() 
	{
		return m_pInstance;
	}

	static void destroyInstance() 
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
private:
	static _Ty* m_pInstance;
};

template<typename _Ty, typename _Tv>
_Ty* Singleton<_Ty, _Tv>::m_pInstance = nullptr;



// 处理模板类型
template <template <typename...> class _Ty, typename... Args>
class Singleton<_Ty<Args...>> 
{
	using Type = _Ty<Args...>;
public:
	template<typename... ArgValues>
	static Type* initInstance(ArgValues&&... args)
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new Type(std::forward<ArgValues>(args)...);
		}
		return m_pInstance;
	}

	static Type* getInstance()
	{
		return m_pInstance;
	}

	static void destroyInstance()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
private:
	static Type* m_pInstance;
};

// 静态成员变量的定义
template <template <typename...> class _Ty, typename... Args>
typename Singleton<_Ty<Args...>>::Type* Singleton<_Ty<Args...>>::m_pInstance = nullptr;

END_NAMESPACE(mmrComm)




#endif
