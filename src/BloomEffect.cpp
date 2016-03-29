#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/BloomEffect.hpp>

BloomEffect::BloomEffect()
{
	m_shaders.load("brightness pass", getParameters().resourcesPath + "shaders/Fullpass.vert", getParameters().resourcesPath + "shaders/Brightness.frag");
	m_shaders.load("down sample pass", getParameters().resourcesPath + "shaders/Fullpass.vert", getParameters().resourcesPath + "shaders/DownSample.frag");
	m_shaders.load("gaussian blur pass", getParameters().resourcesPath + "shaders/Fullpass.vert", getParameters().resourcesPath + "shaders/GuassianBlur.frag");
	m_shaders.load("add pass", getParameters().resourcesPath + "shaders/Fullpass.vert", getParameters().resourcesPath + "shaders/Add.frag");
}

BloomEffect::~BloomEffect()
{
}

void BloomEffect::apply(const sf::RenderTexture& input, sf::RenderTarget& output, const sf::RenderStates& states)
{
	prepareTextures(input.getSize());

	filterBright(input, m_brightnessTexture);

	downsample(m_brightnessTexture, m_firstPassTextures[0]);
	blurMultipass(m_firstPassTextures);

	downsample(m_firstPassTextures[0], m_secondPassTextures[0]);
	blurMultipass(m_secondPassTextures);

	add(m_firstPassTextures[0], m_secondPassTextures[0], m_firstPassTextures[1]);
	m_firstPassTextures[1].display();
	add(input, m_firstPassTextures[1], output, states);
}

void BloomEffect::prepareTextures(sf::Vector2u size)
{
	if (m_brightnessTexture.getSize() != size)
	{
		m_brightnessTexture.create(size.x, size.y);
		m_brightnessTexture.setSmooth(true);

		m_firstPassTextures[0].create(size.x / 2, size.y / 2);
		m_firstPassTextures[0].setSmooth(true);
		m_firstPassTextures[1].create(size.x / 2, size.y / 2);
		m_firstPassTextures[1].setSmooth(true);

		m_secondPassTextures[0].create(size.x / 4, size.y / 4);
		m_secondPassTextures[0].setSmooth(true);
		m_secondPassTextures[1].create(size.x / 4, size.y / 4);
		m_secondPassTextures[1].setSmooth(true);
	}
}

void BloomEffect::filterBright(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& brightness = m_shaders.get("brightness pass");

	brightness.setUniform("source", input.getTexture());
	applyShader(brightness, output);
	output.display();
}

void BloomEffect::blurMultipass(RenderTextureArray& renderTextures)
{
	sf::Vector2u textureSize = renderTextures[0].getSize();

	for (std::size_t count = 0; count < 2; ++count)
	{
		blur(renderTextures[0], renderTextures[1], sf::Vector2f(0.f, 1.f / static_cast<float>(textureSize.y)));
		blur(renderTextures[1], renderTextures[0], sf::Vector2f(1.f / static_cast<float>(textureSize.x), 0.f));
	}
}

void BloomEffect::blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor)
{
	sf::Shader& gaussianBlur = m_shaders.get("gaussian blur pass");

	gaussianBlur.setUniform("source", input.getTexture());
	gaussianBlur.setUniform("offsetFactor", offsetFactor);
	applyShader(gaussianBlur, output);
	output.display();
}

void BloomEffect::downsample(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& downSampler = m_shaders.get("down sample pass");

	downSampler.setUniform("source", input.getTexture());
	downSampler.setUniform("sourceSize", sf::Vector2f(input.getSize()));
	applyShader(downSampler, output);
	output.display();
}

void BloomEffect::add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& output, const sf::RenderStates& states)
{
	sf::Shader& adder = m_shaders.get("add pass");

	adder.setUniform("source", source.getTexture());
	adder.setUniform("bloom", bloom.getTexture());
	applyShader(adder, output, states);
}
