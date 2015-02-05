#ifndef SCRIPTSFUNCTIONS_H
#define SCRIPTSFUNCTIONS_H

#include <iostream>
#include <queue>

#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <entityx/Entity.h>

//Forward declarations
struct Command;

//Foe handling
entityx::Entity nearestFoe(entityx::Entity self, float pixelByMeter);
float distanceFrom(entityx::Entity self, entityx::Entity target);
int directionTo(entityx::Entity self, entityx::Entity target);
int attack(entityx::Entity self, entityx::Entity target);

class NearestFoeQueryCallback : public b2QueryCallback
{
	public:
		NearestFoeQueryCallback(entityx::Entity);
		bool ReportFixture(b2Fixture* fixture);
		entityx::Entity entity;

	private:
		entityx::Entity m_self;
		float m_distance;
};

//Moving
bool canMove(entityx::Entity self);
int move(entityx::Entity self, int direction);
int stop(entityx::Entity self);
bool canJump(const entityx::Entity self);
int jump(const entityx::Entity self, std::queue<Command>& commandQueue);

//Debugging
template <typename T>
int print(T val)
{
	if(typeid(val) == typeid(bool))
		std::cout << (val ? "True\n" : "False\n");
	else
		std::cout << val << "\n";
	return 0;
}

#endif// SCRIPTSFUNCTIONS_H
