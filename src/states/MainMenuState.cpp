#include <TGUI/Gui.hpp>
#include <TheLostGirl/states/GameState.h>
#include <TheLostGirl/states/LoadingState.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/states/MainMenuState.h>

MainMenuState::MainMenuState()
{
	Context::eventManager->subscribe<ParametersChange>(*this);
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(*Context::gui);

	m_background = tgui::VerticalLayout::create();
	m_background->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.f));
	m_background->setSize(bindWidth(gui, 0.5f), bindHeight(gui));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_logo = tgui::Picture::create(Context::parameters->imagePath + "title.png");
	m_logo->setPosition((bindWidth(gui) - bindWidth(m_logo))/2, bindHeight(gui, 0.f));
	gui.add(m_logo);

	m_newButton = tgui::Button::create(Context::parameters->guiConfigFile);
	m_newButton->connect("pressed", &MainMenuState::playGame, this);
	m_background->add(m_newButton);

	m_loadButton = tgui::Button::create(Context::parameters->guiConfigFile);
	m_loadButton->connect("pressed", &MainMenuState::playGame, this);
	m_background->add(m_loadButton);

	m_exitButton = tgui::Button::create(Context::parameters->guiConfigFile);
	m_exitButton->connect("pressed", &MainMenuState::exitGame, this);
	m_background->add(m_exitButton);

    m_background->insertSpace(0, 8.f/3.f);//Add a space before widgets
    m_background->addSpace();//And another space after
	resetTexts();
}

MainMenuState::~MainMenuState()
{
	tgui::Gui& gui(*Context::gui);
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
	m_newButton->setText(Context::langManager->tr("New game"));
	m_loadButton->setText(Context::langManager->tr("Load game"));
	m_exitButton->setText(Context::langManager->tr("Exit"));
}


void MainMenuState::playGame()
{
	requestStackPop();//The main mainu
	requestStackPush<GameState>("resources/levels/S.json");
	requestStackPush<LoadingState>();
}

void MainMenuState::exitGame()
{
	requestStackPop();//The main menu
}
