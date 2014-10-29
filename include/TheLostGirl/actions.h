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
	/// \param entity Entity to move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity& entity, double dt) const;
	
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
	/// \param entity Entity to move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity& entity, double dt) const;
};

/// Structure that bend the given bow.
struct BowBender : public Action
{
	/// Default constructor.
	/// \param _start True if the bending starts, false if it stops.
	BowBender(bool _start);
	
	/// Default destructor
	virtual ~BowBender();
	
	/// Overload of the () operator.
	/// \param entity Entity to move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity& entity, double dt) const;
	
	bool start;///< True if the bending starts, false if it stops.
};

#endif // ACTIONS_H