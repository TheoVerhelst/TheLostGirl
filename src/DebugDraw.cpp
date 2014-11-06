#include <iostream>

#include <SFML/Graphics.hpp>
#include <entityx/Entity.h>
#include <Box2D/Box2D.h>
#include <TGUI/Label.hpp>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/ResourceManager.h>

#include <TheLostGirl/DebugDraw.h>

DebugDraw::DebugDraw(State::Context& context):
	m_context(context)
{}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::ConvexShape polygon(vertexCount);
	for(int32 i{0}; i < vertexCount; ++i)
		polygon.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y)*m_context.parameters.pixelScale);
	polygon.setOutlineColor(sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*127.f));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color::Transparent);
	m_context.window.draw(polygon);
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::ConvexShape polygon(vertexCount);
	for(int32 i{0}; i < vertexCount; ++i)
		polygon.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y)*m_context.parameters.pixelScale);
	polygon.setOutlineColor(sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color(color.r*127.f, color.g*127.f, color.b*127.f, color.a*127.f));
	m_context.window.draw(polygon);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	sf::CircleShape circle(radius * m_context.parameters.pixelScale);
	circle.setOrigin(radius * m_context.parameters.pixelScale, radius * m_context.parameters.pixelScale);
	circle.setPosition(center.x* m_context.parameters.pixelScale, center.y* m_context.parameters.pixelScale);
	circle.setOutlineColor(sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color::Transparent);
	m_context.window.draw(circle);
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	sf::CircleShape circle(radius * m_context.parameters.pixelScale);
	circle.setOrigin(radius * m_context.parameters.pixelScale, radius * m_context.parameters.pixelScale);
	circle.setPosition(center.x* m_context.parameters.pixelScale, center.y* m_context.parameters.pixelScale);
	circle.setOutlineColor(sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color(color.r*127.f, color.g*127.f, color.b*127.f, color.a*127.f));
	m_context.window.draw(circle);
	
	sf::Vertex line[2];
	b2Vec2 axisPoint = center + radius * axis;
	line[0].position = sf::Vector2f(center.x, center.y)*m_context.parameters.pixelScale;
	line[1].position = sf::Vector2f(axisPoint.x, axisPoint.y)*m_context.parameters.pixelScale;
	line[0].color = sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f);
	line[1].color = sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f);
	m_context.window.draw(line, 2, sf::Lines);
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	sf::Vertex line[2];
	line[0].position = sf::Vector2f(p1.x, p1.y)*m_context.parameters.pixelScale;
	line[1].position = sf::Vector2f(p2.x, p2.y)*m_context.parameters.pixelScale;
	line[0].color = sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f);
	line[1].color = sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f);
	m_context.window.draw(line, 2, sf::Lines);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	const float axisScale = 0.4f;
	sf::Vector2f xAxis{xf.q.GetXAxis().x, xf.q.GetXAxis().y};
	xAxis *= m_context.parameters.pixelScale;
	sf::Vector2f yAxis{xf.q.GetYAxis().x, xf.q.GetYAxis().y};
	yAxis *= m_context.parameters.pixelScale;
	sf::Vector2f p1{xf.p.x, xf.p.y};
	p1 *= m_context.parameters.pixelScale;
	sf::Vector2f p2;

	sf::Vertex line1[2];
	line1[0].position = p1;
	line1[0].color = sf::Color::Red;
	line1[1].position = p1 + axisScale * xAxis;
	line1[1].color = sf::Color::Red;
	m_context.window.draw(line1, 2, sf::Lines);
	
	sf::Vertex line2[2];
	line2[0].position = p1;
	line2[0].color = sf::Color::Green;
	line2[1].position = p1 + axisScale * yAxis;
	line2[1].color = sf::Color::Green;
	m_context.window.draw(line2, 2, sf::Lines);
}

void DebugDraw::drawDebugAth()
{
	if(m_context.parameters.debugMode)
	{
		m_debugMode = true;
		//Find out position
		Body::Handle bodyComponent;
		CategoryComponent::Handle categoryComponent;
		Walk::Handle walkComponent;
		b2Vec2 position{0, 0};
		for(auto entity : m_context.entityManager.entities_with_components(bodyComponent, categoryComponent, walkComponent))
		{
			if(categoryComponent->category & Category::Player)
			{
				position = bodyComponent->body->GetPosition();
				break;
			}
		}
		
		//Draw GUI
		tgui::Label::Ptr positionLabel{m_context.gui.get<tgui::Label>("positionLabel")};
		if(positionLabel == nullptr)
		{
			positionLabel = tgui::Label::create();
			positionLabel->setPosition(tgui::bindWidth(m_context.gui, 0.01f), tgui::bindHeight(m_context.gui, 0.01f));
			positionLabel->setTextSize(20);
			positionLabel->setTextFont(std::make_shared<sf::Font>(m_context.fontManager.get("debug")));
			m_context.gui.add(positionLabel, "positionLabel");
		}
		b2Vec2 positionPixels =  (m_context.parameters.pixelScale / m_context.parameters.scale) * position;
		positionLabel->setText("Meters (" + roundOutput(position.x) + ", " + roundOutput(position.y) + ")\n" + 
							   "Pixels (" + roundOutput(positionPixels.x) + ", " + roundOutput(positionPixels.y) + ")");
	}
	else if(not m_context.parameters.debugMode and m_debugMode)
	{
		m_debugMode = false;
		//Remove the ath
		tgui::Label::Ptr positionLabel{m_context.gui.get<tgui::Label>("positionLabel")};
		if(positionLabel != nullptr)
			m_context.gui.remove(positionLabel);
	}
	
}

std::string DebugDraw::roundOutput(float x)
{
	std::string output = std::to_string(x);
	std::size_t pointPosition{output.find(".")};
	if(pointPosition < output.size()-3)
		output = output.substr(0, pointPosition+3);
	return output;
}