#ifndef MMR_UTIL_VARIANT_HPP
#define MMR_UTIL_VARIANT_HPP

#include <common/include/Common_def.h>
#include <common/include/general/TypeOperator.hpp>

#include <typeindex>
#include <iostream>

BEGINE_NAMESPACE(mmrUtil)

/** 获取最大的整数 */
template <size_t arg, size_t... rest>
struct IntegerMax;

template <size_t arg>
struct IntegerMax<arg> : std::integral_constant<size_t, arg>
{
};

template <size_t arg1, size_t arg2, size_t... rest>
struct IntegerMax<arg1, arg2, rest...> : std::integral_constant<size_t, arg1 >= arg2 ? IntegerMax<arg1, rest...>::value
	: IntegerMax<arg2, rest...>::value>
{
};

/** 获取最大的align */
template<typename... Args>
struct MaxAlign : std::integral_constant<int, IntegerMax<std::alignment_of<Args>::value...>::value>
{
};

//template <typename T, typename... List>
//struct IndexOf;
//
//template <typename T, typename Head, typename... Rest>
//struct IndexOf<T, Head, Rest...>
//{
//	enum { value = IndexOf<T, Rest...>::value + 1 };
//};
//
//template <typename T, typename... Rest>
//struct IndexOf<T, T, Rest...>
//{
//	enum { value = 0 };
//};
//
//template <typename T>
//struct IndexOf<T>
//{
//	enum { value = -1 };
//};
//
//template<int index, typename... Types>
//struct At;
//
//template<int index, typename First, typename... Types>
//struct At<index, First, Types...>
//{
//	using type = typename At<index - 1, Types...>::type;
//};
//
//template<typename T, typename... Types>
//struct At<0, T, Types...>
//{
//	using type = T;
//};

template<typename... Types>
class Variant
{
	static_assert(!mmrComm::has_duplicate_types<Types...>(), "template paras has duplicate types.");

	enum
	{
		data_size = IntegerMax<sizeof(Types)...>::value,//要分配的内存
		align_size = MaxAlign<Types...>::value
	};
	using data_t = typename std::aligned_storage<data_size, align_size>::type;
public:
	template<size_t index>
	using IndexType = typename mmrComm::Pos2Type<index, Types...>;

	Variant(void) :type_index_(typeid(void))
	{
	}

	~Variant()
	{
		destroyInner<Types...>(type_index_, &data_);
	}

	Variant(Variant<Types...>&& old) : type_index_(old.type_index_)
	{
		moveInner<Types...>(old.type_index_, &old.data_, &data_); 
	}

	Variant(const Variant<Types...>& old) : type_index_(old.type_index_)
	{
		copyInner<Types...>(old.type_index_, &old.data_, &data_);
	}

	Variant& operator=(const Variant& old)
	{
		copyInner<Types...>(old.type_index_, &old.data_, &data_);
		type_index_ = old.type_index_;
		return *this;
	}

	Variant& operator=(Variant&& old)
	{
		moveInner<Types...>(old.type_index_, &old.data_, &data_);
		type_index_ = old.type_index_;
		return *this;
	}

	template <class T,
		class = typename std::enable_if<mmrComm::ContainsType<typename std::decay<T>::type, Types...>>::type>
		Variant(T&& value) : type_index_(typeid(void))
	{
		destroyInner<Types...>(type_index_, &data_);
		typedef typename std::decay<T>::type U;
		new(&data_) U(std::forward<T>(value));
		type_index_ = std::type_index(typeid(U));
	}

	template<typename T>
	bool is() const
	{
		return (type_index_ == std::type_index(typeid(T)));
	}

	bool Empty() const
	{
		return type_index_ == std::type_index(typeid(void));
	}

	std::type_index type() const
	{
		return type_index_;
	}

	template<typename T>
	typename std::decay<T>::type& get()
	{
		using U = typename std::decay<T>::type;
		static_assert(mmrComm::ContainsType<U, Types...>, "type not contains in para types.");
		if (!is<U>())
		{
			std::cout << typeid(U).name() << " is not defined. "
				<< "current type is " << type_index_.name() << std::endl;
			throw std::bad_cast{};
		}

		return *(U*)(&data_);
	}

	template <typename T>
	size_t indexOf()
	{
		using U = typename std::decay<T>::type;
		static_assert(mmrComm::ContainsType<U, Types...>, "type not contains in para types.");
		return mmrComm::Tag2ID<T, Types...>;
	}

	bool operator==(const Variant& rhs) const
	{
		return type_index_ == rhs.type_index_;
	}

	bool operator<(const Variant& rhs) const
	{
		return type_index_ < rhs.type_index_;
	}

private:
	template<typename T>
	void destroyInner(const std::type_index& id, void *data)
	{
		if (id == std::type_index(typeid(T)))
			reinterpret_cast<T*>(data)->~T();
	}

	template<typename T,typename... Rest>
	typename std::enable_if<(sizeof...(Rest) > 0), void>::type
		destroyInner(const std::type_index& id, void *data)
	{
		if (id == std::type_index(typeid(T)))
			reinterpret_cast<T*>(data)->~T();
		else
			destroyInner<Rest...>(id, data);
	}

	template<typename T>
	void moveInner(const std::type_index& old_t, void *old_v, void *new_v)
	{
		if (old_t == std::type_index(typeid(T)))
			new (new_v)T(std::move(*reinterpret_cast<T*>(old_v)));
	}

	template<typename T, typename... Rest>
	typename std::enable_if<(sizeof...(Rest) > 0), void>::type
	moveInner(const std::type_index& old_t, void *old_v, void *new_v)
	{
		if (old_t == std::type_index(typeid(T)))
			new (new_v)T(std::move(*reinterpret_cast<T*>(old_v)));
		else
			moveInner<Rest...>(old_t, old_v, new_v);
	}

	template<typename T>
	void copyInner(const std::type_index& old_t, const void *old_v, void *new_v)
	{
		if (old_t == std::type_index(typeid(T)))
			new (new_v)T(*reinterpret_cast<const T*>(old_v));
	}

	template<typename T, typename... Rest>
	typename std::enable_if<(sizeof...(Rest) > 0), void>::type
		move0(const std::type_index& old_t, const void *old_v, void *new_v)
	{
		if (old_t == std::type_index(typeid(T)))
			new (new_v)T(*reinterpret_cast<const T*>(old_v));
		else
			copyInner<Rest...>(old_t, old_v, new_v);
	}

private:
	data_t data_;
	std::type_index type_index_;
};

END_NAMESPACE(mmrUtil)
#endif // !MMR_UTIL_VARIANT_HPP
