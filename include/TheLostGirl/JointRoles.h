#ifndef JOINTEROLES_H
#define JOINTEROLES_H

/// \file JointRole.h

/// Enumeration of role of some joints (bending angle, bending recoil, ...).
enum class JointRole : unsigned int
{
	None  = 0,            ///< Role that correspond to none role.
	BendingAngle = 1 << 0,///< Role for the joint that set the angle of bodies that bend a bow.
	BendingPower = 1 << 1,///< Role for the joint that set the recoil of bodies that bend a bow.
};

inline bool jointHasRole(b2Joint* joint, JointRole role)
{
	return joint->GetUserData() and reinterpret_cast<unsigned int>(joint->GetUserData()) & static_cast<unsigned int>(role);
}

#endif//JOINTEROLES_H
