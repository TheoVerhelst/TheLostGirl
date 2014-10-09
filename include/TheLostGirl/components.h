#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <SFML/Graphics/Sprite.hpp>

//Forward declarations
namespace entityx
{
	template <typename Derived>
	struct Component;
}
class b2Body;

///Body component.
///The Body component store a pointer to a b2Body and is used to simulate physics in the game world.
struct Body : public entityx::Component<Body>
{
	b2Body* body;
	Body(b2Body* _body):
		body(_body)
	{}
};

///Sprite component.
///The Sprite component is an essential component for any visible entity.
///For more information about sprites, see the SFML doc.
struct SpriteComponent : public entityx::Component<SpriteComponent>
{
	sf::Sprite* sprite;
	SpriteComponent(sf::Sprite* _sprite):
		sprite(_sprite)
	{}
};

///Walk component.
///The walk component must be added to every entity that want to move itself in the world.
///The walkSpeed member is the maximum speed that the entity can reach when walking.
struct Walk : public entityx::Component<Walk>
{
	float walkSpeed;
	Walk(float _walkSpeed):
		walkSpeed(_walkSpeed)
	{}
};

///Jump component.
///The Jump component must be added to every entity that want to jump.
///The jumpStrength member influences directly the height of the jump of the entity.
struct Jump : public entityx::Component<Jump>
{
	float jumpStrength;
	Jump(float _jumpStrength):
		jumpStrength(_jumpStrength)
	{}
};

///The Controller component.
///It indicates if the entity is controlled by the player or not.
struct Controller : public entityx::Component<Controller>
{
	bool isPlayer;
	Controller(bool _isPlayer):
		isPlayer(_isPlayer)
	{}
};

#endif //COMPONENTS_H