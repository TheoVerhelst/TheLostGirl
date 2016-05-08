#ifndef DEBUGDRAW_HPP
#define DEBUGDRAW_HPP

#include <Box2D/Box2D.h>
#include <TGUI/TGUI.hpp>
#include <TheLostGirl/StateStack.hpp>
#include <TheLostGirl/Context.hpp>

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
		/// Default constructor.
		DebugDraw();

		/// Destructor.
		virtual ~DebugDraw();

		/// Draw a holow Box2D polygon.
		/// \param vertices Vertices to draw.
		/// \param vertexCount Number of vertices.
		/// \param color Color of the lines.
		virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

		/// Draw a plain Box2D polygon.
		/// \param vertices Vertices to draw.
		/// \param vertexCount Number of vertices.
		/// \param color Color of the polygon.
		virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

		/// Draw a holow Box2D circle.
		/// \param center Position of the center of the circle.
		/// \param radius Radius of the circle.
		/// \param color Color of the line.
		virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;

		/// Draw a plain Box2D circle.
		/// \param center Position of the center of the circle.
		/// \param radius Radius of the circle.
		/// \param axis Coordinates in the range [0, 1]^2 defining the orientation of the circle.
		/// \param color Color of the circle.
		virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;

		/// Draw a Box2D segment.
		/// \param p1 First point.
		/// \param p2 Second point.
		/// \param color Color of the segment.
		virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

		/// Draw a Box2D transform.
		/// \param xf Transform to draw.
		virtual void DrawTransform(const b2Transform& xf) override;

		/// Draw various informations on the screen, such as player position, bending angle, ...
		void drawDebugAth();

		/// Set the FPS to display.
		/// \param framesPerSecond New FPS value.
		void setFPS(float framesPerSecond);

		/// Changes the font of the text.
		/// \param font The new font.
		void setFont(std::shared_ptr<sf::Font> font);

	private:
		/// Context type of this class.
		typedef ContextAccessor<
				ContextElement::Parameters,
				ContextElement::Gui,
				ContextElement::PostEffectsTexture,
				ContextElement::EntityManager,
				ContextElement::Window> Context;

		/// Flush the output on std::cout and std::cerr to the console
		void flush();

		/// Return a string containing x with 2 decimals.
		/// \param x Number to convert.
		/// \param decimals Number of decimals to show
		/// \return A formated string.
		static std::string roundOutput(float x, std::size_t decimals=3);

		bool m_debugMode;                     ///< Indicates if the debug ath should be drawn.
		static const bool m_guiConsole;       ///< Indicates whether the chat box can be used to redirect standard streams or not.
		float m_framesPerSecond;              ///< FPS value to display.
		tgui::Label::Ptr m_positionLabel;     ///< The label of the player's position.
		tgui::Label::Ptr m_mousePositionLabel;///< The label of the position of the mouse.
		tgui::Label::Ptr m_FPSLabel;          ///< The label of the FPS counter.
		tgui::ChatBox::Ptr m_console;         ///< The chatbox of the console.
		std::stringstream m_outStringStream;  ///< String used to print standard output into the GUI.
		std::stringstream m_errStringStream;  ///< String used to print standard error into the GUI.
		std::streambuf* m_coutStreambuf;      ///< The old stream buffer of std::cout.
		std::streambuf* m_cerrStreambuf;      ///< The old stream buffer of std::cerr.
		const sf::Color m_errorColor;         ///< The color of an error line in the console.
};

#endif//DEBUGDRAW_HPP
