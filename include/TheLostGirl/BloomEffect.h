#ifndef BLOOMEFFECT_H
#define BLOOMEFFECT_H

#include <array>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <TheLostGirl/PostEffect.h>
#include <TheLostGirl/ResourceManager.h>

/// Shader that apply a bloom effect on the world display.
class BloomEffect : public PostEffect
{
	public:
		/// Constructor.
		BloomEffect();

		/// Destructor.
		virtual ~BloomEffect();

		/// Init the effects.
		void init();

		/// Apply the bloom effect.
		/// \param input Texture on wich the shader will be applied.
		/// \param output Destination where the result of the shader will be drawn.
		/// \param states Various render states to add to the drawing.
		virtual void apply(const sf::RenderTexture& input, sf::RenderTarget& output, const sf::RenderStates& states = sf::RenderStates::Default);

	private:
		/// Array that will store temporary textures.
		typedef std::array<sf::RenderTexture, 2> RenderTextureArray;

		/// Prepare the various texture before applying the shader.
		/// \param size Size of the target.
		void prepareTextures(sf::Vector2u size);

		/// Get only bright parts of the input.
		/// \param input Texture on wich the effect will be applied.
		/// \param output Destination where the result of the effect will be drawn.
		void filterBright(const sf::RenderTexture& input, sf::RenderTexture& output);

		/// Apply 2 blurs on the textures.
		/// \param renderTextures Textures on wich the effect will be applied.
		void blurMultipass(RenderTextureArray& renderTextures);

		/// Apply a blur on the input
		/// \param input Texture on wich the effect will be applied.
		/// \param output Destination where the result of the effect will be drawn.
		/// \param offsetFactor Amplitude of the bloom.
		void blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor);

		/// Set down the resolution of the input.
		/// \param input Texture on wich the effect will be applied.
		/// \param output Destination where the result of the effect will be drawn.
		void downsample(const sf::RenderTexture& input, sf::RenderTexture& output);

		/// Add the source and the bloom and display the result on target.
		/// \param source Texture on wich the effect will be applied.
		/// \param bloom Texture to add to source.
		/// \param target Destination where the result of the effect will be drawn.
		/// \param states Various render states to add to the drawing.
		void add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& target, const sf::RenderStates& states = sf::RenderStates::Default);

		ShaderManager m_shaders;                ///< The shader manager that store the source of the effects.
		sf::RenderTexture m_brightnessTexture;  ///< First temp texture.
		RenderTextureArray m_firstPassTextures; ///< Second temp texture.
		RenderTextureArray m_secondPassTextures;///< Third temp texture.
};

#endif // BLOOMEFFECT_H
