#include <SFML/Graphics/RenderTexture.hpp>
#include <Box2D/Box2D.h>

#include <TheLostGirl/states/IntroState.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/systems.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/LangManager.h>

#include <TheLostGirl/Application.h>

Application::Application(bool debugMode):
	m_parameters{},
	m_window{},
	m_postEffectsTexture{},
	m_gui{},
	m_textureManager{},
	m_fontManager{},
	m_scriptManager{},
	m_langManager{},
	m_eventManager{},
	m_entityManager{m_eventManager},
	m_systemManager{m_entityManager, m_eventManager},
	m_world{m_parameters.gravity},
	m_stateStack{StateStack::Context{m_parameters, m_window, m_postEffectsTexture, m_textureManager, m_fontManager, m_scriptManager,
									m_langManager, m_gui, m_eventManager, m_entityManager, m_systemManager, m_world, m_player}},
	m_player(m_stateStack),
	m_debugDraw(m_stateStack.getContext()),
	m_FPSTimer(),
	m_FPSRefreshRate(sf::milliseconds(50)),
	m_frameTime{sf::seconds(1.f/60.f)}
{
	std::string file("settings.json");
	Json::Value settings;//Will contains the root value after parsing.
	Json::Value model;
	Json::Reader reader;
	std::ifstream settingsFile(file, std::ifstream::binary);
	std::ifstream modelFile("settingsModel.json", std::ifstream::binary);
	if(!reader.parse(settingsFile, settings))//report to the user the failure and their locations in the document.
	{
		std::cerr << "\"" + file + "\": " + reader.getFormattedErrorMessages() << "\n"
				  << "Loaded default settings.\n";
		m_window.create({640, 360}, "The Lost Girl");
		m_postEffectsTexture.create(640, 360);
	}
	else if(!reader.parse(modelFile, model))//report to the user the failure and their locations in the document.
	{
		std::cerr << "\"settingsModel.json\": " + reader.getFormattedErrorMessages() << "\n"
				  << "Loaded default settings.\n";
		m_window.create({640, 360}, "The Lost Girl");
		m_postEffectsTexture.create(640, 360);
	}
	else
	{
		//SuperMegaMagic parsing of the settings file from the model file
		parse(settings, model, "root", "root");
		sf::VideoMode mode;
		if(settings["lang"].asString() == "FR")
			m_langManager.setLang(FR);
		else if(settings["lang"].asString() == "EN")
			m_langManager.setLang(EN);

		switch(settings["resolution"].asInt())
		{
			case 360:
				m_parameters.scaleIndex = 0;
				mode = {640, 360};
				break;
			case 576:
				m_parameters.scaleIndex = 1;
				mode = {1024, 576};
				break;
			case 720:
				m_parameters.scaleIndex = 2;
				mode = {1280, 720};
				break;
			case 900:
				m_parameters.scaleIndex = 3;
				mode = {1600, 900};
				break;
			case 1080:
			default:
				m_parameters.scaleIndex = 4;
				mode = {1900, 1080};
				break;
		}
		m_window.create(mode, "The Lost Girl");
		m_postEffectsTexture.create(mode.width, mode.height);
		m_window.setSize({settings["window size"]["w"].asUInt(), settings["window size"]["h"].asUInt()});
		m_parameters.bloomEnabled = settings["enable bloom"].asBool();
	}
	m_parameters.scale = scales[m_parameters.scaleIndex];
	m_parameters.scaledPixelByMeter = m_parameters.scale*m_parameters.pixelByMeter;
	m_parameters.debugMode = debugMode;
	m_gui.setWindow(m_window);

	sf::View view{m_window.getDefaultView()};
	sf::Event::SizeEvent se{m_window.getSize().x, m_window.getSize().y};
	view.setViewport(handleResize(se));
	if(m_parameters.bloomEnabled)
		m_postEffectsTexture.setView(view);
	else
		m_window.setView(view);
	m_gui.setView(view);
}

Application::~Application()
{
	Json::Value settings;//Will contains the root value after serializing.
	Json::StyledWriter writer;
	std::string file("settings.json");
	std::ofstream settingsFileStream(file, std::ofstream::binary);

	if(m_langManager.getLang() == FR)
		settings["lang"] = "FR";
	else if(m_langManager.getLang() == EN)
		settings["lang"] = "EN";

	switch(m_parameters.scaleIndex)
	{
		case 0:
			settings["resolution"] = 360;
			break;
		case 1:
			settings["resolution"] = 576;
			break;
		case 2:
			settings["resolution"] = 720;
			break;
		case 3:
			settings["resolution"] = 900;
			break;
		case 4:
		default:
			settings["resolution"] = 1080;
			break;
	}

	settings["window size"]["w"] = m_window.getSize().x;
	settings["window size"]["h"] = m_window.getSize().y;
	settings["enable bloom"] = m_parameters.bloomEnabled;

	settingsFileStream << writer.write(settings);
	settingsFileStream.close();
}

int Application::init()
{
	try
	{
		srand(static_cast<unsigned int>(time(nullptr)));//Init random numbers
		registerSystems();
		m_window.setKeyRepeatEnabled(false);//Desactive the key repeating
		m_fontManager.load("menu", m_parameters.textFont);//Load the GUI font
		m_fontManager.load("debug", "resources/fonts/FreeMonoBold.ttf");//Load the debug font
		m_gui.setGlobalFont(std::make_shared<sf::Font>(m_fontManager.get("menu")));//Set the GUI font
		m_world.SetDebugDraw(&m_debugDraw);//Set the debug drawer
		m_debugDraw.SetFlags(b2Draw::e_shapeBit|b2Draw::e_jointBit);//Debug drawing flags
		m_systemManager.configure();//Init the manager
		m_stateStack.pushState<IntroState>();//And add the intro state on top of it
	}
	catch(std::runtime_error& e)
	{
		std::cerr << "Runtime error: " << e.what() << "\n";
		return 1;
	}
	return 0;
}

int Application::run()
{
	try
	{
		sf::Clock clock;
		sf::Time timeSinceLastUpdate{sf::Time::Zero};
		while(m_window.isOpen())
		{
			processInput();
			sf::Time dt = clock.restart();
			m_FPSTimer += dt;
			if(m_FPSTimer > m_FPSRefreshRate)
			{
				m_debugDraw.setFPS(1.f/dt.asSeconds());
				m_FPSTimer %= m_FPSRefreshRate;
			}
			timeSinceLastUpdate += dt;
			while(timeSinceLastUpdate > m_frameTime)
			{
				timeSinceLastUpdate -= m_frameTime;
				update(m_frameTime);
				if(m_stateStack.isEmpty())
					m_window.close();
				processInput();
			}
			render();
		}
	}
	catch(std::runtime_error& e)
	{
		std::cerr << "Runtime error: " << e.what() << "\n";
		return 1;
	}
	catch(std::out_of_range& e)
	{
		std::cerr << "Out of range error (the level designer probably wrongly named entitie's parts): " << e.what() << "\n";
		return 2;
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
			sf::View view{m_window.getDefaultView()};
			view.setViewport(handleResize(event.size));
			if(m_parameters.bloomEnabled)
				m_postEffectsTexture.setView(view);
			else
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
	//Clear the texture, draw on it and display
	m_window.clear({197, 182, 108});
	m_stateStack.draw();

	if(m_parameters.debugMode)
		m_world.DrawDebugData();
	m_debugDraw.drawDebugAth();
	m_gui.draw();
	m_window.display();
}

void Application::registerSystems()
{
	m_systemManager.add<PhysicsSystem>(m_stateStack.getContext());
	m_systemManager.add<PendingChangesSystem>(m_stateStack.getContext());
	m_systemManager.add<AnimationsSystem>(m_stateStack.getContext());
	m_systemManager.add<RenderSystem>(m_stateStack.getContext());
	m_systemManager.add<DragAndDropSystem>(m_stateStack.getContext());
	m_systemManager.add<ScrollingSystem>(m_stateStack.getContext());
	m_systemManager.add<TimeSystem>(m_stateStack.getContext());
	m_systemManager.add<StatsSystem>(m_stateStack.getContext());
	m_systemManager.add<ScriptsSystem>(m_stateStack.getContext());
}
