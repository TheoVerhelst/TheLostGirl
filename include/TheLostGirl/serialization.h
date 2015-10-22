#ifndef SERIALIZATION_H
#define SERIALIZATION_H

//Forward declarations
namespace Json
{
	class Value;
}

namespace entityx
{
	template <typename C>
	class ComponentHandle;
}

/// Serialize and deserialize various components
class Serializer
{
	public:
		/// Constructor.
		/// \param context The current context of the application.
		/// \param entitiesMap The map containing all entities (to find name - entity correspondance).
		Serializer(StateStack::Context context, const std::map<std::string, entityx::Entity>& entitiesMap);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<BodyComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<SpriteComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<TransformComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<InventoryComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<DirectionComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<CategoryComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<FallComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<WalkComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<JumpComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<ArcherComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<HealthComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<StaminaComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<ArrowComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<HardnessComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<ScriptsComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<DetectionRangeComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<DeathComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<NameComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<HandToHandComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<ActorComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<HoldItemComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<ArticuledArmsComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<BowComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value serialize(entityx::ComponentHandle<QuiverComponent> component);

		//End serialize

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param transformComponent Transform component of the entity.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<BodyComponent> component, entityx::ComponentHandle<TransformComponent> transformComponent);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<SpriteComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<TransformComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<InventoryComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param spriteComponent The sprite component of the entity.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component, entityx::ComponentHandle<SpriteComponent> spriteComponent);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<DirectionComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<CategoryComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<FallComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<WalkComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<JumpComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param bodyComponent The body component of the entity.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<ArcherComponent> component, entityx::ComponentHandle<BodyComponent> bodyComponent);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<HealthComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<StaminaComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<ArrowComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<HardnessComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<ScriptsComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<DetectionRangeComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<DeathComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<NameComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<HandToHandComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<ActorComponent> component);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param bodyComponent The body component of the entity.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<HoldItemComponent> component, entityx::ComponentHandle<BodyComponent> bodyComponent);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param bodyComponent The body component of the entity.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<ArticuledArmsComponent> component, entityx::ComponentHandle<BodyComponent> bodyComponent);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param bodyComponent The body component of the entity.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<BowComponent> component, entityx::ComponentHandle<BodyComponent> bodyComponent);

		/// Deserialize the given \a value in the given \a component.
		/// \param component Component in wich deserialize the data.
		/// \param bodyComponent The body component of the entity.
		/// \param value The Json value containing the data.
		void deserialize(const Json::Value& value, entityx::ComponentHandle<QuiverComponent> component, entityx::ComponentHandle<BodyComponent> bodyComponent);

		//End deserialize

	private:
		StateStack::Context m_context;                              ///< The current context of the application.
		const std::map<std::string, entityx::Entity>& m_entitiesMap;///< All the entities of the application.
};

/// Return a string depending of the given \a type.
/// \param type Type to encode in a string.
/// \return A formated string.
std::string typeToStr(Json::ValueType type);

/// Return a Json::ValueType from the the given \a string.
/// \param str String to decode.
/// \return A Json type.
Json::ValueType strToType(std::string str);

/// Check if every element in \a objects corresponds to one element in \a valuesTypes, throw an exception otherwise.
/// If all elements in the object don't have the right type, an exception is raised.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param valuesTypes The mapping between elements names and their types.
void parseObject(const Json::Value& object, const std::string name, std::map<std::string, Json::ValueType> valuesTypes);

/// Check if every element listed in \a valuesTypes exists in \a object and has the right type.
/// If all elements are not found in the object, an exception is raised.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param valuesTypes The mapping between elements names and their types.
void requireValues(const Json::Value& object, const std::string name, std::map<std::string, Json::ValueType> valuesTypes);

/// Check if every element in \a value have the type \a type.
/// If all elements in the object don't have the right type, an exception is raised.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param type The expected type of every element in \a object.
void parseObject(const Json::Value& object, const std::string name, Json::ValueType type);

/// Check if \a value corresponds to one element in \a values, throw an exception otherwise.
/// \param value A Json value.
/// \param name The name of the value.
/// \param values The list of every possible value of \a value.
void parseValue(const Json::Value& value, const std::string name, std::vector<Json::Value> values);

/// Check if \a value have the type \a type.
/// If value don't have the right type, an exception is raised.
/// \param value A Json value.
/// \param name The name of the value.
/// \param type The expected type of \a value.
void parseValue(const Json::Value& value, const std::string name, Json::ValueType type);

/// Check if every element in \a array corresponds to one element in \a values, throw an exception otherwise.
/// \param array A Json value containing the data, it must be an array.
/// \param name The name of the array.
/// \param values The elements expecteds in \a array.
void parseArray(const Json::Value& array, const std::string name, std::vector<Json::Value> values);

/// Check if every element in \a value have the type \a type.
/// If all elements in the array don't have the right type, an exception is raised.
/// \param array A Json value containing the data, it must be an array.
/// \param name The name of the array.
/// \param type The expected type of every element in \a array.
void parseArray(const Json::Value& array, const std::string name, Json::ValueType type);

/// Check if the \a value is conformant to \a model.
/// If not, raise an exception.
/// \param value Value to check.
/// \param model Description model.
/// \param valueName The name of the value.
/// \param modelName The name of the model value.
void parse(Json::Value& value, const Json::Value& model, const std::string& valueName, const std::string& modelName);

#endif//SERIALIZATION_H
