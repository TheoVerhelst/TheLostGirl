#include <iostream>

#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>

#include <TheLostGirl/Command.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/actions.h>

#include <TheLostGirl/Player.h>

Player::Player()
{
	// Set initial key bindings
	m_keyBinding[sf::Keyboard::Q] = Action::MoveLeft;
	m_keyBinding[sf::Keyboard::D] = Action::MoveRight;
	m_keyBinding[sf::Keyboard::Z] = Action::MoveUp;
	m_keyBinding[sf::Keyboard::S] = Action::MoveDown;
	m_keyBinding[sf::Keyboard::Space] = Action::Jump;
	m_keyBinding[sf::Keyboard::B] = Action::Bend;
	m_keyBinding[sf::Keyboard::M] = Action::Sneak;
	m_keyBinding[sf::Keyboard::H] = Action::HandToHand;
	
	// Set initial action bindings, fill the maps
	initializeActions();

	// Assign all categories
	for(auto& pair : m_startActionBinding)
		pair.second.category = Category::Player;
	
	for(auto& pair : m_stopActionBinding)
		pair.second.category = Category::Player;
	
	for(auto& pair : m_immediateActionBinding)
		pair.second.category = Category::Player;
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
}

void Player::assignKey(Action action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for(auto itr = m_keyBinding.begin(); itr != m_keyBinding.end();)
	{
		if(itr->second == action)
			m_keyBinding.erase(itr++);
		else
			++itr;
	}
	// Insert new binding
	m_keyBinding[key] = action;
}

sf::Keyboard::Key Player::getAssignedKey(Action action) const
{
	for(auto pair : m_keyBinding)
	{
		if(pair.second == action)
			return pair.first;
	}
	return sf::Keyboard::Unknown;
}

void Player::initializeActions()
{
	m_startActionBinding[MoveLeft].action = LeftMover(true);
	m_startActionBinding[MoveRight].action =  RightMover(true);
	m_startActionBinding[MoveUp].action =  UpMover(true);
	m_startActionBinding[MoveDown].action =  DownMover(true);
	
	m_stopActionBinding[MoveLeft].action =  LeftMover(false);
	m_stopActionBinding[MoveRight].action = RightMover(false);
	m_stopActionBinding[MoveUp].action = UpMover(false);
	m_stopActionBinding[MoveDown].action = DownMover(false);
	
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