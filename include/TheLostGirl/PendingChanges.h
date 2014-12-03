#ifndef PENDINGCHANGES_H
#define PENDINGCHANGES_H

#include <queue>

#include <TheLostGirl/Command.h>

//Forward declarations
struct b2BodyDef;
class b2Body;
struct b2JointDef;
class b2Joint;

struct PendingChanges
{
	~PendingChanges()
	{}
	
	std::queue<Command> commandQueue;
	std::queue<b2BodyDef*> bodiesToCreate;
	std::queue<b2Body*> bodiesToDestroy;
	std::queue<b2JointDef*> jointsToCreate;
	std::queue<b2Joint*> jointsToDestroy;
};


#endif // PENDINGCHANGES_H

