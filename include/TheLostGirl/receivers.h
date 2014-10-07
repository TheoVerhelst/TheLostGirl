#ifndef RECEIVERS_H
#define RECEIVERS_H

#include <entityx/entityx.h>

#include <TheLostGirl/events.h>

//struct Score : public entityx::Receiver<Score>
//{
//	Score(entityx::Entity _entity, unsigned int initialScore = 0):
//		entity(_entity),
//		count(initialScore)
//	{}
//	
//	void receive(const WonGame &wonGame)
//	{
//		if(wonGame.winner == entity)
//			count++;
//	}
//	entityx::Entity entity;
//	unsigned int count;
//};

#endif // RECEIVERS_H