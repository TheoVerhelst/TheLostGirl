#include <TheLostGirl/Application.h>
#include <TheLostGirl/Context.h>

Parameters* Context::parameters{nullptr};
sf::RenderWindow* Context::window{nullptr};
sf::RenderTexture* Context::postEffectsTexture{nullptr};
ResourceManager<sf::Texture, std::string>* Context::textureManager{nullptr};
ResourceManager<sf::Font, std::string>* Context::fontManager{nullptr};
ResourceManager<sf::SoundBuffer, std::string>* Context::soundManager{nullptr};
ResourceManager<Interpreter, std::string>* Context::scriptManager{nullptr};
LangManager* Context::langManager{nullptr};
tgui::Gui* Context::gui{nullptr};
entityx::EventManager* Context::eventManager{nullptr};
entityx::EntityManager* Context::entityManager{nullptr};
entityx::SystemManager* Context::systemManager{nullptr};
b2World* Context::world{nullptr};
Player* Context::player{nullptr};
StateStack* Context::stateStack{nullptr};

Context::Context(Application& application)
{
	parameters = &application.m_parameters;
	window = &application.m_window;
	postEffectsTexture = &application.m_postEffectsTexture;
	textureManager = &application.m_textureManager;
	fontManager = &application.m_fontManager;
	soundManager = &application.m_soundManager;
	scriptManager = &application.m_scriptManager;
	langManager = &application.m_langManager;
	gui = &application.m_gui;
	eventManager = &application.m_eventManager;
	entityManager = &application.m_entityManager;
	systemManager = &application.m_systemManager;
	world = &application.m_world;
	player = &application.m_player;
	stateStack = &application.m_stateStack;
}
