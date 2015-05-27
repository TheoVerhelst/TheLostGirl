#include <TGUI/Gui.hpp>
#include <entityx/System.h>
#include <TheLostGirl/State.h>
#include <TheLostGirl/states/MainMenuState.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/states/ParametersState.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/states/PauseState.h>

PauseState::PauseState(StateStack& stack) :
	State(stack),
	m_background{nullptr},
	m_pauseLabel{nullptr},
	m_backToGameButton{nullptr},
	m_goToOptionsButton{nullptr},
	m_backToMainMenuButton{nullptr}
{
	getContext().eventManager.subscribe<ParametersChange>(*this);
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);

	m_background = tgui::Panel::create();
	m_background->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.f));
	m_background->setSize(bindWidth(gui, 0.5f), bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	// Left:   25% of window width
	// Top:    10% of window height
	m_pauseLabel = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_pauseLabel->setPosition((bindWidth(m_background) - bindWidth(m_pauseLabel))/2.f, bindHeight(m_background, 0.1f));
	m_pauseLabel->setTextSize(80);
	m_background->add(m_pauseLabel);

	// Left:   25% of window width
	// Top:    40% of window height
	// Width:  50% of window width
	// Height: 15% of window height
	m_backToGameButton = tgui::Button::create(getContext().parameters.guiConfigFile);
	m_backToGameButton->setPosition(0.f, bindHeight(m_background, 0.4f));
	m_backToGameButton->setSize(bindWidth(m_background), bindHeight(m_background, 0.15f));
	m_backToGameButton->setTextSize(50);
	unsigned int backToGameSignal{m_backToGameButton->connect("pressed", &PauseState::backToGame, this)};
	m_background->add(m_backToGameButton);

	// Left:   25% of window width
	// Top:    55% of window height
	m_goToOptionsButton = tgui::Button::copy(m_backToGameButton);
	m_goToOptionsButton->setPosition(0.f, bindHeight(m_background, 0.55f));
	m_goToOptionsButton->connect("pressed", [this]{m_background->hide(); requestStackPush<ParametersState>();});
	m_goToOptionsButton->disconnect(backToGameSignal);
	m_background->add(m_goToOptionsButton);

	// Left:   25% of window width
	// Top:    70% of window height
	m_backToMainMenuButton = tgui::Button::copy(m_backToGameButton);
	m_backToMainMenuButton->setPosition(0.f, bindHeight(m_background, 0.7f));
	m_backToMainMenuButton->connect("pressed", &PauseState::backToMainMenu, this);
	m_backToMainMenuButton->disconnect(backToGameSignal);
	m_background->add(m_backToMainMenuButton);
	resetTexts();
}

PauseState::~PauseState()
{
	getContext().gui.remove(m_background);
}

void PauseState::draw()
{
}

bool PauseState::update(sf::Time)
{
	return false;
}

bool PauseState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		backToGame();
	//If the background was hidden, then a state was pushed on top of this one.
	//But if the code here is executed , that means that this state is again at the top.
	m_background->show();
	return false;
}

void PauseState::receive(const ParametersChange& parametersChange)
{
	if(parametersChange.langChanged)
		resetTexts();
}

void PauseState::resetTexts()
{
	if(m_pauseLabel)
		m_pauseLabel->setText(getContext().langManager.tr("Pause"));
	if(m_backToGameButton)
		m_backToGameButton->setText(getContext().langManager.tr("Back to game"));
	if(m_goToOptionsButton)
		m_goToOptionsButton->setText(getContext().langManager.tr("Parameters"));
	if(m_backToMainMenuButton)
		m_backToMainMenuButton->setText(getContext().langManager.tr("Back to main menu"));
}


inline void PauseState::backToGame()
{
	requestStackPop();
	getContext().player.handleInitialInputState();
}

inline void PauseState::backToMainMenu()
{
	requestStackPop();//The pause state
	requestStackPop();//The HUD state
	requestStackPop();//The game state
	requestStackPush<MainMenuState>();
}
