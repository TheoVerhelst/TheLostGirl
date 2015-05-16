#ifndef JOINTEROLES_H
#define JOINTEROLES_H

/// \file JointRoles.h

/// Enumeration of role of some joints (bending angle, bending recoil, ...).
enum class JointRole : long unsigned int
{
	None  = 0,            ///< Role that correspond to none role.
	BendingAngle = 1 << 0,///< Role for the joint that set the angle of bodies that bend a bow.
	BendingPower = 1 << 1,///< Role for the joint that set the recoil of bodies that bend a bow.
};

/// Check wheter the given joint has all its roles flags that match with the given flags.
/// \param joint Joint to check.
/// \param role Role to check.
/// \return True if the given joint has all its roles flags that match with the given flags, false otherwise.
inline bool jointHasRole(b2Joint* joint, JointRole role)
{
	return joint->GetUserData() and reinterpret_cast<long unsigned int>(joint->GetUserData()) & static_cast<long unsigned int>(role);
}

#endif//JOINTEROLES_H
