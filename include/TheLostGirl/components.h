#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <map>
#include <functional>
#include <string>

#include <SFML/System/Vector3.hpp>

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

/// BodyComponent component.
/// The BodyComponent component store a pointer to a b2Body and is used to simulate physics in the game world.
struct BodyComponent : public entityx::Component<BodyComponent>
{
	b2Body* body;///< Pointer to the physic body.

	/// Default constructor
	/// \param _body Pointer to the physic body.
	BodyComponent(b2Body* _body):
		body{_body}
	{}
};

/// Sprite component.
/// The Sprite component is an essential component for any visible entity.
/// For more information about sprites, see the SFML doc.
struct SpriteComponent : public entityx::Component<SpriteComponent>
{
	sf::Sprite* sprite;        ///< Pointer to the sprite to draw.
	sf::Vector3f worldPosition;///< x and y indicates the target position in the world,and z indicates in wich layer the sprite should be drawn.

	/// Default constructor
	/// \param _sprite Pointer to the sprite to draw.
	/// \param _worldPosition Indicate th' position and the layer the sprite should be drawn.
	SpriteComponent(sf::Sprite* _sprite, sf::Vector3f _worldPosition):
		sprite{_sprite},
		worldPosition{_worldPosition}
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

/// The jump component.
/// The JumpComponent component must be added to every entity that want to jump.
/// The jumpStrength member influences directly the height of the jump of the entity.
struct JumpComponent : public entityx::Component<JumpComponent>
{
	float jumpStrength;///< The power of the entity's jump.

	/// Default constructor
	/// \param _jumpStrength The power of the entity's jump.
	JumpComponent(float _jumpStrength = 1.f):
		jumpStrength{_jumpStrength}
	{}
};

/// Bending component.
/// The BendComponent must be added to every entity that want to bend a bow.
struct BendComponent : public entityx::Component<BendComponent>
{
	float angle;///< The current angle of the bow, in radians
	float power;///< The current power of the bending of the bow.
	float maxPower;///< The maximum power of the bending of the bow.

	/// Default constructor
	/// \param _maxPower The maximum power of the bending of the bow.
	BendComponent(float _maxPower = 1.f):
		angle{0.f},
		power{0.f},
		maxPower{_maxPower}
	{}
};

/// Falling component.
/// The fall component must be added to every entity that can fall.
struct FallComponent : public entityx::Component<FallComponent>
{
	bool inAir;                     ///< True when the entity falls, false otherwise.
	unsigned short int contactCount;///< Number of contacts between the actor and the ground.

	/// Default constructor
	FallComponent(bool _inAir, unsigned short int _contactCount):
		inAir{_inAir},
		contactCount{_contactCount}
	{}
};

/// Enumeration of every possible direction.
enum class Direction
{
	None,  ///< Unhandled direction.
	Left,  ///< Diriged to the left.
	Right, ///< Diriged to the right.
	Top,   ///< Diriged to the top.
	Bottom ///< Diriged to the bottom.
};

/// The Direction component.
/// It indicates where the entity is diriged to.
struct DirectionComponent : public entityx::Component<DirectionComponent>
{
	Direction direction;///< Indicate the effective direction of the entity.
	bool moveToLeft;    ///< Indicate if the entity want to move to left (e.g. left arrow key pressed).
	bool moveToRight;   ///< Indicate if the entity want to move to right (e.g. right arrow key pressed).

	/// Default constructor
	/// \param _direction Indicate the effective direction of the entity.
	DirectionComponent(Direction _direction = Direction::Left):
		direction{_direction},
		moveToLeft{false},
		moveToRight{false}
	{}
};

/// The walk component.
/// The walk component must be added to every entity that want to move itself in the world.
/// The walkSpeed member is the maximum speed that the entity can reach when walking.
struct WalkComponent : public entityx::Component<WalkComponent>
{
	float walkSpeed;            ///< The current speed of the entity.
	Direction effectiveMovement;///< Indicate the real movement of the entity.
	
	/// Default constructor
	/// \param _walkSpeed The current speed of the entity.
	WalkComponent(float _walkSpeed = 1.f):
		walkSpeed{_walkSpeed},
		effectiveMovement{Direction::None}
	{}
};

/// The category component.
struct CategoryComponent : public entityx::Component<CategoryComponent>
{
	unsigned int category;///< Category of the entity.

	/// Default constructor.
	/// \param _category Category of the entity.
	CategoryComponent(unsigned int _category = Category::None):
		category{_category}
	{}
};

/// The ActorID component.
/// Every entities with the same ActorID won't collide.
struct ActorIDComponent : public entityx::Component<ActorIDComponent>
{
	unsigned int ID;///< Identifier of the actor.

	/// Default constructor.
	/// \param _ID Identifier of the actor.
	ActorIDComponent(unsigned int _ID):
		ID{_ID}
	{}
};

/// The sky component
/// It indicates that the entity represents the sky,
/// and if it represent rather the day or the night.
struct SkyComponent : public entityx::Component<SkyComponent>
{
	bool day;///< True if it represent the day, false if it represents the night.

	/// Default constructor
	/// \param _day True if it represent the day, false if it represents the night.
	SkyComponent(bool _day):
		day{_day}
	{}
};

#endif //COMPONENTS_H