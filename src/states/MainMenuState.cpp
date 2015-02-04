#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/states/GameState.h>
#include <TheLostGirl/states/LoadingState.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/Parameters.h>

#include <TheLostGirl/states/MainMenuState.h>

MainMenuState::MainMenuState(StateStack& stack):
	State(stack),
	m_background{nullptr},
	m_logo{nullptr},
	m_newButton{nullptr},
	m_loadButton{nullptr},
	m_exitButton{nullptr}
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
	
	// Left:   25% of window width
	// Top:    40% of window height
	// Width:  50% of window width
	// Height: 15% of window height
	m_newButton = tgui::Button::create();
	m_newButton->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.4f));
	m_newButton->setSize(bindWidth(gui, 0.5f), bindHeight(gui, 0.15f));
	m_newButton->setText(LangManager::tr("New game"));
	m_newButton->getRenderer()->setBorders(0.f, 0.f);
	m_newButton->getRenderer()->setProperty("backgroundcolor", "(255, 255, 255, 0)");
	m_newButton->getRenderer()->setProperty("backgroundcolorhover", "(255, 255, 255, 55)");
	m_newButton->getRenderer()->setProperty("backgroundcolordown", "(255, 255, 255, 90)");
	m_newButton->getRenderer()->setProperty("textcolornormal", "(0, 0, 0)");
	unsigned int playGameSignal{m_newButton->connect("pressed", &MainMenuState::playGame, this)};
	gui.add(m_newButton);

	// Left:   0% of window width
	// Top:    55% of window height
	m_loadButton = tgui::Button::copy(m_newButton);
	m_loadButton->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.55f));
	m_loadButton->setText(LangManager::tr("Load game"));
	gui.add(m_loadButton);

	// Left:   0% of window width
	// Top:    70% of window height
	m_exitButton = tgui::Button::copy(m_newButton);
	m_exitButton->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.7f));
	m_exitButton->setText(LangManager::tr("Exit"));
	m_exitButton->connect("pressed", &MainMenuState::exitGame, this);
	m_exitButton->disconnect(playGameSignal);
	gui.add(m_exitButton);
}

MainMenuState::~MainMenuState()
{
	tgui::Gui& gui(getContext().gui);
	gui.remove(m_background);
	gui.remove(m_logo);
	gui.remove(m_newButton);
	gui.remove(m_loadButton);
	gui.remove(m_exitButton);
}

void MainMenuState::draw()
{
}

bool MainMenuState::update(sf::Time)
{
	return true;
}

bool MainMenuState::handleEvent(const sf::Event&)
{
	return false;
}

void MainMenuState::playGame()
{
	requestStackPop();//The main mainu
//	requestStackPop();//The empty level
	requestStackPush<GameState>("resources/levels/S.json");
	requestStackPush<LoadingState>();
}

void MainMenuState::exitGame()
{
	requestStackPop();//The main menu
//	requestStackPop();//The empty level in the background
}
