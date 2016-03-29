#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/states/MainMenuState.hpp>
#include <TheLostGirl/StateStack.hpp>
#include <TheLostGirl/LangManager.hpp>
#include <TheLostGirl/Player.hpp>
#include <TheLostGirl/states/ParametersState.hpp>
#include <TheLostGirl/systems/PendingChangesSystem.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/states/PauseState.hpp>

PauseState::PauseState()
{
	getEventManager().subscribe<ParametersChange>(*this);
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getGui());

	m_background = std::make_shared<tgui::VerticalLayout>();
	m_background->setPosition(bindWidth(gui) * 0.25f, bindHeight(gui) * 0.f);
	m_background->setSize(bindWidth(gui) * 0.5f, bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_background->addSpace();

	tgui::Panel::Ptr titlePanel = std::make_shared<tgui::Panel>();
	titlePanel->setBackgroundColor(sf::Color::Transparent);
	m_background->add(titlePanel);
	m_pauseLabel = getParameters().guiTheme->load("Label");
	titlePanel->add(m_pauseLabel);
	m_pauseLabel->setTextSize(50);

	m_background->addSpace();

	m_backToGameButton = getParameters().guiTheme->load("Button");
	m_backToGameButton->setTextSize(25);
	m_backToGameButton->connect("pressed", &PauseState::backToGame, this);
	m_background->add(m_backToGameButton);

	m_goToOptionsButton = getParameters().guiTheme->load("Button");
	m_goToOptionsButton->setTextSize(25);
	m_goToOptionsButton->connect("pressed", [this]{m_background->hide(); requestStackPush<ParametersState>();});
	m_background->add(m_goToOptionsButton);

	m_backToMainMenuButton = getParameters().guiTheme->load("Button");
	m_backToMainMenuButton->setTextSize(25);
	m_backToMainMenuButton->connect("pressed", &PauseState::backToMainMenu, this);
	m_background->add(m_backToMainMenuButton);

	m_background->addSpace();
	resetTexts();
	m_pauseLabel->setPosition((bindWidth(titlePanel)-bindWidth(m_pauseLabel))/2, 0.f);
}

PauseState::~PauseState()
{
	getGui().remove(m_background);
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
		m_pauseLabel->setText(getLangManager().tr("Pause"));
	if(m_backToGameButton)
		m_backToGameButton->setText(getLangManager().tr("Back to game"));
	if(m_goToOptionsButton)
		m_goToOptionsButton->setText(getLangManager().tr("Parameters"));
	if(m_backToMainMenuButton)
		m_backToMainMenuButton->setText(getLangManager().tr("Back to main menu"));
}


inline void PauseState::backToGame()
{
	requestStackPop();
	getPlayer().handleInitialInputState();
}

inline void PauseState::backToMainMenu()
{
	requestStackPop();//The pause state
	requestStackPop();//The HUD state
	requestStackPop();//The game state
	requestStackPush<MainMenuState>();
}
