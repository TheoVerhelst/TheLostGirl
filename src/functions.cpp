#include <algorithm>
#include <iostream>
#include <cmath>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <dist/json/json.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/functions.h>

using namespace sf;

FloatRect handleResize(Event::SizeEvent size)
{
	float iw{static_cast<float>(size.width)};
	float ih{static_cast<float>(size.height)};
	float fh(ih), fw(iw);//If size is in a 16:9 ratio, it won't change.

	if(iw / 16.f < ih / 9.f) //Taller than a 16:9 ratio
	{
		fh = iw * (9.0f / 16.0f);
		fw = iw;
	}
	else if(iw / 16.f > ih / 9.f) //Larger than a 16:9 ratio
	{
		fw = ih * (16.0f / 9.0f);
		fh = ih;
	}

	float scalex{fw / iw};
	float scaley{fh / ih};
	return FloatRect((1 - scalex) / 2.0f, (1 - scaley) / 2.0f, scalex, scaley);
}

Color fadingColor(Time dt, Time fadingLength, bool in)
{
	float alpha{cap((dt / fadingLength) * 255.f, 0.f, 255.f)};
	alpha = in ? alpha : 255 - alpha;//Invert the fading if in is false, so if it is a fade out
	return Color(255, 255, 255, static_cast<sf::Uint8>(alpha));
}

std::string typeToStr(Json::ValueType type)
{
	//The "n " or " " liaise the words
	switch(type)
	{
		case Json::booleanValue:
			return " boolean number";
		case Json::intValue:
			return "n integer number";
		case Json::uintValue:
			return "n unsigned integer number";
		case Json::realValue:
			return " real number";
		case Json::stringValue:
			return " string";
		case Json::arrayValue:
			return "n array";
		case Json::objectValue:
			return "n object";
		case Json::nullValue:
		default:
			return " null value";
	}
}

Json::ValueType strToType(std::string str)
{
	if(str == "string")
		return Json::stringValue;
	else if(str == "object")
		return Json::objectValue;
	else if(str == "array")
		return Json::arrayValue;
	else if(str == "real")
		return Json::realValue;
	else if(str == "int")
		return Json::intValue;
	else if(str == "uint")
		return Json::uintValue;
	else if(str == "bool")
		return Json::booleanValue;
	else
		return Json::nullValue;
}

void parseObject(const Json::Value& object, const std::string name, std::map<std::string, Json::ValueType> valuesTypes)
{
	if(object.type() != Json::objectValue)
		throw std::runtime_error("\"" + name + "\" must be an object.");
	else
	{
		for(std::string elementName : object.getMemberNames())
		{
			if(valuesTypes.find(elementName) == valuesTypes.end())//If the value in the object does not exists in the map
				throw std::runtime_error("\"" + name + "." + elementName + "\" identifier is not recognized.");
			else if(not object[elementName].isConvertibleTo(valuesTypes[elementName]))//If the value exists but have not the right type
				throw std::runtime_error("\"" + name + "." + elementName + "\" must be a" + typeToStr(valuesTypes[elementName]) + ".");
		}
	}
}

void requireValues(const Json::Value& object, const std::string name, std::map<std::string, Json::ValueType> valuesTypes)
{
	if(object.type() != Json::objectValue)
		throw std::runtime_error("\"" + name + "\" must be an object.");
	else
	{
		for(auto& pair : valuesTypes)
		{
			if(not object.isMember(pair.first))//If the value in the map does not exists in the object
				throw std::runtime_error("\"" + name + "." + pair.first + "\" value must be defined.");
			else if(not object[pair.first].isConvertibleTo(valuesTypes[pair.first]))//If the value exists but have not the right type
				throw std::runtime_error("\"" + name + "." + pair.first + "\" must be a" + typeToStr(valuesTypes[pair.first]) + " (it is currently a" + typeToStr(object[pair.first].type()) + ").");
		}
	}
}

void parseObject(const Json::Value& object, const std::string name, Json::ValueType type)
{
	if(object.type() != Json::objectValue)
		throw std::runtime_error("\"" + name + "\" must be an object.");
	else
	{
		for(std::string& elementName : object.getMemberNames())
		{
			if(not object[elementName].isConvertibleTo(type))
				throw std::runtime_error("\"" + name + "." + elementName + "\" must be a" + typeToStr(type) + ".");
		}
	}
}

void parseArray(const Json::Value& array, const std::string name, std::vector<Json::Value> values)
{
	if(array.type() != Json::arrayValue)
		throw std::runtime_error("\"" + name + "\" must be an array.");
	else
	{
		for(Json::ArrayIndex i{0}; i < array.size(); ++i)
		{
			if(std::find(values.begin(), values.end(), array[i]) == values.end())//If the value in the array is not in the vector
				throw std::runtime_error("\"" + name + "." + std::to_string(i) + "\" identifier (" + array[i].asString() + ") is not recognized.");
		}
	}
}

void parseValue(const Json::Value& value, const std::string name, std::vector<Json::Value> values)
{
	if(std::find(values.begin(), values.end(), value) == values.end())//If the value is not in the vector
		throw std::runtime_error("\"" + name + "\" value (" + value.asString() + ") is not recognized.");
}

void parseValue(const Json::Value& value, const std::string name, Json::ValueType type)
{
	if(not value.isConvertibleTo(type))
		throw std::runtime_error("\"" + name + "\" must be a" + typeToStr(type) + ".");
}

void parseArray(const Json::Value& array, const std::string name, Json::ValueType type)
{
	if(array.type() != Json::arrayValue)
		throw std::runtime_error("\"" + name + "\" must be an array.");
	else
	{
		for(Json::ArrayIndex i{0}; i < array.size(); ++i)
		{
			if(not array[i].isConvertibleTo(type))
				throw std::runtime_error("\"" + name + "." + i + "\" must be a" + typeToStr(type) + ".");
		}
	}
}

void parse(Json::Value& value, const Json::Value& model, const std::string& valueName, const std::string& modelName)
{
	//Assert that the type is one of those
	parseValue(model["type"], modelName + ".type", {"string", "object", "array", "real", "int", "uint", "bool", "null"});
	//Assert that the value has the right type
	parseValue(value, valueName, strToType(model["type"].asString()));
	if(model["type"].asString() == "object")
	{
		requireValues(model, modelName, {{"free children names", Json::booleanValue}});
		if(model["free children names"].asBool())
		{
			parseObject(model, modelName, {{"free children names", Json::booleanValue},
											{"name", Json::stringValue},
											{"required", Json::booleanValue},
											{"type", Json::stringValue},
											{"children", Json::objectValue}});
			//If names are free, children must be an object
			requireValues(model, modelName, {{"children", Json::objectValue}});
			requireValues(model["children"], modelName + ".children", {{"type", Json::stringValue}});
			//For every child of value
			for(std::string valueChildName : value.getMemberNames())
				//Parse the child according to the model child
				parse(value[valueChildName], model["children"], valueName + "." + valueChildName, modelName + ".children");
		}
		else
		{
			parseObject(model, modelName, {{"free children names", Json::booleanValue},
											{"name", Json::stringValue},
											{"required", Json::booleanValue},
											{"type", Json::stringValue},
											{"children", Json::arrayValue}});
			//If names are not free, children must be an array
			requireValues(model, modelName, {{"children", Json::arrayValue}});
			parseArray(model["children"], modelName, Json::objectValue);
			std::map<std::string, Json::ValueType> requiredChildren;
			std::map<std::string, Json::ValueType> possibleChildren;
			for(unsigned int i{0}; i < model["children"].size(); ++i)
			{
				Json::Value modelChild{model["children"][i]};
				//A not-free-name child must have these three members defined
				//Require name before, and if the name is defined we can require the others member with a fancier exception than ".children.i"
				requireValues(modelChild, modelName + ".children." + std::to_string(i), {{"name", Json::stringValue}});
				const std::string childName{modelChild["name"].asString()};
				requireValues(modelChild, modelName + "." + childName, {{"required", Json::booleanValue},
																		{"type", Json::stringValue}});
				//If the current child is required
				if(modelChild["required"].asBool())
					//Add it to the required children map
					requiredChildren.emplace(childName, strToType(modelChild["type"].asString()));
				//In all case, add the child to the possible children map
				possibleChildren.emplace(childName, strToType(modelChild["type"].asString()));
				if(value.isMember(childName))
					parse(value[childName], modelChild, valueName + "." + childName, modelName + "." + childName);
			}
			requireValues(value, valueName, requiredChildren);
			parseObject(value, valueName, possibleChildren);
		}
	}
	else if(model["type"].asString() == "array")
	{
		parseObject(model, modelName, {{"name", Json::stringValue},
										{"required", Json::booleanValue},
										{"type", Json::stringValue},
										{"children", Json::objectValue}});
		requireValues(model, modelName, {{"children", Json::objectValue}});
		//For each element in value
		for(unsigned int i{0}; i < value.size(); ++i)
			//Parse it according to the model child
			parse(value[i], model["children"], valueName + "." + std::to_string(i), modelName);
	}
	//Base type
	else
	{
		//Assert that the value has only thoses members defined
		parseObject(model, modelName, {{"default value", strToType(model["type"].asString())},
										{"name", Json::stringValue},
										{"required", Json::booleanValue},
										{"type", Json::stringValue},
										{"possible values", Json::arrayValue}});
		//If default and required = true defined at the same time
		if(model.isMember("default value") and model.get("required", false).asBool())
			throw std::runtime_error("\"" + modelName + ".default\" is defined but \"" + modelName + ".required\" is set to true");
		//If value is a null value, so if the value do not exists, and if there is a default value
		if(value == Json::Value(Json::nullValue) and model.isMember("default value"))
			value = model["default value"];

		//Assert that the value is one of the possible values
		if(model.isMember("possible values"))
		{
			std::vector<Json::Value> possibleValues;
			for(unsigned int i{0}; i < model["possible values"].size(); ++i)
				possibleValues.push_back(model["possible values"][i]);
			parseValue(value, valueName, possibleValues);
		}
	}
}

bool hasWhiteSpace(const std::string str)
{
	return std::any_of(str.cbegin(), str.cend(), [](const char& chr){return isspace(chr);});
}

Vector2f b2tosf(const b2Vec2& vec)
{
	return {vec.x, vec.y};
}

b2Vec2 sftob2(const Vector2f& vec)
{
	return {vec.x, vec.y};
}

sf::Color b2ColorToSf(const b2Color& color)
{
	return sf::Color(static_cast<sf::Uint8>(color.r*255.f), static_cast<sf::Uint8>(color.g*255.f),
					 static_cast<sf::Uint8>(color.b*255.f), static_cast<sf::Uint8>(color.a*255.f));
}

bool isPlayer(entityx::Entity entity)
{
	return entity.valid() and entity.has_component<CategoryComponent>() and entity.component<CategoryComponent>()->category & Category::Player;
}

void prettifyButton(tgui::Button::Ptr button)
{
	button->getRenderer()->setBorders(0.f, 0.f);
	button->getRenderer()->setProperty("backgroundcolor", "(255, 255, 255, 0)");
	button->getRenderer()->setProperty("backgroundcolorhover", "(255, 255, 255, 55)");
	button->getRenderer()->setProperty("backgroundcolordown", "(255, 255, 255, 90)");
	button->getRenderer()->setProperty("textcolornormal", "(0, 0, 0)");
}
