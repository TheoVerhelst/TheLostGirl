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
	std::map<std::string, sf::Sprite> sprites;///< Sprites to draw.
};

/// Data about the world position of an entity.
/// This is intented to be used both by b2Body and sf::Sprite.
struct Transform
{
	float x;///< X-ordinate.
	float y;///< Y-ordinate.
	float z;///< Number of the plan.
	float angle;///< Angle of the entity.
};

/// World transform component.
/// It indicates where the entity should be located in the world, for each part of the entity,
/// and the angle of the entity.
/// The real position of the sprite is computed by the ScrollingSystem.
/// This bind position and angle between sprites and b2Body, but an entity can have only one of them.
/// In all case, if an entity have a SpriteComponent or a BodyComponent, it must have a TransformComponent,
/// and a transform must exists in the transforms map for every body/sprite of the entity.
struct TransformComponent : public entityx::Component<TransformComponent>
{
	std::map<std::string, Transform> transforms;///< Indicates the target position in the world, the layer the sprite should be drawn and the angle.
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

/// The Item component.
/// An item is just an entity with this component.
/// The category represents what class of item it is.
/// The type represents what model of item it is.
/// The icon of the item must have a path like resources/images/(resolution)/items/xxx/yyy.png,
/// where the type of the item is yyy and the category is xxx.
struct ItemComponent : public entityx::Component<ItemComponent>
{
	std::string category;///< Category of item (bow, knife, helmet, ...).
	std::string type;    ///< Type of item (dark bow, simple wood arrow, ...).
};

/// The inventory component.
struct InventoryComponent : public entityx::Component<InventoryComponent>
{
	std::vector<entityx::Entity> items;///< List of all stocked items.
	float weight;                      ///< Sum of the weight of every item.
	float maxWeight;                   ///< Maximum weight that that inventory can carry.
};

/// Falling component.
/// The fall component must be added to every entity that can fall.
struct FallComponent : public entityx::Component<FallComponent>
{
	bool inAir;                     ///< True when the entity falls, false otherwise.
	unsigned short int contactCount;///< Number of contacts between the actor and the ground.
	float fallingResistance;        ///< Resistance to falling.
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
