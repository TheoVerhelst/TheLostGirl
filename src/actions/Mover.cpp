#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/SpriteSheetAnimation.hpp>
#include <TheLostGirl/FixtureRoles.hpp>
#include <TheLostGirl/actions/Mover.hpp>

Mover::Mover(Direction _direction, bool _start):
	direction{_direction},
	start{_start}
{
}

void Mover::operator()(entityx::Entity entity) const
{
	if(not TEST(entity and direction != Direction::None))
		return;
//	const auto fallComponent(entity.component<FallComponent>());
//	if(fallComponent and fallComponent->inAir)
//		return;
	const std::map<Direction, std::string> directionToString = {
			{Direction::Left, " left"},
			{Direction::Right, " right"},
			{Direction::Up, " up"},
			{Direction::Down, " down"}};
	std::string directionStr{directionToString.at(direction)};
	std::string oppDirectionStr{directionToString.at(not direction)};
	DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	if(not TEST(directionComponent))
		return;
	if(direction == Direction::Left or direction == Direction::Right)
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
			directionComponent->direction = not direction;
		if(directionComponent->direction != initialDirection)
			flip(entity);

		AnimationsComponent<SpriteSheetAnimation>::Handle animationsComponent(entity.component<AnimationsComponent<SpriteSheetAnimation>>());
		//If the entity have animations
		if(animationsComponent)
		{
			AnimationsManager<SpriteSheetAnimation>& animations(animationsComponent->animationsManager);
			const std::list<std::string> animationsList = {"bend", "fall", "jump"};
			for(const std::string& animation : animationsList)
			{
				//If the manager has this animation
				if(animations.isRegistred(animation + directionStr)
						and animations.isRegistred(animation + oppDirectionStr)
						and animations.isActive(animation + oppDirectionStr))
				{
					if(start)
					{
						float progress{animations.getProgress(animation + oppDirectionStr)};
						if(animations.isPaused(animation + oppDirectionStr))
							animations.activate(animation + directionStr);
						else
							animations.play(animation + directionStr);
						animations.setProgress(animation + directionStr, progress);
						animations.stop(animation + oppDirectionStr);
					}
					else if(moveToOppDirection)
					{
						float progress{animations.getProgress(animation + directionStr)};
						if(animations.isPaused(animation + directionStr))
							animations.activate(animation + oppDirectionStr);
						else
							animations.play(animation + oppDirectionStr);
						animations.setProgress(animation + oppDirectionStr, progress);
						animations.stop(animation + directionStr);
					}
				}
			}
//
//				//If the animations manager have bending animations
//				if(animations.isRegistred("bend" + directionStr)
//					and animations.isRegistred("bend" + oppDirectionStr))
//				{
//					if(start)
//					{
//						float progress{animations.getProgress("bend" + oppDirectionStr)};
//						animations.stop("bend" + oppDirectionStr);
//						animations.activate("bend" + directionStr);
//						animations.setProgress("bend" + directionStr, progress);
//					}
//					else if(moveToOppDirection)
//					{
//						float progress{animations.getProgress("bend" + directionStr)};
//						animations.stop("bend" + directionStr);
//						animations.activate("bend" + oppDirectionStr);
//						animations.setProgress("bend" + oppDirectionStr, progress);
//					}
//				}
//				//If the animations manager falling animations
//				if(animations.isRegistred("fall" + directionStr)
//					and animations.isRegistred("fall" + oppDirectionStr))
//				{
//					//If falling and diriged to the opposite side
//					if(animations.isActive("fall" + oppDirectionStr))
//					{
//						float progress{animations.getProgress("fall" + oppDirectionStr)};
//						animations.stop("fall" + oppDirectionStr);
//						animations.play("fall" + directionStr);
//						animations.setProgress("fall" + directionStr, progress);
//					}
//				}
//				//If the animations manager have jump animations
//				if(animations.isRegistred("jump" + directionStr)
//					and animations.isRegistred("jump" + oppDirectionStr))
//				{
//					//If jumping and diriged to the opposite side
//					if(animations.isActive("jump" + oppDirectionStr))
//					{
//						float progress{animations.getProgress("jump" + oppDirectionStr)};
//						animations.stop("jump" + oppDirectionStr);
//						animations.play("jump" + directionStr);
//						animations.setProgress("jump" + directionStr, progress);
//					}
//				}
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
						walkComponent->effectiveMovement = not direction;
					}
					else
						walkComponent->effectiveMovement = Direction::None;//Stop the player
				}
			}
		}
	}
}

void Mover::flip(entityx::Entity entity) const
{
	return;
	ArcherComponent::Handle archerComponent{entity.component<ArcherComponent>()};
	BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
	ArticuledArmsComponent::Handle armsComponent{entity.component<ArticuledArmsComponent>()};
	if(bodyComponent)
	{
		//Flip the body and get the symmetry point
		const float32 mid{getMid(bodyComponent->body)}, globalMid{mid + bodyComponent->body->GetPosition().x};
		flipFixtures(bodyComponent->body, mid);
		if(archerComponent)
		{
			b2Body* archerBody{archerComponent->quiverJoint->GetBodyB()};
			flipFixtures(archerBody, 0);
			//Make the flipped joint
			b2JointDef* jointDef{getJointDef(archerComponent->quiverJoint)};
			b2World* world{archerBody->GetWorld()};
			world->DestroyJoint(archerComponent->quiverJoint);
			archerComponent->quiverJoint = static_cast<b2WeldJoint*>(world->CreateJoint(jointDef));
			delete jointDef;
		}
		if(armsComponent)
		{
			b2Body* armsBody{armsComponent->armsJoint->GetBodyB()};
			flipFixtures(armsBody, 0);
			flipBody(armsBody, globalMid);
			HoldItemComponent::Handle holdItemComponent{armsComponent->arms.component<HoldItemComponent>()};
			if(holdItemComponent)
			{
				b2Body* itemBody{holdItemComponent->itemJoint->GetBodyB()};
				flipFixtures(itemBody, 0);
				flipBody(itemBody, globalMid);
				BowComponent::Handle bowComponent{holdItemComponent->item.component<BowComponent>()};
				if(bowComponent)
				{
					b2Body* bowBody{bowComponent->notchedArrowJoint->GetBodyB()};
					flipFixtures(bowBody, 0);
					flipBody(bowBody, globalMid);
				}
			}
		}
	}
}

inline float32 Mover::getMid(b2Body* body) const
{
	entityx::Entity entity{*static_cast<entityx::Entity*>(body->GetUserData())};
	if(entity)
	{
		SpriteComponent::Handle spriteComponent{entity.component<SpriteComponent>()};
		if(spriteComponent)
			return spriteComponent->sprite.getLocalBounds().width/(2.f*Context::parameters->pixelByMeter);
	}
	return body->GetLocalCenter().x;
}

void Mover::flipFixtures(b2Body* body, float32 mid) const
{
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
				b2Vec2 v1(edgeShape->m_vertex1), v2(edgeShape->m_vertex2);
				flipPoint(v1, mid);
				flipPoint(v2, mid);
				edgeShape->Set(v1, v2);
				flipPoint(edgeShape->m_vertex0, mid);
				flipPoint(edgeShape->m_vertex3, mid);
				break;
			}
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape{static_cast<b2PolygonShape*>(shape)};
				std::vector<b2Vec2> newPoints(polygonShape->GetVertexCount());
				for(unsigned int i{0}; i < newPoints.size(); ++i)
				{
					newPoints[i] = polygonShape->GetVertex(i);
					flipPoint(newPoints[i], mid);
				}
				polygonShape->Set(newPoints.data(), newPoints.size());
				break;
			}
			case b2Shape::e_chain:
			{
				b2ChainShape* chainShape{static_cast<b2ChainShape*>(shape)};
				std::vector<b2Vec2> newPoints(chainShape->m_count);
				for(unsigned int i{0}; i < newPoints.size(); ++i)
				{
					newPoints[i] = chainShape->m_vertices[i];
					flipPoint(newPoints[i], mid);
				}
				chainShape->CreateChain(newPoints.data(), newPoints.size());

				b2Vec2 prev(chainShape->m_prevVertex), next(chainShape->m_nextVertex);
				flipPoint(prev, mid);
				flipPoint(next, mid);
				if(chainShape->m_hasPrevVertex)
					chainShape->SetPrevVertex(prev);
				if(chainShape->m_hasNextVertex)
					chainShape->SetNextVertex(next);
				break;
			}
			default:
				break;
		}
	}
}

inline void Mover::flipBody(b2Body* body, float32 mid) const
{
	b2Vec2 newPos{body->GetPosition()};
	flipPoint(newPos, mid);
	body->SetTransform(newPos, body->GetAngle());
}

inline void Mover::flipPoint(b2Vec2& vec, float32 mid) const
{
	vec.x = mid + (mid - vec.x);
}
