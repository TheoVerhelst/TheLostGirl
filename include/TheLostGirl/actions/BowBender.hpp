#ifndef BOWBENDER_HPP
#define BOWBENDER_HPP

#include <TheLostGirl/Action.hpp>

namespace entityx
{
    class Entity;
}

/// Structure that bend the bow of the given entity.
/// This action does not have a boolean parameter indicating if the action start or stops,
/// but must be called at every frame when the bending is active,
/// since the angle or the power can change.
class BowBender : public Action
{
	public:
		/// Constructor.
		/// \param _angle The angle of the bending.
		/// \param _power The power of the bending.
		BowBender(float _angle, float _power);

		/// Destructor.
		virtual ~BowBender() = default;

		/// Overload of the () operator.
		/// \param entity Entity that bends a bow.
		virtual void operator()(entityx::Entity entity) const override;

		float angle;///< The angle of the bending.
		float power;///< The power of the bending.
};

#endif//BOWBENDER_HPP
