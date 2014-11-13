#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <map>
#include <functional>
#include <string>

#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/Entity.h>

#include <TheLostGirl/Category.h>
#include <TheLostGirl/Animations.h>

//Forward declarations
class b2Body;
template<typename T>
class Animations;

/// \file components.h

/// Box2D body component.
/// The BodyComponent component store a pointer to a b2Body and is used to simulate physics in the game world.
struct BodyComponent : public entityx::Component<BodyComponent>
{
	std::map<std::string, b2Body*> bodies;///< Pointers to the physic body.
	
	/// Default constructor.
	/// \param _bodies Pointer to the physic body.
	BodyComponent(const std::map<std::string, b2Body*>& _bodies):
		bodies{_bodies}
	{}
};

/// Sprite component.
/// The Sprite component is an essential component for any visible entity.
/// For more information about sprites, see the SFML doc.
struct SpriteComponent : public entityx::Component<SpriteComponent>
{
	std::map<std::string, sf::Sprite> sprites;        ///< Sprites to draw.
	std::map<std::string, sf::Vector2f> worldPositions;///< Indicates the target position in the world.
	float plan;                                       ///< Indicates in wich layer the sprite should be drawn.
	
	/// Default constructor.
	/// \param _sprites Sprite to draw.
	/// \param _worldPositions Indicates the position where the sprite should be drawn.
	/// \param _plan Indicates the layer wherein the sprite should be drawn.
	SpriteComponent(const std::map<std::string, sf::Sprite>& _sprites, const std::map<std::string, sf::Vector2f>& _worldPositions, float _plan):
		sprites{_sprites},
		worldPositions{_worldPositions},
		plan{_plan}
	{}
};

/// Animations component.
/// Essential for every dynamic entity in the game.
template<typename T>
struct AnimationsComponent : public entityx::Component<AnimationsComponent<T>>
{
	std::map<std::string, Animations<T>> animationsManagers;///< Animations managers.

	/// Default constructor.
	/// \param _animationsManagers Animations manager.
	AnimationsComponent(const std::map<std::string, Animations<T>>& _animationsManagers):
		animationsManagers{_animationsManagers}
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
	bool moveToRight;   ///< Indicate if the entity want to move to right (e.g. left arrow key pressed).

	/// Default constructor
	/// \param _direction Indicate the effective direction of the entity.
	/// \param _moveToLeft Indicate if the entity want to move to left (e.g. left arrow key pressed).
	/// \param _moveToRight Indicate if the entity want to move to right (e.g. right arrow key pressed).
	DirectionComponent(Direction _direction, bool _moveToLeft, bool _moveToRight):
		direction{_direction},
		moveToLeft{_moveToLeft},
		moveToRight{_moveToRight}
	{}
};

/// The category component.
struct CategoryComponent : public entityx::Component<CategoryComponent>
{
	unsigned int category;///< Category of the entity.

	/// Default constructor.
	/// \param _category Category of the entity.
	CategoryComponent(unsigned int _category):
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

/// Falling component.
/// The fall component must be added to every entity that can fall.
struct FallComponent : public entityx::Component<FallComponent>
{
	bool inAir;                     ///< True when the entity falls, false otherwise.
	unsigned short int contactCount;///< Number of contacts between the actor and the ground.

	/// Default constructor
	/// \param _inAir True when the entity falls, false otherwise.
	/// \param _contactCount Number of contacts between the actor and the ground.
	FallComponent(bool _inAir, unsigned short int _contactCount):
		inAir{_inAir},
		contactCount{_contactCount}
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
	/// \param _effectiveMovemen Indicate the real movement of the entity.
	WalkComponent(float _walkSpeed, Direction _effectiveMovemen):
		walkSpeed{_walkSpeed},
		effectiveMovement{_effectiveMovemen}
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
	JumpComponent(float _jumpStrength):
		jumpStrength{_jumpStrength}
	{}
};

/// Bending component.
struct BendComponent : public entityx::Component<BendComponent>
{
	float maxPower;///< The maximum power of the bending of the bow.
	float power;   ///< The current power of the bending of the bow.
	float angle;   ///< The current angle of the bow, in radians.

	/// Default constructor
	/// \param _maxPower The maximum power of the bending of the bow.
	/// \param _power The current power of the bending of the bow.
	/// \param _angle TThe current angle of the bow, in radians.
	BendComponent(float _maxPower, float _power, float _angle):
		maxPower{_maxPower},
		power{_power},
		angle{_angle}
	{}
};

/// Health component.
struct HealthComponent : public entityx::Component<HealthComponent>
{
	float maxHealth;///< The maximum health.
	float health;///< The current health.

	/// Default constructor
	/// \param _maxHealth The maximum health.
	/// \param _health The current health.
	HealthComponent(float _maxHealth, float _health):
		maxHealth{_maxHealth},
		health{_health}
	{}
};

/// Stamina component.
struct StaminaComponent : public entityx::Component<StaminaComponent>
{
	float maxStamina;///< The maximum stamina.
	float stamina;///< The current stamina.

	/// Default constructor
	/// \param _maxStamina The maximum stamina.
	/// \param _stamina The current stamina.
	StaminaComponent(float _maxStamina, float _stamina):
		maxStamina{_maxStamina},
		stamina{_stamina}
	{}
};

#endif //COMPONENTS_H