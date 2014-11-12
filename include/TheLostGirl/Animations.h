#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <functional>
#include <string>
#include <map>

#include <SFML/System/Time.hpp>

//Forward declarations
namespace entityx
{
	class Entity;
}
/// Animations class.
/// To use this class, you must add some animations
/// and play one of them with the play() member.
/// Then a simple update call in the animations system do all that need to be done.
///
/// Usage exemple:
/// \code
/// //Make a TimeAnimation structure :
/// SpriteSheetAnimation anim;
/// anim.addFrame(rect1, 0.1f);
/// anim.addFrame(rect2, 0.9f);
///
/// //Add the component to the entity :
/// AnimationsComponent::Handle animationsComponent = entity->assign<AnimationsComponent>(Animations<sf::Sprite>());
/// //Add the animation to the animations manager
/// animationsComponent->animations.addAnimation("run", anim, 1, sf::seconds(3.f), true);
/// animationsComponent->animations.play("run");
/// \endcode
/// \see AnimationSystem
///
/// This animation system is inspired by the Animator system from the Jan Haller's Thor C++ library.
template<typename T>
class Animations
{
	public:
		/// Animation associated with a certain duration.
		struct TimeAnimation
		{
			std::function<void(T&, float)> animation;///< Functor to  the concrete animation.
			unsigned short int importance;           ///< Indicates the importance of the animation, relatively to the others.
			sf::Time duration;                       ///< Duration of the animation.
			bool loops;                              ///< Indicates if the animation must loop or not.
			float progress;                          ///< Current progress of the animation, in the range [0,1].
			bool isPaused;                           ///< Indicates if the animations is paused.
			bool isActive;                           ///< Indicates if the animations is active.

			/// Default constructor.
			/// \param _animation Functor of the animation.
			/// \param _importance Indicates the importance of the animation, relatively to the others.
			/// \param _duration Duration of the animation.
			/// \param _loops Indicates if the animation must loop or not.
			TimeAnimation(std::function<void(T&, float)> _animation, unsigned short int _importance = 0, sf::Time _duration = sf::seconds(1.f), bool _loops = false);
		};

		/// Default constructor
		Animations();

		/// Default destructor
		~Animations();

		/// Add a new animation to the manager.
		/// \param identifier Unique identifier of the animation to add.
		/// \param animation Functor to the animation.
		/// \param importance Order of importance of the animation, relatively to others animations.
		/// \param duration Duration fo the animation.
		/// \param loop Indicate if the animation must loop or not.
		/// \see removeAnimation
		void addAnimation(const std::string& identifier, std::function<void(T&, float)> animation, unsigned short int importance = 0, sf::Time duration = sf::seconds(1.f), bool loop = false);

		/// Remove an animation from the animation manager.
		/// \param identifier Identifier of the animation to delete.
		/// \see addAnimation
		void removeAnimation(const std::string& identifier);

		/// Check if the current animation is paused.
		/// \param identifier Identifier of the animation to check.
		/// \return True if the animation is paused, false otherwise.
		/// \see isActive
		bool isPaused(const std::string& identifier) const;

		/// Check if the current animation is active.
		/// \param identifier Identifier of the animation to check.
		/// \return True if the animation is active, false otherwise.
		/// \see isPaused
		bool isActive(const std::string& identifier) const;

		/// Active and play the \a animation.
		/// \note The animation will be effectively played only
		/// if this is the most important animation in the active animations list.
		/// \param identifier Identifier of the animation to play.
		/// \see activate
		void play(const std::string& identifier);

		/// Activate but do not play the \a animation.
		/// If the animation is stoped, it is equivalent to do:
		/// \code
		/// animations.play("anim");
		/// animations.pause("anim");
		/// \endcode
		/// \note If the anim is already active, this function does nothing.
		/// \param identifier Identifier of the animation to play.
		/// \see play
		void activate(const std::string& identifier);

		/// Pause the given animation.
		/// On the other side, the stop function remove the animation
		/// from the active animations list.
		/// \param identifier Identifier of the animation to pause.
		/// \see stop
		void pause(const std::string& identifier);

		/// Stop and desactive the given animation and remove it from the played animations list.
		/// If another less important animation is currently playing,
		/// then this one will effectively be played.
		/// \param identifier Identifier of the animation to stop.
		/// \see pause
		void stop(const std::string& identifier);

		/// Return the progress of the given animation, in the range [0, 1].
		/// \return The progress of the given animation if the animation is active, 0.f otherwise.
		/// \param identifier Identifier of the animation to get the progress.
		/// \see setPeogress
		float getProgress(const std::string& identifier) const;

		/// Directly set the progress of the animation.
		/// It is useful when the animation must pause on a certain frame,
		/// or if the animation is not time-related.
		/// But you will have then to call the update() member, without parameters.
		/// \param identifier Identifier of the animation of wich to set the progress.
		/// \param newProgress New value to set
		/// \see getPeogress
		void setProgress(const std::string& identifier, float newProgress);

		/// Update the animation system.
		/// \param object Object on wich to apply the animation.
		/// \param dt Elapsed time in the last game frame.
		void update(T& object, sf::Time dt = sf::Time::Zero);

	private:
		std::map<std::string, TimeAnimation> m_animationsMap;///< List of all registred animations.
};

#include <TheLostGirl/Animations.inl>

#endif // ANIMATIONS_H
