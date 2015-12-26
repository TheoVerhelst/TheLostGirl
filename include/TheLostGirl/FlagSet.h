#ifndef FLAGSET_H
#define FLAGSET_H

#include <unordered_set>
#include <stdexcept>

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
		/// Default constructor.
		inline constexpr FlagSet() = default;

		/// Constructor from initializer list.
		/// \param flags Set of flags.
		inline constexpr FlagSet(std::initializer_list<Enum> flags);

		/// Assignment operator from initializer list.
		/// \param flags Set of flags.
		/// \return *this
		inline FlagSet& operator=(std::initializer_list<Enum> flags);

		/// Compare the two FlagSets.
		/// \param rhs FlagSet to compare.
		/// \return true if the state of each flag in *this equals the state of the corresponding flags in rhs, otherwise false.
		inline bool operator==(const FlagSet<Enum, N>& rhs) const;

		/// Compare the two FlagSets.
		/// \param rhs FlagSet to compare.
		/// \return true if !(*this == rhs), otherwise false
		inline bool operator!=(const FlagSet<Enum, N>& rhs) const;

		/// Return the state of the given flag.
		/// \param flag Flag to test.
		/// \return true if the requested flag is set, false otherwise.
		/// \exception std::out_of_range if flag does not correspond to a valid flag within the FlagSet.
		inline bool test(Enum flag) const throw(std::out_of_range);

		///  Checks if any flags are set to true
		/// \return true if any of the flags are set to true, otherwise false.
		inline bool empty() const noexcept;

		/// Returns the number of flags that are set to true.
		/// \return the number of flags that are set to true.
		inline std::size_t count() const;

		/// Returns the number of flags that the FlagSet can hold.
		/// This is not necessarily the number of elements in Enum,
		/// this is the second parameter N.
		/// \return The number of flags that the FlagSet can hold.
		inline constexpr std::size_t size() const noexcept;

		/// Performs logical AND on each corresponding flag between *this and other.
		/// \param other Another FlagSet.
		/// \return *this.
		inline FlagSet<Enum, N>& operator&=(const FlagSet<Enum>& other);

		/// Performs logical OR on each corresponding flag between *this and other.
		/// \param other Another FlagSet.
		/// \return *this.
		inline FlagSet<Enum, N>& operator|=(const FlagSet<Enum>& other);


		/// Performs logical XOR on each corresponding flag between *this and other.
		/// \param other Another FlagSet.
		/// \return *this.
		inline FlagSet<Enum, N>& operator^=(const FlagSet<Enum>& other);

		/// Performs logcial NOT on each flag in *this.
		/// \return A copy of *this with every flag flipped.
		inline FlagSet<Enum, N> operator~() const;

		/// Set all flags to true.
		/// \return *this
		inline FlagSet<Enum, N>& set();

		/// Set the specified \a flag to the specified \a value.
		/// \param flag Flag to set.
		/// \param value Value to set to the flag.
		/// \return *this.
		inline FlagSet<Enum, N>& set(Enum flag, bool value=true);

		/// Set all flags to false.
		/// \return *this.
		inline FlagSet<Enum, N>& reset();

		/// Set the specified \a flag to false.
		/// \param flag Flag to reset.
		/// \return *this.
		inline FlagSet<Enum, N>& reset(Enum flag);

		/// Flip all flags.
		/// \return *this.
		inline FlagSet<Enum, N>& flip();

		/// Flip the specified flag.
		/// \param flag Flag to flip.
		/// \return *this.
		inline FlagSet<Enum, N>& flip(Enum flag);

	private:
		/// Get the index in the bitset for the given \a flag.
		/// \param flag The flag.
		/// \return The index in the bitset for \a flag.
		inline std::size_t getIndex(Enum flag) const;

		std::bitset<N> m_bitset;///< The underlying bitset.
};

/// Performs logical AND on each corresponding flags between lhs and rhs.
/// \param lhs The FlagSet on the left-hand side of the operator.
/// \param rhs The FlagSet on the right-hand side of the operator.
/// \return lhs &= rhs
template<class Enum, std::size_t N>
inline FlagSet<Enum, N> operator&(FlagSet<Enum, N> lhs, const FlagSet<Enum, N>& rhs);


/// Performs logical OR on each corresponding flags between lhs and rhs.
/// \param lhs The FlagSet on the left-hand side of the operator.
/// \param rhs The FlagSet on the right-hand side of the operator.
/// \return lhs |= rhs
template<class Enum, std::size_t N>
inline FlagSet<Enum, N> operator|(FlagSet<Enum, N> lhs, const FlagSet<Enum, N>& rhs);


/// Performs logical XOR on each corresponding flags between lhs and rhs.
/// \param lhs The FlagSet on the left-hand side of the operator.
/// \param rhs The FlagSet on the right-hand side of the operator.
/// \return lhs ^= rhs
template<class Enum, std::size_t N>
inline FlagSet<Enum, N> operator^(FlagSet<Enum, N> lhs, const FlagSet<Enum, N>& rhs);

#include <TheLostGirl/FlagSet.impl>

#endif//FLAGSET_H
