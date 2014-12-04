#ifndef ACTIONS_H
#define ACTIONS_H

#include <iostream>

#include <TheLostGirl/Action.h>

//Forward declarations
namespace entityx
{
	class Entity;
}
enum class Direction;

/// Structure that move an entity to a given side.
struct Mover : public Action
{
	/// Default constructor.
	/// \param _direction 
	/// \param _start True if the movement starts, false if it stops.
	Mover(Direction _direction, bool _start = true);
	
	/// Default destructor
	virtual ~Mover();
	
	/// Overload of the () operator.
	/// \param entity Entity who move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;
	
	Direction direction;///< Indicates the direction of the movement.
	bool start;         ///< True if the movement starts, false if it stops.
};

/// Structure that jump an entity.
struct Jumper : public Action
{
	/// Default constructor.
	Jumper();
	
	/// Default destructor
	virtual ~Jumper();
	
	/// Overload of the () operator.
	/// \param entity Entity who jump.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;
};

/// Structure that bend the bow of the given entity.
/// This action does not have a boolean parameter indicating if the action start or stops,
/// but must be called at every frame when the bending is active,
/// since the angle or the power can change.
struct BowBender : public Action
{
	/// Default constructor.
	/// \param _angle The angle of the bending.
	/// \param _power The power of the bending.
	BowBender(float _angle, float _power);
	
	/// Default destructor
	virtual ~BowBender();
	
	/// Overload of the () operator.
	/// \param entity Entity who bend.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;
	
	float angle;///< The angle of the bending.
	float power;///< The power of the bending.
};

/// Structure that shoot the arrow notched by the entity.
struct ArrowShooter : public Action
{
	/// Default constructor.
	ArrowShooter();
	
	/// Default destructor
	virtual ~ArrowShooter();
	
	/// Overload of the () operator.
	/// \param entity Entity who shoot.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;
};

#endif//ACTIONS_H
