#include <SFML/Graphics.hpp>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/StateIdentifiers.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/functions.h>

#include <TheLostGirl/MainMenuState.h>

MainMenuState::MainMenuState(StateStack& stack, Context context):
	State(stack, context),
	m_background{nullptr},
	m_logo{nullptr},
	m_newButton{nullptr},
	m_loadButton{nullptr},
	m_exitButton{nullptr}
{
	tgui::Gui& gui = getContext().gui;
	m_background = tgui::Picture::create(toPath(windowSize) + "backgroundMenu.png");
	m_background->setPosition(tgui::bindWidth(gui.getContainer())*0.25f, tgui::bindHeight(gui.getContainer())*0.f);
	gui.add(m_background);
	
	m_logo = tgui::Picture::create(toPath(windowSize) + "title.png");
	m_logo->setPosition(tgui::bindWidth(gui.getContainer())*0.25f, tgui::bindHeight(gui.getContainer())*0.f);
	gui.add(m_logo);
	
	// Left:   25% of window width
	// Top:    40% of window height
	// Width:  50% of window width
	// Height: 15% of window height
	m_newButton = tgui::Button::create();
	m_newButton->setPosition(tgui::bindWidth(gui.getContainer())*0.25f, tgui::bindHeight(gui.getContainer())*0.4f);
	m_newButton->setSize(tgui::bindWidth(gui.getContainer())*0.5f, tgui::bindHeight(gui.getContainer())*0.15f);
	m_newButton->setText(LangManager::tr("New game"));
	m_newButton->getRenderer()->setBorders(0.f, 0.f);
	m_newButton->getRenderer()->setProperty("backgroundcolor", "(0, 0, 0, 0)");
	m_newButton->getRenderer()->setProperty("textcolornormal", "(0, 0, 0)");
	m_newButton->getRenderer()->setProperty("textcolorhover", "(60, 60, 60)");
	m_newButton->connect("pressed", &MainMenuState::playGame, this);
	gui.add(m_newButton);

	// Left:   0% of window width
	// Top:    55% of window height
	m_loadButton = tgui::Button::copy(m_newButton);
	m_loadButton->setPosition(tgui::bindWidth(gui.getContainer())*0.25f, tgui::bindHeight(gui.getContainer())*0.55f);
	m_loadButton->setText(LangManager::tr("Load game"));
	gui.add(m_loadButton);

	// Left:   0% of window width
	// Top:    70% of window height
	m_exitButton = tgui::Button::copy(m_newButton);
	m_exitButton->setPosition(tgui::bindWidth(gui.getContainer())*0.25f, tgui::bindHeight(gui.getContainer())*0.7f);
	m_exitButton->setText(LangManager::tr("Exit"));
	m_exitButton->connect("pressed", &MainMenuState::requestStackPop, this);
	gui.add(m_exitButton);
}

MainMenuState::~MainMenuState()
{}

void MainMenuState::draw()
{
}

bool MainMenuState::update(sf::Time dt)
{
	return true;
}

bool MainMenuState::handleEvent(const sf::Event& event)
{
	return false;
}

void MainMenuState::playGame()
{
	requestStackPop();
	requestStackPush(States::Game);	
	tgui::Gui& gui = getContext().gui;
	gui.remove(m_background);
	gui.remove(m_logo);
	gui.remove(m_newButton);
	gui.remove(m_loadButton);
	gui.remove(m_exitButton);
}