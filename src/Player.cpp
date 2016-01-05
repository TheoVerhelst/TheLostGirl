#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/Command.hpp>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/actions.hpp>
#include <TheLostGirl/systems/PendingChangesSystem.hpp>
#include <TheLostGirl/Player.hpp>

Player::Player()
{
	// Set initial inputs bindings
	m_keyBinding[sf::Keyboard::Q]         = Action::MoveLeft;
	m_keyBinding[sf::Keyboard::Left]      = Action::MoveLeft;
	m_keyBinding[sf::Keyboard::D]         = Action::MoveRight;
	m_keyBinding[sf::Keyboard::Right]     = Action::MoveRight;
	m_keyBinding[sf::Keyboard::Z]         = Action::MoveUp;
	m_keyBinding[sf::Keyboard::Up]        = Action::MoveUp;
	m_keyBinding[sf::Keyboard::S]         = Action::MoveDown;
	m_keyBinding[sf::Keyboard::Down]      = Action::MoveDown;
	m_keyBinding[sf::Keyboard::R]         = Action::PickUp;
	m_keyBinding[sf::Keyboard::SemiColon] = Action::PickUp;
	m_keyBinding[sf::Keyboard::LAlt]      = Action::Roulade;
	m_keyBinding[sf::Keyboard::RAlt]      = Action::Roulade;
	m_keyBinding[sf::Keyboard::E]         = Action::GenericAction;
	m_keyBinding[sf::Keyboard::Period]    = Action::GenericAction;
	m_keyBinding[sf::Keyboard::F]         = Action::SearchCorpse;
	m_keyBinding[sf::Keyboard::M]         = Action::SearchCorpse;
	m_keyBinding[sf::Keyboard::LControl]  = Action::FurtherView;
	m_keyBinding[sf::Keyboard::RControl]  = Action::FurtherView;
	m_keyBinding[sf::Keyboard::Space]     = Action::Jump;
	m_keyBinding[sf::Keyboard::LShift]    = Action::Sneak;
	m_keyBinding[sf::Keyboard::RShift]    = Action::Sneak;
	m_keyBinding[sf::Keyboard::A]         = Action::HandToHand;
	m_keyBinding[sf::Keyboard::Comma]     = Action::HandToHand;
	m_keyBinding[sf::Keyboard::Tab]       = Action::Inventory;
	m_keyBinding[sf::Keyboard::L]         = Action::Inventory;

	m_mouseButtonBinding[sf::Mouse::Button::Left]  = Action::Bend;
	m_mouseButtonBinding[sf::Mouse::Button::Right] = Action::Concentrate;

	m_mouseWheelBinding = Action::ChangeArrow;

	// Set initial action bindings, fill the maps
	initializeActions();
}

void Player::handleEvent(const sf::Event& event)
{
	auto& commands(Context::systemManager->system<PendingChangesSystem>()->commandQueue);
	if(event.type == sf::Event::KeyPressed)
	{
		// Check if pressed key appears in key binding, trigger command if so
		auto found = m_keyBinding.find(event.key.code);
		if(found != m_keyBinding.end())
		{
			if(isRealtimeAction(found->second))
				commands.push(m_startActionBinding.at(found->second));
			if(isImmediateAction(found->second))
				commands.push(m_immediateActionBinding.at(found->second));
		}
	}
	else if(event.type == sf::Event::KeyReleased)
	{
		// Check if released key appears in key binding, trigger command if so
		auto found = m_keyBinding.find(event.key.code);
		if(found != m_keyBinding.end() and isRealtimeAction(found->second))
			commands.push(m_stopActionBinding.at(found->second));
	}
	else if(event.type == sf::Event::MouseButtonPressed)
	{
		auto found = m_mouseButtonBinding.find(event.mouseButton.button);
		if(found != m_mouseButtonBinding.end())
		{
			if(isRealtimeAction(found->second))
				commands.push(m_startActionBinding.at(found->second));
			if(isImmediateAction(found->second))
				commands.push(m_immediateActionBinding.at(found->second));
		}
	}
	else if(event.type == sf::Event::MouseButtonReleased)
	{
		auto found = m_mouseButtonBinding.find(event.mouseButton.button);
		if(found != m_mouseButtonBinding.end() and isRealtimeAction(found->second))
			commands.push(m_stopActionBinding.at(found->second));
	}
	else if(event.type == sf::Event::MouseMoved)
	{

	}
	else if(event.type == sf::Event::MouseWheelMoved)
	{
		if(isImmediateAction(m_mouseWheelBinding))
			commands.push(m_immediateActionBinding.at(m_mouseWheelBinding));
	}
	else if(event.type == sf::Event::JoystickButtonPressed)
	{
		auto found = m_joystickButtonBinding.find(event.joystickButton.button);
		if(found != m_joystickButtonBinding.end())
		{
			if(isRealtimeAction(found->second))
				commands.push(m_startActionBinding.at(found->second));
			if(isImmediateAction(found->second))
				commands.push(m_immediateActionBinding.at(found->second));
		}
	}
	else if(event.type == sf::Event::JoystickButtonReleased)
	{
		auto found = m_joystickButtonBinding.find(event.joystickButton.button);
		if(found != m_joystickButtonBinding.end() and isRealtimeAction(found->second))
			commands.push(m_stopActionBinding.at(found->second));
	}
	else if(event.type == sf::Event::JoystickMoved)
	{
//		auto found = m_joystickAxisBinding.find(event.joystickMove.axis);
//		if(found != m_joystickAxisBinding.end())
//		{
//			if(isRealtimeAction(found->second))
//				commands.push(m_stopActionBinding.at(found->second));
//		}
	}
}

void Player::assignKey(Action action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for(auto it = m_keyBinding.begin(); it != m_keyBinding.end();)
	{
		if(it->second == action)
			m_keyBinding.erase(it++);
		else
			++it;
	}
	// Insert new binding
	m_keyBinding[key] = action;
}

void Player::assignMouseButton(Action action, sf::Mouse::Button button)
{
	// Remove all mouse buttons that already map to action
	for(auto it = m_mouseButtonBinding.begin(); it != m_mouseButtonBinding.end();)
	{
		if(it->second == action)
			m_mouseButtonBinding.erase(it++);
		else
			++it;
	}
	// Insert new binding
	m_mouseButtonBinding[button] = action;
}

void Player::assignMouseWheel(Action action)
{
	m_mouseWheelBinding = action;
}

void Player::assignJoystickButton(Action action, unsigned int buttonIndex)
{
	// Remove all joysticks button that already map to action
	for(auto it = m_joystickButtonBinding.begin(); it != m_joystickButtonBinding.end();)
	{
		if(it->second == action)
			m_joystickButtonBinding.erase(it++);
		else
			++it;
	}
	// Insert new binding
	m_joystickButtonBinding[buttonIndex] = action;
}

void Player::assignJoystickAxis(Action action, sf::Joystick::Axis axis)
{
	// Remove all joysticks axis that already map to action
	for(auto it = m_joystickAxisBinding.begin(); it != m_joystickAxisBinding.end();)
	{
		if(it->second == action)
			m_joystickAxisBinding.erase(it++);
		else
			++it;
	}
	// Insert new binding
	m_joystickAxisBinding[axis] = action;
}

std::vector<sf::Keyboard::Key> Player::getAssignedKeys(Action action) const
{
	std::vector<sf::Keyboard::Key> ret;
	for(auto pair : m_keyBinding)
		if(pair.second == action)
			ret.push_back(pair.first);
	return ret;
}

std::vector<sf::Mouse::Button> Player::getAssignedMouseButtons(Action action) const
{
	std::vector<sf::Mouse::Button> ret;
	for(auto pair : m_mouseButtonBinding)
		if(pair.second == action)
			ret.push_back(pair.first);
	return ret;
}

bool Player::isAssignedToMouseWheel(Action action) const
{
	return m_mouseWheelBinding == action;
}

std::vector<unsigned int> Player::getAssignedJoystickButtons(Action action) const
{
	std::vector<unsigned int> ret;
	for(auto pair : m_joystickButtonBinding)
		if(pair.second == action)
			ret.push_back(pair.first);
	return ret;
}

std::vector<sf::Joystick::Axis> Player::getAssignedJoystickAxis(Action action) const
{
	std::vector<sf::Joystick::Axis> ret;
	for(auto pair : m_joystickAxisBinding)
		if(pair.second == action)
			ret.push_back(pair.first);
	return ret;
}

bool Player::isActived(Action action) const
{
	//Use std::find instead of copy of range here
	std::vector<sf::Keyboard::Key> keyBindings{getAssignedKeys(action)};
	std::vector<sf::Mouse::Button> mouseButtonsBindings{getAssignedMouseButtons(action)};
	std::vector<unsigned int> joystickButtonsBindings{getAssignedJoystickButtons(action)};
	std::vector<sf::Joystick::Axis> joystickAxisBindings{getAssignedJoystickAxis(action)};
	for(auto& key : keyBindings)
		if(sf::Keyboard::isKeyPressed(key))
			return true;

	sf::IntRect windowRect({0, 0}, sf::Vector2i(Context::window->getSize()));
	if(windowRect.contains(sf::Mouse::getPosition(*Context::window)))
		for(auto& button : mouseButtonsBindings)
			if(sf::Mouse::isButtonPressed(button))
				return true;

	for(auto& button : joystickButtonsBindings)
		for(unsigned int i{0}; i < sf::Joystick::Count; ++i)
			if(sf::Joystick::isButtonPressed(i, button))
				return true;

	for(auto& axis : joystickAxisBindings)
		for(unsigned int i{0}; i < sf::Joystick::Count; ++i)
			if(std::fabs(sf::Joystick::getAxisPosition(i, axis)) < 1.f)
				return true;

	return false;
}

void Player::handleInitialInputState()
{
	auto& commands(Context::systemManager->system<PendingChangesSystem>()->commandQueue);
	for(auto& pair : m_startActionBinding)
		if(isActived(pair.first))
			commands.push(pair.second);
	//We must do 2 differents loops because an action can have to differents
	//bindings for the beginning and the ending of the action.
	for(auto& pair : m_stopActionBinding)
		if(not isActived(pair.first))
			commands.push(pair.second);
}

void Player::initializeActions()
{
	m_startActionBinding.emplace(Action::MoveLeft, Command({Category::Player}, Mover(Direction::Left)));
	m_startActionBinding.emplace(Action::MoveRight, Command({Category::Player}, Mover(Direction::Right)));
	m_startActionBinding.emplace(Action::MoveUp, Command({Category::Player}, Mover(Direction::Up)));
	m_startActionBinding.emplace(Action::MoveDown, Command({Category::Player}, Mover(Direction::Down)));

	m_stopActionBinding.emplace(Action::MoveLeft, Command({Category::Player}, Mover(Direction::Left, false)));
	m_stopActionBinding.emplace(Action::MoveRight, Command({Category::Player}, Mover(Direction::Right, false)));
	m_stopActionBinding.emplace(Action::MoveUp, Command({Category::Player}, Mover(Direction::Up, false)));
	m_stopActionBinding.emplace(Action::MoveDown, Command({Category::Player}, Mover(Direction::Down, false)));

	m_immediateActionBinding.emplace(Action::Jump, Command({Category::Player}, Jumper()));
	m_immediateActionBinding.emplace(Action::PickUp, Command({Category::Player}, ArrowPicker()));
	m_immediateActionBinding.emplace(Action::HandToHand, Command({Category::Player}, HandToHand()));
	m_immediateActionBinding.emplace(Action::SearchCorpse, Command({Category::Player}, CorpseSearcher()));
	m_immediateActionBinding.emplace(Action::Inventory, Command({Category::Player}, InventoryOpener()));

	//Do not assign a command to the bending action, the DragAndDrop system already does
}

bool Player::isImmediateAction(Action action) const
{
	//If the action is in the table, it's an immediate action
	return m_immediateActionBinding.find(action) != m_immediateActionBinding.end();
}

bool Player::isRealtimeAction(Action action) const
{
	//If the action is in both tables, it's a realtime action
	return m_startActionBinding.find(action) != m_startActionBinding.end()
		and m_stopActionBinding.find(action) != m_stopActionBinding.end();
}
