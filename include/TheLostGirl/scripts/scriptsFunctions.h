#ifndef SCRIPTSFUNCTIONS_H
#define SCRIPTSFUNCTIONS_H

#include <vector>

#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <entityx/Entity.h>

#include <TheLostGirl/scripts/Interpreter.h>

//Forward declarations
struct Command;

/// Cast the variable to the templated type.
/// The variable must be a boost::variant such as defined in Interpreter.h,
/// and the cast will change the internal value of the variant.
/// \param var Variant instance to cast.
template <typename T>
void cast(Data& var)
{
}

/// Cast the variable to boolean type.
/// The variable must be a boost::variant such as defined in Interpreter.h,
/// and the cast will change the internal value of the variant.
/// \param var Variant instance to cast.
template <>
void cast<bool>(Data& var);

/// Cast the variable to integer type.
/// The variable must be a boost::variant such as defined in Interpreter.h,
/// and the cast will change the internal value of the variant.
/// \param var Variant instance to cast.
template <>
void cast<int>(Data& var);

/// Cast the variable to floating point type.
/// The variable must be a boost::variant such as defined in Interpreter.h,
/// and the cast will change the internal value of the variant.
/// \param var Variant instance to cast.
template <>
void cast<float>(Data& var);

/// Cast the variable to string type.
/// The variable must be a boost::variant such as defined in Interpreter.h,
/// and the cast will change the internal value of the variant.
/// \param var Variant instance to cast.
template <>
void cast<std::string>(Data& var);

/// Cast the variable to entity type.
/// The variable must be a boost::variant such as defined in Interpreter.h,
/// and the cast will change the internal value of the variant.
/// \param var Variant instance to cast.
template <>
void cast<entityx::Entity>(Data& var);

/// Implements the < operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the < operator.
Data lowerThanOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the > operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the > operator.
Data greaterThanOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the <= operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the <= operator.
Data lowerEqualOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the >= operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the >= operator.
Data greaterEqualOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the == operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the == operator.
Data equalOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the != operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the != operator.
Data notEqualOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the and operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the and operator.
Data andOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the or operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the or operator.
Data orOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the + operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the + operator.
Data addOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the - operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the - operator.
Data substractOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the * operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the * operator.
Data multiplyOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the / operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the / operator.
Data divideOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the % operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the % operator.
Data moduloOp(const std::vector<Data>& args, StateStack::Context context);

/// Implements the not operator in scripts between two variables.
/// \param args Operands of the operator.
/// \param context Current context of the application.
/// \return the resulting value of the not operator.
Data notOp(const std::vector<Data>& args, StateStack::Context context);


/// Print all variables passed as argument, no matter how many there are.
/// Printed variables are separated by a space
/// \param args Variables to print
/// \param context Current context of the application.
/// \return 0 as integer.
Data print(const std::vector<Data>& args, StateStack::Context context);

//Foe handling

/// Find the entity that is a foe and that is the nearest of the first argument, usually self.
/// First argument must be a valid entity.
/// \param args Entity that is searching for ennemies.
/// \param context Current context of the application.
/// \return The nearest entity if found, Entity() otherwhise.
entityx::Entity nearestFoe(const std::vector<Data>& args, StateStack::Context context);

/// Compute the distance between two entities.
/// Both argument must be valid entities, and must have a body component.
/// \param args The two entities.
/// \param context Current context of the application.
/// \return The distance.
float distanceFrom(const std::vector<Data>& args, StateStack::Context context);

/// Compute the direction that the first entity must have to look directly to the second entity.
/// Both argument must be valid entities, and must have a body component and transform component.
/// \param args The two entities.
/// \param context Current context of the application.
/// \return The integer value of the variable "left" or "right" available in scripts.
int directionTo(const std::vector<Data>& args, StateStack::Context context);

/// Return the direction that has an entity.
/// First argument must be a valid entity, and have a direction component.
/// \param args The entity.
/// \param context Current context of the application.
/// \return The direction of the entity.
int directionOf(const std::vector<Data>& args, StateStack::Context context);

/// Make that first entity attack second entity.
/// \param args Entities that will fight.
/// \param context Current context of the application.
/// \return 0 as integer.
int attack(const std::vector<Data>& args, StateStack::Context context);

/// Callback to find the nearest ennemy of an entity.
class NearestFoeQueryCallback : public b2QueryCallback
{
	public:
		///Constructor
		/// \param self Entity that is searching for ennemies.
		/// \param context Current context of the application.
		NearestFoeQueryCallback(entityx::Entity self, StateStack::Context context);

		/// Check if the given fixture belongs to the nearest foe.
		/// \param fixture Fixture to check.
		/// \return True.
		bool ReportFixture(b2Fixture* fixture);

		entityx::Entity entity;///< The nearest entity.

	private:
		entityx::Entity m_self;       ///< Entity that is searching for ennemies.
		float m_distance;             ///< The shortest distance found.
		StateStack::Context m_context;///< Current context of the application.
};

//Moving

/// Checks if the entity can move.
/// The fist argument must be a valid entity.
/// \param args Entities that want to move.
/// \param context Current context of the application.
/// \return True if the entity can move, false otherwhise.
bool canMove(const std::vector<Data>& args, StateStack::Context context);

/// Make that the entity move to the given direction.
/// \param args The entity that want move and the direction.
/// \param context Current context of the application.
/// \return 0 as integer.
int move(const std::vector<Data>& args, StateStack::Context context);

/// Make that the entity stop all his movements.
/// \param args Entities that want to stop to move.
/// \param context Current context of the application.
/// \return 0 as integer.
int stop(const std::vector<Data>& args, StateStack::Context context);

/// Checks if the entity can jump.
/// The fist argument must be a valid entity.
/// \param args Entities that want to jump.
/// \param context Current context of the application.
/// \return True if the entity can jump, false otherwhise.
bool canJump(const std::vector<Data>& args, StateStack::Context context);

/// Make that the entity performs a jump.
/// \param args Entities that wantto jump.
/// \param context Current context of the application.
/// \return 0 as integer.
int jump(const std::vector<Data>& args, StateStack::Context context);

#endif//SCRIPTSFUNCTIONS_H
