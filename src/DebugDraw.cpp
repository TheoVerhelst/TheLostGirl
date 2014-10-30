#include <iostream>

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <TheLostGirl/Parameters.h>

#include <TheLostGirl/DebugDraw.h>

DebugDraw::DebugDraw(sf::RenderWindow& window, Parameters& parameters):
	m_window(window),
	m_parameters(parameters)
{}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::ConvexShape polygon(vertexCount);
	for(int32 i{0}; i < vertexCount; ++i)
		polygon.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y)*m_parameters.pixelScale);
	polygon.setOutlineColor(sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*127.f));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color::Transparent);
	m_window.draw(polygon);
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::ConvexShape polygon(vertexCount);
	for(int32 i{0}; i < vertexCount; ++i)
		polygon.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y)*m_parameters.pixelScale);
	polygon.setOutlineColor(sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f));
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color(color.r*127.f, color.g*127.f, color.b*127.f, color.a*127.f));
	m_window.draw(polygon);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	sf::CircleShape circle(radius * m_parameters.pixelScale);
	circle.setOrigin(radius * m_parameters.pixelScale, radius * m_parameters.pixelScale);
	circle.setPosition(center.x* m_parameters.pixelScale, center.y* m_parameters.pixelScale);
	circle.setOutlineColor(sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color::Transparent);
	m_window.draw(circle);
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	sf::CircleShape circle(radius * m_parameters.pixelScale);
	circle.setOrigin(radius * m_parameters.pixelScale, radius * m_parameters.pixelScale);
	circle.setPosition(center.x* m_parameters.pixelScale, center.y* m_parameters.pixelScale);
	circle.setOutlineColor(sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f));
	circle.setOutlineThickness(-1.f);
	circle.setFillColor(sf::Color(color.r*127.f, color.g*127.f, color.b*127.f, color.a*127.f));
	m_window.draw(circle);
	
	sf::Vertex line[2];
	b2Vec2 axisPoint = center + radius * axis;
	line[0].position = sf::Vector2f(center.x, center.y)*m_parameters.pixelScale;
	line[1].position = sf::Vector2f(axisPoint.x, axisPoint.y)*m_parameters.pixelScale;
	line[0].color = sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f);
	line[1].color = sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f);
	m_window.draw(line, 2, sf::Lines);
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	sf::Vertex line[2];
	line[0].position = sf::Vector2f(p1.x, p1.y)*m_parameters.pixelScale;
	line[1].position = sf::Vector2f(p2.x, p2.y)*m_parameters.pixelScale;
	line[0].color = sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f);
	line[1].color = sf::Color(color.r*255.f, color.g*255.f, color.b*255.f, color.a*255.f);
	m_window.draw(line, 2, sf::Lines);
}

void DebugDraw::DrawTransform(const b2Transform&)
{
	std::cout << "tranform" << std::endl;
}