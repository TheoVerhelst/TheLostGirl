#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP

#include <type_traits>

template <typename T, T Searched, T... Values>
struct ContainsValue
{
};

template <typename T, T Searched, T FirstValue, T... Rest>
struct ContainsValue<T, Searched, FirstValue, Rest...> :
	public std::conditional<Searched == FirstValue, std::true_type, ContainsValue<T, Searched, Rest...>>::type
{
};

template <typename T, T Searched>
struct ContainsValue<T, Searched> :
	public std::false_type
{
};

template <typename T, typename... Rest>
struct ContainsType
{
};

template <typename T, typename First, typename... Rest>
struct ContainsType<T, First, Rest...> :
	std::conditional<std::is_same<T, First>::value,
		std::true_type,
		ContainsType<T, Rest...>>::type
{
};

template <typename T>
struct ContainsType<T> : std::false_type
{
};

template <std::size_t Index, typename T, typename... Rest>
struct TypeIndexImpl
{
};

template <std::size_t Index, typename T, typename First, typename... Rest>
struct TypeIndexImpl<Index, T, First, Rest...> :
	std::conditional<std::is_same<T, First>::value,
		std::integral_constant<std::size_t, Index>,
		TypeIndexImpl<Index + 1, T, Rest...>>::type
{
};

template <std::size_t Index, typename T>
struct TypeIndexImpl<Index, T>
{
};

template <typename T, typename... Rest>
struct TypeIndex : TypeIndexImpl<0, T, Rest...>
{
};

#endif// TYPE_TRAITS_HPP
