#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

#include <Box2D/Common/b2Draw.h>

//Forward declarations
namespace sf
{
	class RenderWindow;
}
struct Parameters;

class DebugDraw : public b2Draw
{
	public:
		DebugDraw(sf::RenderWindow& window, Parameters& parameters);
		virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
		virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
		virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
		virtual void DrawTransform(const b2Transform& xf);
		
	private:
		sf::RenderWindow& m_window;
		Parameters& m_parameters;
};

#endif // DEBUGDRAW_H
