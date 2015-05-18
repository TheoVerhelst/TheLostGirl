#ifndef FIXTUREROLES_H
#define FIXTUREROLES_H

#include <Box2D/Dynamics/b2Fixture.h>

/// \file FixtureRoles.h

/// Enumeration of role of some fixture (foot, head, ...).
enum class FixtureRole : long unsigned int
{
	None  = 0,      ///< Role that correspond to none role.
	Foot  = 1 << 0, ///< Role for fixtures that observes the collisions with the ground.
	Main  = 1 << 1  ///< Role for fixtures that are the main fixture of a body (e.g. to compute distance between entities).
};

/// Check wheter the given fixture has all its roles flags that match with the given flags.
/// \param fixture Fixture to check.
/// \param role Role to check.
/// \return True if the given fixture has all its roles flags that match with the given flags, false otherwise.
inline bool fixtureHasRole(b2Fixture* fixture, FixtureRole role)
{
	return fixture->GetUserData() and (reinterpret_cast<long unsigned int>(fixture->GetUserData()) & static_cast<long unsigned int>(role));
}

#endif//FIXTUREROLES_H
