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
/// AnimationsComponent::Handle animationsComponent = entity->assign<AnimationsComponent>(new Animations());
/// //Add the animation to the animations manager
/// animationsComponent->animations->addAnimation("run", anim, sf::seconds(3.f), true);
/// animationsComponent->animations->play("run");
/// \endcode
/// \see AnimationSystem
/// 
/// This animation system is inspired by the Animator system in the Jan Haller's Thor C++ library.
class Animations
{
	public:
		/// Typedef for the Animation functor.
		/// The functor must have exactly this signature:
		/// void (entityx::Entity&, float);
		/// \arg \a entityx::Entity& reference to the entity to animate.
		/// \arg \a float Current state of the animation.
		typedef std::function<void(entityx::Entity&, float)> Animation;
		
		/// Animation associated with a certain duration.
		struct TimeAnimation
		{
			Animation animation;///< Functor of the animation.
			sf::Time duration;  ///< Duration of the animation.
			bool loops;          ///< Indicates if the animation must loop or not.
			
			/// Default constructor.
			/// \param _animation Functor of the animation.
			/// \param _duration Duration of the animation.
			/// \param _loops Indicates if the animation must loop or not.
			TimeAnimation(Animation _animation, sf::Time _duration = sf::seconds(1.f), bool _loops = true);
		};
		/// Default constructor
		Animations();
		
		/// Default destructor
		~Animations();
		
		/// Copy constructor
		///  \param other Object to copy from
		Animations(const Animations& other);
		
		/// Assignment operator
		///  \param other Object to assign from
		///  \return A reference to this
		Animations& operator=(const Animations& other);

		/// Add a new animation to the manager.
		/// \param identifier Unique identifier of the animation to add.
		/// \param animation Functor to the animation.
		/// \param duration Duration fo the animation.
		/// \param loop Indicate if the animation must loop or not.
		/// \see removeAnimation
		void addAnimation(const std::string& identifier, Animation animation, sf::Time duration = sf::seconds(1.f), bool loop = true);
		
		/// Remove an animation from the animation manager.
		/// \param identifier Unique identifier of the animation to delete.
		/// \see addAnimation
		void removeAnimation(const std::string& identifier);
		
		/// Check if the current animation is playing
		/// \return True if the animation is playing, false otherwise.
		/// \see play, stop
		bool isPlaying() const;
		
		/// Play the current animation.
		/// \see stop, isPlaying
		void play();
		
		/// Overload of the play member.
		/// Play the \a animation.
		/// This is a shortcut to :
		/// \code
		/// setCurrentAnimation(anim);
		/// play();
		/// \endcode
		/// \param animation Identifier of the animation to play.
		/// \see stop, isPlaying
		void play(std::string animation);
		
		/// Stop the current animation.
		/// \see play, isPlaying
		void stop();
		
		/// Return the progress of the animation, in the range [0, 1].
		/// \return The progress of the animation.
		float getProgress() const;
		
		/// Directly set the progress of the animation.
		/// It is useful when the animation must stops on a certain frame,
		/// or if the animation is not time-related.
		/// But you will have then to call the update() member, without parameters.
		/// \param newProgress New value to set
		void setProgress(float newProgress);
		
		/// Return the identifier of the current animation.
		/// \return The identifier of the current animation.
		const std::string& getCurrentAnimation() const;
		
		/// Set the animation to play.
		/// \param animation Identifier of the animation to play.
		/// \see play
		void setCurrentAnimation(std::string animation);
		
		/// Update the current animation.
		/// \param entity Entity on wich apply the animation.
		/// \param dt Elapsed time in the current frame.
		void update(entityx::Entity& entity, sf::Time dt = sf::Time::Zero);
		
	private:
		std::map<std::string, TimeAnimation> m_animationsMap; ///< List of all registred animation.
		bool m_isPlaying;                                     ///< Indicates if an animation ned to be played.
		float m_progress;                                     ///< Progress of the current animation, in the range [0,1].
		std::string m_currentAnimation;                       ///< Identifier of the animation to play.
};


#endif // ANIMATIONS_H
