#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include <memory>

#include <TGUI/Button.hpp>
#include <TGUI/Picture.hpp>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
class Context;
class StateStack;

class MainMenuState : public State
{
	public:
		MainMenuState(StateStack& stack, Context context);
		~MainMenuState();
		virtual void draw();
		virtual bool update(sf::Time dt);
		virtual bool handleEvent(const sf::Event& event);
		
	private:
		void playGame();
		tgui::Picture::Ptr m_background;
		tgui::Picture::Ptr m_logo;
		tgui::Button::Ptr m_newButton;
		tgui::Button::Ptr m_loadButton;
		tgui::Button::Ptr m_exitButton;
};

#endif // MAINMENUSTATE_H
