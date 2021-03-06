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
	Context::getEventManager().subscribe<ParametersChange>(*this);
	tgui::Gui& gui(Context::getGui());

	m_background = std::make_shared<tgui::Panel>();
	m_background->setPosition(tgui::bindWidth(gui) * 0.25f, tgui::bindHeight(gui) * 0.f);
	m_background->setSize(tgui::bindWidth(gui) * 0.5f, tgui::bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_logo = std::make_shared<tgui::Picture>(Context::getParameters().resourcesPath + "images/title.png");
	m_logo->setPosition((tgui::bindWidth(gui) - tgui::bindWidth(m_logo)) / 2.f, 15.f);
	gui.add(m_logo);
	
	m_buttonsLayout = std::make_shared<tgui::VerticalLayout>();
	m_buttonsLayout->setPosition(tgui::bindLeft(m_background), tgui::bindBottom(m_logo));
	m_buttonsLayout->setSize(tgui::bindWidth(m_background), tgui::bindHeight(m_background) - tgui::bindHeight(m_logo));
	gui.add(m_buttonsLayout);

	m_newButton = Context::getParameters().guiTheme->load("Button");
	m_newButton->connect("pressed", &MainMenuState::playGame, this);
	m_buttonsLayout->add(m_newButton);

	m_loadButton = Context::getParameters().guiTheme->load("Button");
	m_loadButton->connect("pressed", &MainMenuState::playGame, this);
	m_buttonsLayout->add(m_loadButton);

	m_exitButton = Context::getParameters().guiTheme->load("Button");
	m_exitButton->connect("pressed", &MainMenuState::exitGame, this);
	m_buttonsLayout->add(m_exitButton);

	m_buttonsLayout->addSpace(0.5f);
	resetTexts();
}

MainMenuState::~MainMenuState()
{
	tgui::Gui& gui(Context::getGui());
	gui.remove(m_background);
	gui.remove(m_logo);
	gui.remove(m_buttonsLayout);
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
	m_newButton->setText(Context::getLangManager().tr("New game"));
	m_loadButton->setText(Context::getLangManager().tr("Load game"));
	m_exitButton->setText(Context::getLangManager().tr("Exit"));
}


void MainMenuState::playGame()
{
	requestStackPop();//The main mainu
	requestStackPush<GameState>(Context::getParameters().resourcesPath + "levels/newGame.json");
	requestStackPush<LoadingState>();
}

void MainMenuState::exitGame()
{
	requestStackPop();//The main menu
}
