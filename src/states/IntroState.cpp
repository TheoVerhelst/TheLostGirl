#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/TGUI.hpp>
#include <TheLostGirl/states/MainMenuState.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/states/IntroState.h>

IntroState::IntroState()
{
	Context::eventManager->subscribe<ParametersChange>(*this);
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(*Context::gui);

	m_background = std::make_shared<tgui::Panel>();
	m_background->setPosition(bindWidth(gui) * 0.25f, bindHeight(gui) * 0.f);
	m_background->setSize(bindWidth(gui) * 0.5f, bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_logo = std::make_shared<tgui::Picture>(Context::parameters->imagePath + "title.png");
	m_logo->setPosition((bindWidth(gui) - bindWidth(m_logo))/2, bindHeight(gui) * 0.f);
	gui.add(m_logo);

	m_sentence = Context::parameters->guiTheme->load("Label");
	m_sentence->setTextSize(15);
	m_sentence->setPosition((bindWidth(gui) - bindWidth(m_sentence))/2, bindHeight(gui) * 0.5);
	gui.add(m_sentence);
	resetTexts();
}

IntroState::~IntroState()
{
	Context::gui->remove(m_background);
	Context::gui->remove(m_logo);
	Context::gui->remove(m_sentence);
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
		requestStackPush<MainMenuState>();
	}
	return false;
}

void IntroState::receive(const ParametersChange& parametersChange)
{
	if(parametersChange.langChanged)
		resetTexts();
}

void IntroState::resetTexts()
{
	m_sentence->setText(Context::langManager->tr("Press any key to continue..."));
}
