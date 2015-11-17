#include <iostream>
#include <SFML/Graphics.hpp>
#include <TGUI/Gui.hpp>
#include <entityx/Entity.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/DebugDraw.h>

DebugDraw::DebugDraw(tgui::Gui& gui, const std::string& guiCongifFile):
	m_debugMode{true},
	m_positionLabel{tgui::Label::create(guiCongifFile)},
	m_mousePositionLabel{tgui::Label::create(guiCongifFile)},
	m_FPSLabel{tgui::Label::create(guiCongifFile)},
	m_console{tgui::ChatBox::create(guiCongifFile)},
	m_outStringStream{},
	m_errStringStream{},
	m_coutStreambuf{std::cout.rdbuf(m_outStringStream.rdbuf())},
	m_cerrStreambuf{std::cerr.rdbuf(m_errStringStream.rdbuf())}
{
	m_positionLabel->setPosition(tgui::bindWidth(gui, 0.01f), tgui::bindHeight(gui, 0.01f));
	m_positionLabel->setTextSize(20);
	gui.add(m_positionLabel);

	m_mousePositionLabel->setPosition(tgui::bindWidth(gui, 0.35f), tgui::bindHeight(gui, 0.01f));
	m_mousePositionLabel->setTextSize(20);
	gui.add(m_mousePositionLabel);

	m_FPSLabel->setPosition(tgui::bindWidth(gui, 0.7f), tgui::bindHeight(gui, 0.01f));
	m_FPSLabel->setTextSize(20);
	gui.add(m_FPSLabel);
	m_console->setPosition(tgui::bindWidth(gui, 0.5f), tgui::bindHeight(gui, 0.7f));
	m_console->setSize(tgui::bindWidth(gui, 0.5f) - 10.f, tgui::bindHeight(gui, 0.3f) - 10.f);
	m_console->setTextSize(10);
	gui.add(m_console);
}

DebugDraw::~DebugDraw()
{
	std::cout.rdbuf(m_coutStreambuf);
	std::cerr.rdbuf(m_cerrStreambuf);
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::ConvexShape polygon(static_cast<unsigned int>(vertexCount));
	for(int32 i{0}; i < vertexCount; ++i)
		polygon.setPoint(static_cast<unsigned int>(i),
						sf::Vector2f(vertices[i].x, vertices[i].y)*Context::parameters->scaledPixelByMeter);
	polygon.setOutlineColor(sf::Color(static_cast<sf::Uint8>(color.r*255.f),
										static_cast<sf::Uint8>(color.g*255.f),
										static_cast<sf::Uint8>(color.b*255.f),
										static_cast<sf::Uint8>(color.a*127.f)));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color::Transparent);
	if(Context::parameters->bloomEnabled)
		Context::postEffectsTexture->draw(polygon);
	else
		Context::window->draw(polygon);
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	unsigned int count{static_cast<unsigned int>(vertexCount)};
	sf::ConvexShape polygon(count);
	for(unsigned int i{0}; i < count; ++i)
		polygon.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y)*Context::parameters->scaledPixelByMeter);
	polygon.setOutlineColor(b2ColorToSf(color));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color::Transparent);
	if(Context::parameters->bloomEnabled)
		Context::postEffectsTexture->draw(polygon);
	else
		Context::window->draw(polygon);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	sf::CircleShape circle(radius * Context::parameters->scaledPixelByMeter);
	circle.setOrigin(radius * Context::parameters->scaledPixelByMeter, radius * Context::parameters->scaledPixelByMeter);
	circle.setPosition(center.x* Context::parameters->scaledPixelByMeter, center.y* Context::parameters->scaledPixelByMeter);
	circle.setOutlineColor(b2ColorToSf(color));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color::Transparent);
	if(Context::parameters->bloomEnabled)
		Context::postEffectsTexture->draw(circle);
	else
		Context::window->draw(circle);
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	sf::CircleShape circle(radius * Context::parameters->scaledPixelByMeter);
	circle.setOrigin(radius * Context::parameters->scaledPixelByMeter, radius * Context::parameters->scaledPixelByMeter);
	circle.setPosition(center.x* Context::parameters->scaledPixelByMeter, center.y* Context::parameters->scaledPixelByMeter);
	circle.setOutlineColor(b2ColorToSf(color));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color::Transparent);
	if(Context::parameters->bloomEnabled)
		Context::postEffectsTexture->draw(circle);
	else
		Context::window->draw(circle);

	sf::Vertex line[2];
	b2Vec2 axisPoint{center + radius * axis};
	line[0].position = sf::Vector2f(center.x, center.y)*Context::parameters->scaledPixelByMeter;
	line[1].position = sf::Vector2f(axisPoint.x, axisPoint.y)*Context::parameters->scaledPixelByMeter;
	line[0].color = b2ColorToSf(color);
	line[1].color = line[0].color;
	if(Context::parameters->bloomEnabled)
		Context::postEffectsTexture->draw(line, 2, sf::Lines);
	else
		Context::window->draw(line, 2, sf::Lines);
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	sf::Vertex line[2];
	line[0].position = sf::Vector2f(p1.x, p1.y)*Context::parameters->scaledPixelByMeter;
	line[1].position = sf::Vector2f(p2.x, p2.y)*Context::parameters->scaledPixelByMeter;
	line[0].color = b2ColorToSf(color);
	line[1].color = line[0].color;
	if(Context::parameters->bloomEnabled)
		Context::postEffectsTexture->draw(line, 2, sf::Lines);
	else
		Context::window->draw(line, 2, sf::Lines);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	const float axisScale{0.4f};
	sf::Vector2f xAxis{xf.q.GetXAxis().x, xf.q.GetXAxis().y};
	xAxis *= Context::parameters->scaledPixelByMeter;
	sf::Vector2f yAxis{xf.q.GetYAxis().x, xf.q.GetYAxis().y};
	yAxis *= Context::parameters->scaledPixelByMeter;
	sf::Vector2f p1{xf.p.x, xf.p.y};
	p1 *= Context::parameters->scaledPixelByMeter;
	sf::Vector2f p2;

	sf::Vertex line1[2];
	line1[0].position = p1;
	line1[0].color = sf::Color::Red;
	line1[1].position = p1 + axisScale * xAxis;
	line1[1].color = sf::Color::Red;
	if(Context::parameters->bloomEnabled)
		Context::postEffectsTexture->draw(line1, 2, sf::Lines);
	else
		Context::window->draw(line1, 2, sf::Lines);

	sf::Vertex line2[2];
	line2[0].position = p1;
	line2[0].color = sf::Color::Green;
	line2[1].position = p1 + axisScale * yAxis;
	line2[1].color = sf::Color::Green;
	if(Context::parameters->bloomEnabled)
		Context::postEffectsTexture->draw(line2, 2, sf::Lines);
	else
		Context::window->draw(line2, 2, sf::Lines);
}

void DebugDraw::drawDebugAth()
{
	if(Context::parameters->debugMode != m_debugMode)
	{
		if(m_debugMode)
		{
			m_positionLabel->hide();
			m_mousePositionLabel->hide();
			m_FPSLabel->hide();
			m_console->hide();
		}
		else
		{
			m_positionLabel->show();
			m_mousePositionLabel->show();
			m_FPSLabel->show();
			m_console->show();
		}
	}
	m_debugMode = Context::parameters->debugMode;
	if(not m_debugMode)
		return;

	//Draw GUI
	CategoryComponent::Handle categoryComponent;
	BodyComponent::Handle bodyComponent;
	//Position
	b2Vec2 position{0, 0};
	bool found{false};
	for(auto entity : Context::entityManager->entities_with_components(categoryComponent, bodyComponent))
	{
		//We found the player
		if(categoryComponent->category & Category::Player)
		{
			position = bodyComponent->body->GetPosition();
			const b2Vec2 positionPixels{Context::parameters->pixelByMeter * position};
			m_positionLabel->show();
			m_positionLabel->setText("(" + roundOutput(position.x) + ", " + roundOutput(position.y) + ")\n" +
					"(" + roundOutput(positionPixels.x) + ", " + roundOutput(positionPixels.y) + ")");
			found = true;
			break;
		}
	}

	if(not found)
		m_positionLabel->hide();

	//Mouse position
	sf::Vector2f mousePositionPixels, mousePosition;
	if(Context::parameters->bloomEnabled)
		mousePositionPixels = Context::postEffectsTexture->mapPixelToCoords(sf::Mouse::getPosition());
	else
		mousePositionPixels = Context::window->mapPixelToCoords(sf::Mouse::getPosition(*Context::window));
	mousePosition = mousePositionPixels / Context::parameters->scaledPixelByMeter;
	m_mousePositionLabel->setText("(" + roundOutput(mousePosition.x) + ", " + roundOutput(mousePosition.y) + ")\n" +
			"(" + roundOutput(mousePositionPixels.x) + ", " + roundOutput(mousePositionPixels.y) + ")");

	//FPS
	m_FPSLabel->setText("FPS: " + roundOutput(m_framesPerSecond));

	//Console

	m_console->moveToFront();
	if(not m_outStringStream.str().empty())
	{
		m_console->addLine(m_outStringStream.str());
		m_outStringStream.str("");
	}
	if(not m_errStringStream.str().empty())
	{
		m_console->addLine(m_errStringStream.str(), sf::Color(255, 100, 100));
		m_errStringStream.str("");
	}
}

void DebugDraw::setFPS(float framesPerSecond)
{
	m_framesPerSecond = framesPerSecond;
}

void DebugDraw::setTextFont(std::shared_ptr<sf::Font> font)
{
	m_positionLabel->setTextFont(font);
	m_mousePositionLabel->setTextFont(font);
	m_FPSLabel->setTextFont(font);
	m_console->setTextFont(font);
}

std::string DebugDraw::roundOutput(float x, std::size_t decimals)
{
	std::string output{std::to_string(x)};
	std::size_t pointPosition{output.find(".")};
	if(output.size() >= decimals and pointPosition < output.size() - decimals)
		output = output.substr(0, pointPosition + decimals);
	return output;
}
