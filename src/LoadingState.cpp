#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <TheLostGirl/State.h>

#include <TheLostGirl/LoadingState.h>

LoadingState::LoadingState(StateStack& stack, Context context) :
	State(stack, context),
	m_loadLabel(nullptr)
{
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui = getContext().gui;
	
	// Left:   25% of window width
	// Top:    40% of window height
	// Width:  50% of window width
	// Height: 15% of window height
//	m_loadLabel = tgui::Button::create();
//	m_loadLabel->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.4f));
//	m_loadLabel->setSize(bindWidth(gui, 0.5f), bindHeight(gui, 0.15f));
//	m_loadLabel->setText(LangManager::tr("Loading") + "..");
//	m_loadLabel->getRenderer()->setBorders(0.f, 0.f);
//	m_loadLabel->getRenderer()->setProperty("backgroundcolor", "(255, 255, 255, 0)");
//	m_loadLabel->getRenderer()->setProperty("backgroundcolorhover", "(255, 255, 255, 55)");
//	m_loadLabel->getRenderer()->setProperty("backgroundcolordown", "(255, 255, 255, 90)");
//	m_loadLabel->getRenderer()->setProperty("textcolornormal", "(0, 0, 0)");
//	unsigned int playGameSignal = m_loadLabel->connect("pressed", &MainMenuState::playGame, this);
//	gui.add(m_loadLabel);
}

void LoadingState::draw()
{
}

bool LoadingState::update(sf::Time)
{
	return true;
}

bool LoadingState::handleEvent(const sf::Event&)
{
	return false;
}