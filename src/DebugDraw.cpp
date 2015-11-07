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

DebugDraw::DebugDraw(StateStack::Context context):
	m_debugMode{true},
	m_context{context},
	m_positionLabel{tgui::Label::create(m_context.parameters.guiConfigFile)},
	m_mousePositionLabel{tgui::Label::create(m_context.parameters.guiConfigFile)},
	m_FPSLabel{tgui::Label::create(m_context.parameters.guiConfigFile)},
	m_console{tgui::ChatBox::create(m_context.parameters.guiConfigFile)},
	m_outStringStream{},
	m_errStringStream{},
	m_coutStreambuf{std::cout.rdbuf(m_outStringStream.rdbuf())},
	m_cerrStreambuf{std::cerr.rdbuf(m_errStringStream.rdbuf())}
{
	m_positionLabel->setPosition(tgui::bindWidth(m_context.gui, 0.01f), tgui::bindHeight(m_context.gui, 0.01f));
	m_positionLabel->setTextSize(20);
	m_context.gui.add(m_positionLabel);

	m_mousePositionLabel->setPosition(tgui::bindWidth(m_context.gui, 0.35f), tgui::bindHeight(m_context.gui, 0.01f));
	m_mousePositionLabel->setTextSize(20);
	m_context.gui.add(m_mousePositionLabel);

	m_FPSLabel->setPosition(tgui::bindWidth(m_context.gui, 0.7f), tgui::bindHeight(m_context.gui, 0.01f));
	m_FPSLabel->setTextSize(20);
	m_context.gui.add(m_FPSLabel);
	m_console->setPosition(tgui::bindWidth(m_context.gui, 0.5f), tgui::bindHeight(m_context.gui, 0.7f));
	m_console->setSize(tgui::bindWidth(m_context.gui, 0.5f) - 10.f, tgui::bindHeight(m_context.gui, 0.3f) - 10.f);
	m_console->setTextSize(10);
	m_context.gui.add(m_console);
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
						sf::Vector2f(vertices[i].x, vertices[i].y)*m_context.parameters.scaledPixelByMeter);
	polygon.setOutlineColor(sf::Color(static_cast<sf::Uint8>(color.r*255.f),
										static_cast<sf::Uint8>(color.g*255.f),
										static_cast<sf::Uint8>(color.b*255.f),
										static_cast<sf::Uint8>(color.a*127.f)));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color::Transparent);
	if(m_context.parameters.bloomEnabled)
		m_context.postEffectsTexture.draw(polygon);
	else
		m_context.window.draw(polygon);
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	unsigned int count{static_cast<unsigned int>(vertexCount)};
	sf::ConvexShape polygon(count);
	for(unsigned int i{0}; i < count; ++i)
		polygon.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y)*m_context.parameters.scaledPixelByMeter);
	polygon.setOutlineColor(b2ColorToSf(color));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color::Transparent);
	if(m_context.parameters.bloomEnabled)
		m_context.postEffectsTexture.draw(polygon);
	else
		m_context.window.draw(polygon);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	sf::CircleShape circle(radius * m_context.parameters.scaledPixelByMeter);
	circle.setOrigin(radius * m_context.parameters.scaledPixelByMeter, radius * m_context.parameters.scaledPixelByMeter);
	circle.setPosition(center.x* m_context.parameters.scaledPixelByMeter, center.y* m_context.parameters.scaledPixelByMeter);
	circle.setOutlineColor(b2ColorToSf(color));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color::Transparent);
	if(m_context.parameters.bloomEnabled)
		m_context.postEffectsTexture.draw(circle);
	else
		m_context.window.draw(circle);
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	sf::CircleShape circle(radius * m_context.parameters.scaledPixelByMeter);
	circle.setOrigin(radius * m_context.parameters.scaledPixelByMeter, radius * m_context.parameters.scaledPixelByMeter);
	circle.setPosition(center.x* m_context.parameters.scaledPixelByMeter, center.y* m_context.parameters.scaledPixelByMeter);
	circle.setOutlineColor(b2ColorToSf(color));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color::Transparent);
	if(m_context.parameters.bloomEnabled)
		m_context.postEffectsTexture.draw(circle);
	else
		m_context.window.draw(circle);

	sf::Vertex line[2];
	b2Vec2 axisPoint{center + radius * axis};
	line[0].position = sf::Vector2f(center.x, center.y)*m_context.parameters.scaledPixelByMeter;
	line[1].position = sf::Vector2f(axisPoint.x, axisPoint.y)*m_context.parameters.scaledPixelByMeter;
	line[0].color = b2ColorToSf(color);
	line[1].color = line[0].color;
	if(m_context.parameters.bloomEnabled)
		m_context.postEffectsTexture.draw(line, 2, sf::Lines);
	else
		m_context.window.draw(line, 2, sf::Lines);
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	sf::Vertex line[2];
	line[0].position = sf::Vector2f(p1.x, p1.y)*m_context.parameters.scaledPixelByMeter;
	line[1].position = sf::Vector2f(p2.x, p2.y)*m_context.parameters.scaledPixelByMeter;
	line[0].color = b2ColorToSf(color);
	line[1].color = line[0].color;
	if(m_context.parameters.bloomEnabled)
		m_context.postEffectsTexture.draw(line, 2, sf::Lines);
	else
		m_context.window.draw(line, 2, sf::Lines);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	const float axisScale{0.4f};
	sf::Vector2f xAxis{xf.q.GetXAxis().x, xf.q.GetXAxis().y};
	xAxis *= m_context.parameters.scaledPixelByMeter;
	sf::Vector2f yAxis{xf.q.GetYAxis().x, xf.q.GetYAxis().y};
	yAxis *= m_context.parameters.scaledPixelByMeter;
	sf::Vector2f p1{xf.p.x, xf.p.y};
	p1 *= m_context.parameters.scaledPixelByMeter;
	sf::Vector2f p2;

	sf::Vertex line1[2];
	line1[0].position = p1;
	line1[0].color = sf::Color::Red;
	line1[1].position = p1 + axisScale * xAxis;
	line1[1].color = sf::Color::Red;
	if(m_context.parameters.bloomEnabled)
		m_context.postEffectsTexture.draw(line1, 2, sf::Lines);
	else
		m_context.window.draw(line1, 2, sf::Lines);

	sf::Vertex line2[2];
	line2[0].position = p1;
	line2[0].color = sf::Color::Green;
	line2[1].position = p1 + axisScale * yAxis;
	line2[1].color = sf::Color::Green;
	if(m_context.parameters.bloomEnabled)
		m_context.postEffectsTexture.draw(line2, 2, sf::Lines);
	else
		m_context.window.draw(line2, 2, sf::Lines);
}

void DebugDraw::drawDebugAth()
{
	if(m_context.parameters.debugMode != m_debugMode)
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
		std::cout << "out" << std::endl;
		std::cerr << "err" << std::endl;
	}
	m_debugMode = m_context.parameters.debugMode;
	if(not m_debugMode)
		return;

	//Draw GUI
	CategoryComponent::Handle categoryComponent;
	BodyComponent::Handle bodyComponent;
	//Position
	b2Vec2 position{0, 0};
	bool found{false};
	for(auto entity : m_context.entityManager.entities_with_components(categoryComponent, bodyComponent))
	{
		//We found the player
		if(categoryComponent->category & Category::Player)
		{
			position = bodyComponent->body->GetPosition();
			const b2Vec2 positionPixels{m_context.parameters.pixelByMeter * position};
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
	if(m_context.parameters.bloomEnabled)
		mousePositionPixels = m_context.postEffectsTexture.mapPixelToCoords(sf::Mouse::getPosition());
	else
		mousePositionPixels = m_context.window.mapPixelToCoords(sf::Mouse::getPosition(m_context.window));
	mousePosition = mousePositionPixels / m_context.parameters.scaledPixelByMeter;
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
