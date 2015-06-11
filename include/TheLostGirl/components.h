#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Clock.hpp>
#include <entityx/Entity.h>
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
	b2Body body;///< Pointer to the physic body.
};

/// Sprite component.
/// The Sprite component is an essential component for any visible entity.
/// For more information about sprites, see the SFML doc.
struct SpriteComponent : public entityx::Component<SpriteComponent>
{
	sf::Sprite sprite;///< Sprite to draw.
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
/// It indicates where the entity should be located in the world,
/// and the angle of the entity.
/// The real position of the sprite is computed by the ScrollingSystem.
/// This bind position and angle between sprites and b2Body, but an entity can have only one of them.
/// In all case, if an entity have a SpriteComponent or a BodyComponent, it must have a TransformComponent.
struct TransformComponent : public entityx::Component<TransformComponent>
{
	Transform transform;///< Indicates the target position in the world, the layer the sprite should be drawn and the angle.
};

/// Animations manager component.
/// Essential for every animated entity in the game.
template<typename A>
struct AnimationsComponent : public entityx::Component<AnimationsComponent<A>>
{
	AnimationsManager<A> animationsManager;///< Managers for animations of any type.
};

/// Enumeration of every possible direction.
enum class Direction : int
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

/// The category component (player, aggressive, passive, ...).
struct CategoryComponent : public entityx::Component<CategoryComponent>
{
	unsigned int category;///< Category of the entity.
};

/// The item component.
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
	std::list<entityx::Entity> items;///< List of all stocked items.
	float weight;                    ///< Sum of the weight of every item.
	float maxWeight;                 ///< Maximum weight that that inventory can carry.
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
	bool mustJump;     ///< Indicate if the entity have received a jump command, and the jump is not yet handled by the physic system. Similar to moveToLeft in directionComponent.
};

/// Bending component that hold all stats about the ability to use bows and arrows.
struct BowComponent : public entityx::Component<BowComponent>
{
	float initialSpeed;               ///< The initial speed of the arrow when fired at maximum bending, in m/s.
	float power;                      ///< The current beding ratio of the bow, in [0, 1].
	float damages;                    ///< The damages that make a shoot.
	float angle;                      ///< The current angle of the bow, in radians.
	std::list<entityx::Entity> arrows;///< List of all stocked arrows.
	entityx::Entity notchedArrow;     ///< Current arrow notched in the bow.
	b2PrismaticJoint* joint;          ///< The joint that hold the arrow notched.
	unsigned int quiverCapacity;      ///< Maximum number of arrows in the quiver.
};

/// Health component.
struct HealthComponent : public entityx::Component<HealthComponent>
{
	float maximum;     ///< The maximum health.
	float current;     ///< The current health.
	float regeneration;///< The regeneration of health (in units/s).
};

/// Stamina component.
struct StaminaComponent : public entityx::Component<StaminaComponent>
{
	float maximum;     ///< The maximum stamina.
	float current;     ///< The current stamina.
	float regeneration;///< The regeneration of stamina (in units/s).
};

/// Arrow component that hold all stats about an arrow.
/// The friction is applied in order to set the angle of the body tangent to the arrow trajectory.
/// The penetrance indicates how much the arrow can stick into hard targets, and so make damages.
struct ArrowComponent : public entityx::Component<ArrowComponent>
{
	/// Possibles states of an arrow.
	enum ArrowState
	{
		Fired,  ///< The arrow is fired and is in air.
		Sticked,///< The arrow is sticked into something.
		Stored, ///< The arrow is stored into an inventory or in a quiver.
		Notched ///< The arrow is notched in a bow.
	};

	float friction;                 ///< The amplitude of the friction applied on the body.
	sf::Vector2f localFrictionPoint;///< The point where the friction must be applied.
	sf::Vector2f localStickPoint;   ///< The point where the arrow will be sticked when touch a target.
	float penetrance;               ///< Indicates how much the arrow can stick into hard targets.
	float damage;                   ///< Indicates the damage that the arrow can do on the target.
	ArrowState state;               ///< Indicates the current state of the arrow.
	entityx::Entity shooter;        ///< The entity that shooted the arrow at last, invalid of the arrow was never shooted.
};

/// Target hardness component that indicates the treshold of stucking of the arrow into the object.
/// This component must be applied on every object that can be the target of arrows.
struct HardnessComponent : public entityx::Component<HardnessComponent>
{
	float hardness;///< Indicates the treshold of stucking of the arrow into the object.
};

/// Script component that hold scripts names to execute on the entity.
/// The scripts are stored in the ressource manager.
/// They are executed each frame.
/// More documentation about scripting will be released soon.
struct ScriptsComponent : public entityx::Component<ScriptsComponent>
{
    std::list<std::string> scriptsNames;///< Names of scripts that the entity must execute,
};

/// Component that define the behavior of aggressive entities.
struct DetectionRangeComponent : public entityx::Component<DetectionRangeComponent>
{
	float detectionRange;///< Distance from wich the entitie will detect and attack foes.
};

/// Component that indicate if the entity is dead or not.
struct DeathComponent : public entityx::Component<DeathComponent>
{
	/// Store informations about the drop of an item on an entity when this one dead.
	/// For more informations about items, see ItemComponent.
	/// \see ItemComponent
	struct Drop
	{
		std::string category; ///< Category of item (bow, knife, helmet, ...).
		std::string type;     ///< Type of item (dark bow, simple wood arrow, ...).
		float probability;    ///< The probability that one item is dropped.
		unsigned int maxDrops;///< The maximum number of items that can be dropped.
	};
	bool dead;            ///< Indicate if the entity is dead or not.
	std::list<Drop> drops;///< List of all possible drops when the entity dead.
};


/// Store the name of an entity that will be displayed to the player.
struct NameComponent : public entityx::Component<NameComponent>
{
	std::string name;///< There is stored the english version of the name, translation is done by the LangManager.
};

/// Store data about hand to hand abilities.
struct HandToHandComponent : public entityx::Component<HandToHandComponent>
{
	float damages;     ///< Damages that generates an attack.
	sf::Time delay;    ///< Minimal time between two attacks.
	sf::Time lastShoot;///< Elapsed time since the last attack, used with timer below.
	sf::Clock timer;   ///< Timer to compute elapsed time, its value is added to lastShoot at each attack try.
};

/// Store all information that every actor must have, like resistance (because every actor can take damages).
struct ActorComponent : public entityx::Component<ActorComponent>
{
	float handToHandResistance;///< The resistance to hand to hand attacks.
	float magicResistance;     ///< The resistance to magic attacks
	float arrowResistance;     ///< The resistance to flying arrows
	float fireResistance;      ///< The resistance to fire.
	float frostResistance;     ///< The resistance to frost.
	float poisonResistance;    ///< The resistance to poison attacks.
};

#endif //COMPONENTS_H
