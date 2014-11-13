#ifndef ANIMATION_H
#define ANIMATION_H

//Forward declarations
namespace Json
{
	class Value;
}

/// Base class for every animation.
/// The children must define the animate and serialize member.
/// The template type define wich type of object the children animates.
template <typename T>
class Animation
{
	public:
		/// Default destructor
		virtual ~Animation()
		{}
		
		/// Animate an object of type T.
		/// \param object Thing to animate.
		/// \param pregress Progression of the animation.
		virtual void animate(T& object, float progress) = 0;
		
		/// Serialize the animation data.
		/// \return A Json value containing the data.
		virtual Json::Value serialize() const = 0;
};

#endif // ANIMATION_H
