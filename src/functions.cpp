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

std::string toPath(sf::Vector2u resolution)
{
	return std::string("ressources/images/" + std::to_string(resolution.x) + "x" + std::to_string(resolution.y) + "/");
}

Vector2u toVector(std::string resolution)
{
	size_t xPos = resolution.find("x");
	size_t slashPos = resolution.substr(0, xPos).find_last_of("/");//Find the last slash before the x
	std::string x = resolution.substr(slashPos+1, xPos);//All before the x and after the slash
	std::string y = resolution.substr(xPos + 1); //All after the x
	return Vector2u(std::stoul(x), std::stoul(y));
}

Color fadingColor(Time dt, Time fadingLength, bool in)
{
	float alpha = cap((dt / fadingLength) * 255, 0, 255);
	alpha = in ? alpha : 255 - alpha;//Invert the fading if in is false, so if it is a fade out
	return Color(255, 255, 255, alpha);
}

bool valueExists(const Json::Value& rootValue, const std::string rootName, const std::string& childName,  Json::ValueType childType)
{
	if(rootValue.type() == Json::objectValue)
	{
		if(rootValue.isMember(childName))
		{
			Json::Value childValue = rootValue[childName];
			if(childValue.type() == childType)
				return true;
			else
			{
				std::string childTypeStr;
				switch(childType)
				{
					case Json::nullValue:
						childTypeStr = " null";
						break;
					case Json::intValue:
						childTypeStr = "n integer";
						break;
					case Json::uintValue:
						childTypeStr = "n unsigned integer";
						break;
					case Json::realValue:
						childTypeStr = " floating point";
						break;
					case Json::stringValue:
						childTypeStr = " string";
						break;
					case Json::booleanValue:
						childTypeStr = " boolean";
						break;
					case Json::arrayValue:
						childTypeStr = "n array";
						break;
					case Json::objectValue:
						childTypeStr = "n object";
						break;
				}
				//The "n " or " " liaise the words
				throw std::runtime_error(childName + " value in " + rootName + " value not a" + childTypeStr + " value.");
				return false;
			}
		}
		else
			return false;
	}
	else
	{
		throw std::runtime_error(rootName + " value not an object value.");
		return false;
	}
}

bool isRightType(const Json::Value& value, const std::string name, Json::ValueType type)
{
	if(value.type() == Json::objectValue)
		return true;
	else
	{
		std::string typeStr;
		switch(type)
		{
			case Json::nullValue:
				typeStr = " null value";
				break;
			case Json::intValue:
				typeStr = "n integer number";
				break;
			case Json::uintValue:
				typeStr = "n unsigned integer number";
				break;
			case Json::realValue:
				typeStr = " floating point number";
				break;
			case Json::stringValue:
				typeStr = " string";
				break;
			case Json::booleanValue:
				typeStr = " boolean";
				break;
			case Json::arrayValue:
				typeStr = "n array";
				break;
			case Json::objectValue:
				typeStr = "n object";
				break;
		}
		//The "n " or " " liaise the words
		throw std::runtime_error(name + " value must be a" + typeStr + ".");
		return false;
	}
}