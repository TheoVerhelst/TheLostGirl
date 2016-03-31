#ifndef DEATH_HPP
#define DEATH_HPP

#include <TheLostGirl/Action.hpp>
#include <TheLostGirl/Context.hpp>

namespace entityx
{
    class Entity;
}

/// Structure that process the death of an entity
class Death : public Action
{
	public:
		/// Destructor.
		virtual ~Death() = default;

		/// Overload of the () operator.
		/// \param entity Entity that deads.
		virtual void operator()(entityx::Entity entity) const override;

	private:
		/// Context type of this class.
		typedef ContextAccessor<ContextElement::EntityManager> Context;
};

#endif//DEATH_HPP
