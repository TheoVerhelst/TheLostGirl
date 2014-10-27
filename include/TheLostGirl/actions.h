#ifndef ACTIONS_H
#define ACTIONS_H

#include <iostream>

#include <TheLostGirl/Action.h>

//Forward declarations
namespace entityx
{
	class Entity;
}

/// Structure that move an entity to the left.
struct LeftMover : public Action
{
	/// Default constructor.
	/// \param _start True if the movement starts, false if it stops.
	LeftMover(bool _start);
	
	/// Default destructor
	virtual ~LeftMover();
	
	/// Overload of the () operator.
	/// \param entity Entity to move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity& entity, double dt) const;
	
	bool start;///< True if the movement starts, false if it stops.
};

/// Structure that move an entity to the right.
struct RightMover : public Action
{
	/// Default constructor.
	/// \param _start True if the movement starts, false if it stops.
	RightMover(bool _start);
	
	/// Default destructor
	virtual ~RightMover();
	
	/// Overload of the () operator.
	/// \param entity Entity to move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity& entity, double dt) const;
	
	bool start;///< True if the movement starts, false if it stops.
};

/// Structure that move an entity to the top.
struct UpMover : public Action
{
	/// Default constructor.
	/// \param _start True if the movement starts, false if it stops.
	UpMover(bool _start);
	
	/// Default destructor
	virtual ~UpMover();
	
	/// Overload of the () operator.
	/// \param entity Entity to move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity& entity, double dt) const;
	
	bool start;///< True if the movement starts, false if it stops.
};

/// Structure that move an entity to the bottom.
struct DownMover : public Action
{
	/// Default constructor.
	/// \param _start True if the movement starts, false if it stops.
	DownMover(bool _start);
	
	/// Default destructor
	virtual ~DownMover();
	
	/// Overload of the () operator.
	/// \param entity Entity to move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity& entity, double dt) const;
	
	bool start;///< True if the movement starts, false if it stops.
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