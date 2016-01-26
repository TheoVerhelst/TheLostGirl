#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include <entityx/entityx.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/SpriteSheetAnimation.hpp>

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
				m_jsonEntities[entityName][componentName] = implSerialize(entityName, entity.component<T>());
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
		/// Serializes the dependency between two entities,
		/// if this dependency was not already serialized.
		/// More precisely, adds \a dependencyEntityName to
		/// the *"dependency"* part of the dependent entity json value.
		/// \param dependentEntityName The name of the dependent entity.
		/// \param dependencyEntityName The name of the entity that is a dependecy.
		void addEntityDependecy(const std::string& dependentEntityName, const std::string& dependencyEntityName);

		/// Serializes the given component.
		/// \param entityName The name of the entity currently being serialized.
		/// \param component Component to serialize.
		/// \return A Json value.
#define IMPL_SERIALIZE_DECLARATION(ComponentType)                      \
		Json::Value implSerialize(const std::string& entityName, entityx::ComponentHandle<ComponentType> component)

		IMPL_SERIALIZE_DECLARATION(BodyComponent);
		IMPL_SERIALIZE_DECLARATION(SpriteComponent);
		IMPL_SERIALIZE_DECLARATION(TransformComponent);
		IMPL_SERIALIZE_DECLARATION(InventoryComponent);
		IMPL_SERIALIZE_DECLARATION(AnimationsComponent<SpriteSheetAnimation>);
		IMPL_SERIALIZE_DECLARATION(DirectionComponent);
		IMPL_SERIALIZE_DECLARATION(CategoryComponent);
		IMPL_SERIALIZE_DECLARATION(FallComponent);
		IMPL_SERIALIZE_DECLARATION(WalkComponent);
		IMPL_SERIALIZE_DECLARATION(JumpComponent);
		IMPL_SERIALIZE_DECLARATION(ArcherComponent);
		IMPL_SERIALIZE_DECLARATION(HealthComponent);
		IMPL_SERIALIZE_DECLARATION(StaminaComponent);
		IMPL_SERIALIZE_DECLARATION(ArrowComponent);
		IMPL_SERIALIZE_DECLARATION(HardnessComponent);
		IMPL_SERIALIZE_DECLARATION(ScriptsComponent);
		IMPL_SERIALIZE_DECLARATION(DetectionRangeComponent);
		IMPL_SERIALIZE_DECLARATION(DeathComponent);
		IMPL_SERIALIZE_DECLARATION(NameComponent);
		IMPL_SERIALIZE_DECLARATION(HandToHandComponent);
		IMPL_SERIALIZE_DECLARATION(ActorComponent);
		IMPL_SERIALIZE_DECLARATION(ItemComponent);
		IMPL_SERIALIZE_DECLARATION(HoldItemComponent);
		IMPL_SERIALIZE_DECLARATION(ArticuledArmsComponent);
		IMPL_SERIALIZE_DECLARATION(BowComponent);
		IMPL_SERIALIZE_DECLARATION(QuiverComponent);
		//End serialize

#undef IMPL_SERIALIZE_DECLARATION

		/// Deserialize the given \a value in the given \a component.
		/// \param value The Json value containing the data.
		/// \param component Component in wich deserialize the data.
		/// \param entity The entity that is being deserialized.
#define IMPL_DESERIALIZE_DECLARATION(ComponentType)                    \
		void implDeserialize(const Json::Value& value, entityx::ComponentHandle<ComponentType> component, entityx::Entity entity)

		IMPL_DESERIALIZE_DECLARATION(BodyComponent);
		IMPL_DESERIALIZE_DECLARATION(SpriteComponent);
		IMPL_DESERIALIZE_DECLARATION(TransformComponent);
		IMPL_DESERIALIZE_DECLARATION(InventoryComponent);
		IMPL_DESERIALIZE_DECLARATION(AnimationsComponent<SpriteSheetAnimation>);
		IMPL_DESERIALIZE_DECLARATION(DirectionComponent);
		IMPL_DESERIALIZE_DECLARATION(CategoryComponent);
		IMPL_DESERIALIZE_DECLARATION(FallComponent);
		IMPL_DESERIALIZE_DECLARATION(WalkComponent);
		IMPL_DESERIALIZE_DECLARATION(JumpComponent);
		IMPL_DESERIALIZE_DECLARATION(ArcherComponent);
		IMPL_DESERIALIZE_DECLARATION(HealthComponent);
		IMPL_DESERIALIZE_DECLARATION(StaminaComponent);
		IMPL_DESERIALIZE_DECLARATION(ArrowComponent);
		IMPL_DESERIALIZE_DECLARATION(HardnessComponent);
		IMPL_DESERIALIZE_DECLARATION(ScriptsComponent);
		IMPL_DESERIALIZE_DECLARATION(DetectionRangeComponent);
		IMPL_DESERIALIZE_DECLARATION(DeathComponent);
		IMPL_DESERIALIZE_DECLARATION(NameComponent);
		IMPL_DESERIALIZE_DECLARATION(HandToHandComponent);
		IMPL_DESERIALIZE_DECLARATION(ActorComponent);
		IMPL_DESERIALIZE_DECLARATION(ItemComponent);
		IMPL_DESERIALIZE_DECLARATION(HoldItemComponent);
		IMPL_DESERIALIZE_DECLARATION(ArticuledArmsComponent);
		IMPL_DESERIALIZE_DECLARATION(BowComponent);
		IMPL_DESERIALIZE_DECLARATION(QuiverComponent);
		//End deserialize

#undef IMPL_DESERIALIZE_DECLARATION

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
Json::ValueType strToType(const std::string& str);

/// Check if every element in \a objects corresponds to one element in \a valuesTypes, throw an exception otherwise.
/// If all elements in the object don't have the right type, an exception is raised.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param valuesTypes The mapping between elements names and their types.
void parseObject(const Json::Value& object, const std::string& name, std::map<std::string, Json::ValueType> valuesTypes);

/// Check if every element listed in \a valuesTypes exists in \a object and has the right type.
/// If all elements are not found in the object, an exception is raised.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param valuesTypes The mapping between elements names and their types.
void requireValues(const Json::Value& object, const std::string& name, std::map<std::string, Json::ValueType> valuesTypes);

/// Check if every element in \a value have the type \a type.
/// If all elements in the object don't have the right type, an exception is raised.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param type The expected type of every element in \a object.
void parseObject(const Json::Value& object, const std::string& name, Json::ValueType type);

/// Check if \a value corresponds to one element in \a values, throw an exception otherwise.
/// \param value A Json value.
/// \param name The name of the value.
/// \param values The list of every possible value of \a value.
void parseValue(const Json::Value& value, const std::string& name, std::vector<Json::Value> values);

/// Check if \a value have the type \a type.
/// If value don't have the right type, an exception is raised.
/// \param value A Json value.
/// \param name The name of the value.
/// \param type The expected type of \a value.
void parseValue(const Json::Value& value, const std::string& name, Json::ValueType type);

/// Check if every element in \a array corresponds to one element in \a values, throw an exception otherwise.
/// \param array A Json value containing the data, it must be an array.
/// \param name The name of the array.
/// \param values The elements expecteds in \a array.
void parseArray(const Json::Value& array, const std::string& name, std::vector<Json::Value> values);

/// Check if every element in \a value have the type \a type.
/// If all elements in the array don't have the right type, an exception is raised.
/// \param array A Json value containing the data, it must be an array.
/// \param name The name of the array.
/// \param type The expected type of every element in \a array.
void parseArray(const Json::Value& array, const std::string& name, Json::ValueType type);

/// Check if the \a value is conformant to \a model.
/// If not, raise an exception.
/// The value is not const because some of its childs can have default value,
/// so this function set missing values to default (effectively modifying the value).
/// \param value Value to check.
/// \param model Description model.
/// \param valueName The name of the value.
/// \param modelName The name of the model value.
void parse(Json::Value& value, const Json::Value& model, const std::string& valueName, const std::string& modelName);

#endif//SERIALIZATION_HPP
