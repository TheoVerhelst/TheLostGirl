#include <entityx/Entity.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/states/OpenInventoryState.h>
#include <TheLostGirl/actions/CorpseSearcher.h>

CorpseSearcher::CorpseSearcher(StateStack& stateStack):
	m_stateStack(stateStack)
{
}

CorpseSearcher::~CorpseSearcher()
{
}

void CorpseSearcher::operator()(entityx::Entity entity, double) const
{
	if(not entity)
		return;
	if(entity.has_component<TransformComponent>() and entity.has_component<BodyComponent>())
	{
		BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
		if(bodyComponent->bodies.find("main") != bodyComponent->bodies.end())
		{
			b2Body* body{bodyComponent->bodies.at("main")};
			b2World* world{body->GetWorld()};

			//Do the querying
			b2AABB searchBox;
			searchBox.lowerBound = body->GetWorldCenter() - b2Vec2(2, 2);
			searchBox.upperBound = body->GetWorldCenter() + b2Vec2(2, 2);
			CorpseQueryCallback callback;
			world->QueryAABB(&callback, searchBox);

			if(callback.foundEntity.valid())
				m_stateStack.pushState<OpenInventoryState>(callback.foundEntity);
		}
	}
}

bool CorpseQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity entity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	//Return false (and so stop) only if this is a arrow and if this one is sticked.
	bool found{entity.has_component<InventoryComponent>()
			and entity.has_component<DeathComponent>()
			and entity.component<DeathComponent>()->dead};
	if(found)
		foundEntity = entity;
	return not found;
}
