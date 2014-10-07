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

//template <>
//void scaleRes<tgui::Label::Ptr>(tgui::Label::Ptr widget, sf::Vector2f resolution)
//{
//	widget->setPosition(widget->getPosition() * (resolution.x / 1920.f));
//	widget->setTextSize(widget->getTextSize() * (resolution.x / 1920.f));
//}
//
//template <>
//void scaleRes<tgui::Picture::Ptr>(tgui::Picture::Ptr widget, sf::Vector2f resolution)
//{
//	widget->setPosition(widget->getPosition() * (resolution.x / 1920.f));
//	//We don't resize the picture because it have already the right size
//	//(We loaded the picture in toPath(m_resolution))
//}
//
//float angle(Vector2f A, Vector2f B)
//{
//	float delta_x = A.x - B.x;
//	float delta_y = A.y - B.y;
//	return atan2(delta_y, delta_x) * 180 / pi;//That return a value in the right quadrant (atan() doesn't)
//}
//
//float euclidianDistance(Vector2f a, Vector2f b)
//{
//	return sqrt(((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
//}
//
//Vector2f orthogonalProjection(Vector2f P, Vector2f A, Vector2f B)
//{
//	Vector2f projection;
//	if(std::abs(A.x - B.x) > 0.0001f and std::abs(A.y - B.y) > 0.0001f)//We aproximate that the line AB is not aligned with axes
//	{
//		float m{(A.y - B.y)/(A.x - B.x)};
//		float p{A.y - m*A.x};
//		projection.x = (P.y + (P.x/m) - p)/((1/m)+m);
//		projection.y = m*projection.x + p;
//	}
//	else
//	{
//		projection.x = (std::abs(A.x - B.x) < 0.0001f ? A.x : P.x);//Line AB is vertical?
//		projection.y = (std::abs(A.y - B.y) < 0.0001f ? A.y : P.y);//Line AB is horizontal?
//	}
//	return projection;
//}
//
//float intervalDistance(float x, float a, float b)
//{
//	if((x >= a and x <= b) or (x <= a and x >= b))//If x is between a and b
//		return 0;
//	else
//		return std::min(abs(b - x), abs(a - x));
//}
//float segmentDistance(sf::Vector2f P, sf::Vector2f A, sf::Vector2f B)
//{
//	Vector2f projection = orthogonalProjection(P, A, B);
//	if(intervalDistance(projection.x, A.x, B.x) <= 0)//The projection of p is on the segment AB
//		return euclidianDistance(P, projection);
//	else//The projection of p is not on the segment
//		return std::min(euclidianDistance(A, P), euclidianDistance(B, P));
//	
//}
std::string toPath(sf::Vector2f resolution)
{
	return std::string("ressources/images/" + std::to_string(int(resolution.x)) + "x" + std::to_string(int(resolution.y)) + "/");
}

Vector2f toVector(std::string resolution)
{
	size_t xPos = resolution.find("x");
	size_t slashPos = resolution.substr(0, xPos).find_last_of("/");//Find the last slash before the x
	std::string x = resolution.substr(slashPos+1, xPos);//All before the x and after the slash
	std::string y = resolution.substr(xPos + 1); //All after the x
	return Vector2f(std::stof(x), std::stof(y));
}

Color fadingColor(Time elapsedTime, Time fadingLength, bool in)
{
	float alpha = cap((elapsedTime / fadingLength) * 255, 0, 255);
	alpha = in ? alpha : 255 - alpha;//Invert the fading if in is false, so if it is a fade out
	return Color(255, 255, 255, alpha);
}
