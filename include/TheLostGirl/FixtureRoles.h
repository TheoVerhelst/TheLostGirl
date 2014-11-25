#ifndef FIXTUREROLES_H
#define FIXTUREROLES_H

/// \file FixtureRoles.h

/// Enumeration of role of some fixture (foot, head, ...).
enum class FixtureRole : unsigned int
{
	None  = 0,      ///< Role that correspond to none role.
	Foot  = 1 << 0, ///< Role for the fixture that observes the collisions with the ground.
};

#endif // FIXTUREROLES_H
