#ifndef CORPSESEARCHER_HPP
#define CORPSESEARCHER_HPP

#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/Action.hpp>
#include <TheLostGirl/StateStack.hpp>
#include <TheLostGirl/Context.hpp>

/// Structure that searches on a corpse around the entity.
class CorpseSearcher : public Action
{
	public:
		/// Destructor.
		virtual ~CorpseSearcher() = default;

		/// Overload of the () operator.
		/// \param entity Entity that search in corpses.
		virtual void operator()(entityx::Entity entity) const override;
		
		static bool isCorpse(entityx::Entity entity);

	private:
		/// Context type of this class.
		typedef ContextAccessor<ContextElement::StateStack> Context;
};

#endif//CORPSESEARCHER_HPP
