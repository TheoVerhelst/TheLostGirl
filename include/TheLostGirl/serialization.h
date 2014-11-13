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
Json::Value serialize(entityx::ComponentHandle<BodyComponent> component);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager);

/// Serialize the given component.
/// \param component Component to serialize
/// \return A Json value.
Json::Value serialize(entityx::ComponentHandle<AnimationsComponent<sf::Sprite>> component);

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

#endif // SERIALIZATION_H
