#ifndef BLOOMEFFECT_H
#define BLOOMEFFECT_H

#include <array>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <TheLostGirl/PostEffect.h>
#include <TheLostGirl/ResourceManager.h>


class BloomEffect : public PostEffect
{
	public:
		BloomEffect();
		virtual void apply(const sf::RenderTexture& input, sf::RenderTarget& output);

	private:
		typedef std::array<sf::RenderTexture, 2> RenderTextureArray;
		void prepareTextures(sf::Vector2u size);

		void filterBright(const sf::RenderTexture& input, sf::RenderTexture& output);
		void blurMultipass(RenderTextureArray& renderTextures);
		void blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor);
		void downsample(const sf::RenderTexture& input, sf::RenderTexture& output);
		void add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& target);
		
		ShaderManager m_shaders;
		sf::RenderTexture m_brightnessTexture;
		RenderTextureArray m_firstPassTextures;
		RenderTextureArray m_secondPassTextures;
};

#endif // BLOOMEFFECT_H
