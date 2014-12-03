#ifndef PENDINGCHANGES
#define PENDINGCHANGES

#include <queue>

//Forward declarations
struct Command;
struct b2BodyDef;
class b2Body;
struct b2JointDef;
class b2Joint;

struct PendingChanges
{
	std::queue<Command> commandQueue;
	std::queue<b2BodyDef*> bodiesToCreate;
	std::queue<b2Body*> bodiesToDestroy;
	std::queue<b2JointDef*> jointsToCreate;
	std::queue<b2Joint*> jointsToDestroy;
};


#endif // PENDINGCHANGES

