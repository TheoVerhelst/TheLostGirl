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
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<BodyComponent> component, float scale);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager, float scale);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<DirectionComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<CategoryComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<ActorIDComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<SkyComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<FallComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<WalkComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<JumpComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<BendComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<HealthComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<StaminaComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<BodyComponent> component, b2World& world, float scale);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager, float scale);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component, entityx::ComponentHandle<SpriteComponent> spriteComponent, State::Context context);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<DirectionComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<CategoryComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<ActorIDComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<SkyComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<FallComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<WalkComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<JumpComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<BendComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<HealthComponent> component);

/// Deserialize the given \a value in the given \a component.
/// \param component Component in wich deserialize the data
/// \param The Json value containing the data.
void deserialize(const Json::Value& value, entityx::ComponentHandle<StaminaComponent> component);

#endif // SERIALIZATION_H
