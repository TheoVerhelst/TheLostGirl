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
        /// \return A pair where the first element indicates if a mapping was found,
        /// and the second element contains the key mapped with \a action
        /// if found, or the first element in the keyboard's keys enumeration otherwise. 
		std::pair<bool, sf::Keyboard::Key> getAssignedKey(Action action) const;
		
        /// Give the mouse button assigned to \a action.
        /// \param action Assigned action.
        /// \return A pair where the first element indicates if a mapping was found,
        /// and the second element contains the mouse button mapped with \a action
        /// if found, or the first element in the mouse's button enumeration otherwise.
		std::pair<bool, sf::Mouse::Button> getAssignedMouseButton(Action action) const;
		
        /// Check if \a action is assigned to the mouse wheel.
        /// \param action Assigned action.
        /// \return True if \a action is assigned to the mouse wheel, false otherwise.                          
		bool isAssignedToMouseWheel(Action action) const;
		
        /// Give the joystick button assigned to \a action.
        /// \param action Assigned action.
        /// \return A pair where the first element indicates if a mapping was found,
        /// and the second element contains the index of the joystick button mapped with \a action
        /// if found, or 0 otherwise.                           
		std::pair<bool, unsigned int> getAssignedJoystickButton(Action action) const;
		
        /// Give the joystick axis assigned to \a action.
        /// \param action Assigned action.
        /// \return A pair where the first element indicates if a mapping was found,
        /// and the second element contains the joystick axis mapped with \a action
        /// if found, or the first element in the joystick's axis enumeration otherwise.
		std::pair<bool, sf::Joystick::Axis> getAssignedJoystickAxis(Action action) const;
		
		/// Check if the drag and drop is currently active.
		/// The drag and drop is active only when the button binded with the bow bending is pressed.
		/// \return True if the drag and drop is currently active, false otherwise.
		/// \see getDragAndDropState
		bool isDragAndDropActive() const;

	private:
		/// Initialize all the player's actions.
		void initializeActions();
		
		/// Check if the given action is immediate (e.g. knife hit).
		/// \param action Action to check.
		bool isImmediateAction(Action action);
		
		/// Check if the given action is real time (e.g. sneaking).
		/// \param action Action to check.
		bool isRealtimeAction(Action action);

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
