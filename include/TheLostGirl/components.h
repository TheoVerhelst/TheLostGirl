#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <map>
#include <functional>
#include <string>

#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/Entity.h>
#include <dist/json/json-forwards.h>

#include <TheLostGirl/Category.h>
#include <TheLostGirl/AnimationsManager.h>

//Forward declarations
class b2Body;
template<typename T>
class AnimationsManager;

/// \file components.h

/// Box2D body component.
/// The BodyComponent component store a pointer to a b2Body and is used to simulate physics in the game world.
struct BodyComponent : public entityx::Component<BodyComponent>
{
	std::map<std::string, b2Body*> bodies;///< Pointers to the physic body.
};

/// Sprite component.
/// The Sprite component is an essential component for any visible entity.
/// For more information about sprites, see the SFML doc.
struct SpriteComponent : public entityx::Component<SpriteComponent>
{
	std::map<std::string, sf::Sprite> sprites;        ///< Sprites to draw.
	std::map<std::string, sf::Vector2f> worldPositions;///< Indicates the target position in the world.
	float plan;                                       ///< Indicates in wich layer the sprite should be drawn.
};

/// AnimationsManager component.
/// Essential for every dynamic entity in the game.
template<typename A>
struct AnimationsComponent : public entityx::Component<AnimationsComponent<A>>
{
	std::map<const std::string, AnimationsManager<A>> animationsManagers;///< AnimationsManager managers.
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
};

/// The category component.
struct CategoryComponent : public entityx::Component<CategoryComponent>
{
	unsigned int category;///< Category of the entity.
};

/// The ActorID component.
/// Every entities with the same ActorID won't collide.
struct ActorIDComponent : public entityx::Component<ActorIDComponent>
{
	unsigned int ID;///< Identifier of the actor.
};

/// The sky component
/// It indicates that the entity represents the sky,
/// and if it represent rather the day or the night.
struct SkyComponent : public entityx::Component<SkyComponent>
{
	bool day;///< True if it represent the day, false if it represents the night.
};

/// Falling component.
/// The fall component must be added to every entity that can fall.
struct FallComponent : public entityx::Component<FallComponent>
{
	bool inAir;                     ///< True when the entity falls, false otherwise.
	unsigned short int contactCount;///< Number of contacts between the actor and the ground.
};

/// The walk component.
/// The walk component must be added to every entity that want to move itself in the world.
/// The walkSpeed member is the maximum speed that the entity can reach when walking.
struct WalkComponent : public entityx::Component<WalkComponent>
{
	float walkSpeed;            ///< The current speed of the entity.
	Direction effectiveMovement;///< Indicate the real movement of the entity.
};

/// The jump component.
/// The JumpComponent component must be added to every entity that want to jump.
/// The jumpStrength member influences directly the height of the jump of the entity.
struct JumpComponent : public entityx::Component<JumpComponent>
{
	float jumpStrength;///< The power of the entity's jump.
};

/// Bending component.
struct BendComponent : public entityx::Component<BendComponent>
{
	float maxPower;///< The maximum power of the bending of the bow.
	float power;   ///< The current power of the bending of the bow.
	float angle;   ///< The current angle of the bow, in radians.
};

/// Health component.
struct HealthComponent : public entityx::Component<HealthComponent>
{
	float maxHealth;///< The maximum health.
	float health;///< The current health.
};

/// Stamina component.
struct StaminaComponent : public entityx::Component<StaminaComponent>
{
	float maxStamina;///< The maximum stamina.
	float stamina;///< The current stamina.
};

#endif //COMPONENTS_H
