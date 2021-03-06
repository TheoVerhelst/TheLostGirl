#ifndef POSTEFFECT_HPP
#define POSTEFFECT_HPP

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>


namespace sf
{
	class RenderTarget;
	class RenderTexture;
	class Shader;
}

/// Base class for all pretty effects.
class PostEffect : private sf::NonCopyable
{
	public:
		/// Destructor.
		virtual ~PostEffect() = default;

		/// Apply the effect.
		/// \param input Texture on wich the shader will be applied.
		/// \param output Destination where the result of the shader will be drawn.
		/// \param states Various render states to add to the drawing.
		virtual void apply(const sf::RenderTexture& input, sf::RenderTarget& output, const sf::RenderStates& states = sf::RenderStates::Default) = 0;

		/// Check wheter shaders are supported.
		/// \return True if the shaders are supported, false otherwise.
		static bool isSupported();

	protected:
		/// Apply the given shader to the output.
		/// \param shader Shader to draw.
		/// \param output Destination where the result of the shader will be drawn.
		/// \param states Various render states to add to the drawing.
		static void applyShader(const sf::Shader& shader, sf::RenderTarget& output, const sf::RenderStates& states = sf::RenderStates::Default);
};

#endif//POSTEFFECT_HPPP
