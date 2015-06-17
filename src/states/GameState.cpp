#include <fstream>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
#include <Box2D/Dynamics/Joints/b2DistanceJoint.h>
#include <Box2D/Dynamics/Joints/b2PulleyJoint.h>
#include <Box2D/Dynamics/Joints/b2WheelJoint.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>
#include <Box2D/Dynamics/Joints/b2RopeJoint.h>
#include <Box2D/Dynamics/Joints/b2MotorJoint.h>
#include <entityx/entityx.h>
#include <dist/json/json.h>
#include <TheLostGirl/State.h>
#include <TheLostGirl/states/PauseState.h>
#include <TheLostGirl/states/HUDState.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/systems.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/SpriteSheetAnimation.h>
#include <TheLostGirl/SkyAnimation.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/JointRoles.h>
#include <TheLostGirl/serialization.h>
#include <TheLostGirl/states/GameState.h>

//TODO Faire un système multi texture pour une même partie d'entité (soit chunker une grande image, soit faire plusieurs images).
//TODO Component pour la position de combat (parade, attaque, tir, ...).
//TODO Sound manager.
//TODO Faire un item pour l'arc.
//TODO Retirer la fleche pendant le CAC.
//TODO Mettre le carquois de l'autre coté quand dirigé vers la gauche.
//TODO Differents CAC en fonction du déplacement.
//TODO Sauvegarde en fonction des includes.

GameState::GameState(StateStack& stack, std::string file) :
	State(stack),
	m_contactListener(getContext()),
	m_timeSpeed{1.f},
	m_loading{true},
	m_levelIdentifier{""},
	m_numberOfPlans{1},
	m_referencePlan{0.f},
	m_levelRect{0, 0, 1920, 1080}
{
	getContext().eventManager.subscribe<ParametersChange>(*this);
	m_threadLoad = std::thread(&GameState::initWorld, this, file);
}

GameState::~GameState()
{
//	saveWorld("resources/levels/S.json");
	clear();
	if(m_threadLoad.joinable())
		m_threadLoad.join();
}

void GameState::draw()
{
	if(not m_loading)
	{
		getContext().systemManager.update<RenderSystem>(0.f);
		//The drag and drop system draw a line on the screen, so we have to update it here
		getContext().systemManager.update<DragAndDropSystem>(0.f);
	}
}

bool GameState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<ScriptsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<PendingChangesSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<PhysicsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<ScrollingSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<StatsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<TimeSystem>(elapsedTime.asSeconds()*m_timeSpeed);//Scale the time
	return false;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if((event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		or event.type == sf::Event::LostFocus)
		requestStackPush<PauseState>();

	getContext().player.handleEvent(event);
	//Update the drag and drop state
	const bool isDragAndDropActive{getContext().player.isActived(Player::Action::Bend)};
	getContext().systemManager.system<DragAndDropSystem>()->setDragAndDropActivation(isDragAndDropActive);
	return false;
}

void GameState::receive(const ParametersChange&)
{
}

void GameState::saveWorld(const std::string& file)
{
		const float scale{getContext().parameters.scale};
		const float pixelByMeter{getContext().parameters.pixelByMeter};//The pixel/meters scale at the maximum resolution, about 1.f/120.f
		Json::Value root;//Will contains the root value after serializing.
		Json::StyledWriter writer;
		std::ofstream saveFileStream(file, std::ofstream::binary);
		for(auto& entity : m_entities)
		{
			Serializer s(getContext(), m_entities);
			if(entity.second.has_component<BodyComponent>())
				root["entities"][entity.first]["body"] = s.serialize(entity.second.component<BodyComponent>(),);
			if(entity.second.has_component<SpriteComponent>())
				root["entities"][entity.first]["sprite"] = s.serialize(entity.second.component<SpriteComponent>());
			if(entity.second.has_component<TransformComponent>())
				root["entities"][entity.first]["transform"] = s.serialize(entity.second.component<TransformComponent>());
			if(entity.second.has_component<InventoryComponent>())
				root["entities"][entity.first]["inventory"] = s.serialize(entity.second.component<InventoryComponent>());
			if(entity.second.has_component<ArcherComponent>())
				root["entities"][entity.first]["archer"] = s.serialize(entity.second.component<ArcherComponent>());
			if(entity.second.has_component<AnimationsComponent<SpriteSheetAnimation>>())
				root["entities"][entity.first]["spritesheet animations"] = s.serialize(entity.second.component<AnimationsComponent<SpriteSheetAnimation>>());
			if(entity.second.has_component<DirectionComponent>())
				root["entities"][entity.first]["direction"] = s.serialize(entity.second.component<DirectionComponent>());
			if(entity.second.has_component<CategoryComponent>())
				root["entities"][entity.first]["categories"] = s.serialize(entity.second.component<CategoryComponent>());
			if(entity.second.has_component<FallComponent>())
				root["entities"][entity.first]["fall"] = s.serialize(entity.second.component<FallComponent>());
			if(entity.second.has_component<WalkComponent>())
				root["entities"][entity.first]["walk"] = s.serialize(entity.second.component<WalkComponent>());
			if(entity.second.has_component<JumpComponent>())
				root["entities"][entity.first]["jump"] = s.serialize(entity.second.component<JumpComponent>());
			if(entity.second.has_component<HealthComponent>())
				root["entities"][entity.first]["health"] = s.serialize(entity.second.component<HealthComponent>());
			if(entity.second.has_component<StaminaComponent>())
				root["entities"][entity.first]["stamina"] = s.serialize(entity.second.component<StaminaComponent>());
			if(entity.second.has_component<ArrowComponent>())
				root["entities"][entity.first]["arrow"] = s.serialize(entity.second.component<ArrowComponent>());
			if(entity.second.has_component<HardnessComponent>())
				root["entities"][entity.first]["hardness"] = s.serialize(entity.second.component<HardnessComponent>());
			if(entity.second.has_component<ScriptsComponent>())
				root["entities"][entity.first]["scripts"] = s.serialize(entity.second.component<ScriptsComponent>());
			if(entity.second.has_component<DetectionRangeComponent>())
				root["entities"][entity.first]["detection range"] = s.serialize(entity.second.component<DetectionRangeComponent>());
			if(entity.second.has_component<DeathComponent>())
				root["entities"][entity.first]["death"] = s.serialize(entity.second.component<DeathComponent>());
			if(entity.second.has_component<NameComponent>())
				root["entities"][entity.first]["name"] = s.serialize(entity.second.component<NameComponent>());
			if(entity.second.has_component<HandToHandComponent>())
				root["entities"][entity.first]["hand to hand"] = s.serialize(entity.second.component<HandToHandComponent>());
			if(entity.second.has_component<ActorComponent>())
				root["entities"][entity.first]["actor"] = s.serialize(entity.second.component<ActorComponent>());
			if(entity.second.has_component<HoldItemComponent>())
				root["entities"][entity.first]["hold item"] = s.serialize(entity.second.component<HoldItemComponent>());
			if(entity.second.has_component<ArticuledArmsComponent>())
				root["entities"][entity.first]["articuled arms"] = s.serialize(entity.second.component<ArticuledArmsComponent>());
			if(entity.second.has_component<BowComponent>())
				root["entities"][entity.first]["bow"] = s.serialize(entity.second.component<BowComponent>());
			if(entity.second.has_component<QuiverComponent>())
				root["entities"][entity.first]["quiver"] = s.serialize(entity.second.component<QuiverComponent>());
			//End serialize
		}

		//time
		root["time"]["speed"] = m_timeSpeed;
		root["time"]["date"] = getContext().systemManager.system<TimeSystem>()->getRealTime().asSeconds();

		//level data
		root["level"]["identifier"] = m_levelIdentifier;
		root["level"]["box"]["x"] = m_levelRect.left;
		root["level"]["box"]["y"] = m_levelRect.top;
		root["level"]["box"]["w"] = m_levelRect.width;
		root["level"]["box"]["h"] = m_levelRect.height;
		root["level"]["number of plans"] = m_numberOfPlans;
		root["level"]["reference plan"] = m_referencePlan;

		//replaces
		for(auto& planData : m_sceneEntitiesData)
		{
			for(unsigned int i{0}; i < planData.second.size(); ++i)
			{
				root["level"]["replaces"][planData.first][i]["origin"]["x"] = planData.second[i].origin.left;
				root["level"]["replaces"][planData.first][i]["origin"]["y"] = planData.second[i].origin.top;
				root["level"]["replaces"][planData.first][i]["origin"]["w"] = planData.second[i].origin.width;
				root["level"]["replaces"][planData.first][i]["origin"]["h"] = planData.second[i].origin.height;
				for(Json::ArrayIndex j{0}; j < planData.second[i].replaces.size(); ++j)
				{
					root["level"]["replaces"][planData.first][i]["replaces"][j]["x"] = planData.second[i].replaces[j].x;
					root["level"]["replaces"][planData.first][i]["replaces"][j]["y"] = planData.second[i].replaces[j].y;
					root["level"]["replaces"][planData.first][i]["replaces"][j]["z"] = planData.second[i].replaces[j].z;
					root["level"]["replaces"][planData.first][i]["replaces"][j]["angle"] = planData.second[i].replaces[j].angle;
				}
			}
		}

		//joints
		for(b2Joint* joint{getContext().world.GetJointList()}; joint; joint = joint->GetNext())
		{
			b2Body* bodyA{joint->GetBodyA()};//Get the body A
			entityx::Entity entityA{*static_cast<entityx::Entity*>(bodyA->GetUserData())};//Get the entity A from the user data
			assert(isMember(m_entities, entityA));
			std::string entityAName{getKey(m_entities, entityA)};//Get the name of the entity A

			b2Body* bodyB{joint->GetBodyB()};//Get the body B
			entityx::Entity entityB{*static_cast<entityx::Entity*>(bodyB->GetUserData())};//Get the entity B from the user data
			assert(isMember(m_entities, entityB));
			std::string entityBName{getKey(m_entities, entityB)};//Get the name of the entity B
			switch(joint->GetType())
			{
				case e_revoluteJoint:
				{
					b2RevoluteJoint* castedJoint{static_cast<b2RevoluteJoint*>(joint)};
					root["joints"]["revolute joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["revolute joints"].size()-1};
					root["joints"]["revolute joints"][last]["entity A"] = entityAName;
					root["joints"]["revolute joints"][last]["entity B"] = entityBName;
					root["joints"]["revolute joints"][last]["local anchor A"]["x"] = castedJoint->GetLocalAnchorA().x*pixelByMeter;
					root["joints"]["revolute joints"][last]["local anchor A"]["y"] = castedJoint->GetLocalAnchorA().y*pixelByMeter;
					root["joints"]["revolute joints"][last]["local anchor B"]["x"] = castedJoint->GetLocalAnchorB().x*pixelByMeter;
					root["joints"]["revolute joints"][last]["local anchor B"]["y"] = castedJoint->GetLocalAnchorB().y*pixelByMeter;
					root["joints"]["revolute joints"][last]["lower angle"] = castedJoint->GetLowerLimit() * 180.f / b2_pi;
					root["joints"]["revolute joints"][last]["upper angle"] = castedJoint->GetUpperLimit() * 180.f / b2_pi;
					root["joints"]["revolute joints"][last]["enable limit"] = castedJoint->IsLimitEnabled();
					root["joints"]["revolute joints"][last]["maximum motor torque"] = castedJoint->GetMaxMotorTorque()*pixelByMeter;
					root["joints"]["revolute joints"][last]["motor speed"] = castedJoint->GetMotorSpeed()*pixelByMeter;
					root["joints"]["revolute joints"][last]["enable motor"] = castedJoint->IsMotorEnabled();
					if(jointHasRole(joint, JointRole::BendingAngle))
						root["joints"]["revolute joints"][last]["roles"].append("bending angle");
					break;
				}

				case e_prismaticJoint:
				{
					b2PrismaticJoint* castedJoint{static_cast<b2PrismaticJoint*>(joint)};
					root["joints"]["prismatic joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["prismatic joints"].size()-1};
					root["joints"]["prismatic joints"][last]["entity A"] = entityAName;
					root["joints"]["prismatic joints"][last]["entity B"] = entityBName;
					root["joints"]["prismatic joints"][last]["local anchor A"]["x"] = castedJoint->GetLocalAnchorA().x*pixelByMeter;
					root["joints"]["prismatic joints"][last]["local anchor A"]["y"] = castedJoint->GetLocalAnchorA().y*pixelByMeter;
					root["joints"]["prismatic joints"][last]["local anchor B"]["x"] = castedJoint->GetLocalAnchorB().x*pixelByMeter;
					root["joints"]["prismatic joints"][last]["local anchor B"]["y"] = castedJoint->GetLocalAnchorB().y*pixelByMeter;
					root["joints"]["prismatic joints"][last]["local axis A"]["x"] = castedJoint->GetLocalAxisA().x*pixelByMeter;
					root["joints"]["prismatic joints"][last]["local axis A"]["y"] = castedJoint->GetLocalAxisA().y*pixelByMeter;
					root["joints"]["prismatic joints"][last]["enable limit"] = castedJoint->IsLimitEnabled();
					root["joints"]["prismatic joints"][last]["lower translation"] = castedJoint->GetLowerLimit()*pixelByMeter;
					root["joints"]["prismatic joints"][last]["upper translation"] = castedJoint->GetUpperLimit()*pixelByMeter;
					root["joints"]["prismatic joints"][last]["enable motor"] = castedJoint->IsMotorEnabled();
					root["joints"]["prismatic joints"][last]["maximum motor force"] = castedJoint->GetMaxMotorForce()*pixelByMeter;
					root["joints"]["prismatic joints"][last]["motor speed"] = castedJoint->GetMotorSpeed()*pixelByMeter;
					root["joints"]["prismatic joints"][last]["reference angle"] = castedJoint->GetReferenceAngle() * 180.f / b2_pi;
					if(jointHasRole(joint, JointRole::BendingPower))
						root["joints"]["prismatic joints"][last]["roles"].append("bending power");
					break;
				}

				case e_distanceJoint:
				{
					b2DistanceJoint* castedJoint{static_cast<b2DistanceJoint*>(joint)};
					root["joints"]["distance joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["distance joints"].size()-1};
					root["joints"]["distance joints"][last]["entity A"] = entityAName;
					root["joints"]["distance joints"][last]["entity B"] = entityBName;
					root["joints"]["distance joints"][last]["local anchor A"]["x"] = castedJoint->GetLocalAnchorA().x*pixelByMeter;
					root["joints"]["distance joints"][last]["local anchor A"]["y"] = castedJoint->GetLocalAnchorA().y*pixelByMeter;
					root["joints"]["distance joints"][last]["local anchor B"]["x"] = castedJoint->GetLocalAnchorB().x*pixelByMeter;
					root["joints"]["distance joints"][last]["local anchor B"]["y"] = castedJoint->GetLocalAnchorB().y*pixelByMeter;
					root["joints"]["distance joints"][last]["length"] = castedJoint->GetLength()*pixelByMeter;
					root["joints"]["distance joints"][last]["frequency Hz"] = castedJoint->GetFrequency();
					root["joints"]["distance joints"][last]["damping ratio"] = castedJoint->GetDampingRatio();
					break;
				}

				case e_pulleyJoint:
				{
					b2PulleyJoint* castedJoint{static_cast<b2PulleyJoint*>(joint)};
					root["joints"]["pulley joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["pulley joints"].size()-1};
					root["joints"]["pulley joints"][last]["entity A"] = entityAName;
					root["joints"]["pulley joints"][last]["entity B"] = entityBName;
					root["joints"]["pulley joints"][last]["local anchor A"]["x"] = castedJoint->GetBodyA()->GetLocalPoint(castedJoint->GetAnchorA()).x*pixelByMeter;
					root["joints"]["pulley joints"][last]["local anchor A"]["y"] = castedJoint->GetBodyA()->GetLocalPoint(castedJoint->GetAnchorA()).y*pixelByMeter;
					root["joints"]["pulley joints"][last]["local anchor B"]["x"] = castedJoint->GetBodyB()->GetLocalPoint(castedJoint->GetAnchorB()).x*pixelByMeter;
					root["joints"]["pulley joints"][last]["local anchor B"]["y"] = castedJoint->GetBodyB()->GetLocalPoint(castedJoint->GetAnchorB()).y*pixelByMeter;
					root["joints"]["pulley joints"][last]["ground anchor A"]["x"] = castedJoint->GetGroundAnchorA().x*pixelByMeter;
					root["joints"]["pulley joints"][last]["ground anchor A"]["y"] = castedJoint->GetGroundAnchorA().y*pixelByMeter;
					root["joints"]["pulley joints"][last]["ground anchor B"]["x"] = castedJoint->GetGroundAnchorB().x*pixelByMeter;
					root["joints"]["pulley joints"][last]["ground anchor B"]["y"] = castedJoint->GetGroundAnchorB().y*pixelByMeter;
					root["joints"]["pulley joints"][last]["length A"] = castedJoint->GetLengthA()*pixelByMeter;
					root["joints"]["pulley joints"][last]["length B"] = castedJoint->GetLengthB()*pixelByMeter;
					root["joints"]["pulley joints"][last]["ratio"] = castedJoint->GetRatio();
					break;
				}

				case e_gearJoint:
				{
					break;
				}

				case e_wheelJoint:
				{
					b2WheelJoint* castedJoint{static_cast<b2WheelJoint*>(joint)};
					root["joints"]["wheel joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["wheel joints"].size()-1};
					root["joints"]["wheel joints"][last]["entity A"] = entityAName;
					root["joints"]["wheel joints"][last]["entity B"] = entityBName;
					root["joints"]["wheel joints"][last]["local anchor A"]["x"] = castedJoint->GetLocalAnchorA().x*pixelByMeter;
					root["joints"]["wheel joints"][last]["local anchor A"]["y"] = castedJoint->GetLocalAnchorA().y*pixelByMeter;
					root["joints"]["wheel joints"][last]["local anchor B"]["x"] = castedJoint->GetLocalAnchorB().x*pixelByMeter;
					root["joints"]["wheel joints"][last]["local anchor B"]["y"] = castedJoint->GetLocalAnchorB().y*pixelByMeter;
					root["joints"]["wheel joints"][last]["local axis A"]["x"] = castedJoint->GetLocalAxisA().x*pixelByMeter;
					root["joints"]["wheel joints"][last]["local axis A"]["y"] = castedJoint->GetLocalAxisA().y*pixelByMeter;
					root["joints"]["wheel joints"][last]["enable motor"] = castedJoint->IsMotorEnabled();
					root["joints"]["wheel joints"][last]["maximum motor torque"] = castedJoint->GetMaxMotorTorque()*pixelByMeter;
					root["joints"]["wheel joints"][last]["frequency Hz"] = castedJoint->GetSpringFrequencyHz();
					root["joints"]["wheel joints"][last]["damping ratio"] = castedJoint->GetSpringDampingRatio();
					break;
				}

				case e_weldJoint:
				{
					b2WeldJoint* castedJoint{static_cast<b2WeldJoint*>(joint)};
					root["joints"]["weld joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["weld joints"].size()-1};
					root["joints"]["weld joints"][last]["entity A"] = entityAName;
					root["joints"]["weld joints"][last]["entity B"] = entityBName;
					root["joints"]["weld joints"][last]["local anchor A"]["x"] = castedJoint->GetLocalAnchorA().x*pixelByMeter;
					root["joints"]["weld joints"][last]["local anchor A"]["y"] = castedJoint->GetLocalAnchorA().y*pixelByMeter;
					root["joints"]["weld joints"][last]["local anchor B"]["x"] = castedJoint->GetLocalAnchorB().x*pixelByMeter;
					root["joints"]["weld joints"][last]["local anchor B"]["y"] = castedJoint->GetLocalAnchorB().y*pixelByMeter;
					root["joints"]["weld joints"][last]["frequency Hz"] = castedJoint->GetFrequency();
					root["joints"]["weld joints"][last]["damping ratio"] = castedJoint->GetDampingRatio();
					root["joints"]["weld joints"][last]["reference angle"] = castedJoint->GetReferenceAngle() * 180.f / b2_pi;
					break;
				}

				case e_frictionJoint:
				{
					b2FrictionJoint* castedJoint{static_cast<b2FrictionJoint*>(joint)};
					root["joints"]["friction joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["friction joints"].size()-1};
					root["joints"]["friction joints"][last]["entity A"] = entityAName;
					root["joints"]["friction joints"][last]["entity B"] = entityBName;
					root["joints"]["friction joints"][last]["local anchor A"]["x"] = castedJoint->GetLocalAnchorA().x*pixelByMeter;
					root["joints"]["friction joints"][last]["local anchor A"]["y"] = castedJoint->GetLocalAnchorA().y*pixelByMeter;
					root["joints"]["friction joints"][last]["local anchor B"]["x"] = castedJoint->GetLocalAnchorB().x*pixelByMeter;
					root["joints"]["friction joints"][last]["local anchor B"]["y"] = castedJoint->GetLocalAnchorB().y*pixelByMeter;
					root["joints"]["friction joints"][last]["maximum force"] = castedJoint->GetMaxForce()*pixelByMeter;
					root["joints"]["friction joints"][last]["maximum torque"] = castedJoint->GetMaxTorque()*pixelByMeter;
					break;
				}

				case e_ropeJoint:
				{
					b2RopeJoint* castedJoint{static_cast<b2RopeJoint*>(joint)};
					root["joints"]["rope joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["rope joints"].size()-1};
					root["joints"]["rope joints"][last]["entity A"] = entityAName;
					root["joints"]["rope joints"][last]["entity B"] = entityBName;
					root["joints"]["rope joints"][last]["local anchor A"]["x"] = castedJoint->GetLocalAnchorA().x*pixelByMeter;
					root["joints"]["rope joints"][last]["local anchor A"]["y"] = castedJoint->GetLocalAnchorA().y*pixelByMeter;
					root["joints"]["rope joints"][last]["local anchor B"]["x"] = castedJoint->GetLocalAnchorB().x*pixelByMeter;
					root["joints"]["rope joints"][last]["local anchor B"]["y"] = castedJoint->GetLocalAnchorB().y*pixelByMeter;
					root["joints"]["rope joints"][last]["maximum length"] = castedJoint->GetMaxLength()*pixelByMeter;
					break;
				}

				case e_motorJoint:
				{
					b2MotorJoint* castedJoint{static_cast<b2MotorJoint*>(joint)};
					root["joints"]["motor joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["motor joints"].size()-1};
					root["joints"]["motor joints"][last]["entity A"] = entityAName;
					root["joints"]["motor joints"][last]["entity B"] = entityBName;
					root["joints"]["motor joints"][last]["linear offset"]["x"] = castedJoint->GetLinearOffset().x*pixelByMeter;
					root["joints"]["motor joints"][last]["linear offset"]["y"] = castedJoint->GetLinearOffset().y*pixelByMeter;
					root["joints"]["motor joints"][last]["angular offset"] = castedJoint->GetAngularOffset();
					root["joints"]["motor joints"][last]["maximum force"] = castedJoint->GetMaxForce()*pixelByMeter;
					root["joints"]["motor joints"][last]["maximum torque"] = castedJoint->GetMaxTorque()*pixelByMeter;
					root["joints"]["motor joints"][last]["correction factor"] = castedJoint->GetCorrectionFactor();
					break;
				}

				case e_unknownJoint:
				case e_mouseJoint:
				default:
					break;
			}
		}
		saveFileStream << writer.write(root);
		saveFileStream.close();
}

void GameState::initWorld(const std::string& file)
{
	const float scale{getContext().parameters.scale};
	const float pixelByMeter{getContext().parameters.pixelByMeter};//The pixel/meters scale at the maximum resolution, about 120.f
	TextureManager& texManager(getContext().textureManager);
	getContext().eventManager.emit<LoadingStateChange>("Loading save file");
	try
	{
		//Parse the level data
		Json::Value root;//Will contains the root value after parsing.
		Json::Value model;
		Json::Reader reader;
		std::ifstream saveFile(file, std::ifstream::binary);
		std::ifstream modelSaveFile("resources/levels/model.json", std::ifstream::binary);
		if(not reader.parse(saveFile, root))//report to the user the failure and their locations in the document.
			throw std::runtime_error("save file \"" + file + "\" : " + reader.getFormattedErrorMessages());
		if(not reader.parse(modelSaveFile, model))
			throw std::runtime_error("model file \"resources/levels/model.json\": " + reader.getFormattedErrorMessages());

		//SuperMegaMagic parsing of the save file from the model file
		parse(root, model, "root", "root");

		const Json::Value time{root["time"]};
		getContext().systemManager.system<TimeSystem>()->setTotalTime(sf::seconds(time["date"].asFloat()));
		m_timeSpeed = time["speed"].asFloat();
		getContext().systemManager.system<AnimationsSystem>()->setTimeSpeed(m_timeSpeed);

		const Json::Value level{root["level"]};
		//number of plans
		//must load it now in order to know how much plans can be defined
		m_numberOfPlans = level["number of plans"].asUInt();
		const std::string identifier{level["identifier"].asString()};
		if(hasWhiteSpace(identifier))
			throw std::runtime_error("\"level.identifier\" value contains whitespaces.");
		m_levelIdentifier = identifier;
		m_referencePlan = level["reference plan"].asFloat();

		//box
		const Json::Value levelBox{level["box"]};
		m_levelRect.width = levelBox["w"].asInt();
		m_levelRect.height = levelBox["h"].asInt();
		m_levelRect.top = levelBox["x"].asInt();
		m_levelRect.left = levelBox["y"].asInt();

		getContext().systemManager.system<ScrollingSystem>()->setLevelData(m_levelRect, m_referencePlan);
		Json::Value genericEntities;
		if(root.isMember("include"))
		{
			const Json::Value includes{root["include"]};
			for(Json::ArrayIndex i{0}; i < includes.size(); ++i)
			{
				//Parse the level data
				Json::Value includeRoot;//Will contains the root value after parsing.
				std::ifstream includeFile("resources/levels/" + includes[i].asString(), std::ifstream::binary);
				if(not reader.parse(includeFile, includeRoot))//report to the user the failure and their locations in the document.
					throw std::runtime_error("included file \"resources/levels/" + includes[i].asString() + "\" : " + reader.getFormattedErrorMessages());

				//Parsing of the included file from the model file
				parse(includeRoot, model, "root", "root");
				//Add generic entities to others
				Json::Value currentGenericEntities{includeRoot["entities"]};
				for(const std::string& genericEntityName : currentGenericEntities.getMemberNames())
					genericEntities[genericEntityName] = currentGenericEntities[genericEntityName];
			}
		}

		//entities
		//Load entities before everything else allow to call the ScrollingSystem update after each sprite creation,
		//and so place sprite directly at the right position when loading the level, because the player is an entity,
		//and the scrolling system replace sprite according to the player position.
		if(root.isMember("entities"))
		{
			const Json::Value entities{root["entities"]};
			//First add all entities without add components, that's for component that need
			//that m_entities is complete to be assigned (such as InventoryComponent).
			for(const std::string& entityName : entities.getMemberNames())
				m_entities.emplace(entityName, getContext().entityManager.create());

			for(const std::string& entityName : entities.getMemberNames())
			{
				Json::Value entity{entities[entityName]};
				//If the entity is derivated from a generic entities, add components of the base to the entity
				if(entity.isMember("base"))
				{
					const Json::Value base{genericEntities[entity["base"].asString()]};
					for(const std::string& componentName : base.getMemberNames())
						entity[componentName] = base[componentName];
				}
				//Assign to root the new entity
				root["entities"][entityName] = entity;
				if(entity.isMember("name"))
					getContext().eventManager.emit<LoadingStateChange>("Loading " + entity["name"].asString());
				else
					getContext().eventManager.emit<LoadingStateChange>("Loading " + entityName);

				Serializer s(getContext(), m_entities);
				if(entity.isMember("transform"))
					s.deserialize(entity["transform"], m_entities[entityName].assign<TransformComponent>());
				if(entity.isMember("sprite"))
					s.deserialize(entity["sprite"], m_entities[entityName].assign<SpriteComponent>());
				//body
				if(entity.isMember("body"))
				{
					s.deserialize(entity["body"], m_entities[entityName].assign<BodyComponent>(), m_entities.at(entityName).component<TransformComponent>());
					//Assign user data to the body of the entity
					m_entities[entityName].component<BodyComponent>()->body->SetUserData(&m_entities[entityName]);
				}
				//spritesheet animations
				if(entity.isMember("spritesheet animations"))
					s.deserialize(entity["spritesheet animations"], m_entities[entityName].assign<AnimationsComponent<SpriteSheetAnimation>>(), m_entities[entityName].component<SpriteComponent>());
				if(entity.isMember("inventory"))
					s.deserialize(entity["inventory"], m_entities[entityName].assign<InventoryComponent>());
				if(entity.isMember("archer"))
					s.deserialize(entity["archer"], m_entities[entityName].assign<ArcherComponent>());
				if(entity.isMember("categories"))
					s.deserialize(entity["categories"], m_entities[entityName].assign<CategoryComponent>());
				if(entity.isMember("walk"))
					s.deserialize(entity["walk"], m_entities[entityName].assign<WalkComponent>());
				if(entity.isMember("jump"))
					s.deserialize(entity["jump"], m_entities[entityName].assign<JumpComponent>());
				if(entity.isMember("health"))
					s.deserialize(entity["health"], m_entities[entityName].assign<HealthComponent>());
				if(entity.isMember("stamina"))
					s.deserialize(entity["stamina"], m_entities[entityName].assign<StaminaComponent>());
				if(entity.isMember("direction"))
					s.deserialize(entity["direction"], m_entities[entityName].assign<DirectionComponent>());
				if(entity.isMember("fall"))
					s.deserialize(entity["fall"], m_entities[entityName].assign<FallComponent>());
				if(entity.isMember("arrow"))
					s.deserialize(entity["arrow"], m_entities[entityName].assign<ArrowComponent>());
				if(entity.isMember("hardness"))
					s.deserialize(entity["hardness"], m_entities[entityName].assign<HardnessComponent>());
				if(entity.isMember("scripts"))
					s.deserialize(entity["scripts"], m_entities[entityName].assign<ScriptsComponent>());
				if(entity.isMember("detection range"))
					s.deserialize(entity["detection range"], m_entities[entityName].assign<DetectionRangeComponent>());
				if(entity.isMember("death"))
					s.deserialize(entity["death"], m_entities[entityName].assign<DeathComponent>());
				if(entity.isMember("name"))
					s.deserialize(entity["name"], m_entities[entityName].assign<NameComponent>());
				if(entity.isMember("hand to hand"))
					s.deserialize(entity["hand to hand"], m_entities[entityName].assign<HandToHandComponent>());
				if(entity.isMember("actor"))
					s.deserialize(entity["actor"], m_entities[entityName].assign<ActorComponent>());
				if(entity.isMember("hold item"))
					s.deserialize(entity["hold item"], m_entities[entityName].assign<HoldItemComponent>());
				if(entity.isMember("articuled arms"))
					s.deserialize(entity["articuled arms"], m_entities[entityName].assign<ArticuledArmsComponent>());
				if(entity.isMember("bow"))
					s.deserialize(entity["bow"], m_entities[entityName].assign<BowComponent>());
				if(entity.isMember("quiver"))
					s.deserialize(entity["quiver"], m_entities[entityName].assign<QuiverComponent>());
				//End serialize
			}
		}

		if(not getContext().parameters.debugMode)
		{
			//level
			//for each group of replaces
			for(const std::string groupOfReplacesName : level["replaces"].getMemberNames())
			{
				//Filename of the image to load
				const std::string fileTexture{m_levelIdentifier + "_" + groupOfReplacesName};
				//Path of the image to load
				const std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};

				const Json::Value groupOfReplaces{level["replaces"][groupOfReplacesName]};
				//Vector that will be added to m_sceneEntitiesData
				std::vector<SceneReplaces> planData;
				//For each image frame in the group of replaces
				for(Json::ArrayIndex i{0}; i < groupOfReplaces.size(); i++)
				{
					const Json::Value image{groupOfReplaces[i]};
					const Json::Value origin{image["origin"]};

					//Coordinates of the original image
					sf::IntRect originRect;
					originRect.left = static_cast<int>(origin["x"].asFloat()*getContext().parameters.scale);
					originRect.top = static_cast<int>(origin["y"].asFloat()*getContext().parameters.scale);
					originRect.width = static_cast<int>(origin["w"].asFloat()*getContext().parameters.scale);
					originRect.height = static_cast<int>(origin["h"].asFloat()*getContext().parameters.scale);

					SceneReplaces replacesData;
					//Directly take the data from the json value in order to keep the not scaled size
					replacesData.origin = {origin["x"].asInt(), origin["y"].asInt(), origin["w"].asInt(), origin["h"].asInt()};

					//Load the texture
					//Identifier of the texture, in format "level_plan_texture"
					const std::string textureIdentifier{fileTexture + "_" + std::to_string(i)};
					getContext().eventManager.emit<LoadingStateChange>("Loading plan " + groupOfReplacesName);
					texManager.load<sf::IntRect>(textureIdentifier, path, originRect);
					//Replaces
					const Json::Value replaces{image["replaces"]};
					//For each replacing of the image
					for(Json::ArrayIndex j{0}; j < replaces.size(); j++)
					{
						const Json::Value replace{replaces[j]};
						//Identifier of the entity, in format "level_plan_texture_replace"
						const std::string replaceIdentifier{textureIdentifier + "_" + std::to_string(j)};
						//Position where the frame should be replaced
						Transform replaceTransform;
						replaceTransform.x = replace["x"].asFloat()*getContext().parameters.scale;
						replaceTransform.y = replace["y"].asFloat()*getContext().parameters.scale;
						replaceTransform.z = replace["z"].asFloat();
						replaceTransform.angle = replace["angle"].asFloat();

						//Directly take the data from the json value in order to keep the not scaled size
						replacesData.replaces.push_back({replace["x"].asFloat(), replace["y"].asFloat(), replace["z"].asFloat(), replace["angle"].asFloat()});

						//Create an entity
						m_sceneEntities.emplace(replaceIdentifier, getContext().entityManager.create());
						//Create a sprite with the loaded texture
						//Assign the sprite to the entity
						sf::Sprite replaceSpr(texManager.get(textureIdentifier));
						replaceSpr.setPosition(replaceTransform.x, replaceTransform.y);
						replaceSpr.setRotation(replaceTransform.angle);
						SpriteComponent::Handle sprComp{m_sceneEntities[replaceIdentifier].assign<SpriteComponent>()};
						sprComp->sprite = replaceSpr;
						TransformComponent::Handle trsfComp{m_sceneEntities[replaceIdentifier].assign<TransformComponent>()};
						trsfComp->transform = replaceTransform;
						CategoryComponent::Handle catComp{m_sceneEntities[replaceIdentifier].assign<CategoryComponent>()};
						catComp->category = Category::Scene;
					}
					planData.push_back(replacesData);
				}
				m_sceneEntitiesData.emplace(groupOfReplacesName, planData);
			}
			//Load all the image in multiples chunks
			for(unsigned int i{0}; i < m_numberOfPlans; ++i)
			{
				const std::string fileTexture{m_levelIdentifier + "_" + std::to_string(i)};
				const std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};
				const int chunkSize{int(sf::Texture::getMaximumSize())};
				//The length of the plan, relatively to the reference.
				const int planLength{int(float(m_levelRect.width) * std::pow(1.5f, m_referencePlan - float(i)) * getContext().parameters.scale)};
				//Number of chunks to load in this plan
				const int numberOfChunks{(planLength/chunkSize)+1};

				for(int j{0}; j < numberOfChunks; ++j)
				{
					//Identifier of the entity, in format "level_plan_chunk"
					const std::string textureIdentifier{fileTexture + "_" + std::to_string(j)};
					//Size of the chunk to load, may be truncated if we reach the end of the image.
					int currentChunkSize{chunkSize};
					if(j >= planLength/chunkSize)
						currentChunkSize = planLength - chunkSize*j;
					getContext().eventManager.emit<LoadingStateChange>("Loading plan " + std::to_string(i+1));
					texManager.load<sf::IntRect>(textureIdentifier, path, sf::IntRect(j*chunkSize, 0, currentChunkSize, int(float(m_levelRect.height)*getContext().parameters.scale)));
					//Create an entity
					m_sceneEntities.emplace(textureIdentifier, getContext().entityManager.create());
					//Create a sprite with the loaded texture
					//Assign the sprite to the entity
					sf::Sprite chunkSpr(texManager.get(textureIdentifier));
					chunkSpr.setPosition(float(j*chunkSize), 0);
					SpriteComponent::Handle sprComp{m_sceneEntities[textureIdentifier].assign<SpriteComponent>()};
					sprComp->sprite = chunkSpr;
					TransformComponent::Handle trsfComp{m_sceneEntities[textureIdentifier].assign<TransformComponent>()};
					trsfComp->transform = {static_cast<float>(j*chunkSize), 0, static_cast<float>(i), 0};
					CategoryComponent::Handle catComp{m_sceneEntities[textureIdentifier].assign<CategoryComponent>()};
					catComp->category = Category::Scene;
					//Update the ScrollingSystem in order to directly display the sprite at the right position
				}
			}
		}

		//joints
		if(root.isMember("joints"))
		{
			getContext().eventManager.emit<LoadingStateChange>("Assembling entities");
			const Json::Value joints{root["joints"]};
			if(joints.isMember("revolute joints"))
			{
				const Json::Value revoluteJoints{joints["revolute joints"]};
				for(Json::ArrayIndex i{0}; i < revoluteJoints.size(); ++i)
				{
					const Json::Value joint{revoluteJoints[i]};
					const std::string entityA{joint["entity A"].asString()};
					const std::string entityB{joint["entity B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have a body
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});

					b2RevoluteJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->body;
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->body;
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.lowerAngle = joint["lower angle"].asFloat() * b2_pi / 180.f;
					jointDef.upperAngle = joint["upper angle"].asFloat() * b2_pi / 180.f;
					jointDef.enableLimit = joint["enable limit"].asBool();
					jointDef.maxMotorTorque = joint["maximum motor torque"].asFloat()/pixelByMeter;
					jointDef.motorSpeed = joint["motor speed"].asFloat()/pixelByMeter;
					jointDef.enableMotor = joint["enable motor"].asBool();

					//roles
					const Json::Value roles{joint["roles"]};
					for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
						if(roles[j].asString() == "bending angle")
							//Add the role to the definition
							jointDef.userData = add(jointDef.userData, static_cast<long unsigned int>(JointRole::BendingAngle));

					getContext().world.CreateJoint(&jointDef);
				}
			}
			if(joints.isMember("distance joints"))
			{
				const Json::Value distanceJoints{joints["distance joints"]};
				for(Json::ArrayIndex i{0}; i < distanceJoints.size(); ++i)
				{
					const Json::Value joint{distanceJoints[i]};
					const std::string entityA{joint["entity A"].asString()};
					const std::string entityB{joint["entity B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have a body
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});

					b2DistanceJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->body;
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->body;
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.length = joint["length"].asFloat()/pixelByMeter;
					jointDef.frequencyHz = joint["frequency Hz"].asFloat();
					jointDef.dampingRatio = joint["damping ratio"].asFloat();

					getContext().world.CreateJoint(&jointDef);
				}
			}
			if(joints.isMember("friction joints"))
			{
				const Json::Value frictionJoints{joints["friction joints"]};
				for(Json::ArrayIndex i{0}; i < frictionJoints.size(); ++i)
				{
					const Json::Value joint{frictionJoints[i]};
					const std::string entityA{joint["entity A"].asString()};
					const std::string entityB{joint["entity B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have a body
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});

					b2FrictionJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->body;
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->body;
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.maxTorque = joint["maximum torque"].asFloat()/pixelByMeter;
					jointDef.maxForce = joint["maximum force"].asFloat()/pixelByMeter;

					getContext().world.CreateJoint(&jointDef);
				}
			}
			if(joints.isMember("gear joints"))
			{
				const Json::Value gearJoints{joints["gear joints"]};
				for(Json::ArrayIndex i{0}; i < gearJoints.size(); ++i)
				{
					const Json::Value joint{gearJoints[i]};
				}
			}
			if(joints.isMember("motor joints"))
			{
				const Json::Value motorJoints{joints["motor joints"]};
				for(Json::ArrayIndex i{0}; i < motorJoints.size(); ++i)
				{
					const Json::Value joint{motorJoints[i]};
					const std::string entityA{joint["entity A"].asString()};
					const std::string entityB{joint["entity B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have a body
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});

					b2MotorJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->body;
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->body;
					jointDef.maxTorque = joint["maximum torque"].asFloat()/pixelByMeter;
					jointDef.maxForce = joint["maximum force"].asFloat()/pixelByMeter;
					jointDef.linearOffset.x = joint["linear offset"]["x"].asFloat()/pixelByMeter;
					jointDef.linearOffset.y = joint["linear offset"]["y"].asFloat()/pixelByMeter;
					jointDef.angularOffset = joint["angular offset"].asFloat();
					jointDef.correctionFactor = joint["correction factor"].asFloat();

					getContext().world.CreateJoint(&jointDef);
				}
			}
			if(joints.isMember("prismatic joints"))
			{
				const Json::Value prismaticJoints{joints["prismatic joints"]};
				for(Json::ArrayIndex i{0}; i < prismaticJoints.size(); ++i)
				{
					const Json::Value joint{prismaticJoints[i]};
					const std::string entityA{joint["entity A"].asString()};
					const std::string entityB{joint["entity B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have a body
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});

					b2PrismaticJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->body;
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->body;
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.localAxisA.x = joint["local axis A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAxisA.y = joint["local axis A"]["y"].asFloat()/pixelByMeter;
					jointDef.lowerTranslation = joint["lower translation"].asFloat()/pixelByMeter;
					jointDef.upperTranslation = joint["upper translation"].asFloat()/pixelByMeter;
					jointDef.enableLimit = joint["enable limit"].asBool();
					jointDef.maxMotorForce = joint["maximum motor force"].asFloat()/pixelByMeter;
					jointDef.motorSpeed = joint["motor speed"].asFloat()/pixelByMeter/pixelByMeter;
					jointDef.enableMotor = joint["enable motor"].asBool();
					jointDef.referenceAngle = joint["reference angle"].asFloat() * b2_pi / 180.f;

					//roles
					const Json::Value roles{joint["roles"]};
					for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
						if(roles[j].asString() == "bending power")
							//Add the role to the definition
							jointDef.userData = add(jointDef.userData, static_cast<long unsigned int>(JointRole::BendingPower));

					getContext().world.CreateJoint(&jointDef);
				}
			}
			if(joints.isMember("pulley joints"))
			{
				const Json::Value pulleyJoints{joints["pulley joints"]};
				for(Json::ArrayIndex i{0}; i < pulleyJoints.size(); ++i)
				{
					const Json::Value joint{pulleyJoints[i]};
					const std::string entityA{joint["entity A"].asString()};
					const std::string entityB{joint["entity B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have a body
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});

					b2PulleyJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->body;
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->body;
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.groundAnchorA.x = joint["ground anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.groundAnchorA.y = joint["ground anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.groundAnchorB.x = joint["ground anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.groundAnchorB.y = joint["ground anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.lengthA = joint["length A"].asFloat()/pixelByMeter;
					jointDef.lengthB = joint["length B"].asFloat()/pixelByMeter;
					jointDef.ratio = joint["ratio"].asFloat();

					getContext().world.CreateJoint(&jointDef);
				}
			}
			if(joints.isMember("rope joints"))
			{
				const Json::Value ropeJoints{joints["rope joints"]};
				for(Json::ArrayIndex i{0}; i < ropeJoints.size(); ++i)
				{
					const Json::Value joint{ropeJoints[i]};
					const std::string entityA{joint["entity A"].asString()};
					const std::string entityB{joint["entity B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have a body
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});

					b2RopeJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->body;
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->body;
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.maxLength = joint["maximum length"].asFloat()/pixelByMeter;

					getContext().world.CreateJoint(&jointDef);
				}
			}
			if(joints.isMember("weld joints"))
			{
				const Json::Value weldJoints{joints["weld joints"]};
				for(Json::ArrayIndex i{0}; i < weldJoints.size(); ++i)
				{
					const Json::Value joint{weldJoints[i]};
					const std::string entityA{joint["entity A"].asString()};
					const std::string entityB{joint["entity B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have a body
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});

					b2WeldJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->body;
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->body;
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.referenceAngle = joint["reference angle"].asFloat() * b2_pi / 180.f;
					jointDef.frequencyHz = joint["frequency Hz"].asFloat();
					jointDef.dampingRatio = joint["damping ratio"].asFloat();

					getContext().world.CreateJoint(&jointDef);
				}
			}
			if(joints.isMember("wheel joints"))
			{
				const Json::Value wheelJoints{joints["wheel joints"]};
				for(Json::ArrayIndex i{0}; i < wheelJoints.size(); ++i)
				{
					const Json::Value joint{wheelJoints[i]};
					const std::string entityA{joint["entity A"].asString()};
					const std::string entityB{joint["entity B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have a body
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});

					b2WheelJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->body;
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->body;
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.localAxisA.x = joint["local axis A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAxisA.y = joint["local axis A"]["y"].asFloat()/pixelByMeter;
					jointDef.enableMotor = joint["enable motor"].asBool();
					jointDef.maxMotorTorque = joint["maximum motor torque"].asFloat()/pixelByMeter;
					jointDef.motorSpeed = joint["motor speed"].asFloat()/pixelByMeter;
					jointDef.frequencyHz = joint["frequency Hz"].asFloat();
					jointDef.dampingRatio = joint["damping ratio"].asFloat();

					getContext().world.CreateJoint(&jointDef);
				}
			}
		}

		const float daySeconds{std::remainder(getContext().systemManager.system<TimeSystem>()->getRealTime().asSeconds(), 600.f)};
		//Add sky animations.
		if(m_entities.find("day sky") != m_entities.end() and m_entities["day sky"].valid())
		{
			AnimationsComponent<SkyAnimation>::Handle skyAnimationsComp{m_entities["day sky"].assign<AnimationsComponent<SkyAnimation>>()};
			//Add the animation, this is a sky animation, the importance is equal to zero, the duration is 600 seconds (1 day), and it loops.
			skyAnimationsComp->animationsManager.addAnimation("day/night cycle", SkyAnimation(m_entities["day sky"]), 0, sf::seconds(600), true);
			skyAnimationsComp->animationsManager.setProgress("day/night cycle", daySeconds/600.f);
			skyAnimationsComp->animationsManager.play("day/night cycle");
		}

		//Add sky animations.
		if(m_entities.find("night sky") != m_entities.end() and m_entities["night sky"].valid())
		{
			AnimationsComponent<SkyAnimation>::Handle skyAnimationsComp{m_entities["night sky"].assign<AnimationsComponent<SkyAnimation>>()};
			//Add the animation, this is a sky animation, the importance is equal to zero, the duration is 600 seconds (1 day), and it loops.
			skyAnimationsComp->animationsManager.addAnimation("day/night cycle", SkyAnimation(m_entities["night sky"]), 0, sf::seconds(600), true);
			skyAnimationsComp->animationsManager.setProgress("day/night cycle", daySeconds/600.f);
			skyAnimationsComp->animationsManager.play("day/night cycle");
		}

		getContext().systemManager.system<ScrollingSystem>()->searchPlayer(getContext().entityManager);
		getContext().systemManager.update<ScrollingSystem>(0.f);
		getContext().systemManager.update<AnimationsSystem>(0.f);
		getContext().player.handleInitialInputState();
		getContext().world.SetContactListener(&m_contactListener);
		requestStackPop();
		getContext().eventManager.emit<LoadingStateChange>("Loading HUD");
		requestStackPush<HUDState>();
		m_loading = false;
	}
	catch(std::runtime_error& e)
	{
		std::cerr << e.what() << "\n";
		std::cerr << "Failed to load save file \"" << file << "\".\n";
		clear();//Clear game content in order to prevent segmentation faults.
	}
}

void GameState::clear()
{
    for(auto& entity : m_entities)
    {
        if(entity.second.has_component<BodyComponent>())
                getContext().world.DestroyBody(entity.second.component<BodyComponent>()->body);

        entity.second.destroy();
    }
    for(auto& sceneEntity : m_sceneEntities)
        sceneEntity.second.destroy();
}
