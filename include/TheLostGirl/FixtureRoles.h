#ifndef FIXTUREROLES_H
#define FIXTUREROLES_H

#include <Box2D/Box2D.h>

/// \file FixtureRoles.h

/// Enumeration of role of some fixture (foot, head, ...).
enum class FixtureRole : std::size_t
{
	None,   ///< Role that correspond to none role.
	Foot,   ///< Role for fixtures that observes the collisions with the ground.
	Main,   ///< Role for fixtures that are the main fixture of a body (e.g. to compute distance between entities).
	MaxValue///< Sentinel value for using FlagSet.
};

/// Check wheter the given fixture has all its roles flags that match with the given flags.
/// \param fixture Fixture to check.
/// \param role Role to check.
/// \return True if the given fixture has all its roles flags that match with the given flags, false otherwise.
inline bool fixtureHasRole(b2Fixture* fixture, FixtureRole role)
{
	return static_cast<FlagSet<FixtureRole>*>(fixture->GetUserData())->test(role);
}

#endif//FIXTUREROLES_H
