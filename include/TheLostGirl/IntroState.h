#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/functions.h>

class IntroState : public State
{
	public:
		IntroState(StateStack& stack, Context context);
		virtual void draw();
		virtual bool update(sf::Time dt);
		virtual bool handleEvent(const sf::Event& event);

	private:
};

#endif // INTROSTATE_H
