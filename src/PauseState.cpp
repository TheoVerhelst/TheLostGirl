#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/LangManager.h>

#include <TheLostGirl/PauseState.h>

PauseState::PauseState(StateStack& stack, Context context) :
	State(stack, context),
	m_background{nullptr},
	m_pauseLabel{nullptr},
	m_backToGameButton{nullptr},
	m_goToOptionsButton{nullptr},
	m_backToMainMenuButton{nullptr}
{
	tgui::Gui& gui = getContext().gui;
	
	m_background = tgui::Panel::create();
	m_background->setPosition(tgui::bindWidth(gui, 0.25f), tgui::bindHeight(gui, 0.f));
	m_background->setSize(tgui::bindWidth(gui, 0.5f), tgui::bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);
	
	m_pauseLabel = tgui::Label::create();
	m_pauseLabel->setText(LangManager::tr("Pause"));
	m_pauseLabel->setPosition(tgui::bindWidth(gui, 0.25f), tgui::bindHeight(gui, 0.1f));
	m_pauseLabel->setSize(tgui::bindWidth(gui, 0.5f), tgui::bindHeight(gui, 0.15f));
	gui.add(m_pauseLabel);
	
	// Left:   25% of window width
	// Top:    40% of window height
	// Width:  50% of window width
	// Height: 15% of window height
	m_backToGameButton = tgui::Button::create();
	m_backToGameButton->setPosition(tgui::bindWidth(gui, 0.25f), tgui::bindHeight(gui, 0.4f));
	m_backToGameButton->setSize(tgui::bindWidth(gui, 0.5f), tgui::bindHeight(gui, 0.15f));
	m_backToGameButton->setText(LangManager::tr("Back to game"));
	m_backToGameButton->getRenderer()->setBorders(0.f, 0.f);
	m_backToGameButton->getRenderer()->setProperty("backgroundcolor", "(255, 255, 255, 0)");
	m_backToGameButton->getRenderer()->setProperty("backgroundcolorhover", "(255, 255, 255, 55)");
	m_backToGameButton->getRenderer()->setProperty("backgroundcolordown", "(255, 255, 255, 90)");
	m_backToGameButton->getRenderer()->setProperty("textcolornormal", "(0, 0, 0)");
	unsigned int backToGameSignal = m_backToGameButton->connect("pressed", &PauseState::backToGame, this);
	gui.add(m_backToGameButton);

	// Left:   0% of window width
	// Top:    55% of window height
	m_goToOptionsButton = tgui::Button::copy(m_backToGameButton);
	m_goToOptionsButton->setPosition(tgui::bindWidth(gui, 0.25f), tgui::bindHeight(gui, 0.55f));
	m_goToOptionsButton->setText(LangManager::tr("Parameters"));
	m_goToOptionsButton->connect("pressed", &PauseState::goToOptions, this);
	m_goToOptionsButton->disconnect(backToGameSignal);
	gui.add(m_goToOptionsButton);

	// Left:   0% of window width
	// Top:    70% of window height
	m_backToMainMenuButton = tgui::Button::copy(m_backToGameButton);
	m_backToMainMenuButton->setPosition(tgui::bindWidth(gui, 0.25f), tgui::bindHeight(gui, 0.7f));
	m_backToMainMenuButton->setText(LangManager::tr("Back to main menu"));
	m_backToMainMenuButton->connect("pressed", &PauseState::backToMainMenu, this);
	m_backToMainMenuButton->disconnect(backToGameSignal);
	gui.add(m_backToMainMenuButton);
}

void PauseState::draw()
{
}

bool PauseState::update(sf::Time)
{
	return false;
}

bool PauseState::handleEvent(const sf::Event&)
{
	return false;
}

void PauseState::backToGame()
{
	requestStackPop();
	tgui::Gui& gui = getContext().gui;
	gui.remove(m_background);
	gui.remove(m_pauseLabel);
	gui.remove(m_backToGameButton);
	gui.remove(m_goToOptionsButton);
	gui.remove(m_backToMainMenuButton);
}

void PauseState::goToOptions()
{
}

void PauseState::backToMainMenu()
{
}