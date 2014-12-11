#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/Parameters.h>

#include <TheLostGirl/states/IntroState.h>

IntroState::IntroState(StateStack& stack, Context context):
	State(stack, context),
	m_background{nullptr},
	m_logo{nullptr},
	m_sentence{nullptr}
{
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);
	
	m_background = tgui::Panel::create();
	m_background->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.f));
	m_background->setSize(bindWidth(gui, 0.5f), bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);
	
	m_logo = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "title.png");
	m_logo->setPosition((bindWidth(gui) - bindWidth(m_logo))/2, bindHeight(gui, 0.f));
	gui.add(m_logo);
	
	m_sentence = tgui::Label::create();
	m_sentence->setText(LangManager::tr("Press any key to continue..."));
	m_sentence->setTextSize(30);
	m_sentence->setPosition((bindWidth(gui) - bindWidth(m_sentence))/2, bindHeight(gui, 0.5));
	m_sentence->setTextColor(sf::Color::Black);
	gui.add(m_sentence);
}

IntroState::~IntroState()
{
	tgui::Gui& gui(getContext().gui);
	gui.remove(m_background);
	gui.remove(m_logo);
	gui.remove(m_sentence);
}

void IntroState::draw()
{
}

bool IntroState::update(sf::Time)
{
	return true;
}

bool IntroState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed)
	{
		requestStackPop();
		requestStackPush(States::MainMenu);
	}
	return false;
}
