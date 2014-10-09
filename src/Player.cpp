#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>

#include <TheLostGirl/Command.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/components.h>

#include <TheLostGirl/Player.h>

Player::Player()
{
		// Set initial key bindings
//		m_keyBinding[sf::Keyboard::Q] = Action::MoveLeft;
//		m_keyBinding[sf::Keyboard::D] = Action::MoveRight;
//		m_keyBinding[sf::Keyboard::Z] = Action::MoveUp;
//		m_keyBinding[sf::Keyboard::S] = Action::MoveDown;
	
	// Set initial action bindings, fill the maps
	initializeActions();

	// Assign all categories to player's paddle
//	for(auto& pair : m_startActionBinding)
//	{
//		if(m_isLeft)
//			pair.second.category = Category::LeftPlayerPaddle;
//		else
//			pair.second.category = Category::RightPlayerPaddle;
//	}
//	for(auto& pair : m_stopActionBinding)
//	{
//		if(m_isLeft)
//			pair.second.category = Category::LeftPlayerPaddle;
//		else
//			pair.second.category = Category::RightPlayerPaddle;
//	}
//	for(auto& pair : m_realtimeActionBinding)
//	{
//		if(m_isLeft)
//			pair.second.category = Category::LeftPlayerPaddle;
//		else
//			pair.second.category = Category::RightPlayerPaddle;
//	}
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
//	m_startActionBinding[MoveLeft].action = PaddleMover(-paddleMaxSpeed, 0.f);
//	m_startActionBinding[MoveRight].action = PaddleMover(+paddleMaxSpeed, 0.f);
//	m_startActionBinding[MoveUp].action = PaddleMover(0.f, -paddleMaxSpeed);
//	m_startActionBinding[MoveDown].action = PaddleMover(0.f, +paddleMaxSpeed);
//	
//	m_stopActionBinding[MoveLeft].action = PaddleMover(+paddleMaxSpeed, 0.f);
//	m_stopActionBinding[MoveRight].action = PaddleMover(-paddleMaxSpeed, 0.f);
//	m_stopActionBinding[MoveUp].action = PaddleMover(0.f, +paddleMaxSpeed);
//	m_stopActionBinding[MoveDown].action = PaddleMover(0.f, -paddleMaxSpeed);
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