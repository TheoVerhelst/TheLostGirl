#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <TheLostGirl/states/IntroState.hpp>
#include <TheLostGirl/systems.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/LangManager.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/JsonHelper.hpp>
#include <TheLostGirl/Application.hpp>

Application::Application(bool debugMode):
	m_context{*this},
	m_window{{static_cast<unsigned int>(m_parameters.defaultViewSize.x),
			static_cast<unsigned int>(m_parameters.defaultViewSize.y)},
			"The Lost Girl"},
	m_gui{m_window},
	m_entityManager{m_eventManager},
	m_systemManager{m_entityManager, m_eventManager},
	m_world{m_parameters.gravity},
	m_FPSRefreshRate{sf::seconds(1.f/20.f)},
	m_frameTime{sf::seconds(1.f/60.f)}
{
	m_parameters.debugMode = debugMode;
	deserializeSettings();
	m_eventManager.subscribe<ParametersChange>(*this);
	handleResize();
	registerSystems();
	m_window.setKeyRepeatEnabled(false);//Desactive the key repeating
	m_fontManager.load("menu", m_parameters.resourcesPath + "fonts/euphorigenic.ttf");
	m_fontManager.load("debug", m_parameters.resourcesPath + "fonts/FreeMonoBold.ttf");
	m_gui.setFont(std::make_shared<sf::Font>(m_fontManager.get("menu")));
	m_world.SetDebugDraw(&m_debugDraw);
	m_world.SetDestructionListener(&m_destructionListener);
	m_world.SetContactListener(&m_contactListener);
	m_debugDraw.setFont(std::make_shared<sf::Font>(m_fontManager.get("debug")));
	m_debugDraw.SetFlags(b2Draw::e_shapeBit|b2Draw::e_jointBit);//Debug drawing flags
	m_systemManager.configure();
	m_stateStack.pushState<IntroState>();//Add the intro state
}

Application::~Application()
{
	Json::Value settings(Json::objectValue);
	settings["lang"] = m_langManager.getLang();
	settings["window size"]["w"] = m_window.getSize().x;
	settings["window size"]["h"] = m_window.getSize().y;
	settings["enable bloom"] = m_parameters.bloomEnabled;
	settings["fullscreen"] = m_parameters.fullscreen;
	JsonHelper::saveToFile(settings, m_parameters.resourcesPath + "settings.json");
}

void Application::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate{sf::Time::Zero};
	while(m_window.isOpen())
	{
		const sf::Time dt = clock.restart();
		if(m_parameters.debugMode)
		{
			m_FPSTimer += dt;
			if(m_FPSTimer > m_FPSRefreshRate)
			{
				m_debugDraw.setFPS(1.f/dt.asSeconds());
				m_FPSTimer %= m_FPSRefreshRate;
			}
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

void Application::receive(const ParametersChange& changes)
{
	if(changes.langChanged)
		m_langManager.setLang(changes.newLang);
	if(changes.bloomEnabledChanged)
	{
		if(changes.newBloomEnabledState)
		{
			m_postEffectsTexture.setView(m_window.getView());
			m_window.setView(m_window.getDefaultView());
		}
		else
		{
			m_window.setView(m_postEffectsTexture.getView());
			m_postEffectsTexture.setView(m_postEffectsTexture.getDefaultView());
		}
		m_parameters.bloomEnabled = changes.newBloomEnabledState;
	}
	if(changes.fullscreenChanged)
	{
		sf::VideoMode videoMode;
		uint32 style{sf::Style::Default};
		if(changes.newFullScreenState)
		{
			style = sf::Style::Fullscreen;
			videoMode = sf::VideoMode::getFullscreenModes()[changes.videoModeIndex];
		}
		else
			videoMode = {960, 540};
		m_window.create(videoMode, "The Lost Girl", style);
		m_parameters.fullscreen = changes.newFullScreenState;
		handleResize();
	}
}

void Application::processInput()
{
	sf::Event event;
	while(m_window.pollEvent(event))
	{
		if(event.type == sf::Event::Closed)
			m_window.close();
		else if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::F2)
			m_parameters.debugMode = not m_parameters.debugMode;//Toggle the debug mode
		else if(event.type == sf::Event::Resized)
			handleResize();
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
	m_window.clear({155, 155, 155});
	if(m_parameters.bloomEnabled)
		m_postEffectsTexture.clear({155, 155, 155});
	m_stateStack.draw();
	if(m_parameters.debugMode)
		m_world.DrawDebugData();
	m_window.resetGLStates();
	if(m_parameters.bloomEnabled)
	{
		m_postEffectsTexture.display();
		//Draw the texture on the window trough the bloom effect
		m_bloomEffect.apply(m_postEffectsTexture, m_window);
	}
	m_gui.draw();
	m_debugDraw.drawDebugAth();
	m_window.display();
}

void Application::registerSystems()
{
	m_systemManager.add<PhysicsSystem>();
	m_systemManager.add<PendingChangesSystem>();
	m_systemManager.add<AnimationsSystem>();
	m_systemManager.add<RenderSystem>();
	m_systemManager.add<DragAndDropSystem>();
	m_systemManager.add<ScrollingSystem>();
	m_systemManager.add<TimeSystem>();
	m_systemManager.add<StatsSystem>();
	m_systemManager.add<ScriptsSystem>();
}

void Application::deserializeSettings()
{
	try
	{
		Json::Value settings{JsonHelper::loadFromFile(m_parameters.resourcesPath + "settings.json")};
		const Json::Value model{JsonHelper::loadFromFile(m_parameters.resourcesPath + "settingsModel.json")};
		//Parsing of the settings file from the model file
		JsonHelper::parse(settings, model);

		m_langManager.setLang(settings["lang"].asString());
		m_parameters.fullscreen = settings["fullscreen"].asBool();
		m_parameters.bloomEnabled = settings["enable bloom"].asBool();

		const sf::VideoMode mode{settings["window size"]["w"].asUInt(), settings["window size"]["h"].asUInt()};
		uint32 style{m_parameters.fullscreen ? sf::Style::Fullscreen : sf::Style::Default};
		m_window.create(mode, "The Lost Girl", style);
	}
	catch(const std::runtime_error& e)
	{
		//Load default settings
		m_langManager.setLang("EN");
		m_parameters.fullscreen = false;
		m_parameters.bloomEnabled = false;
	}
	m_postEffectsTexture.create(m_window.getSize().x, m_window.getSize().y);
}

void Application::handleResize()
{
	const float iw{float(m_window.getSize().x)};
	const float ih{float(m_window.getSize().y)};
	float fh(ih), fw(iw);//If size is in a 16:9 ratio, it won't change.
	if(iw / 16.f < ih / 9.f) //Taller than a 16:9 ratio
		fh = iw * (9.0f / 16.0f);
	else if(iw / 16.f > ih / 9.f) //Larger than a 16:9 ratio
		fw = ih * (16.0f / 9.0f);
	const float scalex{fw / iw}, scaley{fh / ih};
	sf::View view{m_window.getView()};
	view.setViewport({(1 - scalex) / 2.0f, (1 - scaley) / 2.0f, scalex, scaley});
	view.setSize(m_parameters.defaultViewSize);
	if(m_parameters.bloomEnabled)
		m_postEffectsTexture.setView(view);
	else
		m_window.setView(view);
	view.setCenter(m_parameters.defaultViewSize/2.f);
	m_gui.setView(view);
}
