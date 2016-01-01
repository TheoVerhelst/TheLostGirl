#include <iosfwd>
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
			
			virtual const std::type_info& getId() const = 0;

			virtual std::unique_ptr<AbstractHolder> clone() const = 0;

			virtual void printOnStream(std::ostream& ostream) const = 0;
	};

	template <typename T>
	class ConcreteHolder : public AbstractHolder
	{
		public:
			ConcreteHolder(const T& value);

			ConcreteHolder(T&& value);

			virtual const std::type_info& getId() const override;

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
	struct ContainsType<T, First, Rest...>:
		std::conditional<std::is_same<T, First>::value,
			std::true_type,
			ContainsType<T, Rest...>>::type
	{
	};

	template <typename T>
	struct ContainsType<T> : std::false_type 
	{
	};
}

#include "Variant.impl"

