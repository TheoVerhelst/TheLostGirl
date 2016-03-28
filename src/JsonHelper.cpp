#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <functional>
#include <dist/json/json.h>
#include <TheLostGirl/JsonHelper.hpp>

using namespace std::placeholders;

Json::Value JsonHelper::loadFromFile(const std::string& filePath)
{
	Json::Value root;
	Json::Reader reader;

	std::ifstream fileStream(filePath);
	if(not reader.parse(fileStream, root))//report to the user the failure and their locations in the document.
		throw std::runtime_error("error while parsing json file \"" + filePath + "\" : " + reader.getFormattedErrorMessages());
	fileStream.close();

	return root;
}

void JsonHelper::saveToFile(const Json::Value& value, const std::string& filePath)
{
	std::ofstream saveFileStream(filePath, std::ofstream::binary);
	saveFileStream << value;
	saveFileStream.close();
}

void JsonHelper::merge(Json::Value& left, const Json::Value& right)
{
	if(left.type() != right.type() and not left.isNull())
		throw std::invalid_argument("merge: left and right has different types.");
	else if(right.isArray())
		for(const Json::Value& rightChild : right)
			left.append(rightChild);
	else if(right.isObject())
		for(const std::string& childName : right.getMemberNames())
			merge(left[childName], right[childName]);
	else
		left = right;
}

bool JsonHelper::isSubset(const Json::Value& left, const Json::Value& right)
{
	if(not left.isConvertibleTo(right.type()))
		return false;
	else if(left.isArray())
	{
		for(const Json::Value& child : left)
			if(std::find_if(right.begin(), right.end(), std::bind(isEqual, child, _1)) == right.end())
				return false;
		return true;
	}
	else if(left.isObject())
	{
		for(const std::string& childName : left.getMemberNames())
			if(not isSubset(left[childName], right[childName]))
				return false;
		return true;
	}
	else
		return isEqual(left, right);
}

bool JsonHelper::isEqual(const Json::Value& left, const Json::Value& right)
{
	if(not left.isConvertibleTo(right.type()))
		return false;
	else if(left.isArray())
	{
		if(left.size() != right.size())
			return false;
		for(const Json::Value& child : left)
			if(std::find_if(right.begin(), right.end(), std::bind(isEqual, child, _1)) == right.end())
				return false;
		return true;
	}
	else if(left.isObject())
	{
		if(left.size() != right.size())
			return false;
		for(const std::string& childName : left.getMemberNames())
			if(not isEqual(left[childName], right[childName]))
				return false;
		return true;
	}
	else if(left.isDouble())
		return std::abs(left.asDouble() - right.asDouble()) < 0.0001;
	else
		return left == right;
}

void JsonHelper::substract(Json::Value& left, const Json::Value& right)
{
	if(left.type() != right.type())
		throw std::invalid_argument("substract: left and right has different types.");
	else if(left.isArray())
	{
		Json::Value oldLeft(Json::arrayValue);
		left.swap(oldLeft);
		for(Json::Value child : oldLeft)
			if(std::find_if(right.begin(), right.end(), std::bind(isEqual, child, _1)) == right.end())
				left.append(child);
	}
	else if(left.isObject())
	{
		Json::Value oldLeft(Json::objectValue);
		left.swap(oldLeft);
		for(const std::string& childName : oldLeft.getMemberNames())
		{
			if(not right.isMember(childName))
				left[childName] = oldLeft[childName];
			else if(oldLeft[childName].isObject() or oldLeft[childName].isArray())
			{
				substract(oldLeft[childName], right[childName]);
				left[childName] = oldLeft[childName];
			}
		}
	}
	else
		throw std::invalid_argument("substract: left and right are not array or object.");
}

void JsonHelper::parse(Json::Value& value, const Json::Value& model)
{
	parseImpl(value, model, "root", "root");
}

std::string JsonHelper::typeToStr(Json::ValueType type)
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

Json::ValueType JsonHelper::strToType(const std::string& str)
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

void JsonHelper::parseObject(const Json::Value& object, const std::string& name, std::map<std::string, Json::ValueType> valuesTypes)
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

void JsonHelper::requireValues(const Json::Value& object, const std::string& name, std::map<std::string, Json::ValueType> valuesTypes)
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

void JsonHelper::parseObject(const Json::Value& object, const std::string& name, Json::ValueType type)
{
	if(object.type() != Json::objectValue)
		throw std::runtime_error("\"" + name + "\" must be an object.");
	else
		for(std::string& elementName : object.getMemberNames())
			if(not object[elementName].isConvertibleTo(type))
				throw std::runtime_error("\"" + name + "." + elementName + "\" must be a" + typeToStr(type) + ".");
}

void JsonHelper::parseArray(const Json::Value& array, const std::string& name, std::vector<Json::Value> values)
{
	if(array.type() != Json::arrayValue)
		throw std::runtime_error("\"" + name + "\" must be an array.");
	else
		for(Json::ArrayIndex i{0}; i < array.size(); ++i)
			if(std::find(values.begin(), values.end(), array[i]) == values.end())//If the value in the array is not in the vector
				throw std::runtime_error("\"" + name + "." + std::to_string(i) + "\" identifier (" + array[i].asString() + ") is not recognized.");
}

void JsonHelper::parseValue(const Json::Value& value, const std::string& name, std::vector<Json::Value> values)
{
	if(std::find(values.begin(), values.end(), value) == values.end())//If the value is not in the vector
		throw std::runtime_error("\"" + name + "\" value (" + value.asString() + ") is not recognized.");
}

void JsonHelper::parseValue(const Json::Value& value, const std::string& name, Json::ValueType type)
{
	if(not value.isConvertibleTo(type))
		throw std::runtime_error("\"" + name + "\" must be a" + typeToStr(type) + ".");
}

void JsonHelper::parseArray(const Json::Value& array, const std::string& name, Json::ValueType type)
{
	if(array.type() != Json::arrayValue)
		throw std::runtime_error("\"" + name + "\" must be an array.");
	else
		for(Json::ArrayIndex i{0}; i < array.size(); ++i)
			if(not array[i].isConvertibleTo(type))
				throw std::runtime_error("\"" + name + "." +std::to_string(i) + "\" must be a" + typeToStr(type) + ".");
}

void JsonHelper::parseImpl(Json::Value& value, const Json::Value& model, const std::string& valueName, const std::string& modelName)
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
			for(const std::string& valueChildName : value.getMemberNames())
				//Parse the child according to the model child
				parseImpl(value[valueChildName], model["children"], valueName + "." + valueChildName, modelName + ".children");
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
				const std::string& childName{modelChild["name"].asString()};
				requireValues(modelChild, modelName + "." + childName, {{"required", Json::booleanValue},
																		{"type", Json::stringValue}});
				//If the current child is required
				if(modelChild["required"].asBool())
					//Add it to the required children map
					requiredChildren.emplace(childName, strToType(modelChild["type"].asString()));
				//In all case, add the child to the possible children map
				possibleChildren.emplace(childName, strToType(modelChild["type"].asString()));
				if(value.isMember(childName))
					parseImpl(value[childName], modelChild, valueName + "." + childName, modelName + "." + childName);
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
			parseImpl(value[i], model["children"], valueName + "." + std::to_string(i), modelName);
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
