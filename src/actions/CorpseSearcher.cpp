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
	BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
	if(bodyComponent)
	{
		auto bodyIt(bodyComponent->bodies.find("main"));
		if(bodyIt != bodyComponent->bodies.end())
		{
			//Do the querying
			b2AABB searchBox;
			searchBox.lowerBound = bodyIt->second->GetWorldCenter() - b2Vec2(2, 2);
			searchBox.upperBound = bodyIt->second->GetWorldCenter() + b2Vec2(2, 2);
			CorpseQueryCallback callback;
			bodyIt->second->GetWorld()->QueryAABB(&callback, searchBox);

			if(callback.foundEntity.valid())
				m_stateStack.pushState<OpenInventoryState>(callback.foundEntity);
		}
	}
}

bool CorpseQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity entity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	const DeathComponent::Handle deathComponent(entity.component<DeathComponent>());
	//Return false (and so stop) only if this is a corpse with an inventory
	if(entity.has_component<InventoryComponent>() and deathComponent and deathComponent->dead)
	{
		foundEntity = entity;
		return true;
	}
	else
		return false;
}
