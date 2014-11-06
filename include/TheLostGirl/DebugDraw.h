#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

#include <Box2D/Common/b2Draw.h>

#include <TheLostGirl/State.h>

//Forward declarations
namespace sf
{
	class RenderWindow;
}
struct Parameters;

class DebugDraw : public b2Draw
{
	public:
		DebugDraw(State::Context& context);
		virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
		virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
		virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
		virtual void DrawTransform(const b2Transform& xf);
		void drawDebugAth();
		
	private:
		std::string roundOutput(float x);
		
		bool m_debugMode;
		State::Context& m_context;
};

#endif // DEBUGDRAW_H
