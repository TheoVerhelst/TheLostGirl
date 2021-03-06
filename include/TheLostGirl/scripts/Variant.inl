#include <typeinfo>

template <typename... Types>
Variant<Types...>::Variant(const Variant<Types...>& other):
	m_holder{other.m_holder->clone()}
{
}

template <typename... Types>
Variant<Types...>::Variant(Variant<Types...>&& other):
	m_holder{other.m_holder.release()}
{
}

//Use SFINAE to redirect to copy constructor if T is Variant<Types...>
template <typename... Types>
template <typename T, typename>
Variant<Types...>::Variant(const T& value)
{
	set(value);
}

template <typename... Types>
template <typename T, typename>
Variant<Types...>::Variant(T&& value)
{
	set(std::forward<T>(value));
}

template <typename... Types>
Variant<Types...>& Variant<Types...>::operator=(Variant<Types...> other) noexcept
{
	m_holder.swap(other.m_holder);
	return *this;
}

template <typename... Types>
template <typename T, typename>
Variant<Types...>& Variant<Types...>::operator=(const T& value)
{
	set(value);
	return *this;
}

template <typename... Types>
template <typename T, typename>
Variant<Types...>& Variant<Types...>::operator=(T&& value)
{
	set(std::forward<T>(value));
	return *this;
}

template <typename... Types>
template <typename T>
void Variant<Types...>::set(const T& value)
{
	//Use static_assert instead of std::enable_if because it makes clearer error message
	static_assert(ContainsType<T, Types...>::value, "Invalid template parameter.");
	m_holder.reset(new variantDetails::ConcreteHolder<T, Types...>(value));
}

template <typename... Types>
template <typename T>
void Variant<Types...>::set(T&& value)
{
	static_assert(ContainsType<T, Types...>::value, "Invalid template parameter.");
	m_holder.reset(new variantDetails::ConcreteHolder<T, Types...>(std::forward<T>(value)));
}

template <typename... Types>
template <typename T, typename>
Variant<Types...>::operator T() const
{
	return get<T>();
}

template <typename... Types>
template <typename T>
T Variant<Types...>::get() const
{
	static_assert(ContainsType<T, Types...>::value, "Invalid template parameter.");
	//Runtime error if T is not the current type, altough it is in Types...
	if(typeid(T) != m_holder->getTypeId())
		throw std::runtime_error(std::string("Variant<Types...>::get: bad template parameter for get: expected ")
				+ m_holder->getTypeId().name() + ", got " + typeid(T).name());
	return static_cast<variantDetails::ConcreteHolder<T, Types...>*>(m_holder.get())->getValue();
}

template <typename... Types>
std::size_t Variant<Types...>::getTypeIndex() const
{
	return m_holder->getTypeIndex();
}

template <typename... Types>
std::ostream& operator<<(std::ostream& ostream, const Variant<Types...>& value)
{
	value.m_holder->printOnStream(ostream);
	return ostream;
}

namespace variantDetails
{
	template <typename T, typename... Types>
	ConcreteHolder<T, Types...>::ConcreteHolder(const T& value):
		m_value(value)
	{
	}

	template <typename T, typename... Types>
	ConcreteHolder<T, Types...>::ConcreteHolder(T&& value):
		m_value(std::forward<T>(value))
	{
	}

	template <typename T, typename... Types>
	const std::type_info& ConcreteHolder<T, Types...>::getTypeId() const
	{
		return typeid(T);
	}

	template <typename T, typename... Types>
	std::size_t ConcreteHolder<T, Types...>::getTypeIndex() const
	{
		return TypeIndex<T, Types...>::value;
	}

	template <typename T, typename... Types>
	std::unique_ptr<AbstractHolder> ConcreteHolder<T, Types...>::clone() const
	{
		return std::unique_ptr<AbstractHolder>(new ConcreteHolder<T, Types...>(m_value));
	}

	template <typename T, typename... Types>
	void ConcreteHolder<T, Types...>::printOnStream(std::ostream& ostream) const
	{
		ostream << m_value;
	}

	template <typename T, typename... Types>
	const T& ConcreteHolder<T, Types...>::getValue() const
	{
		return m_value;
	}
}
