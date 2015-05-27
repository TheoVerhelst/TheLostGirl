#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/states/KeyConfigurationState.h>

KeyConfigurationState::KeyConfigurationState(StateStack& stack) :
	State(stack)
{
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);

	m_background = tgui::Panel::create();
	m_background->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.f));
	m_background->setSize(bindWidth(gui, 0.5f), bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_title = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_title->setPosition((bindWidth(m_background) - bindWidth(m_title))/2.f, bindHeight(m_background, 0.1f));
	m_title->setTextSize(80);
	m_background->add(m_title);
}

KeyConfigurationState::~KeyConfigurationState()
{
	getContext().gui.remove(m_background);
}

void KeyConfigurationState::draw()
{
}

bool KeyConfigurationState::update(sf::Time)
{
	return false;
}

bool KeyConfigurationState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		requestStackPop();
	return false;
}

void KeyConfigurationState::resetTexts()
{
	m_title->setText(getContext().langManager.tr("Parameters"));
}
