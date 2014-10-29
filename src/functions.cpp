#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

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
