#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

#include <Box2D/Common/b2Draw.h>

#include <TheLostGirl/StateStack.h>

//Forward declarations
namespace sf
{
	class RenderWindow;
}
struct Parameters;

/// Class that draw all the Box2D entities.
/// Set it as default debug drawer to the b2World,
/// and it will draw fixtures, bodies and so on at each call of b2World::drawDebugData.
class DebugDraw : public b2Draw
{
	public:
		/// Constructor.
		/// \param context Current context of the application.
		DebugDraw(StateStack::Context context);

		/// Draw a holow Box2D polygon.
		/// \param vertices Vertices to draw.
		/// \param vertexCount Number of vertices.
		/// \param color Color of the lines.
		virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

		/// Draw a plain Box2D polygon.
		/// \param vertices Vertices to draw.
		/// \param vertexCount Number of vertices.
		/// \param color Color of the polygon.
		virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

		/// Draw a holow Box2D circle.
		/// \param center Position of the center of the circle.
		/// \param radius Radius of the circle.
		/// \param color Color of the line.
		virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

		/// Draw a plain Box2D circle.
		/// \param center Position of the center of the circle.
		/// \param radius Radius of the circle.
		/// \param axis Coordinates in the range [0, 1]^2 defining the orientation of the circle.
		/// \param color Color of the circle.
		virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

		/// Draw a Box2D segment.
		/// \param p1 First point.
		/// \param p2 Second point.
		/// \param color Color of the segment.
		virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

		/// Draw a Box2D transform.
		/// \param xf Transform to draw.
		virtual void DrawTransform(const b2Transform& xf);

		/// Draw various informations on the screen, such as player position, bending angle, ...
		void drawDebugAth();

		/// Set the FPS to display.
		/// \param framesPerSecond New FPS value.
		void setFPS(float framesPerSecond);

	private:
		/// Return a string containing x with 2 decimals.
		/// \param x Number to convert.
		/// \return A formated string.
		std::string roundOutput(float x);

		bool m_debugMode;             ///< Indicate if the debug ath should be drawn.
		StateStack::Context m_context;///< Current context of the application.
		float m_framesPerSecond;      ///< FPS value to display.
};

#endif//DEBUGDRAW_H
