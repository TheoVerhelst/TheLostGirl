#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <TheLostGirl/states/MainMenuState.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/states/IntroState.h>

IntroState::IntroState()
{
	Context::eventManager->subscribe<ParametersChange>(*this);
	tgui::Gui& gui(*Context::gui);
	const auto guiWidth = tgui::bindWidth(gui);

	m_background = std::make_shared<tgui::Panel>();
	m_background->setPosition(guiWidth * 0.25f, 0.f);
	m_background->setSize(guiWidth * 0.5f, tgui::bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_logo = std::make_shared<tgui::Picture>(Context::parameters->resourcesPath + "images/title.png");
	m_logo->setPosition((tgui::bindWidth(gui) - tgui::bindWidth(m_logo)) / 2.f, tgui::bindHeight(gui) * 0.02f);
	gui.add(m_logo);

	m_sentence = Context::parameters->guiTheme->load("Label");
	m_sentence->setTextSize(20);
	resetTexts();
	m_sentence->setPosition(tgui::bindWidth(gui)/2.f - tgui::bindWidth(m_sentence)/2.f, tgui::bindHeight(gui) * 0.6f);
	gui.add(m_sentence);
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
