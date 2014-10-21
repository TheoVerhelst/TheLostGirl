#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <map>
#include <functional>
#include <string>

#include <TheLostGirl/Category.h>

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
		body{_body}
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
		sprite{_sprite}
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
		animations{_animations}
	{}
};

/// Walk component.
/// The walk component must be added to every entity that want to move itself in the world.
/// The walkSpeed member is the maximum speed that the entity can reach when walking.
struct Walk : public entityx::Component<Walk>
{
	float walkSpeed;   ///< The current speed of the entity.
	bool moveToLeft;   ///< Indicate if the entity want to move to left.
	bool moveToRight;  ///< Indicate if the entity want to move to right.
	
	/// Default constructor
	/// \param _walkSpeed The current speed of the entity.
	Walk(float _walkSpeed = 1.f):
		walkSpeed{_walkSpeed},
		moveToLeft{false},
		moveToRight{false}
	{}
};

/// Jump component.
/// The Jump component must be added to every entity that want to jump.
/// The jumpStrength member influences directly the height of the jump of the entity.
struct Jump : public entityx::Component<Jump>
{
	float jumpStrength;///< The power of the entity's jump.
	bool isJumping;///< True when the entity jumps, false otherwise.
	
	/// Default constructor
	/// \param _jumpStrength The power of the entity's jump.
	Jump(float _jumpStrength = 0):
		jumpStrength{_jumpStrength},
		isJumping{false}
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
		isPlayer{_isPlayer}
	{}
};

/// The Direction component.
/// It indicates where the entity is diriged to.
struct Direction : public entityx::Component<Direction>
{
	bool toLeft;///< That cannot be more explicit.
	
	/// Default constructor
	/// \param _isPlayer that cannot be more explicit.
	Direction(bool _toLeft = true):
		toLeft{_toLeft}
	{}
};

/// The category component.
struct CategoryComponent : public entityx::Component<Controller>
{
	unsigned int category;///< Category of the entity.
	
	/// Default constructor.
	/// \param _category Category of the entity.
	CategoryComponent(unsigned int _category = Category::None):
		category{_category}
	{}
};

#endif //COMPONENTS_H