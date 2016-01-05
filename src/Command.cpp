#include <entityx/entityx.h>
#include <TheLostGirl/Category.hpp>
#include <TheLostGirl/Command.hpp>

Command::Command(const FlagSet<Category>& category, std::function<void(entityx::Entity)> action):
	m_action{action},
	m_targetIsSpecific{false},
	m_category{category}
{
}

Command::Command(entityx::Entity entity, std::function<void(entityx::Entity)> action):
	m_action{action},
	m_targetIsSpecific{true},
	m_entity{entity}
{
}

Command::Command(const Command& other):
	m_action{other.m_action},
	m_targetIsSpecific{other.m_targetIsSpecific}
{
	if(m_targetIsSpecific)
		new (&m_entity) entityx::Entity(other.m_entity);
	else
		new (&m_category) FlagSet<Category>(other.m_category);
}

Command::~Command()
{
	if(m_targetIsSpecific)
		m_entity.~Entity();
	else
		m_category.~FlagSet<Category>();
}

Command& Command::operator=(const Command& other)
{
	if(m_targetIsSpecific == other.m_targetIsSpecific)
	{
		if(m_targetIsSpecific)
			m_entity = other.m_entity;
		else
			m_category = other.m_category;
	}
	else
	{
		if(m_targetIsSpecific)
			allocate(other.m_category);
		else
			allocate(other.m_entity);
		m_targetIsSpecific = other.m_targetIsSpecific;
	}
	m_action = other.m_action;
	return *this;
}

const std::function<void(entityx::Entity)>& Command::getAction() const
{
	return m_action;
}

void Command::setAction(const std::function<void(entityx::Entity)>& action)
{
	m_action = action;
}

bool Command::isTargetSpecific() const
{
	return m_targetIsSpecific;
}

void Command::setTarget(const FlagSet<Category>& category)
{
	if(m_targetIsSpecific)
		allocate(category);
	else
		m_category = category;

}

void Command::setTarget(entityx::Entity entity)
{
	if(m_targetIsSpecific)
		allocate(entity);
	else
		m_entity = entity;
}

const FlagSet<Category>& Command::getCategory() const throw(std::logic_error)
{
	if(m_targetIsSpecific)
		throw std::logic_error("Command::getCategory were called altough a specific entity was set.");
	return m_category;
}

entityx::Entity Command::getEntity() const throw(std::logic_error)
{
	if(not m_targetIsSpecific)
		throw std::logic_error("Command::getEntity were called altough a category was set.");
	return m_entity;
}

void Command::allocate(const FlagSet<Category>& category)
{
	m_entity.~Entity();
	new(&m_category) FlagSet<Category>(category);
}

void Command::allocate(entityx::Entity entity)
{
	m_category.~FlagSet<Category>();
	new(&m_entity) entityx::Entity(entity);
}
