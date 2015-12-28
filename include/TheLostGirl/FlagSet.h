#ifndef FLAGSET_H
#define FLAGSET_H

#include <set>
#include <vector>
#include <algorithm>

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
template <class Enum>
class FlagSet
{
	public:
		inline constexpr FlagSet() = default;

		/// Constructor from initializer list.
		/// \param flags Set of flags.
		inline constexpr FlagSet(std::initializer_list<Enum> flags);

		/// Returns the state of the given flag.
		/// \param flag Flag to test.
		/// \return true if the requested flag is set, false otherwise.
		inline bool test(Enum flag) const;

		/// Checks if none of the flags are set to true.
		/// \return !any().
		inline bool none() const noexcept;

		/// Checks if any of the flags are set to true.
		/// \return size() > 0.
		inline bool any() const noexcept;

		/// Adds the specified element in the FlagSet, if not already present.
		/// \param flag The element to add.
		/// \return true if the insertion took place, false otherwise.
		inline bool set(Enum flag);

		/// Removes all elements from the FlagSet.
		inline void reset() noexcept;

		/// Removes the specified element from the FlagSet, if one exists.
		/// \param flag The element to remove.
		/// \return True if any element was removed, false otherwise.
		inline bool reset(Enum flag);

		/// Returns the number of flags that the FlagSet currently holds.
		/// \return The number of flags that the FlagSet currently holds.
		inline std::size_t size() const noexcept;

		/// Returns the maximum number of flags that the FlagSet can hold.
		/// This is not necessarily the number of elements in Enum,
		/// this is usually std::numeric_limits<std::size_t>::max().
		/// \return The maximum number of flags that the FlagSet can hold.
		inline constexpr std::size_t max_size() const noexcept;

		/// Exchanges the contents of the container with those of other.
		/// Does not invoke any move, copy, or swap operations on individual elements.
		/// \param other FlagSet to exchange the content with.
		inline void swap(FlagSet<Enum>& other);

		/// Compares two FlagSets.
		/// \param rhs FlagSet to compare.
		/// \return true if the state of each flag in *this equals the state of the corresponding flags in rhs, otherwise false.
		inline bool operator==(const FlagSet<Enum>& rhs) const;

		/// Compares two FlagSets.
		/// \param rhs FlagSet to compare.
		/// \return true if !(*this == rhs), otherwise false
		inline bool operator!=(const FlagSet<Enum>& rhs) const;

		/// Performs logical AND on each corresponding flag between *this and other.
		/// \param other Another FlagSet.
		/// \return *this.
		inline FlagSet<Enum>& operator&=(const FlagSet<Enum>& other);

		/// Performs logical OR on each corresponding flag between *this and other.
		/// \param other Another FlagSet.
		/// \return *this.
		inline FlagSet<Enum>& operator|=(const FlagSet<Enum>& other);

		/// Performs logical XOR on each corresponding flag between *this and other.
		/// \param other Another FlagSet.
		/// \return *this.
		inline FlagSet<Enum>& operator^=(const FlagSet<Enum>& other);

	private:
		/// Apply an algorithm between *this and other.
		/// \param other An other FlagSet.
		template <std::back_insert_iterator<std::vector<Enum>>(*Algorithm)
				(typename std::set<Enum>::const_iterator,
				 typename std::set<Enum>::const_iterator,
				 typename std::set<Enum>::const_iterator,
				 typename std::set<Enum>::const_iterator,
				 std::back_insert_iterator<std::vector<Enum>>)>
		inline void applyAlgorithm(const FlagSet<Enum>& other);

		std::set<Enum> m_set;///< The underlying set.
};

/// Performs logical AND on each corresponding flags between lhs and rhs.
/// \param lhs The FlagSet on the left-hand side of the operator.
/// \param rhs The FlagSet on the right-hand side of the operator.
/// \return lhs &= rhs
template<class Enum>
inline FlagSet<Enum> operator&(FlagSet<Enum> lhs, const FlagSet<Enum>& rhs);


/// Performs logical OR on each corresponding flags between lhs and rhs.
/// \param lhs The FlagSet on the left-hand side of the operator.
/// \param rhs The FlagSet on the right-hand side of the operator.
/// \return lhs |= rhs
template<class Enum>
inline FlagSet<Enum> operator|(FlagSet<Enum> lhs, const FlagSet<Enum>& rhs);


/// Performs logical XOR on each corresponding flags between lhs and rhs.
/// \param lhs The FlagSet on the left-hand side of the operator.
/// \param rhs The FlagSet on the right-hand side of the operator.
/// \return lhs ^= rhs
template<class Enum>
inline FlagSet<Enum> operator^(FlagSet<Enum> lhs, const FlagSet<Enum>& rhs);

#include <TheLostGirl/FlagSet.impl>

#endif//FLAGSET_H
