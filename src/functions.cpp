#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <dist/json/json.h>

#include <TheLostGirl/functions.h>

using namespace sf;
using std::cout;
using std::endl;

FloatRect handleResize(Event::SizeEvent size)
{
	unsigned int iw = size.width;
	unsigned int ih = size.height;
	float fh(ih), fw(iw);//If size is in a 16:9 ratio, it won't change.

	if(iw / 16 < ih / 9) //Taller than a 16:9 ratio
	{
		fh = iw * (9.0 / 16.0);
		fw = iw;
	}
	else if(iw / 16 > ih / 9) //Larger than a 16:9 ratio
	{
		fw = ih * (16.0 / 9.0);
		fh = ih;
	}

	float scalex = fw / iw;
	float scaley = fh / ih;
	return FloatRect((1 - scalex) / 2.0f, (1 - scaley) / 2.0f, scalex, scaley);
}

Color fadingColor(Time dt, Time fadingLength, bool in)
{
	float alpha = cap((dt / fadingLength) * 255, 0, 255);
	alpha = in ? alpha : 255 - alpha;//Invert the fading if in is false, so if it is a fade out
	return Color(255, 255, 255, alpha);
}

std::string typeToStr(Json::ValueType type)
{
	//The "n " or " " liaise the words
	switch(type)
	{
		case Json::nullValue:
			return " null value";
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
		default:
			return "ERROR";
	}
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
			else if(object[elementName].type() != valuesTypes[elementName])//If the value exists but have not the right type
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
			else if(object[pair.first].type() != valuesTypes[pair.first])//If the value exists but have not the right type
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
			if(object[elementName].type() != type)
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

void parseArray(const Json::Value& array, const std::string name, Json::ValueType type)
{
	if(array.type() != Json::arrayValue)
		throw std::runtime_error("\"" + name + "\" must be an array.");
	else
	{
		for(Json::ArrayIndex i{0}; i < array.size(); ++i)
		{
			if(array[i].type() != type)
				throw std::runtime_error("\"" + name + "." + i + "\" must be a" + typeToStr(type) + ".");
		}
	}
}

bool hasWhiteSpace(const std::string str)
{
	return std::any_of(str.cbegin(), str.cend(), [](const char& chr){return isspace(chr);});
}