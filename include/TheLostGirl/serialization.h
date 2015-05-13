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

/// \file serialization.h

/// Serialize the given component.
/// \param component Component to serialize.
/// \param pixelByMeter Number of pixel by meter.
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<BodyComponent> component, float pixelByMeter);

/// Serialize the given component.
/// \param component Component to serialize.
/// \param textureManager The texture manager.
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager);

/// Serialize the given component.
/// \param component Component to serialize.
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<TransformComponent> component);

/// Serialize the given component.
/// \param component Component to serialize.
/// \param entitiesMap The map containing all entities (to find name - entity correspondance).
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<InventoryComponent> component, const std::map<std::string, entityx::Entity>& entitiesMap);

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
/// \param entitiesMap The map containing all entities (to find name - entity correspondance).
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<BowComponent> component, const std::map<std::string, entityx::Entity>& entitiesMap);

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

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data.
/// \param pixelByMeter Number of pixel by meter.
/// \param world Current intance of physic world.
/// \param transformComponent Transform component of the entity.
/// \param value The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<BodyComponent> component, entityx::ComponentHandle<TransformComponent> transformComponent, b2World& world, float pixelByMeter);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data.
/// \param textureManager The texture manager.
/// \param path File path to the current image directory.
/// \param value The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager, const std::string& path);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data.
/// \param value The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<TransformComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data.
/// \param entitiesMap The map containing all entities (to find name - entity correspondance).
/// \param value The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<InventoryComponent> component, const std::map<std::string, entityx::Entity>& entitiesMap);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data.
/// \param spriteComponent Sprite component of the entity.
/// \param context Current context of the application.
/// \param value The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component, entityx::ComponentHandle<SpriteComponent> spriteComponent, StateStack::Context context);

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
/// \param entitiesMap The map containing all entities (to find name - entity correspondance).
/// \param value The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<BowComponent> component, const std::map<std::string, entityx::Entity>& entitiesMap);

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
/// \param scriptManager The scripts manager.
void deserialize(const Json::Value& value, entityx::ComponentHandle<ScriptsComponent> component, ScriptManager& scriptManager);

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

#endif//SERIALIZATION_H
