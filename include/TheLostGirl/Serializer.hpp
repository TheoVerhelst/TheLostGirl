#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <entityx/entityx.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/SpriteSheetAnimation.hpp>
#include <TheLostGirl/Context.hpp>

//Forward declarations
namespace Json
{
	class Value;
}

namespace entityx
{
	template <typename C, typename EM>
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
		/// Context type of this class.
		typedef ContextAccessor<ContextElement::Parameters,
                                ContextElement::TextureManager,
                                ContextElement::World,
                                ContextElement::ScriptManager> Context;

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

#endif//SERIALIZER_HPP
