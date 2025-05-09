/**
 * @file TypeOperator.hpp
 * @brief 
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_COMMON_TYPE_OPERATOR_HPP
#define MMR_COMMON_TYPE_OPERATOR_HPP
#include <common/include/Common_def.h>
#include <common/include/general/NullParameter.h>
#include <common/include/general/TypeTraits.h>
/*
	类型相关操作
*/


BEGINE_NAMESPACE(mmrComm)
//--------------一个类型是否可以转换成一组类型中的某一个------------//
//基础模板
template <typename T, typename... Args>
struct IsConvertibleToAny_;

// 递归终止条件：空列表，返回false
template <typename T>
struct IsConvertibleToAny_<T> : std::false_type {};

// 递归情况：检查当前类型是否匹配，或者继续检查剩余类型
template <typename T, typename U, typename... Args>
struct IsConvertibleToAny_<T, U, Args...> :
	std::conditional_t<std::is_convertible<T, U>::value,
	std::true_type,
	IsConvertibleToAny_<T, Args...>> {};

template <typename... Args>
constexpr bool IsConvertibleToAny = IsConvertibleToAny_<Args...>::value;

//---------------检查一组模板参数里面是否有类型重复-------------//
// 基础模板：检查类型是否在类型列表中
template <typename T, typename... Args>
struct IsTypeInList;

// 递归终止条件：空列表，返回false
template <typename T>
struct IsTypeInList<T> : std::false_type {};

// 递归情况：检查当前类型是否匹配，或者继续检查剩余类型
template <typename T, typename U, typename... Args>
struct IsTypeInList<T, U, Args...> :
	std::conditional_t<std::is_same<T, U>::value,
	std::true_type,
	IsTypeInList<T, Args...>> {};

// 基础模板：检查类型列表中是否有重复类型
template <typename... Args>
struct HasDuplicateTypes_;

// 递归终止条件：只有一个类型，肯定没有重复
template <typename T>
struct HasDuplicateTypes_<T> : std::false_type {};

// 递归情况：检查第一个类型是否在剩余类型中，或者继续检查剩余类型
template <typename T, typename... Args>
struct HasDuplicateTypes_<T, Args...> :
	std::conditional_t<IsTypeInList<T, Args...>::value,
	std::true_type,
	HasDuplicateTypes_<Args...>> {};

// 辅助函数模板，方便使用
template <typename... Args>
constexpr bool HasDuplicateTypes = HasDuplicateTypes_<Args...>::value;


//--------------------判断可变参数模板中是否包含某个类型----------------------//
// 基础情况：空列表返回false
template<typename Target, typename...>
struct ContainsType_ : std::false_type {};

// 递归情况：检查第一个类型
template<typename Target, typename First, typename... Rest>
struct ContainsType_<Target, First, Rest...> :
	std::conditional_t<
	std::is_same<Target, First>::value,
	std::true_type,
	ContainsType_<Target, Rest...>
	> {};

// 辅助变量模板
template<typename Target, typename... Ts>
constexpr bool ContainsType = ContainsType_<Target, Ts...>::value;

//--------------------检查一个类型在一组类型中的索引----------------------//
template <typename TFindTag, size_t N, typename TCurTag, typename...TTags>
struct Tag2ID_
{
	constexpr static size_t value = Tag2ID_<TFindTag, N + 1, TTags...>::value;
};

template <typename TFindTag, size_t N, typename...TTags>
struct Tag2ID_<TFindTag, N, TFindTag, TTags...>
{
	constexpr static size_t value = N;
};

template <typename TFindTag, typename...TTags>
constexpr size_t Tag2ID = Tag2ID_<TFindTag, 0, TTags...>::value;


//----------------根据索引获取一组类型中索引对应类型-----------------------//
	/// ====================== Pos2Type ===================================
template <size_t Pos, typename...TTags>
struct Pos2Type_
{
	static_assert((Pos != 0), "Invalid position.");
};

template <size_t Pos, typename TCur, typename...TTags>
struct Pos2Type_<Pos, TCur, TTags...>
{
	using type = typename std::conditional_t<(Pos == 0),
		Identity_<TCur>,
		Pos2Type_<Pos - 1, TTags...>>::type;
};

template <size_t Pos, typename...TTags>
using Pos2Type = typename Pos2Type_<Pos, TTags...>::type;

END_NAMESPACE(mmrComm)

#endif

