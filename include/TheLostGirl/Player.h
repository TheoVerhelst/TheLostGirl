#ifndef PLAYER_H
#define PLAYER_H

#include <map>
#include <queue>
#include <algorithm>
#include <iostream>

#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>

#include <TheLostGirl/Command.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/components.h>

//struct PaddleMover
//{
//	//Struct fonction that accelerate the given entity of vx and vy
//	PaddleMover(float vx, float vy)
//		: velocity(vx, vy)
//	{
//	}
//	void operator()(entityx::Entity& entity, double dt) const
//	{
//		Speed::Handle speed = entity.component<Speed>();
//		speed->x += velocity.x;
//		speed->y += velocity.y;
//	}
//
//	sf::Vector2f velocity;
//};

///Player class that interact with the real player.
///The Player class receive all the keyboards and mouse events in the gameplay and assign every input to a specific action in the game.
class Player
{
	public:
        ///Players's action.
        ///That's all the player can do.
		enum Action
		{
			MoveUp,///< Go in an exit on the top.
			MoveDown,///< Go in an exit on the bottom.
			MoveLeft,///< Run to left.
			MoveRight,///< Run to right.
			Jump,///< Jump.
			Bend,///< Bend the bow.
			Sneak,///< Squat.
			HandToHand///< Give a knife hit.
		};
		Player();
		~Player();
		
        /// Handle the input.
        /// Then assign input to Player::Action ann then assign every Action to a specific Command.
        /// \param event Event to handle.
        /// \param commands Command queue where to push commands.
        /// \return void                  
		void handleEvent(const sf::Event& event, CommandQueue& commands);
		
        /// Assign a key to an Action.
        /// \param action Action to assign.
        /// \param key Key to assign.
        /// \return void
		void assignKey(Action action, sf::Keyboard::Key key);
		
        /// Give the key assigned to Action.
        /// \param action
        /// \return Key mapped with \a action.                          
		sf::Keyboard::Key getAssignedKey(Action action) const;

	private:
		void initializeActions();
		bool isImmediateAction(Action action);
		bool isRealtimeAction(Action action);

		std::map<sf::Keyboard::Key, Action> m_keyBinding;//Binding between keyboard keys and theoric actions
		std::map<Action, Command> m_startActionBinding;//Binding between start of a theoric actions and concrete functions
		std::map<Action, Command> m_stopActionBinding;//Binding between end of a theoric actions and concrete functions
		std::map<Action, Command> m_immediateActionBinding;//Binding between theoric immediate actions and concrete functions
};

#endif // PLAYER_H
