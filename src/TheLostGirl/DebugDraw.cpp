#include <iostream>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/ResourceManager.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/DebugDraw.hpp>

DebugDraw::DebugDraw():
	m_debugMode{true},
	m_framesPerSecond{0.f},
	m_coutStreambuf{std::cout.rdbuf(m_outStringStream.rdbuf())},
	m_cerrStreambuf{std::cerr.rdbuf(m_errStringStream.rdbuf())},
	m_errorColor{255, 100, 100}
{
	m_positionLabel = Context::getParameters().guiTheme->load("Label");
	m_mousePositionLabel = Context::getParameters().guiTheme->load("Label");
	m_FPSLabel = Context::getParameters().guiTheme->load("Label");
	m_console = Context::getParameters().guiTheme->load("ChatBox");
	tgui::Gui& gui = Context::getGui();

	m_positionLabel->setPosition(tgui::bindWidth(gui) * 0.01f, tgui::bindHeight(gui) * 0.01f);
	m_positionLabel->setTextSize(10);
	gui.add(m_positionLabel);

	m_mousePositionLabel->setPosition(tgui::bindWidth(gui) * 0.35f, tgui::bindHeight(gui) * 0.01f);
	m_mousePositionLabel->setTextSize(10);
	gui.add(m_mousePositionLabel);

	m_FPSLabel->setPosition(tgui::bindWidth(gui) * 0.7f, tgui::bindHeight(gui) * 0.01f);
	m_FPSLabel->setTextSize(10);
	gui.add(m_FPSLabel);
	m_console->setPosition(tgui::bindWidth(gui) * 0.5f, tgui::bindHeight(gui) * 0.7f);
	m_console->setSize(tgui::bindWidth(gui) * 0.5f - 10.f, tgui::bindHeight(gui) * 0.3f - 10.f);
	m_console->setTextSize(10);
	gui.add(m_console);
}

DebugDraw::~DebugDraw()
{
	flush();
	std::cout.rdbuf(m_coutStreambuf);
	std::cerr.rdbuf(m_cerrStreambuf);
	for(std::size_t i{0}; i < m_console->getLineAmount(); ++i)
		if(m_console->getLineColor(i) == m_errorColor)
			std::cerr << m_console->getLine(i).toAnsiString() << "\n";
		else
			std::cout << m_console->getLine(i).toAnsiString() << "\n";
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::ConvexShape polygon(static_cast<unsigned int>(vertexCount));
	for(int32 i{0}; i < vertexCount; ++i)
		polygon.setPoint(static_cast<unsigned int>(i),
			sf::Vector2f(vertices[i].x, vertices[i].y)*Context::getParameters().pixelByMeter);
	polygon.setOutlineColor(sf::Color(static_cast<sf::Uint8>(color.r*255.f),
			static_cast<sf::Uint8>(color.g*255.f),
			static_cast<sf::Uint8>(color.b*255.f),
			static_cast<sf::Uint8>(color.a*127.f)));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color::Transparent);
	if(Context::getParameters().bloomEnabled)
		Context::getPostEffectsTexture().draw(polygon);
	else
		Context::getWindow().draw(polygon);
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	unsigned int count{static_cast<unsigned int>(vertexCount)};
	sf::ConvexShape polygon(count);
	for(unsigned int i{0}; i < count; ++i)
		polygon.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y)*Context::getParameters().pixelByMeter);
	polygon.setOutlineColor(b2ColorToSf(color));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color::Transparent);
	if(Context::getParameters().bloomEnabled)
		Context::getPostEffectsTexture().draw(polygon);
	else
		Context::getWindow().draw(polygon);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	sf::CircleShape circle(radius * Context::getParameters().pixelByMeter);
	circle.setOrigin(radius * Context::getParameters().pixelByMeter, radius * Context::getParameters().pixelByMeter);
	circle.setPosition(center.x* Context::getParameters().pixelByMeter, center.y* Context::getParameters().pixelByMeter);
	circle.setOutlineColor(b2ColorToSf(color));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color::Transparent);
	if(Context::getParameters().bloomEnabled)
		Context::getPostEffectsTexture().draw(circle);
	else
		Context::getWindow().draw(circle);
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	sf::CircleShape circle(radius * Context::getParameters().pixelByMeter);
	circle.setOrigin(radius * Context::getParameters().pixelByMeter, radius * Context::getParameters().pixelByMeter);
	circle.setPosition(center.x* Context::getParameters().pixelByMeter, center.y* Context::getParameters().pixelByMeter);
	circle.setOutlineColor(b2ColorToSf(color));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color::Transparent);
	if(Context::getParameters().bloomEnabled)
		Context::getPostEffectsTexture().draw(circle);
	else
		Context::getWindow().draw(circle);

	sf::Vertex line[2];
	b2Vec2 axisPoint{center + radius * axis};
	line[0].position = sf::Vector2f(center.x, center.y)*Context::getParameters().pixelByMeter;
	line[1].position = sf::Vector2f(axisPoint.x, axisPoint.y)*Context::getParameters().pixelByMeter;
	line[0].color = b2ColorToSf(color);
	line[1].color = line[0].color;
	if(Context::getParameters().bloomEnabled)
		Context::getPostEffectsTexture().draw(line, 2, sf::Lines);
	else
		Context::getWindow().draw(line, 2, sf::Lines);
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{

	sf::Vertex line[2];
	line[0].position = sf::Vector2f(p1.x, p1.y)*Context::getParameters().pixelByMeter;
	line[1].position = sf::Vector2f(p2.x, p2.y)*Context::getParameters().pixelByMeter;
	line[0].color = b2ColorToSf(color);
	line[1].color = line[0].color;
	if(Context::getParameters().bloomEnabled)
		Context::getPostEffectsTexture().draw(line, 2, sf::Lines);
	else
		Context::getWindow().draw(line, 2, sf::Lines);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	const float axisScale{0.4f};
	sf::Vector2f xAxis{xf.q.GetXAxis().x, xf.q.GetXAxis().y};
	xAxis *= Context::getParameters().pixelByMeter;
	sf::Vector2f yAxis{xf.q.GetYAxis().x, xf.q.GetYAxis().y};
	yAxis *= Context::getParameters().pixelByMeter;
	sf::Vector2f p1{xf.p.x, xf.p.y};
	p1 *= Context::getParameters().pixelByMeter;
	sf::Vector2f p2;

	sf::Vertex line1[2];
	line1[0].position = p1;
	line1[0].color = sf::Color::Red;
	line1[1].position = p1 + axisScale * xAxis;
	line1[1].color = sf::Color::Red;
	if(Context::getParameters().bloomEnabled)
		Context::getPostEffectsTexture().draw(line1, 2, sf::Lines);
	else
		Context::getWindow().draw(line1, 2, sf::Lines);

	sf::Vertex line2[2];
	line2[0].position = p1;
	line2[0].color = sf::Color::Green;
	line2[1].position = p1 + axisScale * yAxis;
	line2[1].color = sf::Color::Green;
	if(Context::getParameters().bloomEnabled)
		Context::getPostEffectsTexture().draw(line2, 2, sf::Lines);
	else
		Context::getWindow().draw(line2, 2, sf::Lines);
}

void DebugDraw::drawDebugAth()
{
	if(Context::getParameters().debugMode != m_debugMode)
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
	m_debugMode = Context::getParameters().debugMode;
	if(not m_debugMode)
		return;

	//Draw GUI
	CategoryComponent::Handle categoryComponent;
	BodyComponent::Handle bodyComponent;
	//Position
	//TODO move find player in a private static method
	b2Vec2 position{0, 0};
	bool found{false};
	for(auto entity : Context::getEntityManager().entities_with_components(categoryComponent, bodyComponent))
	{
		//We found the player
		if(categoryComponent->category.test(Category::Player))
		{
			position = bodyComponent->body->GetPosition();
			const b2Vec2 positionPixels{Context::getParameters().pixelByMeter * position};
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
	if(Context::getParameters().bloomEnabled)
		mousePositionPixels = Context::getPostEffectsTexture().mapPixelToCoords(sf::Mouse::getPosition());
	else
		mousePositionPixels = Context::getWindow().mapPixelToCoords(sf::Mouse::getPosition(Context::getWindow()));
	mousePosition = mousePositionPixels / Context::getParameters().pixelByMeter;
	m_mousePositionLabel->setText("(" + roundOutput(mousePosition.x) + ", " + roundOutput(mousePosition.y) + ")\n" +
			"(" + roundOutput(mousePositionPixels.x) + ", " + roundOutput(mousePositionPixels.y) + ")");

	//FPS
	m_FPSLabel->setText("FPS: " + roundOutput(m_framesPerSecond));

	//Console
	flush();
}

void DebugDraw::setFPS(float framesPerSecond)
{
	m_framesPerSecond = framesPerSecond;
}

void DebugDraw::setFont(std::shared_ptr<sf::Font> font)
{
	m_positionLabel->setFont(font);
	m_mousePositionLabel->setFont(font);
	m_FPSLabel->setFont(font);
	m_console->setFont(font);
}

void DebugDraw::flush()
{
	const std::string outString(m_outStringStream.str());
	if(not outString.empty())
	{
		m_console->addLine(outString);
		m_outStringStream.str("");
	}
	const std::string errString(m_errStringStream.str());
	if(not errString.empty())
	{
		m_console->addLine(errString, m_errorColor);
		m_errStringStream.str("");
	}
}

std::string DebugDraw::roundOutput(float x, std::size_t decimals)
{
	std::string output{std::to_string(x)};
	std::size_t pointPosition{output.find(".")};
	if(output.size() >= decimals and pointPosition < output.size() - decimals)
		output = output.substr(0, pointPosition + decimals);
	return output;
}
