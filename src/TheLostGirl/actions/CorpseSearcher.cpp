#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/Box2DHelper.hpp>
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
	std::set<entityx::Entity> foundEntities{Box2DHelper::queryEntities(searchBox, isCorpse)};
	if(not foundEntities.empty())
		Context::getStateStack().pushState<OpenInventoryState>(*foundEntities.begin());
}

bool CorpseSearcher::isCorpse(entityx::Entity entity)
{
	const DeathComponent::Handle deathComponent(entity.component<DeathComponent>());
	// Return true only if this is a corpse with an inventory
	return entity.has_component<InventoryComponent>() and deathComponent and deathComponent->dead;
}
