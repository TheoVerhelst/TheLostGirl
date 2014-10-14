#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <map>
#include <functional>
#include <string>

//Forward declarations
namespace sf
{
	class Sprite;
}
namespace entityx
{
	template <typename Derived>
	struct Component;
}
class b2Body;
class Animations;

/// Body component.
/// The Body component store a pointer to a b2Body and is used to simulate physics in the game world.
struct Body : public entityx::Component<Body>
{
	b2Body* body;///< Pointer to the physic body.
	
	/// Default constructor
	/// \param _body Pointer to the physic body.
	Body(b2Body* _body = nullptr):
		body(_body)
	{}
};

/// Sprite component.
/// The Sprite component is an essential component for any visible entity.
/// For more information about sprites, see the SFML doc.
struct SpriteComponent : public entityx::Component<SpriteComponent>
{
	sf::Sprite* sprite;///< Pointer to the sprite to draw.
	
	/// Default constructor
	/// \param _sprite Pointer to the sprite to draw.
	SpriteComponent(sf::Sprite* _sprite = nullptr):
		sprite(_sprite)
	{}
};

/// Component that hold a pointer to an Animation instances.
/// Essential for every dynamic entity in the game.
struct AnimationsComponent : public entityx::Component<AnimationsComponent>
{
	Animations* animations;///< Pointer to the animations manager.
	
	/// Default constructor.
	/// \param _animations Pointer to the animations manager.
	AnimationsComponent(Animations* _animations):
		animations(_animations)
	{}
};

/// Walk component.
/// The walk component must be added to every entity that want to move itself in the world.
/// The walkSpeed member is the maximum speed that the entity can reach when walking.
struct Walk : public entityx::Component<Walk>
{
	float walkSpeed;///< The current speed of the entity.
	
	/// Default constructor
	/// \param _walkSpeed The current speed of the entity.
	Walk(float _walkSpeed = 0):
		walkSpeed(_walkSpeed)
	{}
};

/// Jump component.
/// The Jump component must be added to every entity that want to jump.
/// The jumpStrength member influences directly the height of the jump of the entity.
struct Jump : public entityx::Component<Jump>
{
	float jumpStrength;///< The power of the entity's jump.
	
	/// Default constructor
	/// \param _jumpStrength The power of the entity's jump.
	Jump(float _jumpStrength = 0):
		jumpStrength(_jumpStrength)
	{}
};

/// The Controller component.
/// It indicates if the entity is controlled by the player or not.
struct Controller : public entityx::Component<Controller>
{
	bool isPlayer;///< That cannot be more explicit.
	
	/// Default constructor
	/// \param _isPlayer that cannot be more explicit.
	Controller(bool _isPlayer = false):
		isPlayer(_isPlayer)
	{}
};

#endif //COMPONENTS_H