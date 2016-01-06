#ifndef ACTION_HPP
#define ACTION_HPP

//Forward declarations
namespace entityx
{
	class Entity;
}

/// Base class for every action.
class Action
{
	public:
		/// Destructor.
		virtual ~Action() = default;

		/// Overload of the () operator.
		/// \param entity Entity which do the action.
		virtual void operator()(entityx::Entity entity) const = 0;
};

#endif//ACTION_HPP
