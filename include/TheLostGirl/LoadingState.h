#ifndef LOADINGSTATE_H
#define LOADINGSTATE_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/ResourceManager.h>

class LoadingState : public State
{
	public:
		LoadingState(StateStack& stack, Context context);
		virtual void draw();
		virtual bool update(sf::Time dt);
		virtual bool handleEvent(const sf::Event& event);

	private:
};

#endif // LOADINGSTATE_H
