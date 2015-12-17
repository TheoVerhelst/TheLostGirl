#include <TGUI/TGUI.hpp>
#include <entityx/System.h>
#include <TheLostGirl/states/MainMenuState.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/states/ParametersState.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/states/PauseState.h>

PauseState::PauseState()
{
	Context::eventManager->subscribe<ParametersChange>(*this);
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(*Context::gui);

	m_background = std::make_shared<tgui::VerticalLayout>();
	m_background->setPosition(bindWidth(gui) * 0.25f, bindHeight(gui) * 0.f);
	m_background->setSize(bindWidth(gui) * 0.5f, bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_background->addSpace();

	tgui::Panel::Ptr titlePanel = std::make_shared<tgui::Panel>();
	titlePanel->setBackgroundColor(sf::Color::Transparent);
	m_background->add(titlePanel);
	m_pauseLabel = Context::parameters->guiTheme->load("Label");
	titlePanel->add(m_pauseLabel);
	m_pauseLabel->setTextSize(50);

	m_background->addSpace();

	m_backToGameButton = Context::parameters->guiTheme->load("Button");
	m_backToGameButton->setTextSize(25);
	m_backToGameButton->connect("pressed", &PauseState::backToGame, this);
	m_background->add(m_backToGameButton);

	m_goToOptionsButton = Context::parameters->guiTheme->load("Button");
	m_goToOptionsButton->setTextSize(25);
	m_goToOptionsButton->connect("pressed", [this]{m_background->hide(); requestStackPush<ParametersState>();});
	m_background->add(m_goToOptionsButton);

	m_backToMainMenuButton = Context::parameters->guiTheme->load("Button");
	m_backToMainMenuButton->setTextSize(25);
	m_backToMainMenuButton->connect("pressed", &PauseState::backToMainMenu, this);
	m_background->add(m_backToMainMenuButton);

	m_background->addSpace();
	resetTexts();
	m_pauseLabel->setPosition((bindWidth(titlePanel)-bindWidth(m_pauseLabel))/2, 0.f);
}

PauseState::~PauseState()
{
	Context::gui->remove(m_background);
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
		m_pauseLabel->setText(Context::langManager->tr("Pause"));
	if(m_backToGameButton)
		m_backToGameButton->setText(Context::langManager->tr("Back to game"));
	if(m_goToOptionsButton)
		m_goToOptionsButton->setText(Context::langManager->tr("Parameters"));
	if(m_backToMainMenuButton)
		m_backToMainMenuButton->setText(Context::langManager->tr("Back to main menu"));
}


inline void PauseState::backToGame()
{
	requestStackPop();
	Context::player->handleInitialInputState();
}

inline void PauseState::backToMainMenu()
{
	requestStackPop();//The pause state
	requestStackPop();//The HUD state
	requestStackPop();//The game state
	requestStackPush<MainMenuState>();
}
