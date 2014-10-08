#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include <memory>

#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/functions.h>


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
