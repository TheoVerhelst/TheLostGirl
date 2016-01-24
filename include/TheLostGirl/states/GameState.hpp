#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <thread>
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/State.hpp>
#include <dist/json/json-forwards.h>

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
		/// Structure that hold data about scene entities.
		/// The role of this texture is to hold data that cannot easily be retrieved in the EntityManager instance.
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

		/// Creates a json value from a save file.
		/// \param filePath Path of the save file to convert
		/// \result The resulting json value representing the save file.
		static Json::Value getJsonValue(const std::string& filePath);

		/// Merges two Json value in one.
		/// Every member of the right value will be copied into the left value.
		/// \param left A json value.
		/// \param right Another json value
		static void mergeJsonValues(Json::Value& left, const Json::Value& right);

		/// Checks if the data inside the left value is too in the right value.
		/// If this method returns true, then merging left and right will result in right.
		/// \param left A json value.
		/// \param right Another json value.
		/// \return true if left is a subset of right, false otherwise.
		static bool isSubsetJsonValue(const Json::Value& left, const Json::Value& right);

		/// Recursively checks if two values are strictly equal.
		/// \param left A json value.
		/// \param right Another json value.
		/// \return true if left is strictly equal to the right, false otherwise.
		static bool isEqualJsonValue(const Json::Value& left, const Json::Value& right);

		/// Strip the data in left that is also present in right.
		/// This can be viewed as a set substraction.
		/// \pre isSubsetJsonValue(right, left).
		/// \post isEqualJsonValue(mergeJsonValue(*left after call*, right), *left before call*).
		/// \param left A json value.
		/// \param right Another json value.
		static void substractJsonValues(Json::Value& left, const Json::Value& right);

		/// Initializes the physic world.
		/// \param filePath Path of the save file to load.
		void initWorld(const std::string& filePath);

		/// Clears all entities.
		void clear();

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
