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
