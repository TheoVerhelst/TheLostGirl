#ifndef FLAGSET_H
#define FLAGSET_H

#include <bitset>

/// Set of flags defined by the template parameter enumeration.
/// This is a type-safe replacing for the usual couple unsigned int/enum.
/// Another advandage is the possibility to use a strong-typed enum as flag identifiers.
/// Some assertions are done on the structure of the enum class:
/// <ul>
/// 	<li>any value of the enum must be convertible to an unsigned type,
/// more precisely to `std::size_t`.</li>
/// 	<li>the enum used as template parameter must have its last element named
/// MaxValue and its first element explicitely assigned to 0,
/// or if this is not possible, the highest value in the enum + 1 must
/// be given as second template parameter;</li>
/// </ul>
/// The value of an element `Enum::e` in the enum means the value obtained by
/// the expression `static_cast<std::size_t>(Enum::e);`.
/// If the template parameter N is not consistent, the resulting object
/// may not be consistent, and it is undefined behavior to use it.
template <class Enum, std::size_t N = static_cast<std::size_t>(Enum::MaxValue)>
class FlagSet
{
	public:
		inline constexpr FlagSet() = default;

		inline constexpr FlagSet(std::initializer_list<Enum> flags);

		inline FlagSet& operator=(std::initializer_list<Enum> flags);

		/// Compare the two FlagSets.
		/// \param FlagSet to compare
		/// \return true if the state of each flag in *this equals the state of the corresponding flags in rhs, otherwise false
		inline bool operator==(const FlagSet<Enum, N>& rhs) const;

		/// Compare the two FlagSets.
		/// \param FlagSet to compare
		/// \return true if !(*this == rhs), otherwise false
		inline bool operator!=(const FlagSet<Enum, N>& rhs) const;

		/// Return the state of the given flag.
		/// \param flag Flag to test
		/// \return true if the requested flag is set, false otherwise
		/// \exception std::out_of_range if flag does not correspond to a valid flag within the FlagSet.
		inline bool test(Enum flag) const throw(std::out_of_range);

		///  Checks if all flags are set to true
		/// \return true if all flags are set to true, otherwise false
		inline bool all() const noexcept;

		///  Checks if any flags are set to true
		/// \return true if any of the flags are set to true, otherwise false
		inline bool any() const noexcept;

		///  Checks if none of the flags are set to true
		/// \return true if none of the flags are set to true, otherwise false
		inline bool none() const noexcept;

		/// Returns the number of flags that are set to true.
		/// \return the number of flags that are set to true
		inline std::size_t count() const;

		/// Returns the number of flags that the FlagSet can hold.
		/// This is not necessarily the number of elements in Enum,
		/// this is the second parameter N.
		/// \return the number of flags that the FlagSet can hold
		inline constexpr std::size_t size() const noexcept;

		inline FlagSet<Enum, N>& operator&=(const FlagSet<Enum>& other);

		inline FlagSet<Enum, N>& operator|=(const FlagSet<Enum>& other);

		inline FlagSet<Enum, N>& operator^=(const FlagSet<Enum>& other);

		inline FlagSet<Enum, N> operator~() const;

		inline FlagSet<Enum, N>& set();

		inline FlagSet<Enum, N>& set(Enum flag, bool value=true);

		inline FlagSet<Enum, N>& reset();

		inline FlagSet<Enum, N>& reset(Enum flag);

		inline FlagSet<Enum, N>& flip();

		inline FlagSet<Enum, N>& flip(Enum flag);

	private:
		inline std::size_t getIndex(Enum flag) const;

		std::bitset<N> m_bitset;
};

template<class Enum, std::size_t N>
inline FlagSet<Enum, N> operator&(FlagSet<Enum, N> lhs, const FlagSet<Enum, N>& rhs);

template<class Enum, std::size_t N>
inline FlagSet<Enum, N> operator|(FlagSet<Enum, N> lhs, const FlagSet<Enum, N>& rhs);

template<class Enum, std::size_t N>
inline FlagSet<Enum, N> operator^(FlagSet<Enum, N> lhs, const FlagSet<Enum, N>& rhs);

#include <TheLostGirl/FlagSet.impl>

#endif//FLAGSET_H
