#ifndef SCRIPTSFUNCTIONS_HPP
#define SCRIPTSFUNCTIONS_HPP

#include <vector>
#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/scripts/Interpreter.hpp>

/// Implements the < operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the < operator.
Data lowerThanOp(const std::vector<Data>& args);

/// Implements the > operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the > operator.
Data greaterThanOp(const std::vector<Data>& args);

/// Implements the <= operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the <= operator.
Data lowerEqualOp(const std::vector<Data>& args);

/// Implements the >= operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the >= operator.
Data greaterEqualOp(const std::vector<Data>& args);

/// Implements the == operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the == operator.
Data equalOp(const std::vector<Data>& args);

/// Implements the != operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the != operator.
Data notEqualOp(const std::vector<Data>& args);

/// Implements the and operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the and operator.
Data andOp(const std::vector<Data>& args);

/// Implements the or operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the or operator.
Data orOp(const std::vector<Data>& args);

/// Implements the + operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the + operator.
Data addOp(const std::vector<Data>& args);

/// Implements the - operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the - operator.
Data substractOp(const std::vector<Data>& args);

/// Implements the * operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the * operator.
Data multiplyOp(const std::vector<Data>& args);

/// Implements the / operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the / operator.
Data divideOp(const std::vector<Data>& args);

/// Implements the % operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the % operator.
Data moduloOp(const std::vector<Data>& args);

/// Implements the not operator in scripts between two variables.
/// \param args Operands of the operator.
/// \return the resulting value of the not operator.
Data notOp(const std::vector<Data>& args);


/// Print all variables passed as argument, no matter how many there are.
/// Printed variables are separated by a space
/// \param args Variables to print
/// \return 0 as integer.
Data print(const std::vector<Data>& args);

//Foe handling

/// Find the entity that is a foe and that is the nearest of the first argument, usually self.
/// First argument must be a valid entity.
/// \param args Entity that is searching for ennemies.
/// \return The nearest entity if found, Entity() otherwise.
entityx::Entity nearestFoe(const std::vector<Data>& args);

/// Compute the distance between two entities.
/// Both argument must be valid entities, and must have a body component.
/// \param args The two entities.
/// \return The distance.
float distanceFrom(const std::vector<Data>& args);

/// Compute the direction that the first entity must have to look directly to the second entity.
/// Both argument must be valid entities, and must have a body component and transform component.
/// \param args The two entities.
/// \return The integer value of the variable "left" or "right" available in scripts.
Direction directionTo(const std::vector<Data>& args);

/// Return the direction that has an entity.
/// First argument must be a valid entity, and have a direction component.
/// \param args The entity.
/// \return The direction of the entity.
Direction directionOf(const std::vector<Data>& args);

/// Make that first entity attack.
/// \param args Entity that will attack.
/// \return 0 as integer.
int attack(const std::vector<Data>& args);

/// Callback to find the nearest ennemy of an entity.
class NearestFoeQueryCallback : public b2QueryCallback
{
	public:
		///Constructor
		/// \param self Entity that is searching for ennemies.
		NearestFoeQueryCallback(entityx::Entity self);

		/// Check if the given fixture belongs to the nearest foe.
		/// \param fixture Fixture to check.
		/// \return True.
		bool ReportFixture(b2Fixture* fixture);

		entityx::Entity entity;///< The nearest entity.

	private:
		entityx::Entity m_self;       ///< Entity that is searching for ennemies.
		float m_distance;             ///< The shortest distance found.
};

//Moving

/// Checks if the entity can move.
/// The fist argument must be a valid entity.
/// \param args Entities that want to move.
/// \return true if the entity can move, false otherwise.
bool canMove(const std::vector<Data>& args);

/// Make that the entity move to the given direction.
/// \param args The entity that want move and the direction.
/// \return 0 as integer.
int move(const std::vector<Data>& args);

/// Make that the entity stop all his movements.
/// \param args Entities that want to stop to move.
/// \return 0 as integer.
int stop(const std::vector<Data>& args);

/// Checks if the entity can jump.
/// The fist argument must be a valid entity.
/// \param args Entities that want to jump.
/// \return True if the entity can jump, false otherwise.
bool canJump(const std::vector<Data>& args);

/// Make that the entity performs a jump.
/// \param args Entities that want to jump.
/// \return 0 as integer.
int jump(const std::vector<Data>& args);

/// Checks if an entity is valid.
/// \param args The entity to check.
/// \return true if the entity is valid, false otherwise.
bool isValid(const std::vector<Data>& args);

#endif//SCRIPTSFUNCTIONS_HPP