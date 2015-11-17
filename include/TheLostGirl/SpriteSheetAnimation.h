#ifndef SPRITESHEETANIMATION_H
#define SPRITESHEETANIMATION_H

#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/Context.h>

//Forward declarations
namespace sf
{
	template <typename T>
	class Rect;
	typedef Rect<int> IntRect;
}
namespace Json
{
	class Value;
}

/// Animation of a spritesheet.
/// A spritesheet (or tileset) is an animation tricks
/// that constists in display only one part of a given texture at one time
/// and fastly change this area to give the illusion of continuous movement.
/// In order to use this class, you must add some frames to the object
/// with the addFrame member, and then pass the instance to the
/// addAnimation member of an AnimationsManager object.
class SpriteSheetAnimation
{
	public:
		/// Structure that represent a frame of a spritesheet.
		/// The rect is the area to display on the texture,
		/// and the duration is the relative duration in the range [0, 1] of the frame
		/// in comparaison of the others frames.
		/// If the sum of all durations are greater than 1, the exceding frames
		/// are not displayed.
		/// If the sur of all durations are lower than 1, the last frame is displayed
		/// until the end of the animation.
		struct Frame
		{
			sf::IntRect rect;///< Rect to display.
			float duration;  ///< Relative duration of the frame, in the range [0, 1].

			/// Constructor.
			/// \param _rect Rect to display.
			/// \param _duration Relative duration of the frame, in the range [0, 1].
			Frame(const sf::IntRect& _rect, float _duration):
				rect(_rect),
				duration(_duration)
			{
			}
		};

		/// Constructor.
		/// \param sprite Sprite to animate.
		SpriteSheetAnimation(sf::Sprite& sprite);

		/// Registers a new frame.
		/// \param frame Frame of texture to display on the sprite of the entity.
		void addFrame(const Frame& frame);

		/// Overload of the addFrame function.
		/// Registers a new frame.
		/// \param rect IntRect of the texture to display on the sprite of the entity.
		/// \param duration Relative duration of the frame.
		void addFrame(const sf::IntRect& rect, float duration);

		/// Apply the animation to the sprite passed as argument in the constructor.
		/// In fact, it just search wich rect to apply to the texture of the sprite.
		/// \param progress Progress of the animation to apply.
		void animate(float progress);

		/// Serialize the spritesheet animation.
		/// \return A Json value containing all the data.
		Json::Value serialize() const;

		/// Deserialize the value and set it in the animation.
		/// \param value A Json value containing the data.
		void deserialize(const Json::Value& value);

	private:
		/// Scale the given rectangle with the current scale.
		/// \param rect Rectangle to scale.
		/// \return A scaled rectangle.
		template<typename T>
		sf::Rect<T> scale(sf::Rect<T> rect)
		{
			sf::Rect<T> res;
			res.left = T(float(rect.left) * Context::parameters->scale);
			res.top = T(float(rect.top) * Context::parameters->scale);
			res.width = T(float(rect.width) * Context::parameters->scale);
			res.height = T(float(rect.height) * Context::parameters->scale);
			return res;
		}
		std::vector<Frame> m_frames;  ///< Array of all registred frames.
		sf::Sprite& m_sprite;         ///< Sprite to animate.
};

#endif//SPRITESHEETANIMATION_H
