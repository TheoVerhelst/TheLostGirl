#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/Box2DHelper.hpp>

std::set<entityx::Entity> Box2DHelper::queryEntities(const b2AABB& aabb, const std::function<bool(entityx::Entity)>& predicate)
{
	EntityQueryCallback callback{predicate};
	Context::getWorld().QueryAABB(&callback, aabb);
	return callback.getFoundEntities();
}

Box2DHelper::EntityQueryCallback::EntityQueryCallback(const std::function<bool(entityx::Entity)>& predicate):
	m_predicate{predicate}
{
}

bool Box2DHelper::EntityQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity entity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	if(not TEST(entity.valid()))
		return true;
		
	//Return false (so stop) if the predicate is true
	const bool found{m_predicate(entity)};
	if(found)
		m_foundEntities.insert(entity);
	return not found;
}

std::set<entityx::Entity> Box2DHelper::EntityQueryCallback::getFoundEntities() const
{
	return m_foundEntities;
}
