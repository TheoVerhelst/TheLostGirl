#include <SFML/Graphics.hpp>
#include <TheLostGirl/PostEffect.hpp>

void PostEffect::applyShader(const sf::Shader& shader, sf::RenderTarget& output, const sf::RenderStates& states)
{
	sf::Vector2f outputSize = output.getDefaultView().getSize();

	sf::VertexArray vertices(sf::TrianglesStrip, 4);
	vertices[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 1));
	vertices[1] = sf::Vertex(sf::Vector2f(outputSize.x, 0), sf::Vector2f(1, 1));
	vertices[2] = sf::Vertex(sf::Vector2f(0, outputSize.y), sf::Vector2f(0, 0));
	vertices[3] = sf::Vertex(sf::Vector2f(outputSize), sf::Vector2f(1, 0));

	sf::RenderStates renderStates(states);
	renderStates.shader = &shader;
	renderStates.blendMode = sf::BlendNone;
	output.draw(vertices, renderStates);
}

bool PostEffect::isSupported()
{
	return sf::Shader::isAvailable();
}
