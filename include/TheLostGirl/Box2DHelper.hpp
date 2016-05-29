#ifndef BOX2DHELPER_HPP
#define BOX2DHELPER_HPP

#include <functional>
#include <set>
#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/Context.hpp>

/// Class that helps in various tasks related to Box2D.
class Box2DHelper
{
	public:
		/// Searches for entities in the specified axis-aligned bounding box
		/// that satisfies a given boolean predicate.
		/// \param aabb The axis-aligned bounding box to search in.
		/// \param predicate A function that returns true for the searched
		/// entity.
		/// \return The found entities if some are found, or an empty set
		/// otherwhise.
		static std::set<entityx::Entity> queryEntities(const b2AABB& aabb,
				const std::function<bool(entityx::Entity)>& predicate = [](entityx::Entity){return false;});

	private:
		/// Context type of this class.
		typedef ContextAccessor<ContextElement::World> Context;
		
		/// AABB query callback that indicates if an entity is found into the AABB.
		class EntityQueryCallback : public b2QueryCallback
		{
			public:
				/// Constructor
				/// \param predicate The function to test entities, as described
				/// in \a Box2DHelper::queryEntities.
				EntityQueryCallback(const std::function<bool(entityx::Entity)>& predicate);
				
				/// Handles the world querying.
				/// \param fixture A fixture that overlaps the AABB.
				/// \return True if the querying should continue, false otherwise.
				virtual bool ReportFixture(b2Fixture* fixture) override;
				
				/// Gets the found entity.
				/// \return The found entity, or an invalid entity if no one was
				/// found.
				std::set<entityx::Entity> getFoundEntities() const;
			
			private:
				const std::function<bool(entityx::Entity)>& m_predicate;///< The function to test entities.
				std::set<entityx::Entity> m_foundEntities;              ///< If no entity was found, then this one isn't valid.
		};
};

#endif//BOX2DHELPER_HPP
