#ifndef HANDTOHAND_HPP
#define HANDTOHAND_HPP

#include <unordered_set>

#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/Action.hpp>
#include <TheLostGirl/HashEntity.hpp>

/// Performs a hand to hand attack in front on the entity.
/// This attack strikes all entities that are near the entity at once.
class HandToHand : public Action
{
	public:
		/// Constructor.
		HandToHand() = default;

		/// Destructor.
		virtual ~HandToHand() = default;

		/// Overload of the () operator.
		/// \param entity Entity that does the action.
		virtual void operator()(entityx::Entity entity) const override;
		
		static bool isHittableEntity(entityx::Entity entity, entityx::Entity attacker);
};
#endif//HANDTOHAND_HPP
