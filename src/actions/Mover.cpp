#include <entityx/Entity.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/actions/Mover.h>

Mover::Mover(Direction _direction, bool _start):
	direction{_direction},
	start{_start}
{
}

Mover::~Mover()
{
}

void Mover::operator()(entityx::Entity entity, double) const
{
	if(not entity)
		return;
	std::string directionStr;
	std::string oppDirectionStr;
	Direction oppDirection;
	bool moveIsHorizontal;
	switch(direction)
	{
		case Direction::Left:
			directionStr = " left";
			oppDirectionStr = " right";
			oppDirection = Direction::Right;
			moveIsHorizontal = true;
			break;

		case Direction::Right:
			directionStr = " right";
			oppDirectionStr = " left";
			oppDirection = Direction::Left;
			moveIsHorizontal = true;
			break;

		case Direction::Top:
			directionStr = " top";
			oppDirectionStr = " bottom";
			oppDirection = Direction::Bottom;
			moveIsHorizontal = false;
			break;

		case Direction::Bottom:
			directionStr = " bottom";
			oppDirectionStr = " top";
			oppDirection = Direction::Top;
			moveIsHorizontal = false;
			break;

		case Direction::None:
		default:
			return;
	}
	if(moveIsHorizontal)
	{
		DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
		//For all entities
		if(directionComponent)
		{
			bool moveToOppDirection;
			if(direction == Direction::Left)
			{
				moveToOppDirection = directionComponent->moveToRight;
				directionComponent->moveToLeft = start;
			}
			else
			{
				moveToOppDirection = directionComponent->moveToLeft;
				directionComponent->moveToRight = start;
			}
			const Direction initialDirection{directionComponent->direction};
			if(start)
				//Here in all cases, the new direction will be the same.
				directionComponent->direction = direction;
			else if(moveToOppDirection)
				directionComponent->direction = oppDirection;
			if(directionComponent->direction != initialDirection)
				flip(entity);

			AnimationsComponent<SpriteSheetAnimation>::Handle animationsComponent(entity.component<AnimationsComponent<SpriteSheetAnimation>>());
			//If the entity have animations
			if(animationsComponent)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationsComponent->animationsManager);
				//If the animations manager have bending animations
				if(animations.isRegistred("bend" + directionStr)
					and animations.isRegistred("bend" + oppDirectionStr))
				{
					if(start)
					{
						float progress{animations.getProgress("bend" + oppDirectionStr)};
						animations.stop("bend" + oppDirectionStr);
						animations.activate("bend" + directionStr);
						animations.setProgress("bend" + directionStr, progress);
					}
					else if(moveToOppDirection)
					{
						float progress{animations.getProgress("bend" + directionStr)};
						animations.stop("bend" + directionStr);
						animations.activate("bend" + oppDirectionStr);
						animations.setProgress("bend" + oppDirectionStr, progress);
					}
				}
				//If the animations manager falling animations
				if(animations.isRegistred("fall" + directionStr)
					and animations.isRegistred("fall" + oppDirectionStr))
				{
					//If falling and diriged to the opposite side
					if(animations.isActive("fall" + oppDirectionStr))
					{
						float progress{animations.getProgress("fall" + oppDirectionStr)};
						animations.stop("fall" + oppDirectionStr);
						animations.play("fall" + directionStr);
						animations.setProgress("fall" + directionStr, progress);
					}
				}
				//If the animations manager have jump animations
				if(animations.isRegistred("jump" + directionStr)
					and animations.isRegistred("jump" + oppDirectionStr))
				{
					//If jumping and diriged to the opposite side
					if(animations.isActive("jump" + oppDirectionStr))
					{
						float progress{animations.getProgress("jump" + oppDirectionStr)};
						animations.stop("jump" + oppDirectionStr);
						animations.play("jump" + directionStr);
						animations.setProgress("jump" + directionStr, progress);
					}
				}
				//If the animations manager have walk animations
				if(animations.isRegistred("stay" + directionStr)
					and animations.isRegistred("stay" + oppDirectionStr)
					and animations.isRegistred("move" + directionStr)
					and animations.isRegistred("move" + oppDirectionStr))
				{
					WalkComponent::Handle walkComponent{entity.component<WalkComponent>()};
					if(start)
					{
						//Force to set the right animations
						animations.stop("stay" + oppDirectionStr);
						animations.play("stay" + directionStr);
						animations.stop("move" + oppDirectionStr);
						animations.play("move" + directionStr);
						if(moveToOppDirection)
						{
							//Stop the entity
							walkComponent->effectiveMovement = Direction::None;//Stop the entity
							animations.stop("move" + directionStr);
						}
						else
							walkComponent->effectiveMovement = direction;
					}
					else
					{
						animations.stop("move" + directionStr);
						if(moveToOppDirection)
						{
							//Force to play the opposite direction animations
							animations.play("move" + oppDirectionStr);
							animations.stop("stay" + directionStr);
							animations.play("stay" + oppDirectionStr);
							walkComponent->effectiveMovement = oppDirection;
						}
						else
							walkComponent->effectiveMovement = Direction::None;//Stop the player
					}
				}
			}
		}
	}
}

void Mover::flip(entityx::Entity entity) const
{
	ArcherComponent::Handle archerComponent{entity.component<ArcherComponent>()};
	BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
	ArticuledArmsComponent::Handle armsComponent{entity.component<ArticuledArmsComponent>()};
	if(bodyComponent)
	{
		//Flip the body and get the symmetry point
		const float32 mid{flipFixtures(bodyComponent->body)};
		if(archerComponent)
		{
			std::cout << "Quiver joint :\t\t";
            archerComponent->quiverJoint = flipJoint<b2WeldJointDef>(archerComponent->quiverJoint, mid, archerComponent->quiver);
		}
		if(armsComponent)
		{
			std::cout << "Arms joint :\t\t";
            armsComponent->armsJoint = flipJoint<b2RevoluteJointDef>(armsComponent->armsJoint, mid, armsComponent->arms);
			HoldItemComponent::Handle holdItemComponent{armsComponent->arms.component<HoldItemComponent>()};
			if(holdItemComponent)
			{
				std::cout << "Hold item joint :\t";
				holdItemComponent->joint = flipJoint<b2WeldJointDef>(holdItemComponent->joint, mid, holdItemComponent->item);
				BowComponent::Handle bowComponent{holdItemComponent->item.component<BowComponent>()};
				if(bowComponent)
				{
					std::cout << "Notched arrow joint :\t";
					bowComponent->notchedArrowJoint = flipJoint<b2PrismaticJointDef>(bowComponent->notchedArrowJoint, mid, bowComponent->notchedArrow);
				}
			}
		}
	}
}

float32 Mover::flipFixtures(b2Body* body) const
{
	//Get the half width of the body
	b2AABB box;
	b2Transform identity;
	identity.SetIdentity();
	for(b2Fixture* fixture{body->GetFixtureList()}; fixture; fixture = fixture->GetNext())
		if(fixtureHasRole(fixture, FixtureRole::Main))
			fixture->GetShape()->ComputeAABB(&box, identity, 0);
	const float32 mid{box.GetCenter().x};
	for(b2Fixture* fixture{body->GetFixtureList()}; fixture; fixture = fixture->GetNext())
	{
		b2Shape* shape{fixture->GetShape()};
		switch(shape->GetType())
		{
			case b2Shape::e_circle:
				flipPoint(static_cast<b2CircleShape*>(shape)->m_p, mid);
				break;
			case b2Shape::e_edge:
			{
				b2EdgeShape* edgeShape{static_cast<b2EdgeShape*>(shape)};
				flipPoint(edgeShape->m_vertex0, mid);
				flipPoint(edgeShape->m_vertex1, mid);
				flipPoint(edgeShape->m_vertex2, mid);
				flipPoint(edgeShape->m_vertex3, mid);
				break;
			}
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape{static_cast<b2PolygonShape*>(shape)};
				b2Vec2 vertices[polygonShape->m_count];
				for(int32 i{0}; i < polygonShape->m_count; ++i)
				{
					vertices[i] = polygonShape->m_vertices[i];
					flipPoint(vertices[i], mid);
				}
				polygonShape->Set(vertices, polygonShape->m_count);
				break;
			}
			case b2Shape::e_chain:
			{
				b2ChainShape* chainShape{static_cast<b2ChainShape*>(shape)};
				flipPoint(chainShape->m_prevVertex, mid);
				b2Vec2 vertices[chainShape->m_count];
				for(int32 i{0}; i < chainShape->m_count; ++i)
				{
					vertices[i] = chainShape->m_vertices[i];
					flipPoint(vertices[i], mid);
				}
				chainShape->CreateChain(vertices, chainShape->m_count);
				flipPoint(chainShape->m_nextVertex, mid);
				break;
			}
			default:
				break;
		}
	}
	return mid;
}

inline void Mover::flipPoint(b2Vec2& vec, float32 mid) const
{
	vec.x += 2 * (mid - vec.x);
}
