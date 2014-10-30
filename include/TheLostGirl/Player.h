#ifndef PLAYER_H
#define PLAYER_H

#include <map>
#include <SFML/Window/Event.hpp>

//Forward declarations
struct Command;

/// Player class that interact with the real player.
/// The Player class receive all the keyboards and mouse events in the gameplay and assign every input to a specific action in the game.
class Player
{
	public:
        /// Players's action.
        /// That's all the player can do.
		enum Action
		{
			MoveUp,       ///< Go in an exit on the top.
			MoveDown,     ///< Go in an exit on the bottom.
			MoveLeft,     ///< Run to left.
			MoveRight,    ///< Run to right.
			Bend,         ///< Bend the bow.
			PickUp,       ///< Pick up an arrow on the ground.
			Jump,         ///< Jump.
			Roulade,      ///< Roulade.
			GenericAction,///< Action with the environement (bush, climb a tree...).
			FurtherView,  ///< Look further.
			Sneak,        ///< Squat.
			HandToHand,   ///< Give a knife hit.
			Concentrate,  ///< The archer concetrate and have more precisison.
			ChangeArrow,  ///< Choose another arrow.
			Inventory,    ///< Open the inventory.
			Pause,        ///< Pause the game.
		};
		
		/// Default constructor.
		Player();
		
		/// Default destructor.
		~Player();
		
        /// Handle the input.
        /// Then assign input to Player::Action ann then assign every Action to a specific Command.
        /// \param event Event to handle.
        /// \param commands Command queue where to push commands.
        /// \return void                  
		void handleEvent(const sf::Event& event, std::queue<Command>& commands);
		
        /// Assign a key to an Action.
        /// \param action Action to assign.
        /// \param key Key to assign.
		void assignKey(Action action, sf::Keyboard::Key key);
		
        /// Assign a mouse button to an Action.
        /// \param action Action to assign.
        /// \param button Button to assign.
		void assignMouseButton(Action action, sf::Mouse::Button button);
		
        /// Assign the mouse wheel to an Action.
        /// \param action Action to assign.
		void assignMouseWheel(Action action);
		
        /// Assign a joystic button to an Action.
        /// \param action Action to assign.
        /// \param buttonIndex Index of the button to assign.
		void assignJoystickButton(Action action, unsigned int buttonIndex);
		
        /// Assign a joystick axis to an Action.
        /// \param action Action to assign.
        /// \param axis Axis to assign.
		void assignJoystickAxis(Action action, sf::Joystick::Axis axis);
		
        /// Give the key assigned to \a action.
        /// \param action Assigned action.
        /// \return An array of every key mapped with \a action.
		std::vector<sf::Keyboard::Key> getAssignedKeys(Action action) const;
		
        /// Give the mouse button assigned to \a action.
        /// \param action Assigned action.
        /// \return An array of every mouse button mapped with \a action.
		std::vector<sf::Mouse::Button> getAssignedMouseButtons(Action action) const;
		
        /// Check if \a action is assigned to the mouse wheel.
        /// \param action Assigned action.
        /// \return True if \a action is assigned to the mouse wheel, false otherwise.                       
		bool isAssignedToMouseWheel(Action action) const;
		
        /// Give the joystick button assigned to \a action.
        /// \param action Assigned action.
        /// \return An array of every index of the joystick button mapped with \a action.
		std::vector<unsigned int> getAssignedJoystickButtons(Action action) const;
		
        /// Give the joystick axis assigned to \a action.
        /// \param action Assigned action.
        /// \return An array of every joystick axis mapped with \a action.
		std::vector<sf::Joystick::Axis> getAssignedJoystickAxis(Action action) const;
		
		/// Check if any of the inputs binded with action is currently actived.
		/// \return True if any of the keys binded with action is currently pressed, false otherwise.
		bool isActived(Action action) const;
		
		/// Add commands to the given command queue according to
		/// the current state of the various imputs.
		/// That allow to check the real state of e.g. the keyboard at the beginnning
		/// of the game and move the player if the move key is pressed while the construction of the game state.
		void handleInitialInputState(CommandQueue& commands);

	private:
		/// Initialize all the player's actions.
		void initializeActions();
		
		/// Check if the given action is immediate (e.g. knife hit).
		/// \param action Action to check.
		bool isImmediateAction(Action action) const;
		
		/// Check if the given action is real time (e.g. sneaking).
		/// \param action Action to check.
		bool isRealtimeAction(Action action) const;

		std::map<sf::Keyboard::Key, Action> m_keyBinding;          ///< Binding between keyboard keys and theoric actions.
		std::map<sf::Mouse::Button, Action> m_mouseButtonBinding;  ///< Binding between mouse buttons and theoric actions.
		Action m_mouseWheelBinding;                                ///< Binding between mouse wheel and an unique theoric action.
		std::map<unsigned int, Action> m_joystickButtonBinding;    ///< Binding between joystick buttons and theoric actions.
		std::map<sf::Joystick::Axis, Action> m_joystickAxisBinding;///< Binding between joystick axis and theoric actions.
		std::map<Action, Command> m_startActionBinding;            ///< Binding between start of theoric actions and concrete functions.
		std::map<Action, Command> m_stopActionBinding;             ///< Binding between end of theoric actions and concrete functions.
		std::map<Action, Command> m_immediateActionBinding;        ///< Binding between theoric immediate actions and concrete functions.
};

#endif // PLAYER_H
