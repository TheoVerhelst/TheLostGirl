#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <entityx/entityx.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/SpriteSheetAnimation.h>

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

/// Serialize and deserialize various components.
/// The Serializer have two attributes, a map of entitix::Entity
/// and a Json::Value. When serializing, the components of the entities
/// in the entities map are copied to the Json::Value, and when deserializing
/// the content of the Json::Value are created in the entities map.
class Serializer
{
	public:
		/// Constructor.
		/// \param entities The map containing all entities (to find name - entity correspondance).
		/// \param jsonEntities The json value of the entities.
		Serializer(std::map<std::string, entityx::Entity>& entities, Json::Value& jsonEntities);

		/// Serialize a component of type T.
		/// \param entityName The name of the entity to serialize.
		/// \param componentName The name of the component to serialize.
		template <typename T>
		inline void serialize(const std::string& entityName, const std::string& componentName)
		{
			entityx::Entity entity{m_entities[entityName]};
			if(entity.has_component<T>())
				m_jsonEntities[entityName][componentName] = implSerialize(entity.component<T>());
		}
		/// Deserialize a component of type T.
		/// \param entityName The name of the entity to deserialize.
		/// \param componentName The name of the component to deserialize.
		template <typename T>
		inline void deserialize(const std::string& entityName, const std::string& componentName)
		{
			entityx::Entity entity{m_entities[entityName]};
			if(m_jsonEntities[entityName].isMember(componentName))
				implDeserialize(m_jsonEntities[entityName][componentName], entity.assign<T>(), entity);
		}

	private:
		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<BodyComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<SpriteComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<TransformComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<InventoryComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<DirectionComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<CategoryComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<FallComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<WalkComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<JumpComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<ArcherComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<HealthComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<StaminaComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<ArrowComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<HardnessComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<ScriptsComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<DetectionRangeComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<DeathComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<NameComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<HandToHandComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<ActorComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<ItemComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<HoldItemComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<ArticuledArmsComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<BowComponent> component);

		/// Serialize the given component.
		/// \param component Component to serialize.
		/// \return A Json value.
		Json::Value implSerialize(entityx::ComponentHandle<QuiverComponent> component);

		//End serialize

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<BodyComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<SpriteComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<TransformComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<InventoryComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<DirectionComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<CategoryComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<FallComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<WalkComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<JumpComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<ArcherComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<HealthComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<StaminaComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<ArrowComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<HardnessComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<ScriptsComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<DetectionRangeComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<DeathComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<NameComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<HandToHandComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<ActorComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<ItemComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<HoldItemComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<ArticuledArmsComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<BowComponent> component, entityx::Entity entity);

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<QuiverComponent> component, entityx::Entity entity);

		//End deserialize

		std::map<std::string, entityx::Entity>& m_entities;///< All the entities of the application.
		Json::Value& m_jsonEntities;                       ///< The json values of the entities.
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
