#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <thread>
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/State.hpp>
#include <dist/json/json-forwards.h>
#include <TheLostGirl/Context.hpp>

//Forward declarations
struct ParametersChange;
struct Transform;

/// State that effectively play the game.
/// The game becomes really interesting here.
class GameState : public State
{
	public:
        /// Constructor.
        /// \param file Path name of the save file to load.
		explicit GameState(std::string file);

		/// Destructor.
		/// Removes all bodies, sprites and others from the memory.
		~GameState();

        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw() override;

        /// The logic update function.
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
        /// This function call e.g. the physic update function, the AI function, etc...
		virtual bool update(sf::Time dt) override;

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// \note The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event) override;

		/// Receives an event about a change in the parameters.
		/// \param parametersChange The data about the change.
		void receive(const ParametersChange& parametersChange);

	private:
		/// Context type of this class.
		typedef ContextAccessor<ContextElement::Parameters,
                                ContextElement::EventManager,
                                ContextElement::SystemManager,
                                ContextElement::Player,
                                ContextElement::EntityManager,
                                ContextElement::TextureManager,
                                ContextElement::World> Context;

		/// Structure that holds data about scene entities. The role of this
		/// structure is to hold data that cannot easily be retrieved in the
		/// entityx::EntityManager instance.
		struct SceneReplaces
		{
			sf::IntRect origin;             ///< Coordinate of the texture to place in the world.
			std::vector<Transform> replaces;///< List of places where the texture should be displayed.
		};

		/// Creates a json value from the data in the current game.
		/// This value contains entities, level information and time information.
		/// \return The resulting json value.
		Json::Value getJsonValueFromGame();

		/// Saves the physic world.
		/// \param filePath Path of the save file to override.
		/// \TODO add const qualifier.
		void saveWorld(const std::string& filePath);

		/// Initializes the physic world.
		/// \param filePath Path of the save file to load.
		void initWorld(const std::string& filePath);

		/// Clears all entities.
		void clear();

		/// Loads informations on a save such as levelData, the time, and so on.
		/// \param root The json root value of the save.
		void loadSaveInformations(const Json::Value& root);

		/// Loads generic entities from the list of generic entities.
		/// \param includes A json array containing the entities to load.
		/// \param model The json model used to parse the imported entities.
		/// \return A json value with all generic entities.
		Json::Value loadGenericEntities(const Json::Value& includes, const Json::Value& model);

		/// Effectevely deserializes the entities in jsonEntities, taking the
		/// generic entities in account.
		/// \param jsonEntities Json values to deserialize.
		/// \param genericEntities The generic entities.
		void deserializeEntities(Json::Value& jsonEntities, const Json::Value& genericEntities);

		/// Loads the replaced images (such as trees in first plan) from their
		/// serialized form in a json value.
		/// \param replaces Data about replaced images.
		void loadReplaces(const Json::Value& replaces);

		/// Loads the background images.
		void loadBackground();

		/// Adds the sky animations to sky entities.
		void addSkyAnimations();

		/// Gets a reference to a system. This is only for ease of typing.
		/// \tparam SystemType The type of system to get.
		/// \return A reference to the system.
		template <class SystemType>
		static inline SystemType& getSystem();

		std::map<std::string, entityx::Entity> m_entities;                    ///< All game entities.
		std::map<std::string, entityx::Entity> m_sceneEntities;               ///< All scene entities.
		std::map<std::string, std::vector<SceneReplaces>> m_sceneEntitiesData;///< A map containing data about scene entities.
		float m_timeSpeed;                ///< The speed of the time (usually 1.f). This influes only on the TimeSystem, not on physics!
		std::thread m_threadLoad;         ///< Thread launched when loading the level.
		bool m_loading;                   ///< Indicates wether the loading is finished or not.

		//Level informations
		std::string m_savesDirectoryPath; ///< Path to the save files directory.
		std::string m_levelIdentifier;    ///< Identifer of the level, must be a non-spaced name.
		unsigned int m_numberOfPlans;     ///< Number of plans in the background.
		float m_referencePlan;            ///< Number of the plan where actors evolute.
		sf::FloatRect m_levelRect;        ///< The dimensions of the level, in pixels.
};

#endif//GAMESTATE_HPP
