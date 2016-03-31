#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/states/OpenInventoryState.hpp>
#include <TheLostGirl/actions/CorpseSearcher.hpp>

void CorpseSearcher::operator()(entityx::Entity entity) const
{
	if(not TEST(entity.valid()))
		return;
	BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
	if(not TEST(bodyComponent))
		return;
	//Do the querying
	b2AABB searchBox;
	searchBox.lowerBound = bodyComponent->body->GetWorldCenter() - b2Vec2(2, 2);
	searchBox.upperBound = bodyComponent->body->GetWorldCenter() + b2Vec2(2, 2);
	CorpseQueryCallback callback;
	bodyComponent->body->GetWorld()->QueryAABB(&callback, searchBox);
	if(callback.foundEntity.valid())
		Context::getStateStack().pushState<OpenInventoryState>(callback.foundEntity);
}

bool CorpseQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity entity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	if(not TEST(entity.valid()))
		return true;
	const DeathComponent::Handle deathComponent(entity.component<DeathComponent>());
	//Return false (and so stop) only if this is a corpse with an inventory
	if(entity.has_component<InventoryComponent>() and deathComponent and deathComponent->dead)
	{
		foundEntity = entity;
		return false;
	}
	else
		return true;
}
