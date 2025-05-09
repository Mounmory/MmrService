/**
 * @file StdVariant.hpp
 * @brief 实现一个C++17中的variant类
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_UTIL_STD_VARIANT_H
#define MMR_UTIL_STD_VARIANT_H

#if 0//__cplusplus >= 201703L
#include <variant>
#else //自定义variant
#include <type_traits>
#include <cassert>
#include <tuple>

namespace std
{
	template <typename... T>
	struct variant;

	namespace internals
	{
		template <typename... T>
		struct conjunction : std::true_type {};

		template <typename T0>
		struct conjunction<T0> : T0 {};

		template <typename T0, typename... T>
		struct conjunction<T0, T...> :
			std::conditional<bool(T0::value), conjunction<T...>, T0>::type {};

		template <size_t... V>
		struct max_size;

		template <size_t V>
		struct max_size<V> : std::integral_constant<size_t, V> {};

		template <size_t V1, size_t V2, size_t... V>
		struct max_size<V1, V2, V...> : std::conditional_t<(V1 > V2), max_size<V1, V...>, max_size<V2, V... >> {};

		template <template <typename T1, typename T2> typename TPred, typename TFind, typename... T>
		struct tuple_index;

		template <template <typename T1, typename T2> typename TPred, typename TFind>
		struct tuple_index<TPred, TFind> {
			static constexpr size_t value = -1;
		};

		template <template <typename T1, typename T2> typename TPred, typename TFind, typename T0, typename... T>
		struct tuple_index<TPred, TFind, T0, T...> {
			static constexpr size_t tail_value = tuple_index<TPred, TFind, T...>::value;
			static constexpr size_t value =
				TPred<T0, TFind>::value
				? (tail_value == -1 ? 0 : tail_value + 1)
				: (tail_value == -1 ? -1 : tail_value + 1);
		};

		template <typename T1, typename T2>
		using is_single_constructible = std::is_constructible<T1, T2>;

		template <typename... T>
		struct variant_helpers {
			static constexpr size_t data_size = max_size<sizeof(T)...>::value;
			static constexpr size_t align_size = max_size<alignof(T)...>::value;
			using data_type = std::aligned_storage_t<data_size, align_size>;
		};

		template <typename TVal, typename... T>
		struct constructor_info {
			using arg_type = std::remove_cv_t<std::remove_reference_t<TVal>>;
			static constexpr bool is_same = std::is_same<arg_type, variant<T...>>::value;
			static constexpr size_t direct_index = tuple_index<std::is_same, arg_type, T...>::value;
			static constexpr size_t conversion_index = tuple_index<is_single_constructible, TVal, T...>::value;
			static constexpr size_t actual_index = direct_index == -1 ? conversion_index : direct_index;
			using target_type_w = std::conditional_t<(actual_index == -1), void, std::tuple_element<actual_index, std::tuple<T...>>>;
		};

		template <typename... T>
		struct recursion_helpers;

		template <>
		struct recursion_helpers<> {
			static void copy_construct(void* data, size_t index, const void* old_data) {}
			static void move_construct(void* data, size_t index, void* old_data) {}
			static void destruct(void* data, size_t index) {}
		};

		template <typename T0, typename... T>
		struct recursion_helpers<T0, T...> {
			static void copy_construct(void* data, size_t index, const void* old_data) {
				if (index == 0) {
					new (data) T0(*reinterpret_cast<const T0*>(old_data));
				}
				else {
					recursion_helpers<T...>::copy_construct(data, index - 1, old_data);
				}
			}

			static void move_construct(void* data, size_t index, void* old_data) {
				if (index == 0) {
					new (data) T0(std::move(*reinterpret_cast<T0*>(old_data)));
				}
				else {
					recursion_helpers<T...>::move_construct(data, index - 1, old_data);
				}
			}

			static void destruct(void* data, size_t index) {
				if (index == 0) {
					reinterpret_cast<T0*>(data)->~T0();
				}
				else {
					recursion_helpers<T...>::destruct(data, index - 1);
				}
			}
		};
	}

	template <typename... T>
	struct variant {
	public:
		using helpers = typename internals::variant_helpers<T...>;
		using data_type = typename helpers::data_type;

		template <typename TVal>
		using constructor_info = typename internals::constructor_info<TVal, T...>;

		size_t index = -1;
		data_type data;

		template <typename TVal, typename... TArgs>
		void _construct(TArgs&&... args) {
			//static_assert(std::is_same<TArgs..., std::int64_t>::value, "");
			new (&data) TVal(std::forward<TArgs>(args)...);
		}

		template <typename TVal>
		void _destruct() {
			reinterpret_cast<TVal*>(&data)->~TVal();
		}

	public:
		template <typename TVal, std::enable_if_t<constructor_info<TVal>::actual_index != -1, int> = 0>
		variant(TVal&& val) {
			_construct<typename constructor_info<TVal>::target_type_w::type>(std::forward<TVal>(val));
			index = constructor_info<TVal>::actual_index;
		}

		variant(const variant& v) {
			internals::recursion_helpers<T...>::copy_construct(&data, v.index, &v.data);
			index = v.index;
		}

		variant(variant&& v) noexcept//(std::internals::conjunction<std::is_nothrow_move_constructible<T>..., std::is_nothrow_destructible<T>...>::value) 
		{
			internals::recursion_helpers<T...>::move_construct(&data, v.index, &v.data);
			index = v.index;
		}

		~variant() {
			internals::recursion_helpers<T...>::destruct(&data, index);
		}

		variant& operator=(const variant& v) {
			auto old_index = index;
			index = v.index;
			internals::recursion_helpers<T...>::destruct(&data, old_index);
			internals::recursion_helpers<T...>::copy_construct(&data, v.index, &v.data);
			return *this;
		}

		variant& operator=(variant&& v) noexcept//(std::internals::conjunction<std::is_nothrow_move_constructible<T>::value..., std::is_nothrow_destructible<T>...>::value) 
		{
			auto old_index = index;
			index = v.index;
			internals::recursion_helpers<T...>::destruct(&data, old_index);
			internals::recursion_helpers<T...>::move_construct(&data, v.index, &v.data);
			return *this;
		}

		void* get_data() { return &data; }
		const void* get_data() const { return &data; }
		bool check_index(size_t i) const { return index == i; }
	};

	template <size_t Index, typename... T>
	std::tuple_element_t<Index, std::tuple<T...>>& get(variant<T...>& v) {
		assert(v.check_index(Index));
		return *reinterpret_cast<std::tuple_element_t<Index, std::tuple<T...>>*>(v.get_data());
	}

	template <size_t Index, typename... T>
	const std::tuple_element_t<Index, std::tuple<T...>>& get(const variant<T...>& v) {
		assert(v.check_index(Index));
		return *reinterpret_cast<const std::tuple_element_t<Index, std::tuple<T...>>*>(v.get_data());
	}

	template <typename Tv, typename... T>
	Tv& get(variant<T...>& v) {
		static constexpr size_t Index = internals::constructor_info<Tv, T...>::direct_index;
		static_assert(Index != SIZE_MAX, "invalid variant get");
		assert(v.check_index(Index));
		return *reinterpret_cast<Tv*>(v.get_data());
	}

	template <typename Tv, typename... T>
	const Tv& get(const variant<T...>& v) {
		static constexpr size_t Index = internals::constructor_info<Tv, T...>::direct_index;
		static_assert(Index != SIZE_MAX, "invalid variant get");
		assert(v.check_index(Index));
		return *reinterpret_cast<const Tv*>(v.get_data());
	}

	template <typename Tv, typename... T>
	bool holds_alternative(const variant<T...>* v) {
		return v->check_index(internals::constructor_info<Tv, T...>::direct_index);
	}

	template <size_t Index, typename... T>
	std::tuple_element_t<Index, std::tuple<T...>>* get_if(variant<T...>* v) {
		if (!v->check_index(Index)) {
			return nullptr;
		}
		return reinterpret_cast<std::tuple_element_t<Index, std::tuple<T...>>*>(v->get_data());
	}

	template <size_t Index, typename... T>
	const std::tuple_element_t<Index, std::tuple<T...>>* get_if(const variant<T...>* v) {
		if (!v->check_index(Index)) {
			return nullptr;
		}
		return reinterpret_cast<const std::tuple_element_t<Index, std::tuple<T...>>*>(v->get_data());
	}
}
#endif // 0


#endif
