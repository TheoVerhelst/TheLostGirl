#include <iostream>
#include <fstream>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <dist/json/json.h>

#include <TheLostGirl/State.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/systems.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/ResourceIdentifiers.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/SpriteSheetAnimation.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>

#include <TheLostGirl/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_entities(),
	m_sprites(),
	m_animations(),
	m_ContactListener(),
	m_timeSystem()
{
	Json::Value root; // will contains the root value after parsing.
	Json::Reader reader;
	std::ifstream saveFileStream("ressources/levels/save.json", std::ifstream::binary);
	bool parsingSuccessful = reader.parse(saveFileStream, root);
	if(!parsingSuccessful)
	{
		//report to the user the failure and their locations in the document.
		std::cerr << "Failed to parse save file " << "ressources/levels/save.json" << ":" << std::endl
		<< reader.getFormattedErrorMessages() << std::endl;
	}
	else
		initWorld(root);
	getContext().player.handleInitialInputState(getContext().commandQueue);
}

GameState::~GameState()
{
	for(auto& pair : m_entities)
	{
		getContext().world.DestroyBody(pair.second.component<Body>()->body);
		pair.second.destroy();
	}
	getContext().world.ClearForces();
}

void GameState::draw()
{
	getContext().systemManager.update<Render>(sf::Time::Zero.asSeconds());
	//The drag and drop system draw a line on the screen, so we must put it here
	getContext().systemManager.update<DragAndDropSystem>(sf::Time::Zero.asSeconds());
}

bool GameState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<Physics>(elapsedTime.asSeconds());
	getContext().systemManager.update<Actions>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<FallSystem>(elapsedTime.asSeconds());
	m_timeSystem.update(elapsedTime);
	return false;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);
	
	getContext().player.handleEvent(event, getContext().commandQueue);
	//Update the drag and drop state
	bool isDragAndDropActive{getContext().player.isActived(Player::Action::Bend)};
	getContext().systemManager.system<DragAndDropSystem>()->setDragAndDropActivation(isDragAndDropActive);

	return false;
}

void GameState::initWorld(const Json::Value& levelData)
{
	float scale = getContext().parameters.scale;
	float pixelScale = getContext().parameters.pixelScale;
	getContext().world.SetContactListener(&m_ContactListener);
	
	//Parse the level data
	try
	{
		//entities
		if(valueExists(levelData, "root", "entities", Json::objectValue))
		{
			Json::Value entities = levelData["entities"];
			Json::Value::Members entitiesNames = entities.getMemberNames();
			for(std::string& entityName : entitiesNames)
			{
				Json::Value entity = entities[entityName];
				m_entities.emplace(entityName, getContext().entityManager.create());
				
				//sprite
				if(valueExists(entity, "entities." + entityName, "sprite", Json::stringValue))
				{
					Json::Value sprite = entity["sprite"];
					if(sprite.asString() == "archer")
						m_sprites.emplace(entityName, sf::Sprite(getContext().textureManager.get(Textures::Archer)));
					else if(sprite.asString() == "arms")
						m_sprites.emplace(entityName, sf::Sprite(getContext().textureManager.get(Textures::Arms)));
					else if(sprite.asString() == "bow")
						m_sprites.emplace(entityName, sf::Sprite(getContext().textureManager.get(Textures::Bow)));
					else
						throw std::runtime_error("entities." + entityName + ".sprite name does not exists.");
					m_entities[entityName].assign<SpriteComponent>(&m_sprites[entityName]);
				}
				
				//categories
				if(valueExists(entity, "entities." + entityName, "categories", Json::arrayValue))
				{
					Json::Value categories = entity["categories"];
					unsigned int categoriesInt{0};
					for(Json::ArrayIndex i{0}; i < categories.size(); ++i)
					{
						//For each cateory in the list, add it to the entity's category.
						if(categories[i].type() != Json::stringValue)
							throw std::runtime_error("entities." + entityName + ".categories." + std::to_string(i) + " value is not a string value.");
						else if(categories[i] == "player")
							categoriesInt |= Category::Player;
						else if(categories[i] == "can fall")
							categoriesInt |= Category::CanFall;
						else if(categories[i] == "ground")
							categoriesInt |= Category::Ground;
						else
							throw std::runtime_error("entities." + entityName + ".categories." + std::to_string(i) + " name does not exists.");
					}
					m_entities[entityName].assign<CategoryComponent>(categoriesInt);
				}
				
				//actor ID
				if(valueExists(entity, "entities." + entityName, "actor ID", Json::intValue))
					m_entities[entityName].assign<ActorIDComponent>(entity["actor ID"].asUInt());
				
				//walk
				if(valueExists(entity, "entities." + entityName, "walk", Json::realValue))
					m_entities[entityName].assign<Walk>(entity["walk"].asFloat());
				
				//jump
				if(valueExists(entity, "entities." + entityName, "jump", Json::realValue))
					m_entities[entityName].assign<Jump>(entity["jump"].asFloat());
				
				//direction
				if(valueExists(entity, "entities." + entityName, "direction", Json::stringValue))
				{
					Json::Value direction = entity["direction"];
					if(direction.asString() == "left")
						m_entities[entityName].assign<DirectionComponent>(Direction::Left);
					else if(direction.asString() == "right")
						m_entities[entityName].assign<DirectionComponent>(Direction::Right);
					else if(direction.asString() == "top")
						m_entities[entityName].assign<DirectionComponent>(Direction::Top);
					else if(direction.asString() == "bottom")
						m_entities[entityName].assign<DirectionComponent>(Direction::Bottom);
					else if(direction.asString() == "none")
						m_entities[entityName].assign<DirectionComponent>(Direction::None);
					else
						throw std::runtime_error("entities." + entityName + ".direction name does not exists.");
				}
				
				//fall
				if(valueExists(entity, "entities." + entityName, "fall", Json::nullValue))
					m_entities[entityName].assign<FallComponent>();
				
				//body
				if(valueExists(entity, "entities." + entityName, "body", Json::objectValue))
				{
					Json::Value body = entity["body"];
					b2BodyDef entityBodyDef;
					entityBodyDef.userData = &m_entities[entityName];
					
					//type
					if(valueExists(body, "entities." + entityName + ".body", "type", Json::stringValue))
					{
						Json::Value type = body["type"];
						if(type == "static")
							entityBodyDef.type = b2_staticBody;
						else if(type == "kinematic")
							entityBodyDef.type = b2_kinematicBody;
						else if(type == "dynamic")
							entityBodyDef.type = b2_dynamicBody;
						else
							throw std::runtime_error("entities." + entityName + ".body.type name does not exists.");
					}
					
					//position
					if(valueExists(body, "entities." + entityName + ".body", "position", Json::objectValue))
					{
						Json::Value position = body["position"];
						float x{0}, y{0};
						if(valueExists(position, "entities." + entityName + ".body.position", "x", Json::realValue))
							x = position["x"].asFloat();
						if(valueExists(position, "entities." + entityName + ".body.position", "y", Json::realValue))
							y = position["y"].asFloat();
						entityBodyDef.position = {x*scale/pixelScale, y*scale/pixelScale};
					}
					
					//fixed rotation
					if(valueExists(body, "entities." + entityName + ".body", "fixed rotation", Json::booleanValue))
						entityBodyDef.fixedRotation = body["fixed rotation"].asBool();
					
					b2Body* entityBody = getContext().world.CreateBody(&entityBodyDef);
					m_entities[entityName].assign<Body>(entityBody);
					
					//fixtures
					if(valueExists(body, "entities." + entityName + ".body", "fixtures", Json::arrayValue))
					{
						Json::Value fixtures = body["fixtures"];
						for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
						{
							b2FixtureDef entityFixtureDef;
							//Initialize shapes here in order to keep pointers alive when assigning entityFixtureDef.shape
							b2EdgeShape edgeShape;
							b2PolygonShape polygonShape;
							
							//type
							if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i), "type", Json::stringValue))
							{
								Json::Value type = fixtures[i]["type"];
								if(type == "polygon")
								{
									//box
									if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i), "box", Json::objectValue))
									{
										Json::Value box = fixtures[i]["box"];
										float w{100}, h{100};
										
										//width
										if(valueExists(box, "entities." + entityName + ".body.fixture." + std::to_string(i) + ".box", "w", Json::realValue))
											w = box["w"].asFloat();
										
										//height
										if(valueExists(box, "entities." + entityName + ".body.fixture." + std::to_string(i) + ".box", "h", Json::realValue))
											h = box["h"].asFloat();
										polygonShape.SetAsBox((w*scale/pixelScale)/2, (h*scale/pixelScale)/2, {(w*scale/pixelScale)/2, (h*scale/pixelScale)/2}, 0);
									}
									
									//vertices
									else if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i), "vertices", Json::arrayValue))
									{
										Json::Value vertices = fixtures[i]["vertices"];
										for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
										{
											if(vertices[j].type() != Json::objectValue)
												throw std::runtime_error("entities." + entityName + ".body.fixture." + std::to_string(i) + ".vertices." + std::to_string(j) + " value is not an object value.");
											else
											{
												float x{0}, y{0};
												
												//x
												if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i) + ".vertices." + std::to_string(j), "x", Json::realValue))
													x = fixtures[i]["x"].asFloat();
												
												//y
												if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i) + ".vertices." + std::to_string(j), "y", Json::realValue))
													y = fixtures[i]["y"].asFloat();
												polygonShape.m_vertices[j] = {(x*scale/pixelScale), (y*scale/pixelScale)};
											}
										}
									}
									else
										throw std::runtime_error("entities." + entityName + ".body.fixture." + std::to_string(i) + " has no geometry.");
									entityFixtureDef.shape = &polygonShape;
								}
								else if(type == "edge")
								{
									float x1{0}, y1{0}, x2{0}, y2{0};
									//1
									if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i), "1", Json::objectValue))
									{
										//x
										if(valueExists(fixtures[i]["1"], "entities." + entityName + ".body.fixture." + std::to_string(i) + ".1", "x", Json::realValue))
											x1 = fixtures[i]["1"]["x"].asFloat();
										
										//y
										if(valueExists(fixtures[i]["1"], "entities." + entityName + ".body.fixture." + std::to_string(i) + ".1", "y", Json::realValue))
											y1 = fixtures[i]["1"]["y"].asFloat();
									}
									else
										throw std::runtime_error("entities." + entityName + ".body.fixture." + std::to_string(i) + " has no point 1.");
									
									//2
									if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i), "2", Json::objectValue))
									{
										//x
										if(valueExists(fixtures[i]["2"], "entities." + entityName + ".body.fixture." + std::to_string(i) + ".2", "x", Json::realValue))
											x2 = fixtures[i]["2"]["x"].asFloat();
										
										//y
										if(valueExists(fixtures[i]["2"], "entities." + entityName + ".body.fixture." + std::to_string(i) + ".2", "y", Json::realValue))
											y2 = fixtures[i]["2"]["y"].asFloat();
									}
									else
										throw std::runtime_error("entities." + entityName + ".body.fixture." + std::to_string(i) + " has no point 2.");
									edgeShape.Set({(x1*scale/pixelScale), (y1*scale/pixelScale)}, {(x2*scale/pixelScale), (y2*scale/pixelScale)});
									entityFixtureDef.shape = &edgeShape;
								}
								else if(type == "chain")
								{
								}
								else if(type == "circle")
								{
								}
								else
									throw std::runtime_error("entities." + entityName + ".body.fixture." + std::to_string(i) + ".type name does not exists.");
							}
							
							//density
							if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i), "density", Json::realValue))
								entityFixtureDef.density = fixtures[i]["density"].asFloat();
							
							//friction
							if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i), "friction", Json::realValue))
								entityFixtureDef.friction = fixtures[i]["friction"].asFloat();
							
							//restitution
							if(valueExists(fixtures[i], "entities." + entityName + ".body.fixture." + std::to_string(i), "restitution", Json::realValue))
								entityFixtureDef.restitution = fixtures[i]["restitution"].asFloat();
							entityBody->CreateFixture(&entityFixtureDef);
						}
					}
				}
				
				//animations
				if(valueExists(entity, "entities." + entityName, "spritesheet animations", Json::objectValue))
				{
					Json::Value animations = entity["spritesheet animations"];
					Json::Value::Members animationsNames = animations.getMemberNames();
					m_animations[entityName] = Animations();
					m_entities[entityName].assign<AnimationsComponent>(&m_animations[entityName]);
					for(std::string& animationName : animationsNames)
					{
						Json::Value animation = animations[animationName];
						SpriteSheetAnimation entityAnimation;
						unsigned short int importance{0};
						float duration{0.f};
						bool loop{false};
						
						//importance
						if(valueExists(animation, "entities." + entityName + ".spritesheet animations." + animationName, "importance", Json::intValue))
							importance = animation["importance"].asUInt();
						
						//duration
						if(valueExists(animation, "entities." + entityName + ".spritesheet animations." + animationName, "duration", Json::realValue))
							duration = animation["duration"].asFloat();
						
						//loop
						if(valueExists(animation, "entities." + entityName + ".spritesheet animations." + animationName, "loop", Json::booleanValue))
							loop = animation["loop"].asBool();
						
						//frames
						if(valueExists(animation, "entities." + entityName + ".spritesheet animations." + animationName, "frames", Json::arrayValue))
						{
							Json::Value frames = animation["frames"];
							for(Json::ArrayIndex i{0}; i < frames.size(); ++i)
							{
								if(frames[i].type() != Json::objectValue)
									throw std::runtime_error("entities." + entityName + ".spritesheet animations." + animationName +  ".frames." + std::to_string(i) + " value is not an object value.");
								else
								{
									int x{0}, y{0}, w{0}, h{0};
									float relativeDuration{0.f};
									
									//x
									if(valueExists(frames[i], "entities." + entityName + ".spritesheet animations." + animationName + ".frames." + std::to_string(i), "x", Json::intValue))
										x = frames[i]["x"].asInt();
									
									//y
									if(valueExists(frames[i], "entities." + entityName + ".spritesheet animations." + animationName + ".frames." + std::to_string(i), "y", Json::intValue))
										y = frames[i]["y"].asInt();
									
									//w
									if(valueExists(frames[i], "entities." + entityName + ".spritesheet animations." + animationName + ".frames." + std::to_string(i), "w", Json::intValue))
										w = frames[i]["w"].asInt();
									
									//h
									if(valueExists(frames[i], "entities." + entityName + ".spritesheet animations." + animationName + ".frames." + std::to_string(i), "h", Json::intValue))
										h = frames[i]["h"].asInt();
									
									//relative duration
									if(valueExists(frames[i], "entities." + entityName + ".spritesheet animations." + animationName + ".frames." + std::to_string(i), "relative duration", Json::realValue))
										relativeDuration = frames[i]["relative duration"].asFloat();
									entityAnimation.addFrame(sf::IntRect(static_cast<float>(x)*scale, static_cast<float>(y)*scale, static_cast<float>(w)*scale, static_cast<float>(h)*scale), relativeDuration);
								}
							}
						}
						m_animations[entityName].addAnimation(animationName, entityAnimation, importance, sf::seconds(duration), loop);
					}
					
					//play animations
					if(valueExists(entity, "entities." + entityName, "play animations", Json::arrayValue))
					{
						Json::Value animationsToPlay = entity["play animations"];
						if(not valueExists(entity, "entities." + entityName, "spritesheet animations", Json::objectValue))
							throw std::runtime_error("entities." + entityName + ".play animations value defined but entities." + entityName + " has no spritesheet animations member.");
						else
						{
							for(Json::ArrayIndex i{0}; i < animationsToPlay.size(); ++i)
							{
								if(animationsToPlay[i].type() != Json::stringValue)
									throw std::runtime_error("entities." + entityName + ".play animations." + std::to_string(i) + " value is not a string value.");
								else if(not entity["spritesheet animations"].isMember(animationsToPlay[i].asString()))
									throw std::runtime_error("entities." + entityName + ".play animations." + std::to_string(i) + " animation is not registred in entities." + entityName + ".spritesheet animations");
								else
									m_animations[entityName].play(animationsToPlay[i].asString());
							}
						}
					}
					
					//activate animations
					if(valueExists(entity, "entities." + entityName, "activate animations", Json::arrayValue))
					{
						Json::Value animationsToActivate = entity["activate animations"];
						if(not valueExists(entity, "entities." + entityName, "spritesheet animations", Json::objectValue))
							throw std::runtime_error("entities." + entityName + ".activate animations value defined but entities." + entityName + " has no spritesheet animations member.");
						else
						{
							for(Json::ArrayIndex i{0}; i < animationsToActivate.size(); ++i)
							{
								if(animationsToActivate[i].type() != Json::stringValue)
									throw std::runtime_error("entities." + entityName + ".activate animations." + std::to_string(i) + " value is not a string value.");
								else if(not entity["spritesheet animations"].isMember(animationsToActivate[i].asString()))
									throw std::runtime_error("entities." + entityName + ".activate animations." + std::to_string(i) + " animation is not registred in entities." + entityName + ".spritesheet animations");
								else
									m_animations[entityName].activate(animationsToActivate[i].asString());
							}
						}
					}
				}
			}
		}
		
		//joints
		if(valueExists(levelData, "root", "joints", Json::arrayValue))
		{
			Json::Value joints = levelData["joints"];
			for(Json::ArrayIndex i{0}; i < joints.size(); ++i)
			{
				if(joints[i].type() != Json::objectValue)
					throw std::runtime_error("joints." + std::to_string(i) + " value is not an object value.");
				 else
				 {
				 	Json::Value joint = joints[i];
					std::string first, second;
					b2Body * bodyFirst{nullptr};
					b2Body * bodySecond{nullptr};
					
					//first
					if(valueExists(joint, "joints." + std::to_string(i), "first", Json::stringValue))
						first = joint["first"].asString();
					else
						throw std::runtime_error("joints." + std::to_string(i) + " has no first member.");
					if(m_entities.find(first) == m_entities.end())
						throw std::runtime_error("joints." + std::to_string(i) + ".first is not a member of entities.");
					else if(not m_entities[first].has_component<Body>())
						throw std::runtime_error("entities." + first + " required by joints." + std::to_string(i) + ".first has no body member.");
					else
						bodyFirst = m_entities[first].component<Body>()->body;
					
					//second
					if(valueExists(joint, "joints." + std::to_string(i), "second", Json::stringValue))
						second = joint["second"].asString();
					else
						throw std::runtime_error("joints." + std::to_string(i) + " has no second member.");
					if(m_entities.find(second) == m_entities.end())
						throw std::runtime_error("joints." + std::to_string(i) + ".second is not a member of entities.");
					else if(not m_entities[second].has_component<Body>())
						throw std::runtime_error("entities." + second + " required by joints." + std::to_string(i) + ".second has no body member.");
					else
						bodySecond = m_entities[second].component<Body>()->body;
								
				 	//type
				 	if(valueExists(joint, "joints." + std::to_string(i), "type", Json::stringValue))
					{
						Json::Value type = joint["type"];
						if(type == "revolute joint")
						{
							b2RevoluteJointDef jointDef;
							float xf{0}, yf{0}, xs{0}, ys{0};
							jointDef.bodyA = bodyFirst;
							jointDef.bodyB =  bodySecond;
							
							//local anchor first
							if(valueExists(joint, "joints." + std::to_string(i), "local anchor first", Json::objectValue))
							{
								Json::Value firstAnchor = joint["local anchor first"];
								
								//x
								if(valueExists(firstAnchor, "joints." + std::to_string(i) + ".local anchor first", "x", Json::realValue))
									xf = firstAnchor["x"].asFloat();
								
								//y
								if(valueExists(firstAnchor, "joints." + std::to_string(i) + ".local anchor first", "y", Json::realValue))
									yf = firstAnchor["y"].asFloat();
							}
							jointDef.localAnchorA = {(xf*scale/pixelScale), (yf*scale/pixelScale)};
							
							//local anchor second
							if(valueExists(joint, "joints." + std::to_string(i), "local anchor second", Json::objectValue))
							{
								Json::Value secondAnchor = joint["local anchor second"];
								
								//x
								if(valueExists(secondAnchor, "joints." + std::to_string(i) + ".local anchor second", "x", Json::realValue))
									xs = secondAnchor["x"].asFloat();
								
								//y
								if(valueExists(secondAnchor, "joints." + std::to_string(i) + ".local anchor second", "y", Json::realValue))
									ys = secondAnchor["y"].asFloat();
							}
							jointDef.localAnchorB = {(xs*scale/pixelScale), (ys*scale/pixelScale)};
							
							//lower angle
							if(valueExists(joint, "joints." + std::to_string(i), "lower angle", Json::realValue))
								jointDef.lowerAngle = (joint["lower angle"].asFloat() * b2_pi) / 180.f;
							
							//upper angle
							if(valueExists(joint, "joints." + std::to_string(i), "upper angle", Json::realValue))
								jointDef.upperAngle = (joint["upper angle"].asFloat() * b2_pi) / 180.f;
							
							//enable limit
							if(valueExists(joint, "joints." + std::to_string(i), "enable limit", Json::booleanValue))
								jointDef.enableLimit = joint["enable limit"].asBool();
							
							//max motor torque
							if(valueExists(joint, "joints." + std::to_string(i), "max motor torque", Json::realValue))
								jointDef.maxMotorTorque = joint["max motor torque"].asFloat();
							
							//enable motor
							if(valueExists(joint, "joints." + std::to_string(i), "enable motor", Json::booleanValue))
								jointDef.enableMotor = joint["enable motor"].asBool();
							
							getContext().world.CreateJoint(&jointDef);
						}
						else if(type == "distance joint")
						{
						}
						else if(type == "friction joint")
						{
						}
						else if(type == "gear joint")
						{
						}
						else if(type == "motor joint")
						{
						}
						else if(type == "prismatic joint")
						{
						}
						else if(type == "pulley joint")
						{
						}
						else if(type == "rope joint")
						{
						}
						else if(type == "weld joint")
						{
						}
						else if(type == "wheel joint")
						{
						}
						else
							throw std::runtime_error("joints." + std::to_string(i) + ".type name does not exists.");
					}
					else
						throw std::runtime_error("no joint type specified for joints." + std::to_string(i) + ".");
				 }
			}
		}
	}
	catch(std::runtime_error& e)
	{
		std::cerr << "Failed to parse save file : " << e.what() << std::endl;
		//Clear game content in order to prevent segmentation faults.
		for(auto& pair : m_entities)
		{
			getContext().world.DestroyBody(pair.second.component<Body>()->body);
			pair.second.destroy();
		}
		m_entities.clear();
		m_sprites.clear();
		m_animations.clear();
		getContext().world.ClearForces();
	}
}