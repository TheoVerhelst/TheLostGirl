template <class Enum>
inline constexpr FlagSet<Enum>::FlagSet(std::initializer_list<Enum> flags):
	m_set(flags)
{
}

template <class Enum>
inline bool FlagSet<Enum>::test(Enum flag) const
{
	return m_set.count(flag) != 0;
}

template <class Enum>
inline bool FlagSet<Enum>::none() const noexcept
{
	return m_set.empty();
}

template <class Enum>
inline bool FlagSet<Enum>::any() const noexcept
{
	return !none();
}

template <class Enum>
inline bool FlagSet<Enum>::set(Enum flag)
{
	return m_set.insert(flag).second;
}

template <class Enum>
inline void FlagSet<Enum>::reset() noexcept
{
	m_set.clear();
}

template <class Enum>
inline bool FlagSet<Enum>::reset(Enum flag)
{
	return m_set.erase(flag) > 0;
}

template <class Enum>
inline std::size_t FlagSet<Enum>::size() const noexcept
{
	return m_set.size();
}

template <class Enum>
inline constexpr std::size_t FlagSet<Enum>::max_size() const noexcept
{
	return m_set.max_size();
}

template <class Enum>
inline void FlagSet<Enum>::swap(FlagSet<Enum>& other)
{
	m_set.swap(other.m_set);
}

template <class Enum>
inline bool FlagSet<Enum>::operator==(const FlagSet<Enum>& rhs) const
{
	return m_set == rhs.m_set;
}

template <class Enum>
inline bool FlagSet<Enum>::operator!=(const FlagSet<Enum>& rhs) const
{
	return m_set != rhs.m_set;
}

template <class Enum>
inline FlagSet<Enum>& FlagSet<Enum>::operator&=(const FlagSet<Enum>& other)
{
	applyAlgorithm<std::set_intersection>(other);
	return *this;
}

template <class Enum>
inline FlagSet<Enum>& FlagSet<Enum>::operator|=(const FlagSet<Enum>& other)
{
	applyAlgorithm<std::set_union>(other);
	return *this;
}

template <class Enum>
inline FlagSet<Enum>& FlagSet<Enum>::operator^=(const FlagSet<Enum>& other)
{
	applyAlgorithm<std::set_symmetric_difference>(other);
	return *this;
}

template <class Enum>
template <std::back_insert_iterator<std::vector<Enum>>(*Algorithm)
		(typename std::set<Enum>::const_iterator,
		 typename std::set<Enum>::const_iterator,
		 typename std::set<Enum>::const_iterator,
		 typename std::set<Enum>::const_iterator,
		 std::back_insert_iterator<std::vector<Enum>>)>
inline void FlagSet<Enum>::applyAlgorithm(const FlagSet<Enum>& other)
{
	std::vector<Enum> v;
	Algorithm(m_set.begin(), m_set.end(),
			other.m_set.begin(), other.m_set.end(),
			std::back_inserter(v));
	m_set.clear();
	m_set.insert(v.begin(), v.end());
}

template<class Enum>
inline FlagSet<Enum> operator&(FlagSet<Enum> lhs, const FlagSet<Enum>& rhs)
{
	return lhs &= rhs;
}

template<class Enum>
inline FlagSet<Enum> operator|(FlagSet<Enum> lhs, const FlagSet<Enum>& rhs)
{
	return lhs |= rhs;
}

template<class Enum>
inline FlagSet<Enum> operator^(FlagSet<Enum> lhs, const FlagSet<Enum>& rhs)
{
	return lhs ^= rhs;
}
