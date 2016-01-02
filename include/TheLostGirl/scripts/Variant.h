#ifndef VARIANT_H
#define VARIANT_H

#include <iostream>
#include <type_traits>
#include <memory>

//Forward declarations
namespace variantDetails
{
	class AbstractHolder;

	template <typename T, typename... Rest>
	struct ContainsType;
}

template <typename ... Types>
class Variant
{
	public:
		Variant(const Variant& other);

		Variant(Variant&& other);

		//Use SFINAE to redirect to copy constructor if T is Variant
		template <typename T,
				 typename = typename std::enable_if<variantDetails::ContainsType<T, Types...>::value>::type>
		Variant(const T& value);

		template <typename T,
				 typename = typename std::enable_if<variantDetails::ContainsType<T, Types...>::value>::type>
		Variant(T&& value);

		Variant& operator=(Variant other) noexcept;

		template <typename T,
				 typename = typename std::enable_if<variantDetails::ContainsType<T, Types...>::value>::type>
		Variant& operator=(const T& value);

		template <typename T,
				 typename = typename std::enable_if<variantDetails::ContainsType<T, Types...>::value>::type>
		Variant& operator=(T&& value);

		template <typename T>
		void set(const T& value);

		template <typename T>
		void set(T&& value);

		template <typename T,
				 typename = typename std::enable_if<variantDetails::ContainsType<T, Types...>::value>::type>
		explicit operator T() const;

		template <typename T>
		T get() const;

		std::size_t getTypeIndex() const;

		template <typename... Types2>
		friend std::ostream& operator<<(std::ostream& ostream, const Variant<Types2...>& value);

	private:
		std::unique_ptr<variantDetails::AbstractHolder> m_holder;
};

namespace variantDetails
{
	class AbstractHolder
	{
		public:
			virtual ~AbstractHolder();

			virtual const std::type_info& getTypeId() const = 0;

			virtual std::unique_ptr<AbstractHolder> clone() const = 0;

			virtual void printOnStream(std::ostream& ostream) const = 0;

			virtual std::size_t getTypeIndex() const = 0;
	};

	template <typename T, typename... Types>
	class ConcreteHolder : public AbstractHolder
	{
		public:
			ConcreteHolder(const T& value);

			ConcreteHolder(T&& value);

			virtual const std::type_info& getTypeId() const override;

			virtual std::size_t getTypeIndex() const override;

			virtual std::unique_ptr<AbstractHolder> clone() const override;

			virtual void printOnStream(std::ostream& ostream) const override;

			const T& getValue() const;

		private:
			const T m_value;
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
}

#include <TheLostGirl/scripts/Variant.impl>

#endif//VARIANT_H
