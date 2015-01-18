#ifndef POSTEFFECT_H
#define POSTEFFECT_H

#include <SFML/System/NonCopyable.hpp>


namespace sf
{
	class RenderTarget;
	class RenderTexture;
	class Shader;
}

class PostEffect : sf::NonCopyable
{
	public:
		virtual ~PostEffect();
		virtual void apply(const sf::RenderTexture& input, sf::RenderTarget& output, const sf::RenderStates& states = sf::RenderStates::Default) = 0;
		static bool isSupported();

	protected:
		static void applyShader(const sf::Shader& shader, sf::RenderTarget& output, const sf::RenderStates& states = sf::RenderStates::Default);
};

#endif//POSTEFFECT_HP
