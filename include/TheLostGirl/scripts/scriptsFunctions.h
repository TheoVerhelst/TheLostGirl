#ifndef SCRIPTSFUNCTIONS_H
#define SCRIPTSFUNCTIONS_H

#include <vector>

#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <entityx/Entity.h>

#include <TheLostGirl/scripts/Interpreter.h>

//Forward declarations
struct Command;

Data lowerThanOp(const std::vector<Data>& args, StateStack::Context context);
Data greaterThanOp(const std::vector<Data>& args, StateStack::Context context);
Data lowerEqualOp(const std::vector<Data>& args, StateStack::Context context);
Data greaterEqualOp(const std::vector<Data>& args, StateStack::Context context);
Data equalOp(const std::vector<Data>& args, StateStack::Context context);
Data notEqualOp(const std::vector<Data>& args, StateStack::Context context);
Data andOp(const std::vector<Data>& args, StateStack::Context context);
Data orOp(const std::vector<Data>& args, StateStack::Context context);
Data addOp(const std::vector<Data>& args, StateStack::Context context);
Data substractOp(const std::vector<Data>& args, StateStack::Context context);
Data multiplyOp(const std::vector<Data>& args, StateStack::Context context);
Data divideOp(const std::vector<Data>& args, StateStack::Context context);
Data moduloOp(const std::vector<Data>& args, StateStack::Context context);
Data notOp(const std::vector<Data>& args, StateStack::Context context);

Data print(const std::vector<Data>& args, StateStack::Context context);

//Foe handling
entityx::Entity nearestFoe(const std::vector<Data>& args, StateStack::Context context);
float distanceFrom(const std::vector<Data>& args, StateStack::Context context);
int directionTo(const std::vector<Data>& args, StateStack::Context context);
int attack(const std::vector<Data>& args, StateStack::Context context);

class NearestFoeQueryCallback : public b2QueryCallback
{
	public:
		NearestFoeQueryCallback(entityx::Entity, StateStack::Context);
		bool ReportFixture(b2Fixture* fixture);
		entityx::Entity entity;

	private:
		entityx::Entity m_self;
		float m_distance;
		StateStack::Context m_context;
};

//Moving
bool canMove(const std::vector<Data>& args, StateStack::Context context);
int move(const std::vector<Data>& args, StateStack::Context context);
int stop(const std::vector<Data>& args, StateStack::Context context);
bool canJump(const std::vector<Data>& args, StateStack::Context context);
int jump(const std::vector<Data>& args, StateStack::Context context);

#endif//SCRIPTSFUNCTIONS_H
