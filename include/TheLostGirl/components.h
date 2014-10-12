#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <map>
#include <functional>

#include <SFML/Graphics/Sprite.hpp>

//Forward declarations
namespace entityx
{
	template <typename Derived>
	struct Component;
}
class b2Body;

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

/// Animations component.
/// To use this component, you must add some animations in the animations map,
/// set playAnimation to true and set the currentAnimation identifier
/// according to the animation to play.
/// Then a simple update in the animations system do all that need to be done.
///
/// Usage exemple:
/// \code
/// //Add the component :
/// Animations::Handle animationsComponent = entity->assign<Animations<sf::Sprite, Archer::Animations> >();
/// 
/// //Make a TimeAnimation structure :
/// SpriteSheetAnimation anim;
/// anim.addFrame(rect1, 0.1f);
/// anim.addFrame(rect2, 0.9f);
/// Animations::TimeAnimation timeAnim(anim, sf::Time::seconds(3.f));
/// 
/// //Add the animation to the entity
/// animationsComponent->animations.insert(Archer::Animations::Run, timeAnim);
/// animationsComponent->playAnimation = true;
/// animationsComponent->currentAnimation = Archer::Animations::Run;
/// \endcode
template<typename Animated, typename Identifier>
struct Animations : public entityx::Component<Animations<Animated, Identifier> >
{
	
	typedef std::function<void(Animated&, float)> Animation;
	
	/// Animation associated with a certain duration.
	struct TimeAnimation
	{
		Animation animation;///< Functor of the animation.
		sf::Time duration;///< Duration of the animation.
		
		/// Default constructor.
		/// \param _animation Functor of the animation.
		/// \param _duration Duration of the animation.
		TimeAnimation(Animation _animation, sf::Time _duration):
			animation(_animation),
			duration(_duration)
		{}
	};
	std::map<Identifier, TimeAnimation> animations;///< List of all animation registred.
	bool playAnimation;///< Indicates if an animation ned to be played.
	Identifier currentAnimation;///<Identifier of the animation to play.
	float currentProgress;///< Progress of the current animation, in the range [0,1]
	bool currentAnimationDoesLoop;///< Indicates if the current animation must loop or not.
	
	/// Default constructor.
	Animations():
		animations(),
		playAnimation{false},
		currentAnimation(),
		currentProgress{0.f},
		currentAnimationDoesLoop{false}
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