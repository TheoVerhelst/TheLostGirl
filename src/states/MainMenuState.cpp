#include <TGUI/TGUI.hpp>
#include <TheLostGirl/states/GameState.hpp>
#include <TheLostGirl/states/LoadingState.hpp>
#include <TheLostGirl/LangManager.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/states/MainMenuState.hpp>

MainMenuState::MainMenuState()
{
	getEventManager().subscribe<ParametersChange>(*this);
	tgui::Gui& gui(getGui());

	m_background = std::make_shared<tgui::VerticalLayout>();
	m_background->setPosition(tgui::bindWidth(gui) * 0.25f, tgui::bindHeight(gui) * 0.f);
	m_background->setSize(tgui::bindWidth(gui) * 0.5f, tgui::bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_logo = std::make_shared<tgui::Picture>(getParameters().resourcesPath + "images/title.png");
	m_logo->setPosition((tgui::bindWidth(gui) - tgui::bindWidth(m_logo)) / 2.f, tgui::bindHeight(gui) * 0.02f);
	gui.add(m_logo);

	m_newButton = getParameters().guiTheme->load("Button");
	m_newButton->connect("pressed", &MainMenuState::playGame, this);
	m_background->add(m_newButton);

	m_loadButton = getParameters().guiTheme->load("Button");
	m_loadButton->connect("pressed", &MainMenuState::playGame, this);
	m_background->add(m_loadButton);

	m_exitButton = getParameters().guiTheme->load("Button");
	m_exitButton->connect("pressed", &MainMenuState::exitGame, this);
	m_background->add(m_exitButton);

    m_background->insertSpace(0, 4.f);//Add a space before widgets
    m_background->addSpace();//And another space after
	resetTexts();
}

MainMenuState::~MainMenuState()
{
	tgui::Gui& gui(getGui());
	gui.remove(m_background);
	gui.remove(m_logo);
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

void MainMenuState::receive(const ParametersChange& parametersChange)
{
	if(parametersChange.langChanged)
		resetTexts();
}

void MainMenuState::resetTexts()
{
	m_newButton->setText(getLangManager().tr("New game"));
	m_loadButton->setText(getLangManager().tr("Load game"));
	m_exitButton->setText(getLangManager().tr("Exit"));
}


void MainMenuState::playGame()
{
	requestStackPop();//The main mainu
	requestStackPush<GameState>(getParameters().resourcesPath + "levels/newGame.json");
	requestStackPush<LoadingState>();
}

void MainMenuState::exitGame()
{
	requestStackPop();//The main menu
}
