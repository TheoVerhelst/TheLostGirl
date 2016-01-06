#ifndef DESTRUCTIONLISTENER_HPP
#define DESTRUCTIONLISTENER_HPP

#include <Box2D/Box2D.h>

/// Handle the destruction of fixtures and joints
class DestructionListener : public b2DestructionListener
{
	public:
		/// Destructor.
		virtual ~DestructionListener();

		/// Handle the destruction of a joint.
		/// \param joint The joint that will be destroyed
		virtual void SayGoodbye(b2Joint* joint) override;

		/// Handle the destruction of a fixture.
		/// \param fixture The fixture that will be destroyed
		virtual void SayGoodbye(b2Fixture* fixture) override;
};

#endif//DESTRUCTIONLISTENER_HPP
