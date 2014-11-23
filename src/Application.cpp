#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Gui.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/StateIdentifiers.h>
#include <TheLostGirl/State.h>
#include <TheLostGirl/states.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/systems.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/LangManager.h>

#include <TheLostGirl/Application.h>

Application::Application(bool debugMode):
	m_parameters{EN,
				debugMode,
				1,
				scales[m_parameters.scaleIndex],
				120.f*m_parameters.scale,
				{0.0f, 9.80665f},
				"resources/fonts/euphorigenic.ttf"},
	m_window{sf::VideoMode{static_cast<unsigned int>(1920*m_parameters.scale), static_cast<unsigned int>(1080*m_parameters.scale)}, "The Lost Girl"},
	m_gui{m_window},
	m_textureManager{},
	m_fontManager{},
	m_eventManager{},
	m_entityManager{m_eventManager},
	m_systemManager{m_entityManager, m_eventManager},
	m_commandQueue{},
	m_world{m_parameters.gravity},
	m_stateStack{State::Context{m_parameters,
								m_window,
								m_textureManager,
								m_fontManager,
								m_gui,
								m_eventManager,
								m_entityManager,
								m_systemManager,
								m_world,
								m_player,
								m_commandQueue}},			
	m_debugDraw(m_stateStack.getContext())
{
}

Application::~Application()
{
}

int Application::init()
{
	try
	{
		srand(static_cast<unsigned int>(time(nullptr)));//Init random numbers
		registerStates();
		registerSystems();
		m_window.setKeyRepeatEnabled(false);//Desactive the key repeating
		m_window.setFramerateLimit(60);//Limit the FPS
		LangManager::setLang(m_parameters.lang);//Set and load the lang
		m_fontManager.load("menu", m_parameters.textFont);//Load the GUI font
		m_fontManager.load("debug", "resources/fonts/FreeMonoBold.ttf");//Load the debug font
		m_gui.setGlobalFont(std::make_shared<sf::Font>(m_fontManager.get("menu")));//Set the GUI font
		m_world.SetDebugDraw(&m_debugDraw);//Set the debug drawer
		m_debugDraw.SetFlags(b2Draw::e_shapeBit|b2Draw::e_jointBit|b2Draw::e_pairBit);//Debug drawing flags
		m_systemManager.configure();//Init the manager
		m_stateStack.pushState(States::MainMenu);//Start with the main menu
	}
	catch(std::runtime_error& e)
	{
		std::cerr << "Exception : " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

int Application::run()
{
	try
	{
		//Main loop
		sf::Clock clock;
		while(m_window.isOpen())
		{
			processInput();
			update(clock.restart());
			if(m_stateStack.isEmpty())
				m_window.close();
			render();
		}
	}
	catch(std::runtime_error& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

void Application::processInput()
{
	sf::Event event;
	while(m_window.pollEvent(event))
	{
		if(event.type == sf::Event::Closed)
			m_window.close();
		else if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::F2)
			m_parameters.debugMode = not m_parameters.debugMode;//Switch the debug mode
		else if(event.type == sf::Event::Resized)
		{
			sf::View view = m_window.getDefaultView();//Resized view, maybe not in a 16:9 ratio
			view.setViewport(handleResize(event.size));
			m_window.setView(view);
			m_gui.setView(view);
		}
		m_stateStack.handleEvent(event);
		m_gui.handleEvent(event);
	}
}

void Application::update(sf::Time dt)
{
	m_stateStack.update(dt);
}

void Application::render()
{
	m_window.clear({127, 127, 127});
	m_stateStack.draw();
	m_debugDraw.drawDebugAth();
	if(m_parameters.debugMode)
		m_world.DrawDebugData();
	m_gui.draw();
	m_window.display();
}

void Application::registerStates()
{
	m_stateStack.registerState<MainMenuState>(States::MainMenu);
	m_stateStack.registerState<LoadingState>(States::Loading);
	m_stateStack.registerState<IntroState>(States::Intro);
	m_stateStack.registerState<HUDState>(States::HUD);
	m_stateStack.registerState<GameState>(States::Game);
	m_stateStack.registerState<PauseState>(States::Pause);
}

void Application::registerSystems()
{
	m_systemManager.add<PhysicsSystem>(m_world, m_parameters);
	m_systemManager.add<ActionsSystem>(m_commandQueue);
	m_systemManager.add<AnimationsSystem>();
	m_systemManager.add<RenderSystem>(m_window);
	m_systemManager.add<DragAndDropSystem>(m_window, m_commandQueue);
	m_systemManager.add<ScrollingSystem>(m_window, m_parameters);
	m_systemManager.add<TimeSystem>();
}
