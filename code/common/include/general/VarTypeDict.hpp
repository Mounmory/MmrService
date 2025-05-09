/**
 * @file VarTypeDict.hpp
 * @brief 
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_COMMON_VAR_TYPE_DICT_HPP
#define MMR_COMMON_VAR_TYPE_DICT_HPP
#include <common/include/Common_def.h>
#include <common/include/general/TypeOperator.hpp>
#include <memory>

BEGINE_NAMESPACE(mmrComm)

namespace NSMultiTypeDict
{
	/// ====================== FindTagPos ===================================
	template <size_t N, template<typename...> class TCont, typename...T>
	struct Create_
	{
		using type = typename Create_<N - 1, TCont, NullParameter, T...>::type;
	};

	template <template<typename...> class TCont, typename...T>
	struct Create_<0, TCont, T...>
	{
		using type = TCont<T...>;
	};


	/// ====================== NewTupleType ===================================
	template <typename TVal, size_t N, size_t M, typename TProcessedTypes, typename... TRemainTypes>
	struct NewTupleType_;

	template <typename TVal, size_t N, size_t M, template <typename...> class TCont,
		typename...TModifiedTypes, typename TCurType, typename... TRemainTypes>
		struct NewTupleType_<TVal, N, M, TCont<TModifiedTypes...>,
		TCurType, TRemainTypes...>
	{
		using type = typename NewTupleType_<TVal, N, M + 1,
			TCont<TModifiedTypes..., TCurType>,
			TRemainTypes...>::type;
	};

	template <typename TVal, size_t N, template <typename...> class TCont,
		typename...TModifiedTypes, typename TCurType, typename... TRemainTypes>
		struct NewTupleType_<TVal, N, N, TCont<TModifiedTypes...>, TCurType, TRemainTypes...>
	{
		using type = TCont<TModifiedTypes..., TVal, TRemainTypes...>;
	};

	template <typename TVal, size_t TagPos, typename TCont, typename... TRemainTypes>
	using NewTupleType = typename NewTupleType_<TVal, TagPos, 0, TCont, TRemainTypes...>::type;

} //end namespace NSMultiTypeDict

template <typename...TParameters>
struct VarTypeDict
{
	//类型不能重复
	static_assert(!HasDuplicateTypes<TParameters...>, "template paras has duplicate tags.");

	template <typename...TTypes>
	struct Values
	{
		Values() = default;

		Values(std::shared_ptr<void>(&&input)[sizeof...(TTypes)])
		{
			for (size_t i = 0; i < sizeof...(TTypes); ++i)
			{
				m_tuple[i] = std::move(input[i]);
			}
		}

	public:
		template <typename TTag, typename TVal>
		auto Set(TVal&& val) &&
		{
			using namespace NSMultiTypeDict;
			constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;

			using rawVal = std::decay_t<TVal>;

			m_tuple[TagPos] = std::make_shared<rawVal>(std::forward<TVal>(val));

			using new_type = NewTupleType<rawVal, TagPos, Values<>, TTypes...>;
			return new_type(std::move(m_tuple));
		}

		template <typename TTag>
		auto& Get() const
		{
			using namespace NSMultiTypeDict;
			constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;
			using AimType = Pos2Type<TagPos, TTypes...>;

			void* tmp = m_tuple[TagPos].get();
			AimType* res = static_cast<AimType*>(tmp);
			return *res;
		}

		template <typename TTag>
		using ValueType = Pos2Type<Tag2ID<TTag, TParameters...>, TTypes...>;

	private:
		std::shared_ptr<void> m_tuple[sizeof...(TTypes)];
	};

public:
	static auto Create()
	{
		using type = typename NSMultiTypeDict::Create_<sizeof...(TParameters), Values>::type;
		return type{};
	}
};



END_NAMESPACE(mmrComm)



#endif//MMR_COMMON_VAR_TYPE_DICT_HPP
