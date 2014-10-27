#include <iostream>

#include <entityx/entityx.h>

#include <TheLostGirl/Command.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/actions.h>

#include <TheLostGirl/Player.h>

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
	m_keyBinding[sf::Keyboard::LControl]  = Action::FurtherView;
	m_keyBinding[sf::Keyboard::RControl]  = Action::FurtherView;
	m_keyBinding[sf::Keyboard::Space]     = Action::Jump;
	m_keyBinding[sf::Keyboard::LShift]    = Action::Sneak;
	m_keyBinding[sf::Keyboard::RShift]    = Action::Sneak;
	m_keyBinding[sf::Keyboard::A]         = Action::HandToHand;
	m_keyBinding[sf::Keyboard::Comma]     = Action::HandToHand;
	m_keyBinding[sf::Keyboard::Tab]       = Action::Inventory;
	m_keyBinding[sf::Keyboard::L]         = Action::Inventory;
	m_keyBinding[sf::Keyboard::Escape]    = Action::Pause;
	
	m_mouseButtonBinding[sf::Mouse::Button::Left]  = Action::Bend;
	m_mouseButtonBinding[sf::Mouse::Button::Right] = Action::Concentrate;
	
	m_mouseWheelBinding = Action::ChangeArrow;
	
	// Set initial action bindings, fill the maps
	initializeActions();

	// Assign all categories
	for(auto& pair : m_startActionBinding)
	{
		pair.second.category = Category::Player;
		pair.second.targetIsSpecific = false;
	}
	for(auto& pair : m_stopActionBinding)
	{
		pair.second.category = Category::Player;
		pair.second.targetIsSpecific = false;
	}
	for(auto& pair : m_immediateActionBinding)
	{
		pair.second.category = Category::Player;
		pair.second.targetIsSpecific = false;
	}
}

Player::~Player()
{}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if(event.type == sf::Event::KeyPressed)
	{
		// Check if pressed key appears in key binding, trigger command if so
		auto found = m_keyBinding.find(event.key.code);
		if(found != m_keyBinding.end())
		{
			if(isRealtimeAction(found->second))
				commands.push(m_startActionBinding[found->second]);
			if(isImmediateAction(found->second))
				commands.push(m_immediateActionBinding[found->second]);
		}
	}
	else if(event.type == sf::Event::KeyReleased)
	{
		// Check if released key appears in key binding, trigger command if so
		auto found = m_keyBinding.find(event.key.code);
		if(found != m_keyBinding.end() and isRealtimeAction(found->second))
			commands.push(m_stopActionBinding[found->second]);
	}
	else if(event.type == sf::Event::MouseButtonPressed)
	{
		auto found = m_mouseButtonBinding.find(event.mouseButton.button);
		if(found != m_mouseButtonBinding.end())
		{
			if(isRealtimeAction(found->second))
				commands.push(m_startActionBinding[found->second]);
			if(isImmediateAction(found->second))
				commands.push(m_immediateActionBinding[found->second]);
		}
	}
	else if(event.type == sf::Event::MouseButtonReleased)
	{
		auto found = m_mouseButtonBinding.find(event.mouseButton.button);
		if(found != m_mouseButtonBinding.end() and isRealtimeAction(found->second))
		{
			commands.push(m_stopActionBinding[found->second]);
		}
	}
	else if(event.type == sf::Event::MouseMoved)
	{
		
	}
	else if(event.type == sf::Event::MouseWheelMoved)
	{
		
	}
	else if(event.type == sf::Event::JoystickButtonPressed)
	{
		auto found = m_joystickButtonBinding.find(event.joystickButton.button);
		if(found != m_joystickButtonBinding.end())
		{
			if(isRealtimeAction(found->second))
				commands.push(m_startActionBinding[found->second]);
			if(isImmediateAction(found->second))
				commands.push(m_immediateActionBinding[found->second]);
		}
	}
	else if(event.type == sf::Event::JoystickButtonReleased)
	{
		auto found = m_joystickButtonBinding.find(event.joystickButton.button);
		if(found != m_joystickButtonBinding.end() and isRealtimeAction(found->second))
			commands.push(m_stopActionBinding[found->second]);
	}
	else if(event.type == sf::Event::JoystickMoved)
	{
		
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

std::pair<bool, sf::Keyboard::Key> Player::getAssignedKey(Action action) const
{
	for(auto pair : m_keyBinding)
	{
		if(pair.second == action)
			return std::pair<bool, sf::Keyboard::Key>(true, pair.first);
	}
	return std::pair<bool, sf::Keyboard::Key>(false, sf::Keyboard::Unknown);
}

std::pair<bool, sf::Mouse::Button> Player::getAssignedMouseButton(Action action) const
{
	for(auto pair : m_mouseButtonBinding)
	{
		if(pair.second == action)
			return std::pair<bool, sf::Mouse::Button>(true, pair.first);
	}
	return std::pair<bool, sf::Mouse::Button>(false, sf::Mouse::Button::Left);
}

bool Player::isAssignedToMouseWheel(Action action) const
{
	return m_mouseWheelBinding == action;
}

std::pair<bool, unsigned int> Player::getAssignedJoystickButton(Action action) const
{
	for(auto pair : m_joystickButtonBinding)
	{
		if(pair.second == action)
			return std::pair<bool, unsigned int>(true, pair.first);
	}
	return std::pair<bool, unsigned int>(false, 0);
}

std::pair<bool, sf::Joystick::Axis> Player::getAssignedJoystickAxis(Action action) const
{
	for(auto pair : m_joystickAxisBinding)
	{
		if(pair.second == action)
			return std::pair<bool, sf::Joystick::Axis>(true, pair.first);
	}
	return std::pair<bool, sf::Joystick::Axis>(false, sf::Joystick::Axis::X);
}

bool Player::isDragAndDropActive() const
{
	std::pair<bool, sf::Mouse::Button> result = getAssignedMouseButton(Action::Bend);//Get the button binded with the bending
	return result.first and sf::Mouse::isButtonPressed(result.second);//If there is a binding and if that button is pressed
}


void Player::initializeActions()
{
	m_startActionBinding[MoveLeft].action = LeftMover(true);
	m_startActionBinding[MoveRight].action =  RightMover(true);
	m_startActionBinding[MoveUp].action =  UpMover(true);
	m_startActionBinding[MoveDown].action =  DownMover(true);
	m_startActionBinding[Bend].action = BowBender(true);
	
	m_stopActionBinding[MoveLeft].action =  LeftMover(false);
	m_stopActionBinding[MoveRight].action = RightMover(false);
	m_stopActionBinding[MoveUp].action = UpMover(false);
	m_stopActionBinding[MoveDown].action = DownMover(false);
	m_stopActionBinding[Bend].action = BowBender(false);
	
	m_immediateActionBinding[Jump].action = Jumper();
}

bool Player::isImmediateAction(Action action)
{
	auto found = m_immediateActionBinding.find(action);
	return found != m_immediateActionBinding.end();//If the action is in the table, it's a immediate action
}

bool Player::isRealtimeAction(Action action)
{
	auto foundStart = m_startActionBinding.find(action);
	auto foundStop = m_stopActionBinding.find(action);
	return foundStart != m_startActionBinding.end() and foundStop != m_stopActionBinding.end();//If the action is in the table, it's a realtime action
}